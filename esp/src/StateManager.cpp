#include "StateManager.h"

#include <algorithm>
#include <unordered_set>

const char* stateToString(State state) {
    switch (state) {
        case State::NOT_INITIALIZED: return "NOT_INITIALIZED";
        case State::INITIALIZED: return "INITIALIZED";
        case State::SETUP: return "SETUP";
        default: return "UNKNOWN_STATE";
    }
}

void StateManager::calculateMiddlePoints() {
    std::lock_guard<std::mutex> lock(saveStateMutex);

    std::sort(sensorSaveStates.begin(), sensorSaveStates.end(), [](const SensorSaveState& a, const SensorSaveState& b) {
        return a.getValue() < b.getValue();
    });

    middlePoints.clear();

    for (size_t i = 0; i < sensorSaveStates.size() - 1; ++i) {
        float distance1 = sensorSaveStates[i].getValue();
        float distance2 = sensorSaveStates[i + 1].getValue();
        middlePoints.push_back((distance1 + distance2) / 2.0f);
    }
}

bool StateManager::setup(SensorState state, uint32_t duration) {
    if (state == SensorState::UNKNOWN) {
        logw("Cannot setup SensorState: UNKNOWN.");
        return false;
    }

    auto it = std::ranges::find_if(sensorSaveStates,
                                   [state](const SensorSaveState& savedState) {
                                       return savedState.getState() == state;
                                   });

    if (it != sensorSaveStates.end()) {
        logw("SaveState bereits vorhanden. Kein neues Setup erforderlich.");
        return false;
    }

    if (!sensor) {
        loge("Sensor ist nicht initialisiert. Setup nicht möglich.");
        return false;
    }

    logi("Starte Sensordaten-Aufnahme für %d Sekunden...", duration);

    float medianDistance = sensor->measureMedianDistance(duration);

    if (medianDistance < 0) {
        loge("Ungültige Sensordaten. Setup fehlgeschlagen.");
        return false;
    }

    SensorSaveState newSaveState(medianDistance, state);
    sensorSaveStates.push_back(newSaveState);

    logi("Neuer SaveState erfolgreich erstellt mit Wert: %.2f.", medianDistance);

    if (checkSetupNeedsEmpty()) {
        setState(State::SETUP);
        startSaveStateEvalLoop();
    }

    return true;
}

void StateManager::startSaveStateEvalLoop() {
    calculateMiddlePoints();

    if (taskHandle) {
        logw("Task is already running.");
        return;
    }

    xTaskCreate(
            &StateManager::evaluationTaskWrapper,
            "SaveStateEvaluation",
            4096,
            this,
            1,
            &taskHandle
        );

    logi("SaveState evaluation task started.");
}

void StateManager::deleteSetup() {
    logi("Resetting StateManager...");

    if (taskHandle) {
        logi("Deleting evaluation task...");
        vTaskDelete(taskHandle);
        taskHandle = nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(saveStateMutex);
        sensorSaveStates.clear();
        middlePoints.clear();
        currentSaveState = nullptr;
    }

    currentState = State::INITIALIZED;

    logi("StateManager reset complete.");
}

void StateManager::evaluationTaskWrapper(void* params) {
    StateManager* instance = static_cast<StateManager*>(params);
    instance->evaluationTask();
}

void StateManager::evaluationTask() {
    while (true) {
        if (!sensor) {
            loge("Sensor not initialized!");
            vTaskDelete(nullptr);
            return;
        }

        float currentDistance = sensor->measureDistance();

        {
            std::lock_guard<std::mutex> lock(saveStateMutex);

            for (size_t i = 0; i < sensorSaveStates.size(); ++i) {
                if (i == 0 && currentDistance <= middlePoints[i]) {
                    // Wenn Wert kleiner oder gleich dem ersten Mittelwert ist, wähle den ersten Zustand
                    currentSaveState = &sensorSaveStates[i];
                    break;
                } else if (i == sensorSaveStates.size() - 1 && currentDistance > middlePoints[i - 1]) {
                    // Wenn Wert größer ist als der letzte Mittelwert, wähle den letzten Zustand
                    currentSaveState = &sensorSaveStates[i];
                    break;
                } else if (currentDistance > middlePoints[i - 1] && currentDistance <= middlePoints[i]) {
                    // Wenn Wert zwischen zwei Mittelpunkten liegt, wähle den passenden Zustand
                    currentSaveState = &sensorSaveStates[i];
                    break;
                }
            }
        }

        logi("Updated current save state.");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



std::unordered_set<SensorState> StateManager::getSetupNeeds() const {
    std::unordered_set<SensorState> requiredStates = {
        SensorState::ON,
        SensorState::OFF,
        SensorState::MID
    };

    for (const auto& saveState : sensorSaveStates) {
        requiredStates.erase(saveState.getState());
    }

    return requiredStates;
}
