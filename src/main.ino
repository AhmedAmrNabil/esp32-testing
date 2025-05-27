#include <TFT_eSPI.h>
#include <ESP32Encoder.h>
#include <pins.h>
#include "image.h"
#include "StateMachine.h"
#include "MenuState.h"
#include "CountDownConfigState.h"
#include "defs.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
ESP32Encoder encoder;
unsigned int currentMillis;
StateMachine* stateMachine;

bool lastButtonState = HIGH;
bool buttonPressed = false;
unsigned long pressStartTime = 0;
bool holdEventFired = false;

//=========================================================
void setup() {
    initHardware();
    initDisplay();
    initStateMachine();
    Serial.println("Setup complete, starting loop...");
}

//=========================================================
void loop() {
    currentMillis = millis();
    handleEncoderTurn();
    handleButtonPress();
    updateBuzzer();  // Update buzzer state
    stateMachine->update(currentMillis);
}

//=========================================================
void initHardware() {
    pinMode(CLK, INPUT_PULLUP);
    pinMode(DT, INPUT_PULLUP);
    pinMode(SW, INPUT_PULLUP);
    encoder.attachHalfQuad(DT, CLK);
    encoder.setCount(0);
    ledcSetup(BUZZER_CH, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_CH);
    Serial.begin(115200);
}

void initDisplay() {
    tft.init();
#ifdef USE_DMA_TO_TFT
    tft.initDMA();
#endif
    tft.setRotation(1);
    // tft.fillScreen(TFT_BG_CLR);
    tft.setSwapBytes(true);
    tft.pushImage(0, 0, 240, 135, bg_lo_fi_anime);  // Display initial image
    // Initialize the sprite with TFT dimensions
    sprite.createSprite(tft.width(), tft.height());
    sprite.setSwapBytes(true);
    sprite.pushImage(0, 0, 240, 135, bg_lo_fi_anime);  // Display initial image
    sprite.pushSprite(0, 0);                           // Push empty screen at start
    Serial.println("Display initialized.");
}

void initStateMachine() {
    stateMachine = new StateMachine();
    stateMachine->changeState(new MenuState(stateMachine));  // Start with Menu State
    Serial.println("State machine initialized.");
}

void handleEncoderTurn() {
    int delta = encoder.getCount();
    if (delta != 0) {
        pulseBuzzer();                                    // Provide feedback for button click
        encoder.setCount(0);                              // Reset count after handling
        stateMachine->onEncoderTurn(delta / abs(delta));  // Pass absolute value of delta
    }
}

void handleButtonPress() {
    bool currentButtonState = digitalRead(SW);
    if (currentButtonState == LOW && lastButtonState == HIGH) {
        // Button pressed
        pressStartTime = millis();
        buttonPressed = true;
        holdEventFired = false;
    } else if (currentButtonState == HIGH && lastButtonState == LOW) {
        // Button released
        if (buttonPressed) {
            if (!holdEventFired) {
                pulseBuzzer();  // Provide feedback for button click
                stateMachine->onButtonClick();
            }
            buttonPressed = false;
            holdEventFired = false;
        }
    }

    if (buttonPressed && !holdEventFired && (millis() - pressStartTime >= HOLD_THRESHOLD)) {
        // Button held down
        pulseBuzzer();  // Provide feedback for button click
        stateMachine->onButtonHold();
        holdEventFired = true;
    }

    lastButtonState = currentButtonState;
}

// Globals for buzzer timing
unsigned long buzzerStartTime = 0;
bool buzzerOn = false;
bool buzzerActive = false;

void pulseBuzzer() {
    buzzerStartTime = millis();
    buzzerOn = true;
    buzzerActive = true;
    ledcWrite(BUZZER_CH, 200);  // Turn buzzer ON
}

void updateBuzzer() {
    if (!buzzerActive) return;

    unsigned long elapsed = millis() - buzzerStartTime;

    if (buzzerOn && elapsed >= BUZZER_DURATION) {
        // After 100ms buzzer ON, turn it OFF
        ledcWrite(BUZZER_CH, 0);
        buzzerOn = false;
        buzzerStartTime = millis();  // reset timer for off duration
    } else if (!buzzerOn && elapsed >= BUZZER_DURATION) {
        // After 100ms buzzer OFF, stop pulse
        buzzerActive = false;
    }
}
