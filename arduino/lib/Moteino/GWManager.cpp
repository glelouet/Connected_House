#include <GWManager.h>

void GWManager::init(Moteino *moteino, const char *url, uint16_t port){
  m=moteino;
  m_url=url;
  m_port=port;
  hasEthernet=digitalRead(ETHERNET_PIN);
  if(hasEthernet) {
    client.stop();
    Ethernet.select(ETHERNET_PIN);
    Serial.println("acquiring DHCP, this may take some time");
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
  if(m->radio.hasRcv()
      && strncmp( ((char *) m->radio.getData()), "SEND", strlen("SEND"))==0) {
    char * transmit = ((char *)m->radio.getData())+strlen("SEND");
    char id[20], probeID[4], value[30];
    token(transmit, id, 0, 20);
    token(transmit, probeID, 1, 4);
    token(transmit, value, 2, 30);
    sendProbeData(id, probeID, value);
  }
}

void GWManager::sendProbeData(char *id, char *probeID, char *value) {
  client.stop();
  if(client.connect(m_url, m_port)) {
    client.print("GET /test.php?id=");
    client.print(id);
    client.print("&pb=");
    client.print(probeID);
    client.print("&val=");
    client.print(value);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(m_url);
    client.println();
    client.flush();
  } else {
    Serial.print(F("could not connect to server "));
    Serial.println(m_url);
  }
}

// Update ThingSpeak with the date on the right chanel
void GWManager::updateThingSpeak(char* tsData, char *chanel) {
  if (client.connect("kgb.emn.fr", 8001)) {
    // Sending the data with a POST method
    client.println(F("POST /update HTTP/1.1"));
    client.print(F("Host: api.thingspeak.com\n"));
//    client.print(F("Host: kgb.emn.fr\n"));
    client.print(F("Connection: close\n"));
    client.print(F("X-THINGSPEAKAPIKEY: "));
    client.print(chanel);
    client.print(F("\n"));
    client.print(F("Content-Type: application/x-www-form-urlencoded\n"));
    client.print(F("Content-Length: "));
    client.print(strlen(tsData));
    client.print("\n\n");
    client.print(tsData);
    client.print("\n\n");

    client.flush();
  }
  else Serial.println("PB!!! Conex");

  client.stop();

  m->ledBlink(500);

  digitalWrite(4, HIGH);
}
