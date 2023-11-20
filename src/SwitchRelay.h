#ifndef SWITCH_RELAY_H
#define SWITCH_RELAY_H

#include <Arduino.h>

#define DIGITAL_READ_COUNT          10
#define DIGITAL_READ_THRESHOLD      DIGITAL_READ_COUNT / 2
#define DIGITAL_READ_DELAY_MS       2

enum class SwitchState { 
  Off,
  On
};

class SwitchRelay {
  public:
    void setOn() {
      setState(SwitchState::On);
    }

    void setOff() {
      setState(SwitchState::Off);
    }

    virtual SwitchState getState();
    virtual void setState(SwitchState targetState);
};

class SwitchRelayPin : public SwitchRelay {
  public:
    SwitchRelayPin(uint8_t pin)
      : SwitchRelayPin(pin, OUTPUT)
    { }

    SwitchRelayPin(uint8_t pin, uint8_t pinModeType)
      : SwitchRelayPin(pin, HIGH, LOW, pinModeType)
    { }

    SwitchRelayPin(uint8_t pin, SwitchState state, uint8_t pinModeType)
      : SwitchRelayPin(pin, HIGH, LOW, pinModeType)
    { 
      setState(state);
    }

    SwitchRelayPin(uint8_t pin, uint8_t onValue, uint8_t offValue, uint8_t pinModeType)
      : pin(pin), onValue(onValue), offValue(offValue)
    { 
      pinMode(pin, 2);
      state = _digitalRead(pin);

      pinMode(pin, pinModeType);
    }

    virtual SwitchState getState() {
      return state;
    }

    virtual void setState(SwitchState targetState) {
      digitalWrite(pin, targetState == SwitchState::On ? onValue : offValue);
      state = targetState;
    }
  
  private:
    const uint8_t pin, onValue, offValue;
    SwitchState state = SwitchState::Off;

    SwitchState _digitalRead(uint8_t pin)
    {
      uint8_t val = 0;
      for (uint8_t i=0; i<DIGITAL_READ_COUNT; i++) {
        if (digitalRead(pin) > 0) val++;
        delay(DIGITAL_READ_DELAY_MS);
      }

      if (onValue > 0)
        return val >= DIGITAL_READ_THRESHOLD ? SwitchState::On : SwitchState::Off;
      else 
        return val < DIGITAL_READ_THRESHOLD ? SwitchState::On : SwitchState::Off;
    }
};

class SwitchRelayMock : public SwitchRelay {
  public:
    SwitchRelayMock(SwitchState state = SwitchState::Off) : state(state) 
    { }

    virtual SwitchState getState() {
      return state;
    }

    virtual void setState(SwitchState targetState)
    { }

  private:
    SwitchState state = SwitchState::Off;
};

#endif