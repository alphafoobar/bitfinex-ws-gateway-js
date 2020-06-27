#! /bin/bash

cd $TRAVIS_BUILD_DIR
cd deployment/terraform/ecs-service
terraform init -backend-config="bucket=${STATE_S3_BUCKET}" -backend-config="region=${AWS_DEFAULT_REGION}" -backend-config="dynamodb_table=${STATE_DYNAMODB_TABLE}" -backend-config="kms_key_id=${KMS_KEY_ID}" -backend-config="key=${SERVICE_KEY}" -backend-config="role_arn=${ROLE_ARN}" 1> /dev/null || exit 1
terraform workspace select $1 || terraform workspace new $1
terraform apply -backup="-" -input=false -auto-approve -var role_arn=${ROLE_ARN} -var service_name=${SERVICE_NAME} -var service_version=${TRAVIS_BUILD_NUMBER} -var splunk_url=${SPLUNK_URL} -var splunk_token=${SPLUNK_TOKEN} -var tags="{\"AuthorName\":\"${AUTHOR_NAME}\",\"GitRepository\":\"${TRAVIS_REPO_SLUG}\"}" 1> /dev/null || exit 1
eval $(terraform output -json | jq -r .' | @sh "export CLUSTER_NAME=\(.ecs_cluster_name.value)\nexport DEPLOYMENT_ROLE_ARN=\(.deployment_role_arn.value) "')
eval $(aws sts assume-role --role-arn "$DEPLOYMENT_ROLE_ARN" --role-session-name "${TRAVIS_REPO_SLUG//\//-}" | jq -r '.Credentials | @sh "export AWS_SESSION_TOKEN=\(.SessionToken)\nexport AWS_ACCESS_KEY_ID=\(.AccessKeyId)\nexport AWS_SECRET_ACCESS_KEY=\(.SecretAccessKey) "')
aws ecs wait services-stable --services ${SERVICE_NAME} --cluster ${CLUSTER_NAME} || exit 1
