#ifndef AutomatoNetwork_h_INCLUDED
#define AutomatoNetwork_h_INCLUDED

#include <AutomatoResult.h>

enum AddressType {
  RHMesh,
  IpAddress,
  PainlessMesh
};

class AutomatoAddress {
  public:
    AddressType type;
    union {
      uint8_t rhmeshid;
      uint8_t ipbytes[4];
      uint32_t pmid;
    };
};

AutomatoResult writeAddress(AutomatoAddress &address)
{
    switch (address.type) {
        RHMesh:
          Serial.write("R");
          Serial.write(address.rhmeshid);   
          return AutomatoResult(rc_ok);
        IpAddress:
          Serial.write("I");
          Serial.write(address.ipbytes, 4);   
          return AutomatoResult(rc_ok);

        PainlessMesh:
          Serial.write("P");
          Serial.write(address.pmid, 4);   
          return AutomatoResult(rc_ok);
        default:
          return AutomatoResult(rc_invalid_address_type);
    }
}


class AutomatoProtocol
{
  public:
    AddressType addressType() = 0; 
    // void getMyAddress(AutomatoAddress &me) = 0;
    virtual bool receiveMessage(AutomatoAddress &from, Msgbuf &mb) = 0
    virtual AutomatoResult receiveMessage(AutomatoAddress &to, Msgbuf &mb) = 0
};


// Can handle one serial, one rhmesh, one wifi type.
class AutomatoNetwork 
{
  private:
    AutomatoProtocol *protocols; 
    int protocount;
  public: 
    AutomatoNetwork(AutomatoProtocol protocols[], int protocount);
    void AddProtocol(AutomatoProtocol &protocol);

    AutomatoResult doRemoteControl();

    AutomatoResult sendRequest(AutomatoAddress &to, Msgbuf &mb);
};



#endif // AutomatoNetwork_h_INCLUDED
