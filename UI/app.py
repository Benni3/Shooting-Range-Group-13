from nicegui import ui
from datetime import datetime
import random

import sys
from pathlib import Path

ROOT_DIR = Path(__file__).resolve().parents[1]
sys.path.append(str(ROOT_DIR))

from Bridge.autoconnect import connect_arduino
from Bridge.serial_control import Microcontroller

COLORS = {
    "pink": "#ffadad",
    "orange": "#ffd6a5",
    "yellow": "#fdffb6",
    "green": "#caffbf",
    "cyan": "#9bf6ff",
    "blue": "#a0c4ff",
    "purple": "#bdb2ff",
    "magenta": "#ffc6ff",
    "white": "#fffffc",
    "black": "#000000",
}

TEXT = "#1f2937"
MUTED = "#6b7280"


class ShootingRangeUI:
    def __init__(self):
        self.mcu = None

        self.connected = False
        self.admin_mode = False
        self.emergency_active = False

        self.target_position = 50.00
        self.actual_position = 50.0
        self.rail_length = 90.00

        self.speed = 0.0
        self.horizontal_speed = 0.0
        self.angular_speed = 0.0
        self.vertical_speed = 0.0

        self.pwm_value = 0
        self.mode = "IDLE"
        self.direction = "STOP"
        self.voltage = 12.0
        self.current = 0.0

        self.chart_time = []
        self.voltage_data = []
        self.current_data = []
        self.position_data = []
        self.horizontal_speed_data = []
        self.vertical_speed_data = []

        self.pwm_data = []
        self.angular_speed_data = []

        self.build_ui()
        ui.timer(0.8, self.try_connect, once=True)
        ui.timer(0.5, self.update_visuals)
        ui.timer(1.5, self.check_connection)

    def send_to_bridge(self, command: str, value=None):
        print(f"BRIDGE OUT: {command}", value)

    def check_handshake(self) -> bool:
        if self.mcu and self.mcu.is_connected():
            try:
                self.mcu.send("PING")
                reply = self.mcu.read()

                print("PING:", repr(reply))

                return reply and reply.strip() == "PONG"

            except Exception:
                self.mcu = None
                return False

        return False

    def check_connection(self):
        if self.mcu and self.mcu.is_connected():
            self.connected = self.check_handshake()

            if not self.connected:
                try:
                    self.mcu.close()
                except Exception:
                    pass

                self.mcu = None

        else:
            self.connected = False
            self.mcu = None

            ser = connect_arduino(
                baudrate=9600,
                timeout=2,
                retry=False,
            )

            if ser:
                self.mcu = Microcontroller(serial_connection=ser)
                self.connected = True

        self.update_connection_ui()

    def try_connect(self):
        self.header_status_text.text = "Trying handshake..."

        ser = connect_arduino(
            baudrate=9600,
            timeout=2,
            retry=False,
        )

        if ser:
            self.mcu = Microcontroller(serial_connection=ser)
            self.connected = True
            self.header_status_text.text = f"Connected"
            self.connect_button.visible = False
            self.manual_box.visible = False
        else:
            self.connected = False
            self.header_status_text.text = "Auto failed. Enter port."
            self.connect_button.visible = False
            self.manual_box.visible = True

        self.update_connection_ui()

    def manual_connect(self):
        import serial

        port = self.port_input.value

        try:
            ser = serial.Serial(port, baudrate=9600, timeout=2)
            self.mcu = Microcontroller(serial_connection=ser)

            if self.check_handshake():
                self.connected = True
                self.header_status_text.text = "Connected"
                self.connect_button.visible = False
                self.manual_box.visible = False
            else:
                self.connected = False
                self.header_status_text.text = "Handshake failed"

        except Exception as e:
            self.connected = False
            self.header_status_text.text = f"Manual failed: {e}"

        self.update_connection_ui()

    def update_connection_ui(self):
        text = "Connected" if self.connected else "Not connected"
        color = COLORS["green"] if self.connected else COLORS["pink"]

        self.header_status_text.text = text
        self.header_connection_dot.style(f"background:{color};")

        if self.admin_mode:
            if self.connected:
                self.connect_button.visible = False
                self.manual_box.visible = False
            elif not self.manual_box.visible:
                self.connect_button.visible = True
        else:
            self.connect_button.visible = False
            self.manual_box.visible = False

    def set_position(self, value):
        self.target_position = round(float(value), 2)
        self.target_position = max(0, min(self.rail_length, self.target_position))

        self.position_slider.value = self.target_position
        self.position_input.value = f"{self.target_position:.2f}"
        self.target_position_text.text = f"{self.target_position:.2f}"

        self.admin_position_slider.value = self.target_position
        self.admin_position_input.value = f"{self.target_position:.2f}"
        self.admin_target_position_text.text = f"{self.target_position:.2f}"

        self.send_to_bridge("SET_TARGET_POSITION", self.target_position)

    def set_position_from_input(self):
        try:
            value = float(self.position_input.value)
            value = max(0, min(self.rail_length, value))
            self.set_position(value)
        except ValueError:
            ui.notify("Enter a valid position", color="negative")

    def set_admin_position_from_input(self):
        try:
            value = float(self.admin_position_input.value)
            value = max(0, min(self.rail_length, value))
            self.set_position(value)
        except ValueError:
            ui.notify("Enter a valid position", color="negative")

    def step_position(self, amount):
        value = max(0, min(self.rail_length, self.target_position + amount))
        self.set_position(value)

    def recalibrate_zero(self):
        self.target_position = 0.00
        self.actual_position = 0.00
        self.send_to_bridge("RECALIBRATE_ZERO")
        self.set_position(0.00)
        ui.notify("Recalibrated zero position", color="positive")

    def open_length_dialog(self):
        self.length_input.value = f"{self.rail_length:.2f}"
        self.length_dialog.open()

    def confirm_set_length(self):
        try:
            length = float(self.length_input.value)

            if length <= 0:
                ui.notify("Length must be positive", color="negative")
                return

            if length < self.target_position:
                ui.notify("Length cannot be shorter than current target position", color="negative")
                return

            self.rail_length = round(length, 2)

            self.position_slider.props(f"max={self.rail_length}")
            self.admin_position_slider.props(f"max={self.rail_length}")

            self.send_to_bridge("SET_LENGTH", self.rail_length)
            self.length_dialog.close()
            ui.notify(f"Length set to {self.rail_length:.2f}", color="warning")

        except ValueError:
            ui.notify("Enter a valid length", color="negative")

    def set_mode(self, mode: str):
        self.mode = mode
        self.send_to_bridge("SET_MODE", mode)
        self.update_leds()

    def emergency_stop(self):
        self.emergency_active = not self.emergency_active

        if self.emergency_active:
            self.mode = "EMERGENCY_STOP"
            self.direction = "STOP"
            self.pwm_value = 0
            self.send_to_bridge("EMERGENCY_STOP")

            emergency_html = '<button class="mode-button mode-button-red">⚠ REMOVE FUSE</button>'
            self.emergency_button.content = emergency_html
            self.admin_emergency_button.content = emergency_html
        else:
            self.mode = "IDLE"
            self.send_to_bridge("CLEAR_EMERGENCY_STOP")

            emergency_html = '<button class="mode-button mode-button-red">⚠ EMERGENCY STOP</button>'
            self.emergency_button.content = emergency_html
            self.admin_emergency_button.content = emergency_html

        self.update_leds()

    def set_pwm(self, value):
        self.pwm_value = int(value)
        self.pwm_text.text = str(self.pwm_value)
        self.pwm_live_text.text = f"{self.pwm_value} / 255"
        self.admin_pwm_slider.value = self.pwm_value
        self.send_to_bridge("SET_PWM", self.pwm_value)

    def set_pwm_percent(self, percent):
        self.set_pwm(round(255 * percent / 100))

    def set_direction(self, direction: str):
        self.direction = direction
        self.send_to_bridge("SET_DIRECTION", direction)
        self.update_leds()

    def build_ui(self):
        ui.add_head_html(f"""
        <style>
            body {{
                background: {COLORS["white"]};
                color: {TEXT};
                font-family: Inter, ui-sans-serif, system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
            }}

            .page {{
                width: 100%;
                max-width: 1800px;
                margin: 0 auto;
                padding: 36px 56px;
            }}

            .card {{
                background: rgba(255,255,255,0.94);
                border: 1px solid rgba(31,41,55,0.08);
                border-radius: 28px;
                box-shadow: 0 18px 44px rgba(31,41,55,0.08);
            }}

            .title {{
                font-size: 34px;
                font-weight: 950;
                letter-spacing: -0.045em;
                color: {TEXT};
            }}

            .subtitle {{
                color: {MUTED};
                font-size: 16px;
                font-weight: 500;
            }}

            .label-dark {{
                color: {TEXT};
                font-weight: 800;
            }}

            .dot {{
                width: 34px;
                height: 34px;
                border-radius: 999px;
                display: inline-block;
                box-shadow: 0 8px 20px rgba(31,41,55,0.14);
                border: 2px solid rgba(31,41,55,0.08);
            }}

            .led-dot {{
                width: 24px;
                height: 24px;
                border-radius: 999px;
                display: inline-block;
                background: #e5e7eb;
                box-shadow: inset 0 0 0 2px rgba(31,41,55,0.08);
            }}

            .soft-btn {{
                border-radius: 16px;
                font-weight: 900;
                min-height: 48px;
                color: {TEXT} !important;
                box-shadow: 0 10px 22px rgba(31,41,55,0.10);
            }}

            .mode-button {{
                width: 100%;
                height: 68px;
                border-radius: 18px;
                border: none;
                color: white;
                font-size: 20px;
                font-weight: 950;
                cursor: pointer;
                box-shadow: 0 12px 26px rgba(31,41,55,0.18);
            }}

            .mode-button-blue {{
                background: #5b9bd5;
            }}

            .mode-button-red {{
                background: #dc2626;
                border: 5px solid #7f1d1d;
                box-shadow: 0 0 0 8px rgba(220,38,38,0.22), 0 16px 36px rgba(220,38,38,0.35);
            }}

            .mode-button-red:hover {{
                background: #b91c1c;
            }}

            .metric {{
                font-size: 34px;
                font-weight: 950;
                color: {TEXT};
                letter-spacing: -0.04em;
            }}

            .q-field__native, .q-field__label {{
                color: {TEXT} !important;
            }}

            .q-slider__track-container {{
                height: 10px !important;
            }}

            .graph-box {{
                min-height: 360px;
            }}
        </style>
        """)

        self.password_dialog = ui.dialog()
        with self.password_dialog, ui.card().classes("card p-6"):
            ui.label("Admin access").classes("text-2xl label-dark")
            self.password_input = ui.input(
                label="Password",
                password=True,
                password_toggle_button=True,
            ).classes("w-72")
            ui.button("Open admin", on_click=self.check_admin_password).classes("soft-btn").style(
                f"background:{COLORS['purple']};"
            )

        self.length_dialog = ui.dialog()
        with self.length_dialog, ui.card().classes("card p-6"):
            ui.label("Set rail length").classes("text-2xl label-dark")
            ui.label("Important: do not set this longer than the real physical rail.").classes("subtitle")
            ui.label("Only update this if you are completely sure.").classes("subtitle")

            self.length_input = ui.input(
                label="New length",
                value=f"{self.rail_length:.2f}",
            ).props("type=number step=0.01 outlined").classes("w-72")

            with ui.row().classes("gap-3 mt-4"):
                ui.button("Cancel", on_click=self.length_dialog.close).classes("soft-btn").style(
                    f"background:{COLORS['cyan']};"
                )

                ui.button("I am sure — set length", on_click=self.confirm_set_length).classes("soft-btn").style(
                    f"background:{COLORS['pink']};"
                )

        with ui.column().classes("page gap-5"):

            with ui.row().classes("w-full items-center justify-between"):
                with ui.column().classes("gap-0"):
                    ui.label("DTU Shooting Range").classes("title")
                    ui.label("Position control dashboard").classes("subtitle")

                with ui.row().classes("items-center gap-3"):
                    self.header_status_text = ui.label("Not connected").classes("subtitle")

                    self.connect_button = ui.button(
                        "Connect",
                        on_click=self.try_connect,
                    ).classes("soft-btn").style(f"background:{COLORS['blue']};")
                    self.connect_button.visible = False

                    self.manual_box = ui.row().classes("items-center gap-2")
                    with self.manual_box:
                        self.port_input = ui.input(
                            placeholder="/dev/cu.usbmodem11302",
                        ).props("dense outlined").classes("w-64")
                        ui.button("Connect", on_click=self.manual_connect).classes("soft-btn").style(
                            f"background:{COLORS['purple']};"
                        )
                    self.manual_box.visible = False

                    self.header_connection_dot = ui.element("span").classes("dot").style(
                        f"background:{COLORS['pink']};"
                    )

            with ui.card().classes("card w-full p-5"):
                with ui.row().classes("w-full items-center justify-between"):
                    self.led_stop = self.led("Stop", COLORS["pink"])
                    self.led_slow = self.led("Slow", COLORS["yellow"])
                    self.led_fast = self.led("Fast", COLORS["blue"])
                    self.led_back = self.led("Back", COLORS["purple"])
                    self.led_forward = self.led("Forward", COLORS["green"])
                    self.led_emergency = self.led("Emergency", COLORS["orange"])

            self.user_panel = ui.column().classes("w-full gap-5")
            with self.user_panel:
                with ui.card().classes("card w-full p-8"):
                    with ui.column().classes("w-full gap-6"):

                        with ui.row().classes("w-full items-center justify-between"):
                            with ui.column().classes("gap-1"):
                                ui.label("Target position").classes("text-2xl label-dark")
                                ui.label("Use the slider or enter an exact value.").classes("subtitle")

                            with ui.column().classes("items-end gap-0"):
                                ui.label("Target").classes("subtitle")
                                self.target_position_text = ui.label(f"{self.target_position:.2f}").classes("metric")

                        with ui.grid(columns="1fr 160px 90px").classes("w-full items-end gap-5"):
                            self.position_slider = ui.slider(
                                min=0,
                                max=self.rail_length,
                                value=self.target_position,
                                on_change=lambda e: self.set_position(e.value),
                            ).props("step=0.01").classes("w-full")

                            self.position_input = ui.input(
                                label="Exact",
                                value=f"{self.target_position:.2f}",
                            ).props("type=number step=0.01 outlined dense").classes("w-full")

                            ui.button("SET", on_click=self.set_position_from_input).classes("soft-btn w-full").style(
                                f"background:{COLORS['cyan']};"
                            )

                        with ui.grid(columns=3).classes("w-full gap-4"):
                            ui.html('<button class="mode-button mode-button-blue">FAST MODE</button>').on(
                                "click", lambda: self.set_mode("FAST")
                            )

                            ui.html('<button class="mode-button mode-button-blue">SLOW MODE</button>').on(
                                "click", lambda: self.set_mode("SLOW")
                            )

                            self.emergency_button = ui.html(
                                '<button class="mode-button mode-button-red">⚠ EMERGENCY STOP</button>'
                            ).on("click", lambda: self.emergency_stop())

                with ui.card().classes("card w-full p-7"):
                    with ui.row().classes("w-full justify-between"):
                        self.state_card = self.small_metric("State", "IDLE", COLORS["magenta"])
                        self.actual_card = self.small_metric("Actual", "50.0", COLORS["green"])
                        self.speed_user_card = self.small_metric("Speed", "0.0", COLORS["cyan"])
                        self.direction_card = self.small_metric("Direction", "STOP", COLORS["purple"])

            self.admin_panel = ui.column().classes("w-full gap-5")
            self.admin_panel.visible = False

            with self.admin_panel:
                with ui.card().classes("card w-full p-8"):
                    ui.label("Target position").classes("text-2xl label-dark")
                    ui.label("Fine control before diagnostics.").classes("subtitle")

                    with ui.row().classes("w-full items-center justify-between mt-4"):
                        with ui.column().classes("items-start gap-0"):
                            ui.label("Target").classes("subtitle")
                            self.admin_target_position_text = ui.label(f"{self.target_position:.2f}").classes("metric")

                    with ui.grid(columns="120px 1fr 160px 90px 120px").classes("w-full items-end gap-5 mt-6"):
                        ui.button("← 0.10", on_click=lambda: self.step_position(-0.10)).classes("soft-btn w-full").style(
                            f"background:{COLORS['purple']};"
                        )

                        self.admin_position_slider = ui.slider(
                            min=0,
                            max=self.rail_length,
                            value=self.target_position,
                            on_change=lambda e: self.set_position(e.value),
                        ).props("step=0.01").classes("w-full")

                        self.admin_position_input = ui.input(
                            label="Exact",
                            value=f"{self.target_position:.2f}",
                        ).props("type=number step=0.01 outlined dense").classes("w-full")

                        ui.button("SET", on_click=self.set_admin_position_from_input).classes("soft-btn w-full").style(
                            f"background:{COLORS['cyan']};"
                        )

                        ui.button("0.10 →", on_click=lambda: self.step_position(0.10)).classes("soft-btn w-full").style(
                            f"background:{COLORS['green']};"
                        )

                    with ui.grid(columns=3).classes("w-full gap-4 mt-6"):
                        ui.html('<button class="mode-button mode-button-blue">FAST MODE</button>').on(
                            "click", lambda: self.set_mode("FAST")
                        )

                        ui.html('<button class="mode-button mode-button-blue">SLOW MODE</button>').on(
                            "click", lambda: self.set_mode("SLOW")
                        )

                        self.admin_emergency_button = ui.html(
                            '<button class="mode-button mode-button-red">⚠ EMERGENCY STOP</button>'
                        ).on("click", lambda: self.emergency_stop())

                    with ui.grid(columns=2).classes("w-full gap-5 mt-6"):
                        self.admin_position_chart = self.chart_card(
                            "Horizontal position",
                            "Actual position over time",
                            "pos",
                        )
                        self.admin_horizontal_speed_chart = self.chart_card(
                            "Horizontal speed",
                            "Positive forward, negative backward",
                            "speed",
                        )

                with ui.card().classes("card w-full p-8"):
                    ui.label("Admin Console").classes("text-3xl label-dark")
                    ui.label("Diagnostics, position tuning and manual motor control.").classes("subtitle")

                    with ui.row().classes("w-full justify-between mt-8"):
                        self.admin_state_card = self.small_metric("State", "IDLE", COLORS["magenta"])
                        self.admin_actual_card = self.small_metric("Actual", "50.0", COLORS["green"])
                        self.admin_horizontal_speed_card = self.small_metric("Horizontal speed", "0.0", COLORS["cyan"])
                        self.admin_direction_card = self.small_metric("Direction", "STOP", COLORS["purple"])

                with ui.card().classes("card w-full p-8"):
                    ui.label("Manual motor control").classes("text-2xl label-dark")
                    ui.label("Set motor output power and direction directly from the admin console.").classes("subtitle")

                    with ui.row().classes("w-full items-center justify-between mt-6"):
                        with ui.column().classes("gap-0"):
                            ui.label("PWM output").classes("label-dark")
                            self.pwm_live_text = ui.label("0 / 255").classes("metric")

                        with ui.row().classes("gap-3"):
                            ui.button("0%", on_click=lambda: self.set_pwm_percent(0)).classes("soft-btn").style(
                                f"background:{COLORS['pink']};"
                            )
                            ui.button("25%", on_click=lambda: self.set_pwm_percent(25)).classes("soft-btn").style(
                                f"background:{COLORS['orange']};"
                            )
                            ui.button("50%", on_click=lambda: self.set_pwm_percent(50)).classes("soft-btn").style(
                                f"background:{COLORS['yellow']};"
                            )
                            ui.button("75%", on_click=lambda: self.set_pwm_percent(75)).classes("soft-btn").style(
                                f"background:{COLORS['green']};"
                            )
                            ui.button("100%", on_click=lambda: self.set_pwm_percent(100)).classes("soft-btn").style(
                                f"background:{COLORS['cyan']};"
                            )

                    self.admin_pwm_slider = ui.slider(
                        min=0,
                        max=255,
                        value=0,
                        on_change=lambda e: self.set_pwm(e.value),
                    ).classes("w-full mt-4")

                    ui.label("Direction").classes("label-dark mt-6")
                    with ui.grid(columns=3).classes("w-full gap-4 mt-3"):
                        ui.button("← Back", on_click=lambda: self.set_direction("BACK")).classes("soft-btn").style(
                            f"background:{COLORS['purple']};"
                        )
                        ui.button("Stop", on_click=lambda: self.set_direction("STOP")).classes("soft-btn").style(
                            f"background:{COLORS['pink']};"
                        )
                        ui.button("Forward →", on_click=lambda: self.set_direction("FORWARD")).classes("soft-btn").style(
                            f"background:{COLORS['green']};"
                        )

                with ui.card().classes("card w-full p-8"):
                    ui.label("Live diagnostics").classes("text-2xl label-dark")

                    with ui.grid(columns=4).classes("w-full gap-4 mt-6"):
                        self.voltage_text = self.admin_metric("Voltage", "0.00 V", COLORS["yellow"])
                        self.current_text = self.admin_metric("Current", "0.00 A", COLORS["orange"])
                        self.pwm_text = self.admin_metric("PWM", "0", COLORS["blue"])
                        self.admin_speed_text = self.admin_metric("Angular speed", "0.00", COLORS["cyan"])

                    with ui.grid(columns=2).classes("w-full gap-5"):
                        self.voltage_chart = self.chart_card("Voltage", "Motor supply voltage", "V")
                        self.current_chart = self.chart_card("Current", "Ampere draw", "A")
                        self.pwm_chart = self.chart_card("PWM", "PWM output over time", "PWM")
                        self.angular_speed_chart = self.chart_card("Angular speed", "Estimated motor angular speed", "rad/s")


                with ui.row().classes("w-full gap-4 mt-6"):
                    ui.button("Recalibrate zero", on_click=self.recalibrate_zero).classes("soft-btn grow").style(
                        f"background:{COLORS['orange']};"
                    )

                    ui.button("Set length", on_click=self.open_length_dialog).classes("soft-btn grow").style(
                        f"background:{COLORS['pink']};"
                    )

        with ui.page_sticky(position="bottom-right", x_offset=24, y_offset=24):
            self.mode_button = ui.button(
                icon="settings",
                on_click=self.toggle_admin,
            ).props("round size=lg").style(
                f"background:{COLORS['purple']}; color:{TEXT};"
            )

        self.update_leds()
        self.update_connection_ui()

    def check_admin_password(self):
        if self.password_input.value == "1234":
            self.password_dialog.close()
            self.open_admin()
        else:
            ui.notify("Wrong password", color="negative")

    def open_admin(self):
        self.admin_mode = True
        self.user_panel.visible = False
        self.admin_panel.visible = True
        self.connect_button.visible = not self.connected
        self.manual_box.visible = False
        self.mode_button.props("icon=person")
        self.update_connection_ui()

    def open_user(self):
        self.admin_mode = False
        self.user_panel.visible = True
        self.admin_panel.visible = False
        self.connect_button.visible = False
        self.manual_box.visible = False
        self.mode_button.props("icon=settings")
        self.update_connection_ui()

    def toggle_admin(self):
        if self.admin_mode:
            self.open_user()
        else:
            self.password_input.value = ""
            self.password_dialog.open()

    def led(self, name, color):
        with ui.row().classes("items-center gap-2"):
            dot = ui.element("span").classes("led-dot")
            ui.label(name).classes("label-dark")
            return {"dot": dot, "color": color}

    def small_metric(self, title, value, color):
        with ui.card().classes("card p-5 grow"):
            ui.label(title).classes("subtitle")
            label = ui.label(value).classes("text-2xl label-dark")
            ui.element("div").classes("w-full h-2 rounded-full mt-3").style(f"background:{color};")
            return label

    def admin_metric(self, title, value, color):
        with ui.card().classes("card p-5"):
            ui.label(title).classes("subtitle")
            label = ui.label(value).classes("text-3xl label-dark")
            ui.element("div").classes("w-full h-2 rounded-full mt-3").style(f"background:{color};")
            return label

    def chart_card(self, title, subtitle, unit):
        option = {
            "title": {
                "text": title,
                "subtext": subtitle,
                "left": 10,
                "top": 8,
                "textStyle": {"color": TEXT, "fontSize": 18, "fontWeight": "bold"},
                "subtextStyle": {"color": MUTED, "fontSize": 12},
            },
            "tooltip": {"trigger": "axis"},
            "grid": {"left": 55, "right": 25, "top": 80, "bottom": 45},
            "xAxis": {
                "type": "category",
                "data": [],
                "axisLabel": {"color": MUTED},
                "axisLine": {"lineStyle": {"color": "#d1d5db"}},
            },
            "yAxis": {
                "type": "value",
                "name": unit,
                "nameTextStyle": {"color": MUTED},
                "axisLabel": {"color": MUTED},
                "splitLine": {"lineStyle": {"color": "#eef2f7"}},
            },
            "series": [
                {
                    "data": [],
                    "type": "line",
                    "smooth": True,
                    "symbol": "none",
                    "lineStyle": {"width": 3},
                    "areaStyle": {"opacity": 0.10},
                }
            ],
        }

        with ui.card().classes("card p-4 graph-box"):
            chart = ui.echart(option).classes("w-full h-96")
            return chart

    def update_leds(self):
        for led in [
            self.led_stop,
            self.led_slow,
            self.led_fast,
            self.led_back,
            self.led_forward,
            self.led_emergency,
        ]:
            led["dot"].style("background:#e5e7eb;")

        if self.mode == "FAST":
            self.led_fast["dot"].style(f"background:{self.led_fast['color']};")
        elif self.mode == "SLOW":
            self.led_slow["dot"].style(f"background:{self.led_slow['color']};")
        elif self.mode == "EMERGENCY_STOP":
            self.led_emergency["dot"].style(f"background:{self.led_emergency['color']};")
        else:
            self.led_stop["dot"].style(f"background:{self.led_stop['color']};")

        if self.direction == "FORWARD":
            self.led_forward["dot"].style(f"background:{self.led_forward['color']};")
        elif self.direction == "BACK":
            self.led_back["dot"].style(f"background:{self.led_back['color']};")

    def update_echart(self, chart, labels, values):
        chart.options["xAxis"]["data"] = labels
        chart.options["series"][0]["data"] = values
        chart.update()

    def update_visuals(self):
        old_position = self.actual_position

        self.actual_position += (self.target_position - self.actual_position) * 0.08

        raw_speed = (self.actual_position - old_position) * 20

        self.horizontal_speed = raw_speed

        if self.horizontal_speed > 0.01:
            self.direction = "FORWARD"
        elif self.horizontal_speed < -0.01:
            self.direction = "BACK"
        else:
            self.direction = "STOP"

        self.speed = self.horizontal_speed
        self.angular_speed = self.pwm_value / 255 * 100
        self.vertical_speed = random.uniform(-2.0, 2.0)

        self.voltage = 12.0 + random.uniform(-0.2, 0.2)
        self.current = abs(self.pwm_value / 255) * 2.2 + random.uniform(0, 0.08)
        self.pwm_data.append(round(self.pwm_value, 2))
        self.angular_speed_data.append(round(self.angular_speed, 2))

        now = datetime.now().strftime("%H:%M:%S")

        self.chart_time.append(now)
        self.voltage_data.append(round(self.voltage, 2))
        self.current_data.append(round(self.current, 2))
        self.position_data.append(round(self.actual_position, 2))
        self.horizontal_speed_data.append(round(self.horizontal_speed, 2))
        self.vertical_speed_data.append(round(self.vertical_speed, 2))

        self.chart_time = self.chart_time[-40:]
        self.voltage_data = self.voltage_data[-40:]
        self.current_data = self.current_data[-40:]
        self.position_data = self.position_data[-40:]
        self.horizontal_speed_data = self.horizontal_speed_data[-40:]
        self.vertical_speed_data = self.vertical_speed_data[-40:]
        self.pwm_data = self.pwm_data[-40:]
        self.angular_speed_data = self.angular_speed_data[-40:]

        self.state_card.text = self.mode
        self.actual_card.text = f"{self.actual_position:.1f}"
        self.speed_user_card.text = f"{self.speed:.2f}"
        self.direction_card.text = self.direction
        self.update_leds()

        self.admin_state_card.text = self.mode
        self.admin_actual_card.text = f"{self.actual_position:.1f}"
        self.admin_horizontal_speed_card.text = f"{self.horizontal_speed:.2f}"
        self.admin_direction_card.text = self.direction

        self.voltage_text.text = f"{self.voltage:.2f} V"
        self.current_text.text = f"{self.current:.2f} A"
        self.pwm_text.text = str(self.pwm_value)
        self.admin_speed_text.text = f"{self.angular_speed:.2f}"

        if self.admin_panel.visible:
            self.update_echart(self.admin_position_chart, self.chart_time, self.position_data)
            self.update_echart(self.admin_horizontal_speed_chart, self.chart_time, self.horizontal_speed_data)

            self.update_echart(self.voltage_chart, self.chart_time, self.voltage_data)
            self.update_echart(self.current_chart, self.chart_time, self.current_data)
            self.update_echart(self.pwm_chart, self.chart_time, self.pwm_data)
            self.update_echart(self.angular_speed_chart, self.chart_time, self.angular_speed_data)

ShootingRangeUI()

ui.run(
    title="DTU Shooting Range Control",
    reload=False,
    dark=False,
)