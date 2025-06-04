#include <Arduino.h>
#include <TFT_eSPI.h>
#include <ezBuzzer.h>

#include "StateMachine.h"
#include "State.h"
#include "CountDownState.h"
#include "MenuState.h"
#include "image.h"
#include "defs.h"

extern TFT_eSPI tft;        // TFT display object
extern TFT_eSprite sprite;  // Sprite for double buffering
extern ezBuzzer buzzer;     // Buzzer object

int melody[] = {
    1,
    0,
    1,
};

int noteDurations[] = {
    10, 10, 10};

CountDownState::CountDownState(StateMachine* sm, int time, unsigned int currentMillis) : State(sm), stateMachine(sm) {
    // Constructor initializes the countdown time
    countdownTime = time * 60;               // Convert minutes to seconds
    this->lastUpdateMillis = currentMillis;  // Store the last update time
    Serial.printf("CountDownState initialized with countdown time: %d seconds\n", countdownTime);
    isRunning = 1;  // Set the countdown as running
}

void CountDownState::enter() {
    Serial.println("Entering Count Down State");
}

void CountDownState::update(unsigned int currentMillis) {
    if (currentMillis - lastUpdateMillis >= 1000) {  // Update every second
        lastUpdateMillis = currentMillis;            // Update the last update time
        if (isRunning) {
            if (countdownTime > 0) {
                countdownTime--;  // Decrement countdown time
            } else {
                Serial.println("Countdown finished!");
                // buzzer.beep(1000);
                buzzer.playMelody(melody, noteDurations, 3);             // Beep for 1 second when countdown finishes
                stateMachine->changeState(new MenuState(stateMachine));  // Return to Menu State
            }
        }
    }

    // sprite.pushImage(0, 0, tft.width(), tft.height(), BG_IMG_ARR);  // Display background image
    sprite.fillSprite(BG_CLR);  // Clear the sprite with background color

    String title = "Count Down";
    if (!isRunning) {
        title = "Paused";
    }
    sprite.setTextSize(3);  // Larger size for top row
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextColor(TITLE_CLR);
    sprite.drawString(title, tft.width() / 2, sprite.fontHeight() / 2);

    char time[10];
    snprintf(time, sizeof(time), "%02d:%02d", countdownTime / 60, countdownTime % 60);
    sprite.setTextSize(5);
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextColor(MAIN_CLR);
    sprite.drawString(time, tft.width() / 2, sprite.fontHeight() * 1.6);

    sprite.pushSprite(0, 0);  // Push the sprite to the display
}

void CountDownState::exit() {
    Serial.println("Exiting Count Down State");
}

void CountDownState::onEncoderTurn(int delta) {
}

void CountDownState::onButtonClick() {
    // pause functionality
    isRunning = !isRunning;  // Toggle the running state
    Serial.printf("Countdown %s\n", isRunning ? "resumed" : "paused");
}

void CountDownState::onButtonHold() {
    // Return to Menu State on button hold
    Serial.println("Return to Menu State on button hold");
    stateMachine->changeState(new MenuState(stateMachine));
}
