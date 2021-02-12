#!/bin/sh

REGISTRY_ID=$(echo "$IOT_REGISTRY")
REGISTRY_PASSWORD=$(echo "$IOT_REGISTRY_PASSWORD")

EMPTY_LINE=""

echo "--------------------------------------------------------"
echo "Credentials: '${REGISTRY_ID}' : '${REGISTRY_PASSWORD}'"
echo "Topic:       'registries/${REGISTRY_ID}/events'"
echo "--------------------------------------------------------"
echo $EMPTY_LINE

yc iot mqtt subscribe \
--username "${REGISTRY_ID}" \
--password "${REGISTRY_PASSWORD}" \
--topic "\$registries/${REGISTRY_ID}/events" \
--qos 1