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

  Msgbuf mb;

  int A7 = 32;  // arbitrary pin number

  // build a message of each type.
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

bool setup_writememFailsAppropriately()
{
  Msgbuf mb;
  // MAX_WRITEMEM fails
  char test[MAX_WRITEMEM + 1];
  if (!setup_writemem(mb.payload, 0, MAX_WRITEMEM, (void*)test))
  {
    cout << "MAX_WRITEMEM should succeed as length for setup_writemem, but it failed" << endl;
    return false;
  }

  // greater than MAX_WRITEMEM fails. 
  if (setup_writemem(mb.payload, 0, MAX_WRITEMEM + 1, (void*)test))
  {
    cout << "MAX_WRITEMEM + 1 should fail as length for setup_writemem, but it succeeded" << endl;
    return false;
  }

  // less than MAX_WRITEMEM succeeds. 
  if (!setup_writemem(mb.payload, 0, MAX_WRITEMEM - 1, (void*)test) || 
    !setup_writemem(mb.payload, 0, 0, (void*)test))
  {
    cout << "length less than MAX_WRITEMEM failed for setup_writemem" << endl;
    return false;
  }

  return true;
}

bool test_writemem_at_data_boundaries()
{
  // set up an automato data area; write to the beginning and ending of the area with 
  // writemem.  does it write over the boundary?  work up to the edge of the data area?

  return false; // unimplemented
}

bool testreadmem_setups()
{
  return false;  // unimplemented
}

bool received_mems_fail_appropriately()
{
  // when invalid mem messages are recieved, they are detected.

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

  if (!setup_writememFailsAppropriately())
  {
    ++failures;
    cout << "test failed: " << "setup_writememFailsAppropriately()" << endl;
  }
  
  cout << failures << " tests failed." << endl;

  return 0;
}

