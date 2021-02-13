resource "null_resource" "iot_registry" {

  triggers = {
    iot_registry_name = yandex_iot_core_registry.iot_registry.name
  }

  provisioner "local-exec" {
    command = format("python ./scripts/export-env.py %s %s", self.triggers.iot_registry_name,
    yandex_iot_core_registry.iot_registry.id)
  }

  provisioner "local-exec" {
    command = format("python ./scripts/export-env.py %s_password \"%s\"", self.triggers.iot_registry_name,
    random_password.iot_registry_password.result)
  }

  provisioner "local-exec" {
    when = destroy
    command = format("python ./scripts/export-env.py %s", self.triggers.iot_registry_name)
  }

  provisioner "local-exec" {
    when = destroy
    command = format("python ./scripts/export-env.py %s_password", self.triggers.iot_registry_name)
  }

}

resource "null_resource" "metrics_func" {

  triggers = {
    func_name = yandex_function.metrics_func.name
    func_id = yandex_function.metrics_func.id
  }

  provisioner "local-exec" {
    command = format("python ./scripts/export-env.py iot_%s %s", self.triggers.func_name, self.triggers.func_id)
  }

  provisioner "local-exec" {
    when = destroy
    command = format("python ./scripts/export-env.py iot_%s", self.triggers.func_name)
  }

}

resource "null_resource" "alice_func" {

  triggers = {
    func_name = yandex_function.alice_func.name
    func_id = yandex_function.alice_func.id
  }

  provisioner "local-exec" {
    command = format("python ./scripts/export-env.py iot_%s %s", self.triggers.func_name, self.triggers.func_id)
  }

  provisioner "local-exec" {
    when = destroy
    command = format("python ./scripts/export-env.py iot_%s", self.triggers.func_name)
  }

}

resource "null_resource" "air_sensor" {

  count = var.air_device_count

  triggers = {
    count = var.air_device_count
    sensor_name = yandex_iot_core_device.air_sensor[count.index].name
    sensor_id = yandex_iot_core_device.air_sensor[count.index].id
  }

  provisioner "local-exec" {
    command = format("python ./scripts/export-env.py iot_%s %s", self.triggers.sensor_name, self.triggers.sensor_id)
  }

  provisioner "local-exec" {
    command = format("python ./scripts/export-env.py iot_%s_password \"%s\"",
    self.triggers.sensor_name, random_password.air_device_passwords[count.index].result)
  }

  provisioner "local-exec" {
    when = destroy
    command = format("python ./scripts/export-env.py iot_%s", self.triggers.sensor_name)
  }

  provisioner "local-exec" {
    when = destroy
    command = format("python ./scripts/export-env.py iot_%s_password", self.triggers.sensor_name)
  }

}