/*
 * UIPEthernet TcpClient example.
 *
 * UIPEthernet is a TCP/IP stack that can be used with a enc28j60 based
 * Ethernet-shield.
 *
 * UIPEthernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 *      -----------------
 *
 * This TcpClient example gets its local ip-address via dhcp and sets
 * up a tcp socket-connection to 192.168.0.1 port 5000 every 5 Seconds.
 * After sending a message it waits for a response. After receiving the
 * response the client disconnects and tries to reconnect after 5 seconds.
 *
 * Copyright (C) 2013 by Norbert Truchsess <norbert.truchsess@t-online.de>
 */
#include <Arduino.h>

#include <UIPEthernet.h>
#include <utility/enc28j60.h>

EthernetClient client;
unsigned long send_delay = 1000;
unsigned long next = 0;
size_t buff_size = 500;

void setup() {

  Serial.begin(9600);
  Serial.println("Serial initialized");

  byte mac[6] = {0x00, 0x50, 0x56, 0x13, 0x37, 0x20};
  delay(1000); // we need to wait before initializing ethernet
  Serial.println("starting ethernet");
  Ethernet.begin(mac);
  Serial.print(F("My IP address: "));
  for (size_t idx = 0; idx < 4; idx++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[idx], DEC);
    if (idx != 3)
      Serial.print(F("."));
  }
  Serial.println();
  delay(1000);
  next = millis() + send_delay;
  Serial.println("end setup");
}

void loop() {
  unsigned long time = millis();
  if (next < time) {
    Ethernet.maintain();
    next += send_delay;
    if (next <= time)
      next = time + 1;
    if (client.connected()) {
      Serial.println("already connected");
    } else {
      Serial.print("connecting results : ");
      Serial.println(client.connect("tarzan.info.emn.fr", 80));
    }
    int sent = client.println("GET /ping.php HTTP/1.1");
    sent = client.println(F("Host: tarzan.info.emn.fr"));
    client.println();
  }
  if (client.connected() && client.available() > 0) {
    Serial.println("client received data  : ");
    unsigned char buffer[buff_size + 1];
    while (client.available() > 0) {
      int read = client.read(buffer, buff_size);
      buffer[read] = '\0';
      Serial.println(read);
    }
  }
}
