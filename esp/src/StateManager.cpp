#include "StateManager.h"

#include <algorithm>

const char* stateToString(State state) {
    switch (state) {
        case State::NOT_INITIALIZED: return "NOT_INITIALIZED";
        case State::INITIALIZED: return "INITIALIZED";
        case State::SETUP: return "SETUP";
        default: return "UNKNOWN_STATE";
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

    return true;
}
