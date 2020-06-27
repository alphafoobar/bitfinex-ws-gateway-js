module "container_definition" {
  source = "git::https://github.com/bnc-projects/terraform-ecs-container-definition.git?ref=1.0.0"
  environment = [
    {
      name = "environment",
      value = terraform.workspace
    }
  ]
  healthCheck = {
    "command" = [
      "CMD-SHELL",
      "curl --silent --fail --max-time 30 http://localhost:7681/ping.html || exit 1"
    ],
    "interval" = 30,
    "retries" = 3,
    "startPeriod" = 5,
    "timeout": 5
  }
    logConfiguration  = {
      "logDriver": "awslogs",
      "options": {
        "awslogs-region"        = var.aws_region
        "awslogs-group"         = "/ecs/service/${local.service_name}"
      }
    }
  image = format("%s:%s", data.terraform_remote_state.ecr.outputs.repository_url, var.service_version)
  name = local.service_name
  cpu = var.cpu
  memory = var.memory
  memoryReservation = var.memory_reservation
  portMappings = [
    {
      containerPort = 7681
      hostPort = 0,
      protocol = "tcp"
    }
  ]
}

resource "aws_ecs_task_definition" "task_definition" {
  container_definitions = "[${module.container_definition.container_definition}]"
  family = local.service_name
  cpu = var.cpu
  memory = var.memory
  execution_role_arn = aws_iam_role.execution_task_role.arn
  task_role_arn = aws_iam_role.task_role.arn
  tags = local.tags
}

resource "aws_ecs_service" "ec2_service" {
  name = local.service_name
  cluster = data.terraform_remote_state.ops.outputs.ecs_name
  desired_count = 1
  health_check_grace_period_seconds = 120
  iam_role = aws_iam_role.iam_role_for_ec2_service.arn
  task_definition = aws_ecs_task_definition.task_definition.arn
  launch_type = "EC2"
  scheduling_strategy = "REPLICA"
  enable_ecs_managed_tags = false
  propagate_tags = "TASK_DEFINITION"

  ordered_placement_strategy {
    type  = "binpack"
    field = "cpu"
  }

  load_balancer {
    target_group_arn = aws_lb_target_group.target_group.arn
    container_name = local.service_name
    container_port = 7681
  }

  lifecycle {
    ignore_changes = [
      desired_count
    ]
    create_before_destroy = true
  }

  tags = local.tags
}

