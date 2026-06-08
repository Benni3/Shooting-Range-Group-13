import serial
import time


class Microcontroller:
    def __init__(self, port="/dev/tty.usbmodem101", baudrate=9600):
        self.ser = serial.Serial(port, baudrate, timeout=1)
        time.sleep(2)

    def send(self, command: str):
        self.ser.write((command + "\n").encode())

    def read(self):
        return self.ser.readline().decode().strip()

    def close(self):
        self.ser.close()