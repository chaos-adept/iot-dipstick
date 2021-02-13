#include "z19b_sensor.h"

#define BAUDRATE 9600                                      // Native to the sensor (do not change)
#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_BAUDRATE 115200

Z19BSensor::Z19BSensor(int rxPin, int txPin) : mySerial(rxPin, txPin), myMHZ19() {
    onDataClean();
}

Z19BSensor::~Z19BSensor() {

}

/*
   Every time communication is initiated, a code is subsequetnly
   generated to represent the result of the attempt. This can
   be used to monitor your program and pinpoint any difficulties
   your program might be having.

   The codes are as followed:

        RESULT_NULL = 0,             // This should not occur, and suggests a
   library logic error RESULT_OK = 1,               // Communication was
   sucessfull RESULT_TIMEOUT = 2,          // Timed out waiting for a response
        RESULT_MATCH = 3,            // Recieved data does not match the usual
   syntax expected RESULT_CRC = 4,              // Recieved data does not match
   the CRC given RESULT_FILTER = 5,           // Filter was triggered (see
   FilterUsage example) RESULT_FAILED = 6            // Not currently used
*/
void Z19BSensor::onLoopCycle() {
               // Buffer for CO2
    CO2 = myMHZ19.getCO2();  // Request CO2 (as ppm)

    if (myMHZ19.errorCode ==
        RESULT_OK)  // RESULT_OK is an alis for 1. Either can be used to confirm
                    // the response was OK.
    {
        DEBUG_SERIAL.print("CO2 Value successfully Recieved: ");
        DEBUG_SERIAL.println(CO2);
        DEBUG_SERIAL.print("Response Code: ");
        DEBUG_SERIAL.println(myMHZ19.errorCode);  // Get the Error Code value
    } else {
        DEBUG_SERIAL.println("Failed to recieve CO2 value - Error");
        DEBUG_SERIAL.print("Response Code: ");
        DEBUG_SERIAL.println(myMHZ19.errorCode);  // Get the Error Code value
    }
}

bool Z19BSensor::isAlive() {
    return myMHZ19.errorCode == RESULT_OK;
}

void Z19BSensor::onDataClean() {
    CO2 = 400;
}

void Z19BSensor::begin() {
    mySerial.begin(BAUDRATE);  // Uno Exammple: Begin Stream with MHZ19 baudrate
    // myMHZ19.printCommunication();                            // Error Codes
    // are also included here if found (mainly for debugging/interest)

    myMHZ19.begin(mySerial);  // *Important, Pass your Stream reference
}

MetricResult Z19BSensor::getMetrics() {
    return { String("co2 PPM"), String("Float"), String(CO2) };
}