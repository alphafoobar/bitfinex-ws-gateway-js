# How to Terraform

## Init

```bash
terraform init
```

## Plan

```bash
TF_WORKSPACE=example && terraform plan -var aws_profile=<aws-profile>
```


## Apply

```bash
TF_WORKSPACE=example && terraform apply -var aws_profile=<aws-profile>
```

## Destroy

```bash
TF_WORKSPACE=example && terraform destroy -var aws_profile=<aws-profile>
```
