#ifndef ARDUINO_LIB_Moteino_RF69Manager_H_
#define ARDUINO_LIB_Moteino_RF69Manager_H_

#include <Arduino.h>
#include <RFM69_ATC.h>
#include <Params.h>

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

  void init(NetParams * params);
  void loop();

  // send data to a RF69 station if exists and connected
  // as this waits for an ACK, returns true if the ACK was received before timeout
  bool sendSync(char *data, byte targetId);

  // send data to a RF69 station, whether it exists or not.
  // does not request ACK unless ack is set to true
  void sendAsync(char *data, byte targetId, bool ack=false);

  // send data to the broadcast on same network
  void sendBC(char *data);


  // return true if last loop() retrieved a char * from the rf69
  bool hasRcv();

  // return true if the last loop() did modifuy the network status.
  bool hasChg();

  //get the data received by last loop()
  volatile uint6_t * getData();


  //set radio network
  void setNet(uint8_t net);

  //return the used net.
  uint8_t getNet();

  //assign a net from param or discover it
  void findNet();

  // search radio network and crypt key (discovery)
  void searchNet();


  //set the ip to use
  void setIP(uint8_t ip);

  //return the used IP.
  uint8_t getIp();

  // discover IP or set the IP is specified in params
  void findIp();

  // search radio ip
  void searchIP();


  bool isPairing();

  // set pairing mode on, answering rdNet:key to broadcast on net word ; if bool==false, deactivate pair
  void pair(bool on=true);


private :

  // wireless radio
  RFM69_ATC radio;

  NetParams * netparams;

  State state=IDLE;
  bool m_rcv = false;
  bool m_chg = false;


  //scan net
  char *DISCO_NET_TRAME="REQ"MOTEINO_VERSION;
  unsigned long last_scan=0;
  unsigned long scan_net_delay=2000;

  // periodically send network request.
  void loopScanNet();


  // IP discovery
  char *DISCO_IP_TRAME="ping";
  // IP used or trying to be used.
  uint8_t m_IP=0;

  // find first IP not in use.
  void loopScanIP();


  //normal loop : did we received data ?
  void loopTransmit();


  //time at which we deactivate pairing
  unsigned long pairingEnd=0;

  void loopPairing();

};

#endif
