# output "service_account" {
#   value = yandex_iam_service_account.emulator_sa.id
# }

output "iot_core" {
  value = yandex_iot_core_registry.iot_registry.id
}

output "iot_registry_password" {
  value = random_password.iot_registry_password.result
}

output "air_devices_ids" {
  value = yandex_iot_core_device.air_sensor.*.id
}

output "air_device_passwords" {
  value = random_password.air_device_passwords[*].result
}

output "alice_func" {
  value = yandex_function.alice_func.id
}

output "metrics_func" {
  value = yandex_function.metrics_func.id
}