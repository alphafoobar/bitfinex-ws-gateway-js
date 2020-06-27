terraform {
  required_version = ">= 0.12"
  backend "s3" {
    # Region cannot be referenced as a variable.
    region = "ap-southeast-2"
    # Profile cannot be referenced as a variable.
    profile  = "kaizen-terraform"

    bucket = "ops.kaizen7.nz"
    key = "example/bitfinex-c-gateway/ecr"
    encrypt = true
  }
}

provider "aws" {
  region  = var.aws_region
  version = "~> 2.36.0"
  profile = var.aws_profile
}


data "terraform_remote_state" "ops" {
  backend = "s3"
  config = {
    bucket = "ops.kaizen7.nz"
    key = "ops/example/example"
    region   = var.aws_region
    profile  = var.aws_profile
  }
}
