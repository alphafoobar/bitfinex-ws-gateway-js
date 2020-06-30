locals {
  service_name = "bitfinex-js-gateway"
  tags = merge({
    owner       = "kaizen7"
    environment = terraform.workspace
  }, var.tags)
}
