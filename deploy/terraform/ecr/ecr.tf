module "ecr" {
  source = "git::https://github.com/bnc-projects/terraform-aws-ecr.git?ref=1.1.1"
  allowed_read_principals = [
    data.terraform_remote_state.ops.outputs.deployment_role_arn
  ]
  allowed_write_principals = [
    data.terraform_remote_state.ops.outputs.deployment_role_arn
  ]
  ecr_repo_name = local.service_name
  tags          = local.tags
}
