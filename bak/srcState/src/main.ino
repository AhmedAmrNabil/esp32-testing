#include <TFT_eSPI.h>
#include <ESP32Encoder.h>
#include <defs.h>
#include <ezBuzzer.h>

#include "image.h"
#include "StateMachine.h"
#include "MenuState.h"
#include "CountDownConfigState.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
ESP32Encoder encoder;
unsigned int currentMillis;
StateMachine* stateMachine;
ezBuzzer buzzer(BUZZER_PIN);  // Create ezBuzzer object that attaches to a pin

bool lastButtonState = HIGH;
bool buttonPressed = false;
unsigned long pressStartTime = 0;
bool holdEventFired = false;
extern int melody[];
extern int noteDurations[];
int lastEncoderCount = 0;

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
    buzzer.loop();  // Call the buzzer loop to handle beeping
    stateMachine->update(currentMillis);
}

//=========================================================
void initHardware() {
    pinMode(CLK, INPUT_PULLUP);
    pinMode(DT, INPUT_PULLUP);
    pinMode(SW, INPUT_PULLUP);
    encoder.attachHalfQuad(CLK, DT);
    encoder.setCount(0);
    ledcSetup(BUZZER_CH, BUZZER_PWM_FREQ, BUZZER_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_CH);
    buzzer.playMelody(melody, noteDurations, 3);  // Start melody
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
    tft.pushImage(0, 0, 240, 135, BG_IMG_ARR);  // Display initial image
    // Initialize the sprite with TFT dimensions
    sprite.createSprite(tft.width(), tft.height());
    sprite.setSwapBytes(true);
    sprite.pushImage(0, 0, 240, 135, BG_IMG_ARR);  // Display initial image
    sprite.pushSprite(0, 0);                       // Push empty screen at start
    Serial.println("Display initialized.");
}

void initStateMachine() {
    stateMachine = new StateMachine();
    stateMachine->changeState(new MenuState(stateMachine));  // Start with Menu State
    Serial.println("State machine initialized.");
}

void handleEncoderTurn() {
    int currentCount = encoder.getCount();
    int delta = currentCount - lastEncoderCount;  // Calculate the change in encoder count
    int detents = delta / 2;
    if (detents != 0) {
        buzzer.beep(50);                       // Provide feedback for button click
        lastEncoderCount += 2 * detents;       // Update last count to reflect the detents
        stateMachine->onEncoderTurn(detents);  // Pass absolute value of delta
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
                buzzer.beep(50);  // Provide feedback for button click
                stateMachine->onButtonClick();
            }
            buttonPressed = false;
            holdEventFired = false;
        }
    }

    if (buttonPressed && !holdEventFired && (millis() - pressStartTime >= HOLD_THRESHOLD)) {
        // Button held down
        buzzer.beep(100);  // Provide feedback for button click
        stateMachine->onButtonHold();
        holdEventFired = true;
    }

    lastButtonState = currentButtonState;
}
