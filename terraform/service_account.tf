resource "yandex_iam_service_account" "iot_service_account" {
  name        = "iot-service-account"
  description = "Service Account for Device handling"
}

resource "yandex_resourcemanager_folder_iam_member" "iot_service_account_invoker" {
  folder_id = var.folder_id
  member    = "serviceAccount:${yandex_iam_service_account.iot_service_account.id}"
  role      = "serverless.functions.invoker"
}

resource "yandex_resourcemanager_folder_iam_member" "iot_service_account_writer" {
  folder_id = var.folder_id
  member    = "serviceAccount:${yandex_iam_service_account.iot_service_account.id}"
  role      = "iot.devices.writer"
}

resource "yandex_resourcemanager_folder_iam_member" "editor" {
  folder_id = var.folder_id
  member    = "serviceAccount:${yandex_iam_service_account.iot_service_account.id}"
  role      = "editor"
}

resource "yandex_resourcemanager_folder_iam_member" "iot_service_account" {
  folder_id   = var.folder_id
  member      = "serviceAccount:${yandex_iam_service_account.iot_service_account.id}"
  role        = "viewer"
  sleep_after = 30
  depends_on = [
    yandex_resourcemanager_folder_iam_member.iot_service_account_invoker,
    yandex_resourcemanager_folder_iam_member.iot_service_account_writer,
    yandex_resourcemanager_folder_iam_member.editor
  ]
}