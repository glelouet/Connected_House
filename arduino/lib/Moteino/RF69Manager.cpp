#include <RF69Manager.h>

State RF69Manager::getState(){
  return state;
}

void RF69Manager::init(NetParams * params){
  m_params = params;
  //init with any net/IP then ask to get them
  radio.initialize(RF69_433MHZ);
  #ifdef IS_RFM69HW //only for RFM69HW
    radio.setHighPower();
  #endif
  radio.enableAutoPower(-60);
}

void RF69Manager::loop(){
  m_rdRcv=false;
  m_changed=false;
  switch (state) {
    case GETNET :
      loopScanNet();
    break;
    case GETIP:
      loopScanIP();
    break;
    case TRANSMIT :
      loopTransmit();
    break;
    case PAIRING :
      loopPairing();
    break;
  }
}

void RF69Manager::loopScanNet(){
  if (radio.receiveDone()
      && radio.DATALEN==3+1+RF69_CRYPT_SIZE
      && strncmp((char *)radio.DATA, "net", 3)==0 ) {
    netparams->paired=true;
    netparams->rdNet=radio.DATA[3];
    m_changed=true;
    for(int i=0;i<RF69_CRYPT_SIZE;i++) {
      netparams->rdKey[i]=radio.DATA[i+4];
    }
    radio.promiscuous(false);
    rdLedDisco();
    rdFindNet();
  } else if(millis()-last_scan>scan_net_delay) {
    last_scan=millis();
    sendBCRF69(RD_NET_DISCO);
  }
}
