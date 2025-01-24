#include <string>

#ifndef SENSORSAVESTATE_H
#define SENSORSAVESTATE_H

enum class SensorState {
  UNKNOWN,
  ON,
  MID,
  OFF
};

const char* sensorStateToString(SensorState state);

class SensorSaveState {
  public:

    static const SensorSaveState& Unknown() {
      static SensorSaveState unknownInstance(-1.0f, SensorState::UNKNOWN);
      return unknownInstance;
    }

    SensorSaveState(float val, SensorState s) : value(val), state(s) {}

    const char* stateToString() const;

    static SensorState stringToState(const std::string& str);

    SensorState getState() const {
      return state;
    }

    float getValue() const {
      return value;
    }

  private:
    float value;
    SensorState state;
};

#endif //SENSORSAVESTATE_H
