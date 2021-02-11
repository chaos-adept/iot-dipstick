#!/bin/sh

WIFI_ENDPOINT_NAME=ssid
WIFI_ENDPOINT_PASSWORD=password
IOT_CORE_REGISTRY_ID=$(echo "$IOT_REGISTRY")
IOT_CORE_DEVICE_ID=$(echo "$IOT_AIR_SENSOR_DEVICE_1")
IOT_CORE_DEVICE_PASSWORD=$(echo "$IOT_AIR_SENSOR_DEVICE_1_PASSWORD")

EMPTY_LINE=""

echo "--------------------------------------------------------"
echo "WIFI Endpoint Name: '${WIFI_ENDPOINT_NAME}'"
echo "WIFI Endpoint Password: '${WIFI_ENDPOINT_PASSWORD}'"
echo "$EMPTY_LINE"
echo "IOT Registry Id: '${IOT_CORE_REGISTRY_ID}'"
echo "IOT Device Id: '${IOT_CORE_DEVICE_ID}'"
echo "IOT Device Password: '${IOT_CORE_DEVICE_PASSWORD}'"
echo "--------------------------------------------------------"
echo "$EMPTY_LINE"

cp "../air-monitor/src/local_specific_variables.h.sample" "../air-monitor/src/local_specific_variables.h"
sed -i -e "s/%WIFI_ENDPOINT_NAME%/${WIFI_ENDPOINT_NAME}/g" \
                              -e "s/%WIFI_ENDPOINT_PASSWORD%/${WIFI_ENDPOINT_PASSWORD}/g" \
                              -e "s/%IOT_CORE_REGISTRY_ID%/${IOT_CORE_REGISTRY_ID}/g" \
                              -e "s/%IOT_CORE_DEVICE_ID%/${IOT_CORE_DEVICE_ID}/g" \
                              -e "s/%IOT_CORE_DEVICE_PASSWORD%/${IOT_CORE_DEVICE_PASSWORD}/g" \
                               ../air-monitor/src/local_specific_variables.h


