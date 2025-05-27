#ifndef STATE_H
#define STATE_H

class StateMachine;
class State {
   protected:
    StateMachine* stateMachine;

    // Constructor is protected to prevent direct instantiation
    State(StateMachine* sm) : stateMachine(sm) {}

   public:
    virtual ~State() = default;
    virtual void enter() = 0;
    virtual void update(unsigned int currentMillis) = 0;
    virtual void exit() = 0;

    // Input events
    virtual void onEncoderTurn(int delta) = 0;  // +1 or -1
    virtual void onButtonClick() = 0;
    virtual void onButtonHold() = 0;
};

#endif  // STATE_H