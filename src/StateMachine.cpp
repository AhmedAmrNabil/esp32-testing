#include "StateMachine.h"
#include "State.h"
StateMachine::StateMachine(State* initialState) : currentState(initialState) {
    currentState->enter();
}
StateMachine::StateMachine() {
    currentState = nullptr;  // Initialize with no state
}

void StateMachine::changeState(State* newState) {
    if (currentState) {
        currentState->exit();  // Exit the current state before changing
        delete currentState;   // Clean up the old state
    }
    currentState = newState;
    currentState->enter();
}

void StateMachine::update(unsigned int currentMillis) {
    if (currentState) {
        currentState->update(currentMillis);
    }
}

void StateMachine::onEncoderTurn(int delta) {
    if (currentState) {
        currentState->onEncoderTurn(delta);
    }
}

void StateMachine::onButtonClick() {
    if (currentState) {
        currentState->onButtonClick();
    }
}

void StateMachine::onButtonHold() {
    if (currentState) {
        currentState->onButtonHold();
    }
}

StateMachine::~StateMachine() {
    delete currentState;  // Clean up the current state
}