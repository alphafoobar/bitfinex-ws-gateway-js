variable "aws_profile" {
  type = string
  default = "default"
}

variable "aws_region" {
  type = string
  default = "ap-southeast-2"
}

variable "tags" {
  description = "A map of tags to add to all resources"
  default     = {}
}
