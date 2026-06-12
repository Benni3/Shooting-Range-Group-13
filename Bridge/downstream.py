from dataclasses import dataclass


@dataclass
class DownstreamCommand:
    command: str
    value: str | int | float | None = None


def set_target_position(value: float) -> DownstreamCommand:
    return DownstreamCommand("SET_TARGET_POSITION", value)


def set_mode(mode: str) -> DownstreamCommand:
    return DownstreamCommand("SET_MODE", mode)


def emergency_stop() -> DownstreamCommand:
    return DownstreamCommand("EMERGENCY_STOP")


def clear_emergency_stop() -> DownstreamCommand:
    return DownstreamCommand("CLEAR_EMERGENCY_STOP")


def set_pwm(value: int) -> DownstreamCommand:
    return DownstreamCommand("SET_PWM", value)


def set_direction(direction: str) -> DownstreamCommand:
    return DownstreamCommand("SET_DIRECTION", direction)


def recalibrate_zero() -> DownstreamCommand:
    return DownstreamCommand("RECALIBRATE_ZERO")


def set_length(value: float) -> DownstreamCommand:
    return DownstreamCommand("SET_LENGTH", value)


def ping() -> DownstreamCommand:
    return DownstreamCommand("PING")