#include <Arduino.h>
#include <ESP32Encoder.h>
#include <BleGamepad.h>

#define ENC_CLK 18
#define ENC_DT 19
#define ENC_SW 21

ESP32Encoder encoder;
BleGamepad gamepad;

long lastPos = 0;
int joyX = 16384;  // fake joystick x-axis (-127 to 127)

void setup() {
    Serial.begin(115200);
    encoder.attachHalfQuad(ENC_DT, ENC_CLK);
    encoder.setCount(0);
    encoder.setFilter(1023);
    pinMode(ENC_SW, INPUT_PULLDOWN);

    gamepad.begin();
}

void loop() {
    if (gamepad.isConnected()) {
        long newPos = encoder.getCount();

        // Convert encoder delta to X movement
        if (newPos != lastPos) {
            int delta = newPos - lastPos;
            lastPos = newPos;

            joyX += delta * 2048;  // Adjust sensitivity here
            joyX = constrain(joyX, 0, 32767);
            gamepad.setLeftThumb(joyX, 16384);  // X only
        }

        // Button press
        static bool lastBtn = LOW;
        bool btn = digitalRead(ENC_SW);
        if (btn != lastBtn) {
            lastBtn = btn;
            if (btn == HIGH) {
                gamepad.press(BUTTON_1);
            } else {
                gamepad.release(BUTTON_1);
            }
        }
    }

    delay(10);
}
