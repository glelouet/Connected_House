#include <GWManager.h>

void GWManager::init(){
  hasEthernet=digitalRead(ETHERNET_PIN);
  if(hasEthernet) {
      client.stop();
      Ethernet.select(ETHERNET_PIN);
      Ethernet.begin(ethMac);
      Serial.print(F("My IP address: "));
      for (byte thisByte = 0; thisByte < 4; thisByte++) {
        // print the value of each byte of the IP address:
        Serial.print(Ethernet.localIP()[thisByte], DEC);
        Serial.print(F("."));
      }
      Serial.println();

      W5100.setRetransmissionTime(2000); //where each unit is 100us, so 0x07D0 (decimal 2000) means 200ms.
      W5100.setRetransmissionCount(3); //That gives me a 3 second timeout on a bad server connection.
  }
}

void GWManager::loop(){

}
