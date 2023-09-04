#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <Arduino.h>
#include <functional>

typedef enum MotionState { 
  None = 0,
  Detected = 1
} MotionState_t;

typedef std::function<void(MotionState_t)> MotionSensorEventCallback;

class MotionSensor
{
  public:
    MotionSensor(uint8_t pin) : pin(pin)
    {
      pinMode(pin, INPUT);
      setState(digitalRead(pin));
    }

    MotionState_t getState()
    {
      return state;
    }

    void onChanged(MotionSensorEventCallback cb)
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
          onMotionStateCallback((MotionState_t)s);
      }
    }

  private:
    uint8_t pin;
    MotionState_t state = None, lastState = None;
    MotionSensorEventCallback onMotionStateCallback = NULL;
    unsigned long lastReadValueMs = 0;

    void setState(int value) {
      log_d("Motion sensor @%d: value=%d", pin, value);

      lastState = state;
      state = value == 0 ? None : Detected;
    }
};

#endif