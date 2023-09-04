#ifndef SWITCH_RELAY_H
#define SWITCH_RELAY_H

#include <Arduino.h>

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
      : SwitchRelayPin(pin, SwitchState::Off, HIGH, LOW, pinModeType)
    { }

    SwitchRelayPin(uint8_t pin, SwitchState state, uint8_t pinModeType)
      : SwitchRelayPin(pin, state, HIGH, LOW, pinModeType)
    { }

    SwitchRelayPin(uint8_t pin, uint8_t onValue, uint8_t offValue, uint8_t pinModeType)
      : SwitchRelayPin(pin, SwitchState::Off, onValue, offValue, pinModeType)
    { }

    SwitchRelayPin(uint8_t pin, SwitchState state, uint8_t onValue, uint8_t offValue, uint8_t pinModeType)
      : pin(pin), onValue(onValue), offValue(offValue)
    { 
      pinMode(pin, pinModeType);
      setState(state);
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