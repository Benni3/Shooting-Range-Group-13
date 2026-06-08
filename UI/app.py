print("APP STARTED")

import customtkinter as ctk
from autoconnect import connect_arduino
from serial_control import Microcontroller

ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")


class App(ctk.CTk):
    def __init__(self):
        super().__init__()

        print("APP INIT")

        self.title("DTU Microcontroller Control")
        self.geometry("500x350")

        self.mcu = None

        self.header = ctk.CTkFrame(self, fg_color="transparent")
        self.header.pack(fill="x", padx=20, pady=(15, 5))

        self.title_label = ctk.CTkLabel(
            self.header,
            text="DTU Microcontroller Control",
            font=("Arial", 18, "bold"),
        )
        self.title_label.pack(side="left")

        self.status_light = ctk.CTkLabel(
            self.header,
            text="●",
            font=("Arial", 28),
            text_color="red",
        )
        self.status_light.pack(side="right")

        self.status_label = ctk.CTkLabel(self, text="Not connected")
        self.status_label.pack(pady=10)

        self.connect_button = ctk.CTkButton(
            self,
            text="Auto Connect",
            command=self.connect,
        )
        self.connect_button.pack(pady=10)

        self.led_on_button = ctk.CTkButton(
            self,
            text="LED ON",
            command=lambda: self.send("LED_ON"),
        )
        self.led_on_button.pack(pady=10)

        self.led_off_button = ctk.CTkButton(
            self,
            text="LED OFF",
            command=lambda: self.send("LED_OFF"),
        )
        self.led_off_button.pack(pady=10)

        print("SCHEDULING CONNECT")
        self.after(1000, self.connect)

    def set_connected(self, connected: bool, message: str):
        print("SET CONNECTED:", connected, message)

        self.status_light.configure(
            text_color="green" if connected else "red"
        )
        self.status_label.configure(text=message)

    def connect(self):
        print("CONNECT FUNCTION CALLED")

        if self.mcu and self.mcu.is_connected():
            print("Already connected")
            return

        ser = connect_arduino(
            baudrate=9600,
            timeout=2,
            retry=False,
        )

        print("SER RESULT:", ser)

        if ser:
            self.mcu = Microcontroller(serial_connection=ser)
            self.set_connected(True, f"Connected to {ser.port}")
        else:
            self.set_connected(False, "No Arduino found")
            self.after(2000, self.connect)

    def send(self, command):
        print("SEND:", command)

        if self.mcu and self.mcu.is_connected():
            self.mcu.send(command)
            self.status_label.configure(text=f"Sent: {command}")
        else:
            self.set_connected(False, "Not connected")
            self.after(2000, self.connect)


if __name__ == "__main__":
    app = App()
    app.mainloop()