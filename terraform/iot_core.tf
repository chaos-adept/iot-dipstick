resource "yandex_iot_core_registry" "iot_registry" {
  name        = "iot_registry"
  description = "Registry for devices"

  passwords = [
    random_password.iot_registry_password.result
  ]
}


resource "yandex_iot_core_device" "air_sensor" {
  count       = var.air_device_count

  registry_id = yandex_iot_core_registry.iot_registry.id
  name        = "air_sensor_device_${count.index + 1}"
  description = "air sensor ${count.index + 1}"

  passwords = [
    random_password.air_device_passwords[count.index].result
  ]
}