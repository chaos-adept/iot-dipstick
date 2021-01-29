import os
import unittest

from assertpy import assert_that
from mockito import when, unstub, verify, ANY
from paho.mqtt import publish

from index import sentLightCmd


class AliceFuncTestCase(unittest.TestCase):

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
        verify(publish, times=1).single('$devices/MOCK_DEVICE_ID/commands', payload=False, qos=0, retain=False,
                                        hostname='mqtt.cloud.yandex.net', port=8883, client_id='alice-func',
                                        keepalive=2,
                                        will=None,
                                        auth={'username': 'MOCK_REGISTRY_ID', 'password': 'MOCK_REGISTRY_PASSWORD'},
                                        tls={'ca_certs': 'MOCK_CA_CERT'})

        verify(os, times=5)
        unstub(publish)

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
        verify(publish, times=1).single('$devices/MOCK_DEVICE_ID/commands', payload=False, qos=0, retain=False,
                                        hostname='mqtt.cloud.yandex.net', port=8883, client_id='alice-func',
                                        keepalive=2,
                                        will=None,
                                        auth={'username': 'MOCK_REGISTRY_ID', 'password': 'MOCK_REGISTRY_PASSWORD'},
                                        tls={'ca_certs': 'MOCK_CA_CERT'})

        verify(os, times=5)
        unstub(publish)


if __name__ == '__main__':
    unittest.main()
