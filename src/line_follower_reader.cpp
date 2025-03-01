#include "line_follower_reader.hpp"

t_line_follow_values line_reader{0, 0, 0, 0};

void line_follower_reader_init(void)
{

    pinMode(LINE_FOLLOWER_OUT1_PIN, INPUT);
    pinMode(LINE_FOLLOWER_OUT2_PIN, INPUT);
    pinMode(LINE_FOLLOWER_OUT3_PIN, INPUT);
    pinMode(LINE_FOLLOWER_OUT4_PIN, INPUT);
    pinMode(LINE_FOLLOWER_OUT5_PIN, INPUT);
}

void line_follower_read(void *arg)
{
    while (true)
    {
        line_reader.out1 = analogRead(LINE_FOLLOWER_OUT1_PIN);
        line_reader.out2 = analogRead(LINE_FOLLOWER_OUT2_PIN);
        line_reader.out3 = analogRead(LINE_FOLLOWER_OUT3_PIN);
        line_reader.out4 = analogRead(LINE_FOLLOWER_OUT4_PIN);
        line_reader.out5 = analogRead(LINE_FOLLOWER_OUT5_PIN);
        Serial.printf("line 1: %d\nline 2: %d\nline 3: %d\nline 4: %d\nline 5: %d\n\n",
                      line_reader.out1,
                      line_reader.out2,
                      line_reader.out3,
                      line_reader.out4,
                      line_reader.out5);

        delay(1000);
    }
}