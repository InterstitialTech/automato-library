#ifndef AUTOMATO_H
#define AUTOMATO_H

#if defined(ARDUINO_AUTOMATO_SENSOR)
  #include "Automato_Sensor.h"
#elif defined(ARDUINO_AUTOMATO_BRAIN)
  #include "Automato_Brain.h"
#endif

#endif /* AUTOMATO_H */
