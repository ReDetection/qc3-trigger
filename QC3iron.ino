#include "Button.h"
#include <QC3Control.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

QC3Control quickCharge(2, 0);
short voltages[] = {5000, 9000, 12000, 16000, 20000, 3600};
#define MAXVOLTAGE (20000)
#define VOLTAGESCOUNT (6)
char currentVoltageIndex = 0;
Button next(4, LOW);
#define LED 1

volatile char stillSleep;
ISR(PCINT0_vect) {
  stillSleep = 0;
}

void sleepIntervals(int times, uint8_t wdtEach) {
  stillSleep = 1;
  wdt_enable(wdtEach);
  for(int delaysLeft = times; delaysLeft > 0 && stillSleep; delaysLeft--) {
    ADCSRA &= ~_BV(ADEN);
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();  
    WDTCR |= _BV(WDIE);
    GIMSK = _BV(PCIE);
    PCMSK = _BV(PCINT4);
    sei();
    sleep_cpu();
    wdt_reset();
    cli();
  }
  wdt_disable();
}

void blink(char times, short intervalHi, short intervalLo) {
  for(;times>0;times--) {
    digitalWrite(LED, HIGH);
    sleepIntervals(intervalHi/15, WDTO_15MS);
    if (stillSleep == 0) return;
    digitalWrite(LED, LOW);
    sleepIntervals(intervalLo/15, WDTO_15MS);
    if (stillSleep == 0) return;
  }
}

void blinkVoltage(short milliVolts) {
  blink(1, milliVolts / 10, (MAXVOLTAGE - milliVolts) / 10);
}

void setup() {
  wdt_disable();
  pinMode(LED, OUTPUT);
  quickCharge.begin();
  blink(3, 100, 100);
}

void loop() {
  short milliVolts = voltages[currentVoltageIndex];
  quickCharge.setMilliVoltage(milliVolts);
  blinkVoltage(milliVolts);
  if (next.read()) {
    currentVoltageIndex = (currentVoltageIndex + 1) % VOLTAGESCOUNT;
  }
}
