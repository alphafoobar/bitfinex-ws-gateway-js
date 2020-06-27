locals {
  service_name = "bitfinex-c-gateway"
  tags = merge({
    owner       = "kaizen7"
    environment = terraform.workspace
  }, var.tags)
}