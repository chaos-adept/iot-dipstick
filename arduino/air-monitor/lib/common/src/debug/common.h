#ifndef COMMON_SENSOR_DEBUG_H
#define COMMON_SENSOR_DEBUG_H

#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_BAUDRATE 115200

#ifdef LOGGING_ENABLED
#define TRACELN(msg) DEBUG_SERIAL.println(msg)
#define TRACE(msg)  DEBUG_SERIAL.print(msg)
#define TRACEVALN(msg, val) TRACE(msg); TRACE(" "); TRACELN(val)
#else
#define TRACELN(msg)
#define TRACE(msg)
#define TRACEVALN(msg, val)
#endif

#endif
