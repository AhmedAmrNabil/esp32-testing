#ifndef STATEMACHINE_H
#define STATEMACHINE_H

class State;
class StateMachine {
   private:
    State* currentState;

   public:
    StateMachine(State* initialState);
    StateMachine();

    void changeState(State* newState);

    void update(unsigned int currentMillis);

    void onEncoderTurn(int delta);

    void onButtonClick();

    void onButtonHold();

    ~StateMachine();
};

#endif  // STATEMACHINE_H