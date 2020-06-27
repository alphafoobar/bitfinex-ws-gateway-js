#! /bin/bash

cd deployment/terraform/ecr
terraform init -backend-config="bucket=${STATE_S3_BUCKET}" -backend-config="region=${AWS_DEFAULT_REGION}" -backend-config="dynamodb_table=${STATE_DYNAMODB_TABLE}" -backend-config="kms_key_id=${KMS_KEY_ID}" -backend-config="key=${KEY}" -backend-config="role_arn=${ROLE_ARN}" 1>/dev/null || exit 1
terraform apply -backup="-" -input=false -auto-approve -var role_arn=${ROLE_ARN} -var service_name=${SERVICE_NAME} -var tags="{\"AuthorName\":\"${AUTHOR_NAME}\",\"GitRepository\":\"${TRAVIS_REPO_SLUG}\"}" 1>/dev/null || exit 1
export REPOSITORY_URI=$(terraform output repository_url) || exit 1
cd $TRAVIS_BUILD_DIR
eval $(aws sts assume-role --role-arn "$OPERATIONS_ROLE_ARN" --role-session-name "${TRAVIS_REPO_SLUG//\//-}" | jq -r '.Credentials | @sh "export AWS_SESSION_TOKEN=\(.SessionToken)\nexport AWS_ACCESS_KEY_ID=\(.AccessKeyId)\nexport AWS_SECRET_ACCESS_KEY=\(.SecretAccessKey) "') || exit 1
$(aws ecr get-login --no-include-email --region $AWS_DEFAULT_REGION) || exit 1
./gradlew docker dockerTag dockerTagsPush publish -PTAG=$TRAVIS_BUILD_NUMBER -PREPOSITORY_URI=$REPOSITORY_URI || exit 1

# Only tag latest when we are building master
if [ "$TRAVIS_BRANCH" == "master" ]; then
  ./gradlew dockerPush -PTAG=$TRAVIS_BUILD_NUMBER -PREPOSITORY_URI=$REPOSITORY_URI || exit 1
fi
