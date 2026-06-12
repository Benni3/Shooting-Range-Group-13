from Bridge.upstream import UpstreamState
from Bridge.downstream import DownstreamCommand


def encode_command(command: DownstreamCommand) -> str:
    if command.value is None:
        return command.command

    return f"{command.command}:{command.value}"


def parse_upstream(line: str) -> UpstreamState:
    """
    Example MC line:

    POS=42.20,TARGET=55.50,HSPD=0.30,VSPD=0.00,ASPD=12.0,VOLT=11.9,CUR=0.4,PWM=120,MODE=FAST,DIR=FORWARD,EMG=0
    """

    state = UpstreamState()

    parts = line.strip().split(",")

    for part in parts:
        if "=" not in part:
            continue

        key, value = part.split("=", 1)
        key = key.strip()
        value = value.strip()

        if key == "POS":
            state.actual_position = float(value)
        elif key == "TARGET":
            state.target_position = float(value)
        elif key == "HSPD":
            state.horizontal_speed = float(value)
        elif key == "VSPD":
            state.vertical_speed = float(value)
        elif key == "ASPD":
            state.angular_speed = float(value)
        elif key == "VOLT":
            state.voltage = float(value)
        elif key == "CUR":
            state.current = float(value)
        elif key == "PWM":
            state.pwm = int(float(value))
        elif key == "MODE":
            state.mode = value
        elif key == "DIR":
            state.direction = value
        elif key == "EMG":
            state.emergency_active = value == "1"
        elif key == "TEMP":
            state.motor_temperature = float(value)
        elif key == "RPOS":
            state.rotational_position = float(value)
        elif key == "RACC":
            state.angular_acceleration = float(value)
        elif key == "HACC":
            state.horizontal_acceleration = float(value)

    state.fast_active = state.mode == "FAST"
    state.slow_active = state.mode == "SLOW"
    state.stop_active = state.mode == "IDLE" or state.direction == "STOP"
    state.forward_active = state.direction == "FORWARD"
    state.back_active = state.direction == "BACK"

    return state