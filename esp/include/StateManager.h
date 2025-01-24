#include <atomic>
#include <thread>
#include <unordered_set>
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"

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
        loadSaveStateNVS();
        checkForStartLoop();
        return true;
    }

    bool checkSetupNeedsEmpty() const {
        return getSetupNeeds().empty();
    }

    std::unordered_set<SensorState> getSetupNeeds() const;

    State getCurrentState() const {
        return currentState;
    }

    const SensorSaveState* getCurrentSaveState() const {
        return currentSaveState;
    }

    bool setup(SensorState state, uint32_t duration);

    void startSaveStateEvalLoop();

    void deleteSetup();

private:

    StateManager()
      : Logger("StateManager"),
        sensor(nullptr),
        currentState(State::NOT_INITIALIZED),
        currentSaveState(nullptr){}

    void calculateMiddlePoints();
    void evaluationTask();
    static void evaluationTaskWrapper(void* params);
    void deleteSaveStateNVS();
    void saveSaveStateNVS();
    void loadSaveStateNVS();
    void checkForStartLoop();

    UltrasonicSensor* sensor;
    State currentState;
    std::vector<SensorSaveState> sensorSaveStates;
    SensorSaveState* currentSaveState;

    std::vector<float> middlePoints;
    TaskHandle_t taskHandle;
    std::mutex saveStateMutex;


    void setState(State newState) {
        currentState = newState;
    }

};

#endif //STATEMANAGER_H
