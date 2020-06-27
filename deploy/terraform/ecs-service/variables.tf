variable "alarm_actions" {
  default     = []
  description = "The list of ARNs which will be triggered when the alarms trigger"
  type        = list(string)
}

variable "aws_profile" {
  type = string
  default = "default"
}

variable "aws_region" {
  type = string
  default = "ap-southeast-2"
}

variable "cpu" {
  type        = number
  default     = 256
  description = "The CPU limit for the task and container."
}

variable "memory" {
  type        = number
  default     = 64
  description = "The hard memory limit for the task and container"
}

variable "memory_reservation" {
  type        = number
  default     = 64
  description = "The soft memory limit for the task and container"
}

variable "service_version" {
  type = string
  default = "1"
}

variable "tags" {
  type = map(string)
  description = "A map of tags that will be added to all resources"
  default = {}
}
