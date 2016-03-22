#include <RF69Manager.h>

namespace rfm69 {

void RF69Manager::init(NetParams *params) {
  netparams = params;
  radio.initialize(RF69_433MHZ, 0, 0);
#ifdef IS_RFM69HW // only for RFM69HW
  radio.setHighPower();
#endif
  radio.enableAutoPower(-60);
}

void RF69Manager::loop() {
  m_chg = false;
  m_rcv = false;
  switch (state) {
  case GETNET:
    loopScanNet();
    break;
  case GETIP:
    loopScanIP();
    break;
  case TRANSMIT:
    loopTransmit();
    break;
  case PAIRING:
    loopPairing();
    break;
  case IDLE:
    break;
  }
}

bool RF69Manager::sendSync(const char *trame, byte targetId) {
  return radio.sendWithRetry(targetId, ((uint8_t *)trame), strlen(trame));
}

void RF69Manager::sendAsync(const char *trame, byte targetId, bool ack) {
  radio.send(targetId, ((uint8_t *)trame), strlen(trame), ack);
}

void RF69Manager::sendBC(const char *data) {
  radio.send(RF69_BROADCAST_ADDR, data, strlen(data));
}

void RF69Manager::setNet(uint8_t net) {
  netparams->rdNet = net;
  netparams->paired = true;
  m_chg = true;
  radio.setNetwork(net);
  findIp();
}

uint8_t RF69Manager::getNet() {
  if (netparams->paired)
    return netparams->rdNet;
  else
    return SCANNET;
}

void RF69Manager::findNet() {
  if (netparams->paired) {
    radio.setNetwork(netparams->rdNet);
    radio.encrypt(netparams->rdKey);
    findIp();
  } else {
    searchNet();
  }
}

void RF69Manager::searchNet() {
  netparams->paired = false;
  m_chg = true;
  state = GETNET;
  last_scan = 0;
  radio.promiscuous();
  radio.setNetwork(SCANNET);
  radio.encrypt(0);
}

void RF69Manager::setIP(uint8_t ip) {
  netparams->rdIP = m_IP = ip;
  radio.setAddress(ip);
  state = TRANSMIT;
  m_chg = true;
}

uint8_t RF69Manager::getIp() { return m_IP; }

void RF69Manager::findIp() {
  if (netparams->rdIP != RF69_BROADCAST_ADDR) {
    m_IP = netparams->rdIP;
    radio.setAddress(m_IP);
    state = TRANSMIT;
  } else {
    searchIP();
  }
}

void RF69Manager::searchIP() {
  if (state == GETIP)
    return;
  // change the IP in params to broadcast
  if (netparams->rdIP != RF69_BROADCAST_ADDR) {
    netparams->rdIP = RF69_BROADCAST_ADDR;
    m_chg = true;
  }
  state = GETIP;
  m_IP = RF69_BROADCAST_ADDR;
  radio.setAddress(RF69_BROADCAST_ADDR);
  radio.promiscuous();
}

bool RF69Manager::isPairing() { return state == PAIRING; }

void RF69Manager::pair(bool on) {
  if (on) {
    state = PAIRING;
    pairingEnd = millis() + PAIRING_MS;
    radio.promiscuous();
    radio.setNetwork(SCANNET);
    radio.encrypt(0);
  } else {
    radio.promiscuous(false);
    findNet();
  }
}

void RF69Manager::loopScanNet() {
  if (radio.receiveDone() && radio.DATALEN == 3 + 1 + RF69_CRYPT_SIZE &&
      strncmp((char *)radio.DATA, "net", 3) == 0) {
    m_chg = true;
    netparams->paired = true;
    netparams->rdNet = radio.DATA[3];
    for (int i = 0; i < RF69_CRYPT_SIZE; i++) {
      netparams->rdKey[i] = radio.DATA[i + 4];
    }
    radio.promiscuous(false);
    findNet();
  } else if (millis() - last_scan > scan_net_delay) {
    last_scan = millis();
    sendBC(DISCO_NET_TRAME);
  }
}

void RF69Manager::loopScanIP() {
  ++m_IP;
  if (m_IP == GWIP)
    m_IP++;
  bool ack = sendSync(DISCO_IP_TRAME, m_IP);
  if (!ack) {
    // maybe we can get this IP. We may also be competing for this IP : wait
    // random
    delay(random(1000));
    if (sendSync(DISCO_IP_TRAME, m_IP))
      return;
    Serial.print(F("acquired IP "));
    Serial.println(m_IP);
    radio.promiscuous(false);
    radio.setAddress(m_IP);
    state = TRANSMIT;
  }
}

void RF69Manager::loopTransmit() {
  if (radio.receiveDone()) {
    if (radio.SENDERID == m_IP) {
      // another station has same IP : get the IP again
      findIp();
      return;
    }
    m_rcv = true;
    if (strcmp(DISCO_IP_TRAME, (char *)radio.DATA) == 0) {
      radio.sendACK();
    }
  }
}

void RF69Manager::loopPairing() {
  if (pairingEnd <= millis()) {
    pair(false);
    Serial.println("pairing timeout");
    m_chg = true;
  } else {
    if (radio.receiveDone()) {
      if (strcmp(DISCO_NET_TRAME, (char *)radio.DATA) == 0) {
        char data[3 + 1 + RF69_CRYPT_SIZE + 1]; //"net",netId, cryptkey, 0
        data[0] = 'n';
        data[1] = 'e';
        data[2] = 't';
        data[3] = netparams->rdNet;
        for (int i = 0; i < RF69_CRYPT_SIZE; i++) {
          data[i + 4] = netparams->rdKey[i];
        }
        data[3 + 1 + RF69_CRYPT_SIZE] = 0;
        sendBC(data);
      }
    }
  }
}
}
