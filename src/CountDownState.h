#ifndef COUNTDOWNSTATE_H
#define COUNTDOWNSTATE_H
#include "State.h"
#include "StateMachine.h"



class CountDownState : public State {
   private:
    StateMachine* stateMachine;
    int countdownTime;  // Countdown time in seconds
    unsigned int lastUpdateMillis;
    int isRunning;  // Flag to indicate if the countdown is running

   public:
    CountDownState(StateMachine* sm, int time, unsigned int lastUpdatedMillis = 0);

    void enter() override;

    void update(unsigned int currentMillis) override;

    void exit() override;

    void onEncoderTurn(int delta) override;

    void onButtonClick() override;

    void onButtonHold() override;
};

#endif  // COUNTDOWNSTATE_H