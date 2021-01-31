import os
import unittest
from unittest.mock import patch

from assertpy import assert_that
from mockito import when

from index import msgHandler

import base64
import json

MOCK_VERBOSE_LOGGING = "True"


class PublishMetricsTestCase(unittest.TestCase):

    def setUp(self) -> None:
        when(os).getenv('VERBOSE_LOGGING').thenReturn(MOCK_VERBOSE_LOGGING)

    @patch("index.pushMetrics")
    def test_testHandlingRequest(self, mock_pushMetrics):
        # given

        mock_pushMetrics.return_value = None

        class MockContext:
            token = {'access_token': "MOCK_TOKEN"}

        payload = json.dumps({}).encode()
        event = {"messages": [{"details": {"device_id": "devId", "payload": base64.encodebytes(payload)}}]}

        # when
        actual = msgHandler(event, MockContext())

        # given
        assert_that(actual).is_equal_to({
            'statusCode': 200,
            'headers': {
                'Content-Type': 'text/plain'
            },
            'isBase64Encoded': False
        })

        mock_pushMetrics.assert_called_once_with("MOCK_TOKEN", {'device_id': 'devId'})


if __name__ == '__main__':
    unittest.main()
