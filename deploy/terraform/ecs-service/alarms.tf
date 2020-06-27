resource "aws_cloudwatch_metric_alarm" "service_not_healthy_alarm" {
  alarm_actions       = var.alarm_actions
  alarm_description   = format("%s service is below desired running count", local.service_name)
  alarm_name          = format("%s-not-healthy", local.service_name)
  comparison_operator = "LessThanThreshold"
  dimensions          = {
    LoadBalancer = data.terraform_remote_state.ops.outputs.public_lb_name
    TargetGroup  = aws_lb_target_group.target_group.arn_suffix
  }
  evaluation_periods  = 2
  metric_name         = "HealthyHostCount"
  namespace           = "AWS/ApplicationELB"
  period              = "60"
  statistic           = "Minimum"
  tags                = var.tags
  threshold           = 1
  treat_missing_data  = "breaching"
}

resource "aws_cloudwatch_metric_alarm" "service_cpu_utilization_alarm" {
  alarm_actions       = var.alarm_actions
  alarm_description   = format("%s service cpu utilization is greater than %s percent of reserved cpu", local.service_name, 90)
  alarm_name          = format("%s-cpu-utilization-alarm", local.service_name)
  comparison_operator = "GreaterThanThreshold"
  dimensions          = {
    ServiceName = local.service_name
    ClusterName = data.terraform_remote_state.ops.outputs.ecs_name
  }
  evaluation_periods  = 5
  metric_name         = "CPUUtilization"
  namespace           = "AWS/ECS"
  period              = "60"
  statistic           = "Average"
  tags                = var.tags
  threshold           = 90
  treat_missing_data  = "missing"
}

resource "aws_cloudwatch_metric_alarm" "service_memory_utilization_alarm" {
  alarm_actions       = var.alarm_actions
  alarm_description   = format("%s service memory utilization is greater than %s percent of reserved cpu", local.service_name, 90)
  alarm_name          = format("%s-memory-utilization-alarm", local.service_name)
  comparison_operator = "GreaterThanThreshold"
  dimensions          = {
    ServiceName = local.service_name
    ClusterName = data.terraform_remote_state.ops.outputs.ecs_name
  }
  evaluation_periods  = 5
  metric_name         = "MemoryUtilization"
  namespace           = "AWS/ECS"
  period              = "60"
  statistic           = "Average"
  tags                = var.tags
  threshold           = 90
  treat_missing_data  = "breaching"
}
