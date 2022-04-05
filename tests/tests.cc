#include <AutomatoResult.h>
#include <iostream>
#include <Arduino.h>
#include <AutomatoMsg.h>

using namespace std;

// AutomatoResult serialMessage(ResultCode rc) {
//   Serial.println(rc);
//   return ret;
// }


auto invalidmsg = "unknown error code"; 

bool invalidRcGivesExpectedString() 
{
   auto ar = AutomatoResult((ResultCode)(rc_count + 1));
   return (strcmp(ar.as_string(), invalidmsg) == 0);
}
 
bool allRcsHaveStrings() 
{
  for (uint8_t rc = 0; rc < rc_count; ++rc)
  {
    auto ar = AutomatoResult((ResultCode)rc);
    if (strcmp(ar.as_string(), invalidmsg) == 0) 
      return false;
  }
  return true;
}

bool onlyOkIsTrue() 
{
  for (uint8_t rc = 0; rc < rc_count; ++rc)
  {
    auto ar = AutomatoResult((ResultCode)rc);
    if (ar && ar.resultCode() != rc_ok)
      return false;
  }
  return true;
}


bool test_payload_size(Payload &p, bool *tested_messages)
{
  if (payloadSize(p) == 0) {
    cout << "payloadSize unimplemented for type: " << p.type << endl;
    return false;
  }
  else
  {
    tested_messages[p.type] = true;
    return true;
  }
}

bool payloadSizeImplementedForAll()
{
  // track what message types have been tested. 
  bool tested_messages[pt_count];
  for (int i = 0; i < pt_count; ++i)
    tested_messages[i] = false;

  // build a message of each type.
  Msgbuf mb;

  int A7 = 32;

  setup_ack(mb.payload);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;
  setup_fail(mb.payload, rc_no_message_received);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;
  setup_pinmode(mb.payload, A7, 0);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;
  setup_readpin(mb.payload, A7);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;
  setup_readpinreply(mb.payload, A7, 0);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;
  setup_writepin(mb.payload, A7, 0);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;
  setup_readanalog(mb.payload, A7);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;
  setup_readanalogreply(mb.payload, A7, 0);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;

  setup_readmem(mb.payload, 10, 10);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;
  const char *test = "1234567890";
  setup_readmemreply(mb.payload,10, (void*)test);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;
  setup_writemem(mb.payload, 0, 10, (void*)test);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;
  setup_readhumidity(mb.payload);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;
  setup_readhumidityreply(mb.payload, 1.0);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;
  setup_readtemperature(mb.payload);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;
  setup_readtemperaturereply(mb.payload, 1.0);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;

  setup_readinfo(mb.payload);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;
  setup_readinforeply(mb.payload, 1.0, 0, 0);
  if (!test_payload_size(mb.payload, tested_messages))
    return false;

  for (int i = 0; i < pt_count; ++i)
  {
    if (tested_messages[i] == false)
    {
      cout << "payloadSize untested for payload type: " << i << endl;
      return false;
    }
  }

  return true;

}

bool testreadmem_setups()
{
  return false;  // unimplemented
}

int main(int argc, char *argv[])
{
  int failures(0);
  
  if (!invalidRcGivesExpectedString())
  {
    ++failures;
    cout << "test failed: " << "invalidRcGivesExpectedString()" << endl;
  }

  if (!allRcsHaveStrings())
  {
    ++failures;
    cout << "test failed: " << "allRcsHaveStrings()" << endl;
  }
  
  if (!onlyOkIsTrue())
  {
    ++failures;
    cout << "test failed: " << "onlyOkIsTrue()" << endl;
  }

  if (!payloadSizeImplementedForAll())
  {
    ++failures;
    cout << "test failed: " << "payloadSizeImplementedForAll()" << endl;
  }
  
  cout << failures << " tests failed." << endl;

  return 0;
}

