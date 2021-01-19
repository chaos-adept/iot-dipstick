variable "metric_func_execution_timeout" {
  description = "Timeout for metric handling execution in seconds"
  default     = "3"
}

variable "alice_func_execution_timeout" {
  description = "Timeout for alice voice command handling execution in seconds"
  default     = "10"
}

variable air_device_count {
  description = "Count of devices, which be present for air monitoring"
  default     = "2"
}

variable verbose_logs {
  description = "turning on verbose logging"
  default = true
}