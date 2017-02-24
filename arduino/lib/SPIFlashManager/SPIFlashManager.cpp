#include "SPIFlashManager.h"

SPIFlashManager::SPIFlashManager()
  :flash(FLASH_PIN, 0xEF30) // 0xEF30 for windbond 4mbit flash
  {}

void SPIFlashManager::init(){
  if (flash.initialize()) {
    uint8_t *uniq_id = flash.readUniqueId();
    flashId = 0;
    for (byte i = 4; i < 8; i++) {
      flashId = flashId << 8 | uniq_id[i];
    }
  } else
    Serial.println(F("SPI Flash Init FAIL!"));
}

void SPIFlashManager::loop(){

}
