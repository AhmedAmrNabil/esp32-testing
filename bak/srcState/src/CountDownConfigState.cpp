#include <Arduino.h>
#include <TFT_eSPI.h>

#include "StateMachine.h"
#include "MenuState.h"
#include "State.h"
#include "MenuState.h"
#include "CountDownConfigState.h"
#include "CountDownState.h"
#include "image.h"
#include "defs.h"

extern TFT_eSPI tft;        // TFT display object
extern TFT_eSprite sprite;  // Sprite for double buffering

CountDownConfigState::CountDownConfigState(StateMachine* sm) : State(sm), stateMachine(sm), countdownTime(10) {
    // Constructor initializes the countdown time to a default value, e.g., 10 seconds
    Serial.println("CountDownConfigState initialized with countdown time: 10 seconds");
}

void CountDownConfigState::enter() {
    Serial.println("Entering Count Down Config State");
    countdownTime = 20;
    // Initialize countdown time or display
}

void CountDownConfigState::update(unsigned int currentMillis) {
    this->currentMillis = currentMillis;                            // Update currentMillis for this state
    sprite.pushImage(0, 0, tft.width(), tft.height(), BG_IMG_ARR);  // Display background image
    // write current option to the screen

    sprite.setTextSize(3);  // Larger size for top row
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextColor(TFT_WHITE);
    sprite.drawString("Time", tft.width() / 2, sprite.fontHeight() / 2);

    char time[10];
    sniprintf(time, sizeof(time), "%02d:00", countdownTime);
    sprite.setTextSize(5);
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextColor(TFT_WHITE);
    sprite.drawString(time, tft.width() / 2, sprite.fontHeight() * 1.6);
    sprite.pushSprite(0, 0);  // Push the sprite to the display
}

void CountDownConfigState::exit() {
    Serial.println("Exiting Count Down Config State");
}

void CountDownConfigState::onEncoderTurn(int delta) {
    countdownTime += delta;
    if (countdownTime < 1) countdownTime = 1;  // Prevent negative countdown time
    Serial.printf("Countdown time set to: %d minutes\n", countdownTime);
}

void CountDownConfigState::onButtonClick() {
    // Handle confirmation of the countdown time
    Serial.printf("Countdown time confirmed: %d minutes\n", countdownTime);
    // Transition to Count Down State with the configured time
    stateMachine->changeState(new CountDownState(stateMachine, countdownTime, currentMillis));  // Pass currentMillis to CountDownState
}

void CountDownConfigState::onButtonHold() {
    // Handle long press actions if needed
    Serial.println("Return to Menu State on button hold");
    stateMachine->changeState(new MenuState(stateMachine));  // Transition back to Menu State
}