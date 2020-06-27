data "aws_iam_policy_document" "ecs-log-publishing-policy" {
  statement {
    actions = [
      "logs:CreateLogStream",
      "logs:PutLogEvents",
      "logs:PutLogEventsBatch",
    ]

    resources = ["arn:aws:logs:*"]

    principals {
      identifiers = ["ecs.amazonaws.com"]
      type        = "Service"
    }
  }
}

resource "aws_cloudwatch_log_resource_policy" "ecs-log-publishing-policy" {
  policy_document = data.aws_iam_policy_document.ecs-log-publishing-policy.json
  policy_name     = "ecs-log-publishing-policy"
}

resource "aws_cloudwatch_log_group" "ec2-service" {
  name = "/ecs/service/${local.service_name}"
  retention_in_days = 5
  tags = local.tags
}
