data "aws_iam_policy_document" "task_service_assume_role" {
  statement {
    sid = "AllowECSTaskToAssumeRole"
    effect = "Allow"

    actions = [
      "sts:AssumeRole"
    ]

    principals {
      type = "Service"
      identifiers = [
        "ecs-tasks.amazonaws.com"
      ]
    }
  }
}

data "aws_iam_policy_document" "permissions" {
  statement {
    sid    = "AllowDescribeTargetGroup"
    effect = "Allow"

    actions = [
      "elasticloadbalancing:DescribeTargetHealth"
    ]

    resources = [
      "*"
    ]
  }

  statement {
    sid    = "AllowCloudwatchMetrics"
    effect = "Allow"

    actions = [
      "cloudwatch:PutMetricData",
      "cloudwatch:PutMetricData",
      "cloudwatch:PutMetricData"
    ]

    resources = [
      "*"
    ]
  }
}

// Add additional permissions to this role if required
resource "aws_iam_role" "task_role" {
  name = format("%s-task", local.service_name)
  assume_role_policy = data.aws_iam_policy_document.task_service_assume_role.json
  tags = local.tags
}

resource "aws_iam_role_policy" "permissions" {
  name   = "allow_permissions"
  role   = aws_iam_role.task_role.id
  policy = data.aws_iam_policy_document.permissions.json
}

data "aws_iam_policy" "execution_policy" {
  arn = "arn:aws:iam::aws:policy/service-role/AmazonECSTaskExecutionRolePolicy"
}

resource "aws_iam_role" "execution_task_role" {
  name = format("%s-execution", local.service_name)
  assume_role_policy = data.aws_iam_policy_document.task_service_assume_role.json
  tags = local.tags
}

resource "aws_iam_role_policy_attachment" "ecs_task_default_policy" {
  role = aws_iam_role.execution_task_role.name
  policy_arn = data.aws_iam_policy.execution_policy.arn
}

# Ec2 service iam role
data "aws_iam_policy_document" "service_assume_role" {
  statement {
    sid    = "AllowECSToAssumeRoles"
    effect = "Allow"

    actions = [
      "sts:AssumeRole"
    ]

    principals {
      type        = "Service"
      identifiers = [
        "ecs.amazonaws.com"
      ]
    }
  }
}

resource "aws_iam_role" "iam_role_for_ec2_service" {
  name               = "iam_role_for_ec2_service_${local.service_name}"
  assume_role_policy = data.aws_iam_policy_document.service_assume_role.json
  tags               = local.tags
}

resource "aws_iam_role_policy_attachment" "ecs_service_policy" {
  role       = aws_iam_role.iam_role_for_ec2_service.name
  policy_arn = "arn:aws:iam::aws:policy/service-role/AmazonEC2ContainerServiceRole"
}