#include "Room.h"
#include <BH1750.h>

Room::Room(int redPin, int greenPin, BH1750 &lightMeter, int id)
  : redLight(redPin), greenLight(greenPin), lightMeter(lightMeter), id(id) {}

float Room::getLux() {
  return this->lightMeter.readLightLevel();
}
void Room::loop() {
  if (this->isSafe()) {
    this->redLight.off();
    this->greenLight.on();
  } else {
    this->greenLight.off();
    this->getLux() < 0 ? this->redLight.wink() : this->redLight.on();
  }
}

bool Room::isBHerror() {
  return this->BHError;
}

void Room::setBHerror(bool value) {
  this->BHError = value;
}
bool Room::isSafe() {

  return this->getLux() > 10;
}
String Room::toJson() {
  float lux = this->getLux();
  bool error = lux < 0;
  
  return String("{\"is_safe\":") + (this->isSafe() ? "true" : "false") + 
         ",\"room_id\":" + String(this->id) + 
         ",\"error\":" + (error ? "true" : "false") + "}";
}

