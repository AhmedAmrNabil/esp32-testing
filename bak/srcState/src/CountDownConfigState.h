#ifndef COUNTDOWNCONFIGSTATE_H
#define COUNTDOWNCONFIGSTATE_H
#include "State.h"
#include "StateMachine.h"
#include <Arduino.h>

class CountDownConfigState : public State {
   private:
    StateMachine* stateMachine;
    int countdownTime;  // Countdown time in seconds
    unsigned int currentMillis = 0;

   public:
    CountDownConfigState(StateMachine* sm);

    void enter() override;

    void update(unsigned int currentMillis) override;

    void exit() override;

    void onEncoderTurn(int delta) override;

    void onButtonClick() override;

    void onButtonHold() override;
};

#endif  // COUNTDOWNCONFIGSTATE_H