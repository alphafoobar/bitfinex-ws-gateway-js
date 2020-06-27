resource "aws_lb_target_group" "target_group" {
  deregistration_delay = 30
  health_check {
    healthy_threshold = 3
    interval = 30
    matcher = "200-299"
    path = "/ping.html"
    protocol = "HTTP"
    timeout = 5
    unhealthy_threshold = 2
  }
  name = substr("tg-${local.service_name}", 0, min(length("tg-${local.service_name}"), 32))
  protocol = "HTTP"
  port = 80
  vpc_id = data.terraform_remote_state.ops.outputs.vpc_id
  tags = var.tags
  target_type = "instance"
}

resource "aws_lb_listener_rule" "https_listener_rule" {
  action {
    type = "forward"
    target_group_arn = aws_lb_target_group.target_group.arn
  }

  condition {
    path_pattern {
      values = [
        "/*",
      ]
    }
  }
  priority = 1
  listener_arn = data.terraform_remote_state.ops.outputs.public_lb_listener.arn
}