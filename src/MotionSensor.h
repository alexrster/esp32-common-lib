#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <Arduino.h>
#include <functional>

enum class MotionState : uint8_t { 
  None = 0,
  Detected
};

class MotionSensor
{
  public:
    typedef std::function<void(MotionSensor*, MotionState)> MotionEventCallback;

    MotionSensor(uint8_t pin) : pin(pin)
    {
      pinMode(pin, INPUT);
      setState(digitalRead(pin));
    }

    MotionState getState()
    {
      return state;
    }

    void onChanged(MotionEventCallback cb)
    {
      onMotionStateCallback = cb;
    }

    void loop(unsigned long now) {
      if (now - lastReadValueMs < 1000) return;

      lastReadValueMs = now;
      int s = digitalRead(pin);
      if (s != (int)state) {
        setState(s);

        if (onMotionStateCallback != NULL)
          onMotionStateCallback(this, state);
      }
    }

  private:
    uint8_t pin;
    MotionState state = MotionState::None, lastState = MotionState::None;
    MotionEventCallback onMotionStateCallback = NULL;
    unsigned long lastReadValueMs = 0;

    void setState(int value) {
      log_d("Motion sensor @%d: value=%d", pin, value);

      lastState = state;
      state = value == 0 ? MotionState::None : MotionState::Detected;
    }
};

#endif