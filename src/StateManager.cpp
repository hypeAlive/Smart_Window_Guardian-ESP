#include "StateManager.h"

const char* stateToString(State state) {
    switch (state) {
        case State::NOT_INITIALIZED: return "NOT_INITIALIZED";
        case State::INITIALIZED: return "INITIALIZED";
        case State::SETUP: return "SETUP";
        default: return "UNKNOWN_STATE";
    }
}

