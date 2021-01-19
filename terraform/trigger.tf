resource "yandex_function_trigger" "device_metric_trigger" {
  name        = "device-metric-trigger"
  description = "Handle MQTT device messages and forward them to the cloud function"

  iot {
    registry_id = yandex_iot_core_registry.iot_registry.id
    topic       = "$devices/+/events"
  }

  function {
    id                 = yandex_function.metrics_func.id
    service_account_id = yandex_iam_service_account.iot_service_account.id
  }
}