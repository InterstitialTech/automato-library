/*

An Arduino library for the Automato sensor board

*/

#include <Automato.h>
#include <Esp.h>

uint64_t Automato::macAddress() 
{
  return ESP.getEfuseMac();
}


