#ifndef ARDUINO_LIB_Moteino_W5100Manager_H_
#define ARDUINO_LIB_Moteino_W5100Manager_H_

#include <Moteino.h>
#include <Ethernet.h>
#include <utility/w5100.h>

class W5100Manager {

public:
  // extract the tokenNb-th token from Message into response, where tokens are
  // separated by ';', first token is 0
  // maxResp is the buffer size allocated to Response(NOT strlen(Response) !)
  static void token(char *Message, char *Response, size_t tokenNb,
                    size_t maxResp, char sep = ';') {
    int char_idx = 0;
    for (; Message[char_idx] != '\0' && tokenNb > 0; char_idx++) {
      if (Message[char_idx] == sep) {
        tokenNb--;
      }
    }
    if (tokenNb == 0) {
      int cp_idx = char_idx;
      for (; (cp_idx - char_idx) < (maxResp - 1) && Message[cp_idx] != '\0' &&
             Message[cp_idx] != sep;
           cp_idx++) {
        Response[cp_idx - char_idx] = Message[cp_idx];
      }
      Response[cp_idx - char_idx] = '\0';
    } else {
      Message[0] = '\0';
    }
  }

  bool hasEthernet() { return digitalRead(ETHERNET_PIN); }

  void init(Moteino *moteino, const char *url, uint16_t port);

  void loop();

  // send data with format id:probeId:value to the php server
  void sendProbeData(char *id, char *probeID, char *value);

  void updateThingSpeak(char *tsData, char *chanel);

private:
  // self_management when idle
  void loop_idle();

  EthernetClient client;

  Moteino *m;

  const char *m_url;
  uint16_t m_port;

  byte ethMac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

  const byte ETHERNET_PIN = 4;

  void checkEth();
};

#endif
