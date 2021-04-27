#ifndef COMMON_SENSOR_DEBUG_H
#define COMMON_SENSOR_DEBUG_H

#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_BAUDRATE 115200

#ifdef LOGGING_ENABLED
#define TRACELN(msg) DEBUG_SERIAL.println(msg)
#define TRACE(msg)  DEBUG_SERIAL.print(msg)
#else
#define TRACELN(msg)
#define TRACE(msg)
#endif

#endif
