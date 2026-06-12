from Bridge.downstream import DownstreamCommand
from Bridge.protocol import encode_command, parse_upstream
from Bridge.upstream import UpstreamState


class Microcontroller:
    def __init__(self, serial_connection):
        self.ser = serial_connection

        # Important: never let readline block NiceGUI
        if self.ser is not None:
            self.ser.timeout = 0

    def send(self, command: str):
        if not self.is_connected():
            return

        message = command.strip() + "\n"
        self.ser.write(message.encode())
        self.ser.flush()

    def send_command(self, command: DownstreamCommand):
        self.send(encode_command(command))

    def read(self) -> str:
        if not self.is_connected():
            return ""

        if self.ser.in_waiting <= 0:
            return ""

        return self.ser.readline().decode(errors="ignore").strip()

    def read_state(self) -> UpstreamState | None:
        while self.is_connected() and self.ser.in_waiting > 0:
            line = self.read()

            if not line:
                return None

            if line.startswith("POS="):
                return parse_upstream(line)

        return None

    def is_connected(self) -> bool:
        return self.ser is not None and self.ser.is_open

    def close(self):
        if self.ser and self.ser.is_open:
            self.ser.close()