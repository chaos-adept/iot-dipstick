#include <abstract_sensor.h>

#ifndef JSON_FORMATTER_H
#define JSON_FORMATTER_H

char* threadUnsafeFormatMetricsAsJson(unsigned long datetime, MetricResult* metricResults, int metricsCount);

#endif