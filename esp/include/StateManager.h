#include <vector>

#include "Logger.h"
#include "sensor/UltrasonicSensor.h"
#include "sensor/SensorSaveState.h"

#ifndef STATEMANAGER_H
#define STATEMANAGER_H

enum class State {
    NOT_INITIALIZED,
    INITIALIZED,
    SETUP
};

const char* stateToString(State state);

class StateManager : protected Logger {
public:
    static StateManager& getInstance() {
        static StateManager instance;
        return instance;
    }

    bool init(UltrasonicSensor* ultrasonicSensor) {
        sensor = ultrasonicSensor;
        setState(State::INITIALIZED);
        logi("Initialized successfully.");
        return true;
    }

    State getCurrentState() const {
        return currentState;
    }

    const SensorSaveState* getCurrentSaveState() const {
        return currentSaveState;
    }

    bool setup(SensorState state, uint32_t duration);

private:

    StateManager()
      : Logger("StateManager"),
        sensor(nullptr),
        currentState(State::NOT_INITIALIZED),
        currentSaveState(nullptr){}

    UltrasonicSensor* sensor;
    State currentState;
    std::vector<SensorSaveState> sensorSaveStates;
    SensorSaveState* currentSaveState;

    void setState(State newState) {
        currentState = newState;
    }

};

#endif //STATEMANAGER_H
