#!/bin/sh

ALICE_FUNC_ID=$(echo "$IOT_ALICE_FUNC" | tr -d \")

EMPTY_LINE=""

echo "--------------------------------------------------------"
echo "Alice Function Id:       '${ALICE_FUNC_ID}'"
echo "--------------------------------------------------------"
echo "$EMPTY_LINE"

yc serverless function invoke $ALICE_FUNC_ID --data-file 'alice-func-data.json'