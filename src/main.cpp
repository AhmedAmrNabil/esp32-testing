#include <Arduino.h>
#include <ESP32Encoder.h>
#include <BleKeyboard.h>

#define CLK 18           // CLK ENCODER
#define DT 19            // DT ENCODER
#define SW 21            // SW ENCODER
#define FN_TOUCH_PIN 15  // Touch pin = GPIO 15

ESP32Encoder encoder;
BleKeyboard bleKeyboard("ESP32 Knob");

long lastPosition = 0;
bool lastButtonState = HIGH;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;  // milliseconds
unsigned long lastLoopTime = 0;
const unsigned long loopInterval = 10;

volatile int touchState = false;

// Touch threshold (adjust after testing)
const int touchThreshold = 30;

void onFnTouch() {
    touchState = true;
}

void setup() {
    delay(1000);
    Serial.begin(115200);

    // Init encoder
    encoder.attachHalfQuad(DT, CLK);
    encoder.setCount(0);
    encoder.setFilter(1023);

    pinMode(SW, INPUT_PULLUP);

    touchAttachInterrupt(FN_TOUCH_PIN, onFnTouch, touchThreshold);
    // Start BLE Keyboard
    bleKeyboard.begin();
}

void loop() {
    // Read and print touch value
    unsigned long now = millis();
    if (now - lastLoopTime < loopInterval) return;
    lastLoopTime = now;

    if (!bleKeyboard.isConnected()) return;

    long newPosition = encoder.getCount();
    bool fnState = touchState;
    if (touchState) touchState = false;

    if (newPosition != lastPosition) {
        if (newPosition > lastPosition) {
            if (fnState)
                bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
            else
                bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
        } else {
            if (fnState)
                bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
            else
                bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
        }
        lastPosition = newPosition;
    }

    int reading = digitalRead(SW);
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }
    lastButtonState = reading;

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == HIGH) {
                if (fnState)
                    bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
                else
                    bleKeyboard.write(KEY_MEDIA_MUTE);
            }
        }
    }
}
