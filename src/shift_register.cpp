#include "shift_register.hpp" 

void shift_register_init(void){
    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(DS_PIN, OUTPUT);  
}

void shift_register_shift(uint8_t value){
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DS_PIN, CLOCK_PIN, LSBFIRST, value);
    digitalWrite(LATCH_PIN, HIGH);
}