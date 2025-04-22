#include "LedLib.h"
#include <Arduino.h>

LedLib::LedLib(int gpio) {
  this->gpio = gpio;
  pinMode(this->gpio, OUTPUT);
  this->off();
  this->state = false;
}

void LedLib::wink(unsigned long delayMs) {
  unsigned long currentMillis = millis();
  if (currentMillis - this->lastToggleTime >= delayMs) {
    this->lastToggleTime = currentMillis;
    this->state = !this->state;
    digitalWrite(this->gpio, this->state ? HIGH : LOW);
  }
}
bool LedLib::isOn() {

  return this->state;
}
void LedLib::on() {
  digitalWrite(this->gpio, HIGH);
  this->state = true;
}

void LedLib::off() {
  digitalWrite(this->gpio, LOW);
  this->state = false;
}
