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
unsigned long next = 0 ;

void setup() {

  Serial.begin(9600);
  Serial.println("Serial initialized");

  byte mac[6] = { 0x00, 0x50, 0x56, 0x13, 0x37, 0x20 };
  Ethernet.begin(mac);
  Serial.print(F("My IP address: "));
  for (size_t idx = 0; idx < 4; idx++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[idx], HEX);
    if(idx!=3)Serial.print(F("."));
  }
  Serial.println();
}

void loop() {
  unsigned long time = millis();
  if (next<time) {
      next += 20000;
      if(next<=time) next=time+1;
      Serial.println("Client connect");
      if (client.connected()){
        Serial.println("already connected");
      } else {
        Serial.println("eth client not connected");
        Serial.print("connecting results : ");
        Serial.println(client.connect("tarzan.info.emn.fr",80));
      }
      // replace hostname with name of machine running tcpserver.pl
      // if (client.connect("server.local",5000))
      Serial.println("sending trame");
      int sent = client.println("GET /test.php HTTP/1.1");
      Serial.print("sent ");
      Serial.print(sent);
      Serial.println(" bytes");
      sent = client.println(F("Host: tarzan.info.emn.fr"));
      Serial.print("sent ");
      Serial.print(sent);
      Serial.println(" bytes");
      client.println();
  }
  if(client.connected() && client.available()>0){
    Serial.println("client received data  : ");
    unsigned char buffer[200+1];
    while(client.available()>0){
      int  read = client.read(buffer, 200);
      buffer[read]='\0';
      Serial.println((char *)buffer);
    }
    Serial.println("Client disconnect");
    client.stop();
  }
}
