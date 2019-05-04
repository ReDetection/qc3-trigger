
class Button {
  byte pin;
  boolean lastState = LOW;
  boolean pressedState = HIGH;
  unsigned long int nextAllowedOn = 0;
  unsigned long int threshold;
  unsigned long int (*millisF)();
  int (*pinReadF)(uint8_t);

  public:
  Button(byte pin1, boolean pressedState1 = HIGH, unsigned long int threshold1 = 200, unsigned long int (*millisFunc)() = millis, void (*enablePinFunc)(uint8_t, uint8_t) = pinMode, int (*pinReadFunc)(uint8_t) = digitalRead) {
    pin = pin1;
    threshold = threshold1;
    millisF = millisFunc;
    pinReadF = pinReadFunc;
    pressedState = pressedState1;
    if (enablePinFunc != 0) {
      enablePinFunc(pin, pressedState == HIGH ? INPUT : INPUT_PULLUP);
    }
  }

  boolean read() {
    if (lastState == pressedState) {
      lastState = pinReadF(pin);
      return false;
    } 
        
    unsigned long long now = millisF();
    if (now > nextAllowedOn && pinReadF(pin) == pressedState) {
      lastState = pressedState;
      nextAllowedOn = now + threshold;
      return true;
    }

    return false;
  }
};
