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
  
  cout << failures << " tests failed." << endl;

  return 0;
}

