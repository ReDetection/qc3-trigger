#include "Button.h"
#include <QC3Control.h>

typedef void (*voltageFunc)();
QC3Control quickCharge(0, 2);
short voltages[] = {5000, 12000, 16000, 20000, 9000, 3600};
#define MAXVOLTAGE (20000)
#define VOLTAGESCOUNT (6)
char currentVoltageIndex = 0;
Button next(4, LOW);
#define LED 1

unsigned long zeroPhaseTime = 0;
short currentPhase() {
  unsigned long now = millis();
  short result = (now - zeroPhaseTime) % (MAXVOLTAGE / 10);
  zeroPhaseTime = now - result;
  return result;
}

void refreshLed() {
  short phase = currentPhase();
  digitalWrite(LED, phase < voltages[currentVoltageIndex] / 10 ? HIGH : LOW);
}

void setQC3Voltage() {
    short milliVolts = voltages[currentVoltageIndex];
    quickCharge.setMilliVoltage(milliVolts);
}

void set5V() {
  quickCharge.set5V();
}

void set9V() {
  quickCharge.set9V();
}

void set12V() {
  quickCharge.set12V();
}

voltageFunc functions[VOLTAGESCOUNT] = {set5V, set12V, setQC3Voltage, setQC3Voltage, set9V, setQC3Voltage};
void setup() {
  pinMode(LED, OUTPUT);
  quickCharge.begin(true);
}

void loop() {
  refreshLed();
  if (next.read()) {
    currentVoltageIndex = (currentVoltageIndex + 1) % VOLTAGESCOUNT;
    functions[currentVoltageIndex]();
    zeroPhaseTime = millis();
  }
}
