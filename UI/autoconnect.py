from serial.tools import list_ports
import serial
import time

PROJECT_ID = "DTU_CONTROLLER"


def find_arduino_ports():
    ports_found = []

    print("Scanning ports...")

    for port in list_ports.comports():
        print("FOUND:", port.device, port.description, port.manufacturer, port.vid, port.pid)

        device = (port.device or "").lower()
        description = (port.description or "").lower()
        manufacturer = (port.manufacturer or "").lower()

        if "usbmodem" in device:
            ports_found.append(port.device)
        elif "arduino" in description:
            ports_found.append(port.device)
        elif "arduino" in manufacturer:
            ports_found.append(port.device)
        elif "usb" in description:
            ports_found.append(port.device)

    print("Candidate ports:", ports_found)
    return ports_found


def check_handshake(ser):
    try:
        time.sleep(2)

        ser.reset_input_buffer()
        ser.reset_output_buffer()

        for _ in range(5):
            print("Sending handshake...")
            ser.write(b"WHO_ARE_YOU?\n")
            time.sleep(0.5)

            while ser.in_waiting > 0:
                reply = ser.readline().decode(errors="ignore").strip()
                print("Handshake reply:", repr(reply))

                if reply == PROJECT_ID:
                    return True

        return False

    except Exception as e:
        print("Handshake error:", e)
        return False


def connect_arduino(baudrate=9600, timeout=2, retry=False, retry_delay=2):
    while True:
        ports = find_arduino_ports()

        for port in ports:
            try:
                print("Trying port:", port)

                ser = serial.Serial(
                    port,
                    baudrate=baudrate,
                    timeout=timeout,
                )

                if check_handshake(ser):
                    print("Connected:", port)
                    return ser

                print("Handshake failed on:", port)
                ser.close()

            except Exception as e:
                print("Failed to open:", port, e)

        if not retry:
            return None

        time.sleep(retry_delay)