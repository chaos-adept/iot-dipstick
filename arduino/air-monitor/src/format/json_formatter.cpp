#include "json_formatter.h"
#include "../time_utils.h"

char msg[512];
char metricItemBuff[100];
char *metricItemFormat = "{\"Type\":\"%s\",\"Name\":\"%s\",\"Value\":\"%s\"}\n ";

#define DEBUG_SERIAL Serial

// char msg[512];
// const char *msgTemplateDefault =
//     "{\"TimeStamp\":\"%s\",\n "
//     "\"Values\":\n["
//     "{\"Type\":\"Float\",\"Name\":\"Dust ug/m3\",\"Value\":\"%d.%02d\"},\n "
//     "{\"Type\":\"Float\",\"Name\":\"Dust Voltage\",\"Value\":\"%d.%02d\"},\n
//     "
//     "{\"Type\":\"Float\",\"Name\":\"Dust Percent "
//     "Voltage\",\"Value\":\"%d.%02d\"},\n "
//     "{\"Type\":\"Float\",\"Name\":\"Temperature\",\"Value\":\"%d.%02d\"},\n "
//     "{\"Type\":\"Float\",\"Name\":\"Humidity\",\"Value\":\"%d.%02d\"}\n]}";
// const char *msgTemplateWithoutTemp =
//     "{\"TimeStamp\":\"%s\",\n \"Values\":\n[\n "
//     "{\"Type\":\"Float\",\"Name\":\"Dust ug/m3\",\"Value\":\"%d.%02d\"}\n]}";

void appendMetricJsonItem(MetricResult &result, char *buff) {
    sprintf(metricItemBuff, metricItemFormat, result.kind.c_str(),
            result.valueTypeName.c_str(), result.valueAsJsonPropVal.c_str());
    strcat(buff, metricItemBuff);
}

/**
    sample of output:
     {
        TimeStamp":"DATETIME",
        Values: [
            {"Type":"Float","Name":"Dust ug/m3","Value":"1.12"},
            {"Type":"Float","Name":"Dust Voltage","Value":"11.22"}
            }
        ]
     }
 */
char *threadUnsafeFormatMetricsAsJson(unsigned long datetime,
                                      MetricResult* metrics, int metricsCount) {
    // print head
    sprintf(msg, "{\"TimeStamp\":\"%s\",\n \"Values\":\n [",
            getFormattedDate(datetime).c_str());

    // print array
    DEBUG_SERIAL.print("metrics count: ");
    DEBUG_SERIAL.println(metricsCount);
    for (int indx = 0; indx < metricsCount - 1; indx++) {
        appendMetricJsonItem(metrics[indx], msg);
        strcat(msg, ",");
    }

    // print the last element
    appendMetricJsonItem(metrics[metricsCount - 1], msg);

    // print the foother
    strcat(msg, "]}");
}