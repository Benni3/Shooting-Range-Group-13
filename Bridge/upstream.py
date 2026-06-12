from dataclasses import dataclass


@dataclass
class UpstreamState:
    connected: bool = False

    actual_position: float = 0.0
    target_position: float = 0.0

    horizontal_speed: float = 0.0
    vertical_speed: float = 0.0

    horizontal_acceleration: float = 0.0

    angular_speed: float = 0.0
    rotational_position: float = 0.0
    angular_acceleration: float = 0.0

    voltage: float = 0.0
    current: float = 0.0
    pwm: int = 0

    mode: str = "IDLE"
    direction: str = "STOP"

    stop_active: bool = False
    slow_active: bool = False
    fast_active: bool = False
    back_active: bool = False
    forward_active: bool = False
    emergency_active: bool = False

    motor_temperature: float = 0.0
    