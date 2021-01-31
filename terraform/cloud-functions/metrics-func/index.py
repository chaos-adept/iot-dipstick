import datetime
import logging
import requests
import os
import json
import base64

METRICS_PUSH_URL = 'https://monitoring.api.cloud.yandex.net/monitoring/v2/data/write'
METRICS_SERVICE = 'custom'

logger = logging.getLogger()
logger.setLevel(logging.INFO)

verboseLogging = bool(os.getenv('VERBOSE_LOG')) ## Convert to bool

if  verboseLogging:
    logger.info('Loading my-function')

def pushMetrics(iamToken, msg):
    folderId = os.environ["METRICS_FOLDER_ID"]
    metrics = makeAllMetrics(msg)
    if verboseLogging:
        logger.info(f'Metrics request: {metrics}')
    resp = requests.post(
        METRICS_PUSH_URL,
        json=metrics,
        headers={"Authorization": "Bearer " + iamToken},
        params={"folderId": folderId, "service": METRICS_SERVICE}
    )
    if verboseLogging:
        logger.info(f'Metrics response: {resp}')
        logger.info(f'Metrics response.content: {resp.content}')
    return resp

"""
Input Json format is:
{
    "TimeStamp":"2020-05-19T18:41:37.145+03:00",
    "Values":[
        {"Type":"Float","Name":"Humidity","Value":"90.22961"},
        {"Type":"Float","Name":"CarbonDioxide","Value":"125.06672"},
        {"Type":"Float","Name":"Pressure","Value":"32.808365"},
        {"Type":"Float","Name":"Temperature","Value":"31.049744"}
        ]
}
"""
def makeAllMetrics(msg):
    metrics = [makeFloatMetric(v["Name"], v["Value"]) for v in msg["Values"] if v["Value"] is not None]
    ts = msg["TimeStamp"]
    return {
        "ts": ts,
        "labels": {
            "device_id": msg["device_id"],
        },
        "metrics": metrics
    }

def makeFloatMetric(name, value):
    return {
        "name": name,
        "type": "DGAUGE",
        "value": float(value),
    }

def makeBoolMetric(name, value):
    return {
        "name": name,
        "type": "IGAUGE",
        "value": int(value == "True"),
    }

"""
    Entry-point for Serverless Function.
    :param event: IoT message payload.
    :param context: information about current execution context.
    :return: sucessfull response statusCode: 200
"""
def msgHandler(event, context):
    statusCode = 500  ## Error response by default

    if verboseLogging:
        logger.info(event)
        logger.info(context)

    json_msg = event["messages"][0]
    device_id = json_msg["details"]["device_id"]
    
    event_payload = base64.b64decode(json_msg["details"]["payload"])


    if verboseLogging:
        logger.info(f'Event: {event_payload}')

    payload_json = json.loads(event_payload)
    payload_json["device_id"] = device_id

    iam_token = context.token["access_token"]
    pushMetrics(iam_token, payload_json)

    statusCode = 200
    
    return {
        'statusCode': statusCode,
        'headers': {
            'Content-Type': 'text/plain'
        },
        'isBase64Encoded': False
    }



"""
Data for test:

{
    "messages": [
        {
            "event_metadata": {
                "event_id": "160d239876d9714800",
                "event_type": "yandex.cloud.events.iot.IoTMessage",
                "created_at": "2020-05-08T19:16:21.267616072Z",
                "folder_id": "b112345678910"
            },
            "details": {
                "registry_id": "are1234567890",
                "device_id": "are0987654321",
                "mqtt_topic": "$devices/are0987654321/events",
                "payload": "eyJWYWx1ZXMiOiBbeyJUeXBlIjogIkZsb2F0IiwgIlZhbHVlIjogIjI1Ljc0IiwgIk5hbWUiOiAiSHVtaWRpdHkifSwgeyJUeXBlIjogIkZsb2F0IiwgIlZhbHVlIjogIjgwLjY1IiwgIk5hbWUiOiAiVGVtcGVyYXR1cmUifSwgeyJUeXBlIjogIkJvb2wiLCAiVmFsdWUiOiAiRmFsc2UiLCAiTmFtZSI6ICJXYXRlciBzZW5zb3IifSwgeyJUeXBlIjogIkJvb2wiLCAiVmFsdWUiOiAiRmFsc2UiLCAiTmFtZSI6ICJTbW9rZSBzZW5zb3IifSwgeyJUeXBlIjogIkJvb2wiLCAiVmFsdWUiOiAiRmFsc2UiLCAiTmFtZSI6ICJSb29tIGRvb3Igc2Vuc29yIn0sIHsiVHlwZSI6ICJCb29sIiwgIlZhbHVlIjogIkZhbHNlIiwgIk5hbWUiOiAiUmFjayBkb29yIHNlbnNvciJ9XSwgIlRpbWVTdGFtcCI6ICIyMDIwLTA1LTIxVDIzOjEwOjE2WiIsICJEZXZpY2VJZCI6ICIwZTNjZTFkMC0xNTA0LTQzMjUtOTcyZi01NWM5NjEzMTk4MTQifQ=="
            }
        }
    ]
}
"""