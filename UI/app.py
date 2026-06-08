import customtkinter as ctk
from serial_control import Microcontroller

ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")


class App(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.title("DTU Microcontroller Control")
        self.geometry("500x350")

        self.mcu = None

        self.port_entry = ctk.CTkEntry(self, placeholder_text="Serial port, e.g. /dev/tty.usbmodem101")
        self.port_entry.pack(pady=15, padx=20, fill="x")

        self.connect_button = ctk.CTkButton(self, text="Connect", command=self.connect)
        self.connect_button.pack(pady=10)

        self.led_on_button = ctk.CTkButton(self, text="LED ON", command=lambda: self.send("LED_ON"))
        self.led_on_button.pack(pady=10)

        self.led_off_button = ctk.CTkButton(self, text="LED OFF", command=lambda: self.send("LED_OFF"))
        self.led_off_button.pack(pady=10)

        self.status_label = ctk.CTkLabel(self, text="Not connected")
        self.status_label.pack(pady=20)

    def connect(self):
        port = self.port_entry.get()

        try:
            self.mcu = Microcontroller(port=port, baudrate=9600)
            self.status_label.configure(text=f"Connected to {port}")
        except Exception as e:
            self.status_label.configure(text=f"Connection failed: {e}")

    def send(self, command):
        if self.mcu:
            self.mcu.send(command)
            self.status_label.configure(text=f"Sent: {command}")
        else:
            self.status_label.configure(text="Not connected")


app = App()
app.mainloop()