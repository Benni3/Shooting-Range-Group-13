// #include <Arduino.h>
// #include <encoder.h>

// #define PIN_ENCODER_A 2
// #define PIN_ENCODER_B 3

// static volatile long encoder_counts = 0;

// static int previous_a = 0;
// static int previous_b = 0;

// static unsigned long last_time = 0;
// static long last_count = 0;

// static float angular_speed = 0.0f;
// static float horizontal_speed = 0.0f;

// static float angular_acceleration = 0.0f;
// static float horizontal_acceleration = 0.0f;

// static float previous_angular_speed = 0.0f;
// static float previous_horizontal_speed = 0.0f;

// /*
//     Adjust this when you know exactly
//     how many transitions one revolution produces.
// */
// static const float COUNTS_PER_REV = 4.0f;

// /*
//     You measured:
//     7.5 revolutions = 1 cm
// */
// static const float REV_PER_CM = 7.5f;

// void encoder_init()
// {
//     pinMode(PIN_ENCODER_A, INPUT);
//     pinMode(PIN_ENCODER_B, INPUT);

//     previous_a = digitalRead(PIN_ENCODER_A);
//     previous_b = digitalRead(PIN_ENCODER_B);

//     last_time = millis();
// }

// void encoder_update()
// {
//     int a = digitalRead(PIN_ENCODER_A);
//     int b = digitalRead(PIN_ENCODER_B);

//     /*
//         Quadrature decoding
//     */
//     if (a != previous_a)
//     {
//         if (a == b)
//         {
//             encoder_counts++;
//         }
//         else
//         {
//             encoder_counts--;
//         }
//     }

//     previous_a = a;
//     previous_b = b;

//     unsigned long now = millis();

//     if (now - last_time >= 50)
//     {
//         float dt = (now - last_time) / 1000.0f;

//         long delta_counts =
//             encoder_counts - last_count;

//         float delta_rev =
//             delta_counts / COUNTS_PER_REV;

//         angular_speed =
//             delta_rev * 360.0f / dt;

//         horizontal_speed =
//             delta_rev / REV_PER_CM / dt;

//         angular_acceleration =
//             (angular_speed - previous_angular_speed)
//             / dt;

//         horizontal_acceleration =
//             (horizontal_speed - previous_horizontal_speed)
//             / dt;

//         previous_angular_speed =
//             angular_speed;

//         previous_horizontal_speed =
//             horizontal_speed;

//         last_count =
//             encoder_counts;

//         last_time =
//             now;
//     }
// }

// long get_encoder_counts()
// {
//     return encoder_counts;
// }

// float get_rotational_position()
// {
//     return
//         encoder_counts
//         * 360.0f
//         / COUNTS_PER_REV;
// }

// float get_horizontal_position()
// {
//     float revolutions =
//         encoder_counts / COUNTS_PER_REV;

//     return revolutions / REV_PER_CM;
// }

// float get_angular_speed()
// {
//     return angular_speed;
// }

// float get_horizontal_speed()
// {
//     return horizontal_speed;
// }

// float get_angular_acceleration()
// {
//     return angular_acceleration;
// }

// float get_horizontal_acceleration()
// {
//     return horizontal_acceleration;
// }