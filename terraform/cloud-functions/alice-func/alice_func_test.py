import json
import os
import unittest
from unittest.mock import patch

import requests
from assertpy import assert_that
from mockito import when, unstub, verify, ANY
from paho.mqtt import publish

from index import sentLightCmd, getWheather, msgHandler


class AliceFuncTestCase(unittest.TestCase):

    def setUp(self) -> None:
        pass

    def test_sent_light_is_true(self):
        when(os).getenv('CA_CERT').thenReturn('MOCK_CA_CERT')
        when(os).getenv('VERBOSE_LOGGING').thenReturn('MOCK_VERBOSE_LOGGING')
        when(os).getenv('REGISTRY_ID').thenReturn('MOCK_REGISTRY_ID')
        when(os).getenv('REGISTRY_PASSWORD').thenReturn('MOCK_REGISTRY_PASSWORD')
        when(os).getenv('DEVICE_ID').thenReturn('MOCK_DEVICE_ID')

        when(publish).single(ANY, payload=ANY, qos=ANY, retain=ANY,
                             hostname=ANY, port=ANY, client_id=ANY, keepalive=ANY,
                             will=ANY, auth=ANY,
                             tls=ANY)

        actual = sentLightCmd(True)

        assert_that(actual).is_equal_to("свет: включен")
        verify(publish, times=1).single('$registries/MOCK_REGISTRY_ID/commands', payload=True, qos=0, retain=False,
                                        hostname='mqtt.cloud.yandex.net', port=8883, client_id='alice-func',
                                        keepalive=2, will=None,
                                        auth={'username': 'MOCK_REGISTRY_ID', 'password': 'MOCK_REGISTRY_PASSWORD'},
                                        tls={'ca_certs': 'MOCK_CA_CERT'})

        verify(os, times=5)
        unstub(publish, os)

    def test_sent_light_is_false(self):
        when(os).getenv('CA_CERT').thenReturn('MOCK_CA_CERT')
        when(os).getenv('VERBOSE_LOGGING').thenReturn('MOCK_VERBOSE_LOGGING')
        when(os).getenv('REGISTRY_ID').thenReturn('MOCK_REGISTRY_ID')
        when(os).getenv('REGISTRY_PASSWORD').thenReturn('MOCK_REGISTRY_PASSWORD')
        when(os).getenv('DEVICE_ID').thenReturn('MOCK_DEVICE_ID')

        when(publish).single(ANY, payload=ANY, qos=ANY, retain=ANY,
                             hostname=ANY, port=ANY, client_id=ANY, keepalive=ANY,
                             will=ANY, auth=ANY,
                             tls=ANY)

        actual = sentLightCmd(False)

        assert_that(actual).is_equal_to("свет: выключен")
        verify(publish, times=1).single('$registries/MOCK_REGISTRY_ID/commands', payload=False, qos=0, retain=False,
                                        hostname='mqtt.cloud.yandex.net', port=8883, client_id='alice-func',
                                        keepalive=2, will=None,
                                        auth={'username': 'MOCK_REGISTRY_ID', 'password': 'MOCK_REGISTRY_PASSWORD'},
                                        tls={'ca_certs': 'MOCK_CA_CERT'})

        verify(os, times=5)
        unstub(publish, os)

    def test_get_weather(self):
        when(os).getenv('METRICS_FOLDER_ID').thenReturn('MOCK_METRICS_FOLDER_ID')
        when(os).getenv('VERBOSE_LOGGING').thenReturn('MOCK_VERBOSE_LOGGING')

        class Response:
            content = {"metrics": [{"timeseries": {"doubleValues": [22, 33]}}]}

        response = Response()

        when(requests).post(ANY, json=ANY, headers=ANY, params=ANY).thenReturn(response)
        when(json).loads(ANY).thenReturn(response.content)

        actual = getWheather("msg", "token")

        assert_that(actual).is_equal_to("температура 22 градусов.")
        verify(requests, times=1).post('https://monitoring.api.cloud.yandex.net/monitoring/v2/data/read',
                                       json=ANY, headers={'Authorization': 'Bearer token'},
                                       params={'folderId': 'MOCK_METRICS_FOLDER_ID', 'service': 'custom'})
        unstub(requests, json, os)


    def test_get_weather_is_empty(self):
        when(os).getenv('METRICS_FOLDER_ID').thenReturn('MOCK_METRICS_FOLDER_ID')
        when(os).getenv('VERBOSE_LOGGING').thenReturn('MOCK_VERBOSE_LOGGING')

        class MockResponse:
            content = {"metrics": [{"timeseries": {"doubleValues": []}}]}

        response = MockResponse()

        when(requests).post(ANY, json=ANY, headers=ANY, params=ANY).thenReturn(response)
        when(json).loads(ANY).thenReturn(response.content)

        actual = getWheather("msg", "token")

        assert_that(actual).is_equal_to("нет данных о температуре за последние 3 часа")
        verify(requests, times=1).post('https://monitoring.api.cloud.yandex.net/monitoring/v2/data/read',
                                       json=ANY, headers={'Authorization': 'Bearer token'},
                                       params={'folderId': 'MOCK_METRICS_FOLDER_ID', 'service': 'custom'})
        unstub(requests, json, os)


    @patch("index.sentLightCmd")
    def test_msgHandler(self, mock_sentLightCmd):
        when(os).getenv('VERBOSE_LOGGING').thenReturn('MOCK_VERBOSE_LOGGING')

        mock_sentLightCmd.return_value = 'is_on'

        event = {'request': {'nlu': {'tokens': ["включить"]}}, 'session': 'MOCK_SESSION', 'version': 'MOCK_VERSION'}

        class MockContext:
            token = {'access_token': "MOCK_TOKEN"}

        actual = msgHandler(event, MockContext())
        assert_that(actual).is_equal_to({'version': 'MOCK_VERSION', 'session': 'MOCK_SESSION',
                                         'response': {'text': 'is_on', 'end_session': False}})

        assert_that(mock_sentLightCmd.call_count).is_equal_to(1)
        unstub(os)


    @patch("index.getWheather")
    def test_msgHandler_if_have_no_tokens(self, mock_getWheather):
        when(os).getenv('VERBOSE_LOGGING').thenReturn('MOCK_VERBOSE_LOGGING')

        mock_getWheather.return_value = 'weather_is'

        event = {'request': {'nlu': {'tokens': []}}, 'session': 'MOCK_SESSION', 'version': 'MOCK_VERSION'}

        class MockContext:
            token = {'access_token': "MOCK_TOKEN"}

        actual = msgHandler(event, MockContext())
        assert_that(actual).is_equal_to({'version': 'MOCK_VERSION', 'session': 'MOCK_SESSION',
                                         'response': {'text': 'weather_is', 'end_session': False}})

        assert_that(mock_getWheather.call_count).is_equal_to(1)
        unstub(os)


if __name__ == '__main__':
    unittest.main(verbosity=2)
