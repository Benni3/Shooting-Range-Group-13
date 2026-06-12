from dataclasses import dataclass


@dataclass
class DownstreamCommand:
    command: str
    value: str | int | float | None = None


def set_target_position(value: float):
    return DownstreamCommand("SET_TARGET_POSITION", value)


def set_mode(mode: str):
    return DownstreamCommand("SET_MODE", mode)


def emergency_stop():
    return DownstreamCommand("EMERGENCY_STOP")


def clear_emergency_stop():
    return DownstreamCommand("CLEAR_EMERGENCY_STOP")


def set_pwm(value: int):
    return DownstreamCommand("SET_PWM", value)


def set_direction(direction: str):
    return DownstreamCommand("SET_DIRECTION", direction)


def recalibrate_zero():
    return DownstreamCommand("RECALIBRATE_ZERO")


def set_length(value: float):
    return DownstreamCommand("SET_LENGTH", value)


def ping():
    return DownstreamCommand("PING")