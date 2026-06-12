from Bridge.downstream import DownstreamCommand
from Bridge.protocol import encode_command, parse_upstream
from Bridge.upstream import UpstreamState


class Microcontroller:
    def __init__(self, serial_connection):
        self.ser = serial_connection

    def send(self, command: str):
        if not self.is_connected():
            return

        self.ser.write((command + "\n").encode())

    def send_command(self, command: DownstreamCommand):
        message = encode_command(command)
        self.send(message)

    def read(self) -> str:
        if not self.is_connected():
            return ""

        return self.ser.readline().decode(errors="ignore").strip()

    def read_state(self) -> UpstreamState | None:
        line = self.read()

        if not line:
            return None

        if line in ("READY", "PONG", "DTU_CONTROLLER"):
            return None

        if "=" not in line:
            return None

        return parse_upstream(line)

    def is_connected(self) -> bool:
        return self.ser is not None and self.ser.is_open

    def close(self):
        if self.ser and self.ser.is_open:
            self.ser.close()