locals {
  service_name = "bitfinex-js-gateway"
  port = 8080
  tags = merge({
    owner       = "kaizen7"
    environment = terraform.workspace
  }, var.tags)
}
