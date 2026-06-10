class Microcontroller:
    def __init__(self, serial_connection):
        self.ser = serial_connection

    def send(self, command: str):
        self.ser.write((command + "\n").encode())

    def read(self):
        return self.ser.readline().decode(errors="ignore").strip()

    def is_connected(self):
        return self.ser is not None and self.ser.is_open

    def close(self):
        if self.ser and self.ser.is_open:
            self.ser.close()