#ifndef MENUSTATE_H
#define MENUSTATE_H
#include "State.h"
#include "StateMachine.h"
#include <Arduino.h>

enum MenuItem {
    COUNT_UP,
    COUNT_DOWN,
    RESET_TIMER
};

class MenuState : public State {
   private:
    StateMachine* stateMachine;
    MenuItem selectedItem;

   public:
    MenuState(StateMachine* sm);

    void enter() override;

    void update(unsigned int currentMillis) override;

    void exit() override;

    void onEncoderTurn(int delta) override;

    void onButtonClick() override;

    void onButtonHold() override;
};

#endif  // MENUSTATE_H