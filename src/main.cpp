#include <Arduino.h>
#include <ESP32Encoder.h>
#include <BleKeyboard.h>

// ====== Pin Definitions ======
#define PIN_CLK 18
#define PIN_DT 19
#define PIN_BUTTON 21
#define PIN_TOUCH_FN 15

// ====== Debounce Settings ======
const unsigned long debounceDelay = 50;

// ====== Loop Timing ======
const unsigned long loopInterval = 10;

// ====== Touch Settings ======
const int touchThreshold = 30;

// ====== Globals ======
ESP32Encoder encoder;
BleKeyboard bleKeyboard("ESP32 Knob");

volatile bool touchTriggered = false;
long lastEncoderPosition = 0;
bool lastButtonState = HIGH;
bool currentButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long lastLoopTime = 0;

// ====== Function Declarations ======
void setupEncoder();
void setupButton();
void setupTouch();
void setupBleKeyboard();

void handleEncoder();
void handleButton();
bool isTouchActive();
void IRAM_ATTR onTouchInterrupt();

// ====== Setup ======
void setup() {
    delay(1000);
    Serial.begin(115200);

    setupEncoder();
    setupButton();
    setupTouch();
    setupBleKeyboard();
}

// ====== Main Loop ======
void loop() {
    if (millis() - lastLoopTime < loopInterval) return;
    lastLoopTime = millis();

    if (!bleKeyboard.isConnected()) return;

    handleEncoder();
    handleButton();
}

// ====== Setup Components ======
void setupEncoder() {
    encoder.attachHalfQuad(PIN_DT, PIN_CLK);
    encoder.setCount(0);
    encoder.setFilter(1023);
}

void setupButton() {
    pinMode(PIN_BUTTON, INPUT_PULLUP);
}

void setupTouch() {
    touchAttachInterrupt(PIN_TOUCH_FN, onTouchInterrupt, touchThreshold);
}

void setupBleKeyboard() {
    bleKeyboard.begin();
}

// ====== Handle Touch ======
bool isTouchActive() {
    if (touchTriggered) {
        touchTriggered = false;
        return true;
    }
    return false;
}

void IRAM_ATTR onTouchInterrupt() {
    touchTriggered = true;
}

// ====== Handle Encoder Movement ======
void handleEncoder() {
    long newPos = encoder.getCount();
    if (newPos == lastEncoderPosition) return;

    bool fnPressed = isTouchActive();

    if (newPos > lastEncoderPosition) {
        bleKeyboard.write(fnPressed ? KEY_MEDIA_NEXT_TRACK : KEY_MEDIA_VOLUME_UP);
    } else {
        bleKeyboard.write(fnPressed ? KEY_MEDIA_PREVIOUS_TRACK : KEY_MEDIA_VOLUME_DOWN);
    }

    lastEncoderPosition = newPos;
}

// ====== Handle Button Press ======
void handleButton() {
    int reading = digitalRead(PIN_BUTTON);

    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != currentButtonState) {
            currentButtonState = reading;

            // Button was released
            if (currentButtonState == HIGH) {
                bool fnPressed = isTouchActive();
                bleKeyboard.write(fnPressed ? KEY_MEDIA_PLAY_PAUSE : KEY_MEDIA_MUTE);
            }
        }
    }

    lastButtonState = reading;
}
