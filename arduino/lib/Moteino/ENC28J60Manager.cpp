#include <ENC28J60Manager.h>

void ENC28J60Manager::init(Moteino *moteino, const char *url, uint16_t port) {
  m = moteino;
  m_url = url;
  m_port = port;
  client.stop();
  long time = millis();
  if (time < start_delay) {
    delay(start_delay - time);
  }
  Serial.println(F("acquiring DHCP, this may take some time"));
  Ethernet.begin(ethMac);
  Serial.print(F("My IP address: "));
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], HEX);
    if (thisByte != 4)
      Serial.print(F("."));
  }
  Serial.println();
}

void ENC28J60Manager::loop() {
  // Serial.println(F("starting ENC28J60Manager loop"));
  boolean activity = false;
  if (m->radio.hasRcv() &&
      strncmp(((char *)m->radio.getData()), "SEND", strlen("SEND")) == 0) {
    activity = true;
    char *transmit = ((char *)m->radio.getData()) + strlen("SEND");
    char id[20], probeID[4], value[30];
    token(transmit, id, 0, 20);
    token(transmit, probeID, 1, 4);
    token(transmit, value, 2, 30);
    sendProbeData(id, probeID, value);
  }
  if (client.connected() && client.available() > 0) {
    activity = true;
    Serial.print(F("discarding "));
    Serial.print(client.available());
    Serial.println(F(" bytes from eth rcv buffer"));
    while (client.available() > 0) {
      unsigned char buff[200 + 1];
      int read = client.read(buff, 200);
      Serial.print(read);
      Serial.print(F(" "));
      // Serial.println(((char*)buff));
    }
    Serial.println(F("eth rcv buffer empty"));
  }
  if (!activity) {
    loop_idle();
  }
}

void ENC28J60Manager::checkEth() {
  Serial.println("eth check start");
  if (!client.connected()) {
    Serial.println("trying to connect");
    client.connect(m_url, m_port);
    if (!client.connected()) {
      Serial.println(F("reseting eth"));
      client.stop();
      delay(start_delay);
      Ethernet.begin(ethMac);
      Serial.println("trying to re connect post reset");
      client.connect(m_url, m_port);
      Serial.print("re connection result ");
      Serial.println(client.connected());
    }
  }
  Serial.println("eth check performed");
}

void ENC28J60Manager::loop_idle() {
  switch (Ethernet.maintain()) {
  case DHCP_CHECK_RENEW_FAIL:
    // renewed fail
    Serial.println(F("Error: DHCP renewed fail"));
    break;
  case 2:
    // renew
    Serial.println(F("DHCP renew ok"));
    break;
  case 3:
    // rebind fail
    Serial.println(F("Error: DHCP rebind fail"));
    break;
  case 4:
    // rebind
    Serial.println(F("DHCP rebind ok"));
    break;
  default:
    // nothing happened
    break;
  }
}

void ENC28J60Manager::sendProbeData(char *id, char *probeID, char *value) {
  checkEth();
  if (client.connected()) {
    Serial.println(F("sending data on eth"));
    client.print(F("GET /test.php?id="));
    client.print(id);
    client.print(F("&pb="));
    client.print(probeID);
    client.print(F("&val="));
    client.print(value);
    client.println(F(" HTTP/1.1"));
    client.print(F("Host: "));
    client.println(m_url);
    client.println();
    client.flush();
  } else {
    Serial.print(F("could not connect to server "));
    Serial.print(m_url);
    Serial.print(F(":"));
    Serial.println(m_port);
  }
}

// Update ThingSpeak with the date on the right chanel
void ENC28J60Manager::updateThingSpeak(char *tsData, char *chanel) {
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
  } else
    Serial.println(F("PB!!! Conex"));

  client.stop();

  m->ledBlink(500);

  digitalWrite(4, HIGH);
}
