resource "yandex_iot_core_registry" "iot_registry" {
  name        = "iot_registry"
  description = "Registry for devices"

  passwords = [
    random_password.iot_registry_password.result
  ]

  provisioner "local-exec" {
    command = format("python ./helpers/export-env.py %s %s", self.name, self.id)
  }

  provisioner "local-exec" {
    command = format("python ./helpers/export-env.py %s_password %s", self.name, random_password.iot_registry_password.result)
  }

  provisioner "local-exec" {
    when = destroy
    command = format("python ./helpers/export-env.py %s", self.name)
  }

  provisioner "local-exec" {
    when = destroy
    command = format("python ./helpers/export-env.py %s_password", self.name)
  }
}


resource "yandex_iot_core_device" "air_sensor" {
  count       = var.air_device_count

  registry_id = yandex_iot_core_registry.iot_registry.id
  name        = "air_sensor_device_${count.index + 1}"
  description = "air sensor ${count.index + 1}"

  passwords = [
    random_password.air_device_passwords[count.index].result
  ]

  provisioner "local-exec" {
    command = format("python ./helpers/export-env.py iot_%s %s", self.name, self.id)
  }

  provisioner "local-exec" {
    command = format("python ./helpers/export-env.py iot_%s_password %s", self.name, random_password.air_device_passwords[count.index].result)
  }

  provisioner "local-exec" {
    when = destroy
    command = format("python ./helpers/export-env.py iot_%s", self.name)
  }

  provisioner "local-exec" {
    when = destroy
    command = format("python ./helpers/export-env.py iot_%s_password", self.name)
  }
}