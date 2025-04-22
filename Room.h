#ifndef ROOM_H
#define ROOM_H

#include "LedLib.h"
#include <BH1750.h>


class Room {
public:
  Room(int redPin, int greenPin, BH1750 &lightMeter, int id);
  float getLux();
  LedLib redLight;
  LedLib greenLight;
  void loop();
  bool isSafe();
  String toJson();
  bool isBHerror();
  void setBHerror(bool value);

private:
  int id;
  BH1750 &lightMeter;
  bool BHError = false;
};

#endif
