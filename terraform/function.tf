locals {
  metric_func_source_dir = "${path.module}/cloud-functions/metrics-func"
  alice_func_source_dir = "${path.module}/cloud-functions/alice-func"
}

resource "random_uuid" "metrics_func" {
  keepers = {
    for filename in fileset(local.metric_func_source_dir, "**/*"):
    filename => filemd5("${local.metric_func_source_dir}/${filename}")
  }
}

resource "random_uuid" "alice_func" {
  keepers = {
    for filename in fileset(local.alice_func_source_dir, "**/*"):
    filename => filemd5("${local.alice_func_source_dir}/${filename}")
  }
}

data "archive_file" "metrics_func_zip" {
  type        = "zip"

  output_path = "${path.module}/gen/metrics_func_zip.zip"
  source_dir  = local.metric_func_source_dir
}

data "archive_file" "alice_func_zip" {
  type        = "zip"

  output_path = "${path.module}/gen/alice_func_zip.zip"
  source_dir  = local.alice_func_source_dir
}

resource "yandex_function" "metrics_func" {
  name              = "metrics-func"
  description       = "Handle device metrics and store them into monitoring"
  user_hash         = random_uuid.metrics_func.result
  runtime           = "python38"
  entrypoint        = "index.msgHandler"
  memory            = "128"
  execution_timeout = var.metric_func_execution_timeout

  content {
    zip_filename = data.archive_file.metrics_func_zip.output_path
  }

  service_account_id = yandex_iam_service_account.iot_service_account.id
  depends_on         = [yandex_resourcemanager_folder_iam_member.iot_service_account, random_uuid.metrics_func]

  environment = {
      METRICS_FOLDER_ID = var.folder_id
      VERBOSE_LOG = title(var.verbose_logs)
  }
}

resource "yandex_function" "alice_func" {
  name              = "alice-func"
  description       = "Handle alice commands"
  user_hash         = random_uuid.alice_func.result
  runtime           = "python38"
  entrypoint        = "index.msgHandler"
  memory            = "128"
  execution_timeout = var.alice_func_execution_timeout

  content {
    zip_filename = data.archive_file.alice_func_zip.output_path
  }

  service_account_id = yandex_iam_service_account.iot_service_account.id
  depends_on         = [yandex_resourcemanager_folder_iam_member.iot_service_account, random_uuid.alice_func]

  environment = {
      METRICS_FOLDER_ID = var.folder_id
      VERBOSE_LOG = title(var.verbose_logs)

      CA_CERT = "ca.crt"
      VERBOSE_LOG = title(var.verbose_logs)
      REGISTRY_ID = yandex_iot_core_registry.iot_registry.id
      REGISTRY_PASSWORD = random_password.iot_registry_password.result
      METRICS_FOLDER_ID = var.folder_id
  }
}