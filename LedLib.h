#ifndef LED_LIB_H
#define LED_LIB_H

class LedLib {
public:
  LedLib(int gpio);
  void wink(unsigned long delayMs = 500);
  void off();
  void on();
  bool isOn();

private:
  unsigned long lastToggleTime = 0;
  int gpio;
  bool state;
};

#endif
