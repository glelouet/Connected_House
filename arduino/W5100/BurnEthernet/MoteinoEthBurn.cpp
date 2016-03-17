#include <Arduino.h>

#include <EtherCard.h>

const byte ETHERNET_PIN=4;
int SERIAL_BAUD = 9600;

byte Ethernet::buffer[700];

static byte ethMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
const char *m_url = "tarzan.info.emn.fr";
uint16_t m_port = 80;

static void my_callback (byte status, word off, word len) {
  Serial.println("got return from browse");
}

void setup(){
  Serial.begin(SERIAL_BAUD);
  Serial.println(F("serial initialized"));
  if (ether.begin(sizeof Ethernet::buffer, ethMac,ETHERNET_PIN) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  else
    Serial.println("eth controler initialized");
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
  else
    Serial.println("DHCP ok");
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  if (!ether.dnsLookup(m_url))
    Serial.println("DNS failed");
  ether.persistTcpConnection(true);
}

void loop() {
  word len = ether.packetReceive(); // go receive new packets
  word pos = ether.packetLoop(len); // respond to incoming pings
  ether.browseUrl("/", "test.php?id=0&pb=0&val=-5", m_url, my_callback);
}
