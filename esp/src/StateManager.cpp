#include "StateManager.h"
#include "nvs.h"
#include "esp_task_wdt.h"

#include <algorithm>
#include <unordered_set>
#include <cstring>

void StateManager::saveSaveStateNVS() {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open("sensor_storage", NVS_READWRITE, &nvsHandle);
    if (err != ESP_OK) {
        loge("Speichern in NVS fehlgeschlagen: Fehler beim Öffnen.");
        return;
    }

    size_t index = 0;
    err = nvs_set_u32(nvsHandle, "save_count", sensorSaveStates.size());
    if (err != ESP_OK) {
        loge("Speichern der Anzahl der SaveStates fehlgeschlagen.");
        nvs_close(nvsHandle);
        return;
    }

    for (const auto &saveState: sensorSaveStates) {
        std::string keyState = "state_" + std::to_string(index);
        std::string keyValue = "value_" + std::to_string(index);

        err = nvs_set_i32(nvsHandle, keyState.c_str(), static_cast<int32_t>(saveState.getState()));
        if (err != ESP_OK) {
            loge("Speichern des Zustands fehlgeschlagen.");
            break;
        }

        uint32_t floatBits;
        float value = saveState.getValue();
        std::memcpy(&floatBits, &value, sizeof(float));
        err = nvs_set_u32(nvsHandle, keyValue.c_str(), floatBits);
        if (err != ESP_OK) {
            loge("Speichern des Float-Werts fehlgeschlagen.");
            break;
        }

        ++index;
    }

    if (index == sensorSaveStates.size()) {
        logi("Alle Sensorzustände wurden erfolgreich in NVS gespeichert.");
    }

    nvs_commit(nvsHandle);
    nvs_close(nvsHandle);
}

void StateManager::loadSaveStateNVS() {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open("sensor_storage", NVS_READONLY, &nvsHandle);
    if (err != ESP_OK) {
        logw("Keine gespeicherten Zustände in NVS gefunden.");
        return;
    }

    uint32_t count = 0;
    err = nvs_get_u32(nvsHandle, "save_count", &count);
    if (err != ESP_OK) {
        logw("Keine gespeicherte Anzahl von Zuständen in NVS gefunden.");
        nvs_close(nvsHandle);
        return;
    }

    sensorSaveStates.clear();

    for (size_t i = 0; i < count; ++i) {
        std::string keyState = "state_" + std::to_string(i);
        std::string keyValue = "value_" + std::to_string(i);

        int32_t state = 0;
        uint32_t floatBits = 0;

        err = nvs_get_i32(nvsHandle, keyState.c_str(), &state);
        if (err != ESP_OK) {
            logw("Zustand %zu konnte nicht geladen werden.", i);
            break;
        }

        err = nvs_get_u32(nvsHandle, keyValue.c_str(), &floatBits);
        if (err != ESP_OK) {
            logw("Wert %zu konnte nicht geladen werden.", i);
            break;
        }

        float value;
        std::memcpy(&value, &floatBits, sizeof(float));
        sensorSaveStates.emplace_back(value, static_cast<SensorState>(state));
    }

    if (sensorSaveStates.size() == count) {
        logi("Alle gespeicherten Zustände erfolgreich geladen.");
    }

    nvs_close(nvsHandle);
}

const char *stateToString(State state) {
    switch (state) {
        case State::NOT_INITIALIZED: return "NOT_INITIALIZED";
        case State::INITIALIZED: return "INITIALIZED";
        case State::SETUP: return "SETUP";
        default: return "UNKNOWN_STATE";
    }
}

void StateManager::deleteSaveStateNVS() {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open("sensor_storage", NVS_READWRITE, &nvsHandle);
    if (err != ESP_OK) {
        loge("Fehler beim Öffnen von NVS zum Löschen.");
        return;
    }

    esp_err_t deleteErr = nvs_erase_all(nvsHandle);
    if (deleteErr != ESP_OK) {
        loge("Fehler beim Löschen von NVS-Daten: %d", deleteErr);
    } else {
        logi("Alle gespeicherten Sensordaten erfolgreich gelöscht.");
    }

    nvs_commit(nvsHandle);
    nvs_close(nvsHandle);

    sensorSaveStates.clear();
}


void StateManager::calculateMiddlePoints() {
    std::lock_guard<std::mutex> lock(saveStateMutex);

    std::sort(sensorSaveStates.begin(), sensorSaveStates.end(), [](const SensorSaveState &a, const SensorSaveState &b) {
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
                                   [state](const SensorSaveState &savedState) {
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

    saveSaveStateNVS();

    checkForStartLoop();

    return true;
}

void StateManager::checkForStartLoop() {
    if (checkSetupNeedsEmpty()) {
        setState(State::SETUP);
        startSaveStateEvalLoop();
    }
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
        2,
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
    } {
        std::lock_guard<std::mutex> lock(saveStateMutex);
        sensorSaveStates.clear();
        middlePoints.clear();
        currentSaveState = nullptr;
    }

    logi("Deleting NVS sensors.");
    deleteSaveStateNVS();

    currentState = State::INITIALIZED;

    logi("StateManager reset complete.");
}

void StateManager::evaluationTaskWrapper(void *params) {
    StateManager *instance = static_cast<StateManager *>(params);

    while (true) {
        instance->evaluationTask();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void StateManager::evaluationTask() {
    if (!sensor) {
        loge("Sensor not initialized!");
        vTaskDelete(nullptr);
        return;
    }

    float currentDistance = sensor->measureDistance();

    SensorSaveState* newSaveState = calculateNewSaveState(currentDistance);

    if (newSaveState && newSaveState != currentSaveState) {
        {
            std::lock_guard<std::mutex> lock(saveStateMutex);
            currentSaveState = newSaveState;
        }

        logi("Updated current save state. %s", sensorStateToString(currentSaveState->getState()));
    }

}

SensorSaveState* StateManager::calculateNewSaveState(float currentDistance) {
    const auto it = std::ranges::lower_bound(middlePoints, currentDistance);

    size_t index = std::distance(middlePoints.begin(), it);

    if (index == 0) {
        return &sensorSaveStates[0];
    }

    if (index == middlePoints.size()) {
        return &sensorSaveStates[sensorSaveStates.size() - 1];
    }

    if (currentDistance > middlePoints[index - 1] && currentDistance <= middlePoints[index]) {
        return &sensorSaveStates[index];
    }

    return nullptr;
}


std::unordered_set<SensorState> StateManager::getSetupNeeds() const {
    std::unordered_set<SensorState> requiredStates = {
        SensorState::ON,
        SensorState::OFF,
        SensorState::MID
    };

    for (const auto &saveState: sensorSaveStates) {
        requiredStates.erase(saveState.getState());
    }

    return requiredStates;
}
