resource "random_password" "iot_registry_password" {
  length = 16
  special = true
}

resource "random_password" "air_device_passwords" {
  count = var.air_device_count
  length = 16
  special = true
  lower = true
  upper = true
  min_upper = 1
  min_numeric = 1
  min_lower = 1
  min_special = 1
  override_special = "_%@"
}