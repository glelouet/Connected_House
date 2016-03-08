#include <EthShield.h>

void EthShield::init(){
  hasEthernet=digitalRead(ETHERNET_PIN);
  if(hasEthernet) {
      ethc.stop();

      delay(4000);
      // Change the PIN used for the ethernet
      Ethernet.select(ETHERNET_PIN);


      // Connect to network amd obtain an IP address using DHCP
      if (Ethernet.begin(netparams.ethMac) == 0)
      {
        if(debug(DEBUG_WARN)) Serial.println(F("DHCP Failed, reset Arduino to try again"));
      }
      else
      {
        if(debug(DEBUG_INFO)) Serial.println(F("Arduino connected to network using DHCP"));
      }

      if(debug(DEBUG_INFO)) {
        Serial.print(F("My IP address: "));
        for (byte thisByte = 0; thisByte < 4; thisByte++) {
          // print the value of each byte of the IP address:
          Serial.print(Ethernet.localIP()[thisByte], DEC);
          Serial.print(F("."));
        }
        Serial.println();
      }
      W5100.setRetransmissionTime(0x07D0); //where each unit is 100us, so 0x07D0 (decimal 2000) means 200ms.
      W5100.setRetransmissionCount(3); //That gives me a 3 second timeout on a bad server connection.

      delay(100);
  }
}

void EthShield::loop(){

}
