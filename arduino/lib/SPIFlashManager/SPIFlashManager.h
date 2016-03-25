#ifndef SPIFlashManager_H
#define SPIFlashManager_h

class SPIFlashManager;

#include <Moteino.h>
#include <SPIFlash.h>

class SPIFlashManager {

private:
  uint32_t flashId = 0;

  #ifdef __AVR_ATmega1284P__
    byte FLASH_PIN = 23; // FLASH SS on D23
  #else
    byte FLASH_PIN = 8; // FLASH SS on D8
  #endif
    // self flasher
    SPIFlash flash;

public:
  // init the flash part
  void init();

  void loop();

  uint32_t getId() { return flashId; }
}

#endif
