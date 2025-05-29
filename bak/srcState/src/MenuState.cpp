#include <Arduino.h>
#include <TFT_eSPI.h>

#include "StateMachine.h"
#include "MenuState.h"
#include "State.h"
#include "CountDownConfigState.h"
#include "image.h"
#include "defs.h"

extern TFT_eSPI tft;        // TFT display object
extern TFT_eSprite sprite;  // Sprite for double buffering
String menuOptions[3] = {"Up", "Down", "Reset"};

MenuState::MenuState(StateMachine* sm) : State(sm), stateMachine(sm), selectedItem(COUNT_UP) {}

void MenuState::enter() {
    // Initialize menu items or display
    Serial.println("Entering Menu State");
    // Display menu items on the screen
}

void MenuState::update(unsigned int currentMillis) {
    // Update menu display or handle user input
    // Serial.println("Updating Menu State");
    // Serial.printf("Menu item selected: %d\n", selectedItem);
    sprite.pushImage(0, 0, tft.width(), tft.height(), BG_IMG_ARR);  // Display background image
    // write current option to the screen

    sprite.setTextSize(3);  // Larger size for top row
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextColor(TFT_WHITE);
    sprite.drawString("Main Menu", tft.width() / 2, sprite.fontHeight() / 2);

    sprite.setTextSize(5);
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextColor(TFT_WHITE);
    sprite.drawString(menuOptions[selectedItem], tft.width() / 2, sprite.fontHeight() * 1.6);
    sprite.pushSprite(0, 0);  // Push the sprite to the display
}

void MenuState::exit() {
    // Clean up or save state if necessary
    Serial.println("Exiting Menu State");
}

void MenuState::onEncoderTurn(int delta) {
    selectedItem = (MenuItem)((selectedItem + 3 + delta) % 3);
    // Handle selection wrap-around if necessary
    Serial.printf("Menu item selected: %d\n", selectedItem);
}

void MenuState::onButtonClick() {
    // Handle selection confirmation
    Serial.printf("Menu item %d confirmed\n", selectedItem);
    switch (selectedItem) {
        case COUNT_UP:
            Serial.println("Starting Count Up");
            // Transition to Count Up State or start counting up logic
            // stateMachine->changeState(new CountUpState(stateMachine));
            break;
        case COUNT_DOWN:
            Serial.println("Configuring Count Down");
            // Transition to Count Down Config State
            stateMachine->changeState(new CountDownConfigState(stateMachine));
            break;
        case RESET_TIMER:
            Serial.println("Resetting Timer");
            // Reset timer logic here
            break;
    }
}

void MenuState::onButtonHold() {
    // Handle long press actions if needed
    Serial.println("Button held in Menu State");
}