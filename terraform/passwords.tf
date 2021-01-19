resource "random_password" "iot_registry_password" {
  length = 16
  special = true
}

resource "random_password" "air_device_passwords" {
  count = var.air_device_count
  length = 16
  special = true
  override_special = "_%@"
}