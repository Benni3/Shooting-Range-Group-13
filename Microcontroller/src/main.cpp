#include <Arduino.h>
#include "handshake.h"

String command = "";

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);
    delay(1000);

    Serial.println("READY");
}

void loop()
{
    if (Serial.available() > 0)
    {
        command = Serial.readStringUntil('\n');
        command.trim();

        if (is_handshake_command(command.c_str()))
        {
            Serial.println(get_project_id());
        }

        else if (is_ping_command(command.c_str()))
        {
            Serial.println("PONG");
        }

        else if (command == "LED_ON")
        {
            digitalWrite(LED_BUILTIN, HIGH);
            Serial.println("LED_ON_OK");
        }

        else if (command == "LED_OFF")
        {
            digitalWrite(LED_BUILTIN, LOW);
            Serial.println("LED_OFF_OK");
        }

        else
        {
            Serial.print("UNKNOWN_COMMAND: ");
            Serial.println(command);
        }
    }
}