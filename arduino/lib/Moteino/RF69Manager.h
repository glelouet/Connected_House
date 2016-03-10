#ifndef ARDUINO_LIB_Moteino_RF69Manager_H_
#define ARDUINO_LIB_Moteino_RF69Manager_H_

#include <Arduino.h>
#include <RFM69_ATC.h>

class RF69Manager{

public :

  enum State {
    IDLE,GETNET, GETIP, TRANSMIT, PAIRING
  };

  // return the present connection state of the radio : idle, search netowrk, set ip, transmit
  State getState();

  const uint8_t SCANNET    = 255;
  const uint8_t GWIP       = 0;
  const long    PAIRING_MS = 60000;

private :

  State state=idle;


public :

  void init();
  void loop();

  // send data to a RF69 station if exists and connected
  // as this waits for an ACK, returns true if the ACK was received before timeout
  bool sendSync(char *data, byte targetId);

  //send data to a RF69 station, whether it exists or not. does not request ACK
  void sendAsync(char *data, byte targetId, bool ack=false);

  // send data to the broadcast on same network
  void sendBC(char *data);

  // return true if last loop() retrieved a char * from the rf69
  bool hasRcv();


  //set radio network
  void setNet(uint8_t net);

  // search radio network and crypt key
  void searchNet();

  //return the used net.
  uint8_t getNet();

  //set the ip to use
  void setIP(uint8_t ip);

  // search radio ip
  void searchIP();

  //return the used IP.
  uint8_t getIp();

  bool isPairing();

  // set pairing mode on, answering rdNet:key to broadcast on net word ; if bool==false, deactivate pair
  void pair(bool on=true);


private :

  // wireless radio
  RFM69_ATC radio;

  bool m_rdRcv = false;

  //scan net
  unsigned long last_scan=0;
  unsigned long scan_net_delay=2000;

  //time at which we deactivate pairing
  unsigned long pairingEnd=0;

  char *RD_NET_DISCO="REQ"MOTEINO_VERSION;
  char *RD_LED_DISCO="DISCO";
  char *RD_IP_DISCO="ping";

  // periodically send network request.
  void loopScanNet();

  //last IP we sent message to
  uint8_t lastIP=0;

  // find first IP not in use.
  void loopScanIP();

  unsigned long radio_next_led=0;
  unsigned long radio_count_delay=3000;
  unsigned long radio_ledcount_duration=1500;

  void loopPairing();

  void loopTransmit();

};

#endif
