from serial.tools import list_ports
import serial
import time

ARDUINO_VIDS = {
    0x2341,
    0x2A03,
}

PROJECT_ID = "DTU_CONTROLLER"


def find_arduino_ports():
    ports_found = []

    for port in list_ports.comports():
        description = (port.description or "").lower()
        manufacturer = (port.manufacturer or "").lower()

        if port.vid in ARDUINO_VIDS:
            ports_found.append(port.device)
        elif "arduino" in description:
            ports_found.append(port.device)
        elif "arduino" in manufacturer:
            ports_found.append(port.device)
        elif "usb serial" in description:
            ports_found.append(port.device)
        elif "usb" in description:
            ports_found.append(port.device)

    return ports_found


def check_handshake(ser):
    try:
        ser.reset_input_buffer()
        ser.reset_output_buffer()

        ser.write(b"WHO_ARE_YOU?\n")
        time.sleep(0.2)

        reply = ser.readline().decode(errors="ignore").strip()

        return reply == PROJECT_ID

    except Exception:
        return False


def connect_arduino(
    baudrate=9600,
    timeout=1,
    retry=False,
    retry_delay=2,
):
    while True:
        ports = find_arduino_ports()

        for port in ports:
            try:
                ser = serial.Serial(
                    port,
                    baudrate=baudrate,
                    timeout=timeout,
                )

                time.sleep(2)

                if check_handshake(ser):
                    print(f"Connected to Arduino on {port}")
                    return ser

                ser.close()

            except Exception as e:
                print(f"Failed on {port}: {e}")

        if not retry:
            return None

        time.sleep(retry_delay)