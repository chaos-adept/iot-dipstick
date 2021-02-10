import os
import logging
import datetime as dt
import json
import re
import paho.mqtt.publish as publish
import requests
from functools import reduce

logger = logging.getLogger()
logger.setLevel(logging.INFO)

lightCmdRegexp = re.compile("(свет)|(вкл)|(выкл)")
lightCMDTurnOffRegexp = re.compile("выкл")

weatherForecastHoursInterval = 3

def is_verbose_logging_enabled() -> bool:
    return os.getenv('VERBOSE_LOG') == "True"

METRICS_PUSH_URL = 'https://monitoring.api.cloud.yandex.net/monitoring/v2/data/read'
METRICS_SERVICE = 'custom'

if is_verbose_logging_enabled():
    logger.info('Loading msgHandler function')


def getWheather(iamToken):
    if is_verbose_logging_enabled():
        logger.info(f'get weather metrics')

    folderId = os.getenv('METRICS_FOLDER_ID')

    minutesInterval = 60 * weatherForecastHoursInterval
    toTime = dt.datetime.utcnow().replace(microsecond=0)
    fromTime = toTime - dt.timedelta(minutes=minutesInterval)

    requestBody = {
        "query": "\"Temperature\"{service=\"custom\", device_id=\"*\"}",
        "fromTime": f"{fromTime.isoformat()}Z",
        "toTime": f"{toTime.isoformat()}Z",
        "downsampling": {
            "gridAggregation": "LAST",
            "gapFilling": "NONE",
            "gridInterval": minutesInterval * 1000 * 60
        }
    }
    if is_verbose_logging_enabled():
        logger.info(f'Metrics request: {requestBody}')
    resp = requests.post(
        METRICS_PUSH_URL,
        json=requestBody,
        headers={"Authorization": "Bearer " + iamToken},
        params={"folderId": folderId, "service": METRICS_SERVICE}
    )
    if is_verbose_logging_enabled():
        logger.info(f'Metrics response: {resp}')
        logger.info(f'Metrics response.content: {resp.content}')
    metrics = json.loads(resp.content)["metrics"]

    temperatureValuesParitions = map(lambda item: item["timeseries"]["doubleValues"], metrics)
    temperatureValues = reduce(lambda a, b: a + b, temperatureValuesParitions)

    if not temperatureValues:
        return f"нет данных о температуре за последние {weatherForecastHoursInterval} часа"

    temperature = temperatureValues[0]
    text = f"температура {temperature} градусов."
    if is_verbose_logging_enabled():
        logger.info(f'temperature: {temperature}')

    return text


def sentLightCmd(isOn):
    ca_certs = os.getenv('CA_CERT')
    registryId = os.getenv('REGISTRY_ID')
    auth = {'username': registryId, 'password': os.getenv('REGISTRY_PASSWORD')}
    topic = f"$registries/{registryId}/commands/light"

    publish.single(topic, payload=isOn, qos=0, retain=False, hostname="mqtt.cloud.yandex.net",
                   port=8883, client_id="alice-func", keepalive=2, will=None, auth=auth, tls={'ca_certs': ca_certs})

    state = "включен" if isOn else "выключен"
    return f"свет: {state}"


"""
    Entry-point for Serverless Function.
    :param event: IoT message payload.
    :param context: information about current execution context.
    :return: successfull response statusCode: 200
"""


def msgHandler(event, context):
    statusCode = 200
    iamToken = context.token["access_token"]
    if is_verbose_logging_enabled():
        logger.info(event)
        logger.info(context)
        logger.info(f"iamToken: {iamToken}")

    msg = event["body"]
    session = None if not event.__contains__('session') else event["session"]
    version = None if not event.__contains__('version') else event["version"]

    if is_verbose_logging_enabled():
        logger.info(f'msg[0]: {msg}')

    tokens = msg["nlu"]["tokens"]

    if (any(lightCmdRegexp.match(s) for s in tokens)):
        isOn = not any(lightCMDTurnOffRegexp.match(s) for s in tokens)
        text = sentLightCmd(isOn)
    else:
        text = getWheather(iamToken)

    return {
        'status': statusCode,
        'version': version,
        'session': session,
        'response': {
            'text': text,
            'end_session': False
        }
    }