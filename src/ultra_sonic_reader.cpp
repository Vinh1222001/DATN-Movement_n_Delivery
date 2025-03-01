#include "ultra_sonic_reader.hpp" 

distance_reader ultra_sonic_reader{0,0};

void ultra_sonic_reader_init(void){
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

void ultra_sonic_read(void* arg){
    while (true)
    {
        digitalWrite(TRIGGER_PIN, LOW);
        delayMicroseconds(2);
        // Sets the TRIGGER_PIN on HIGH state for 10 micro seconds
        digitalWrite(TRIGGER_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIGGER_PIN, LOW);
        
        // Reads the ECHO_PIN, returns the sound wave travel time in microseconds
        ultra_sonic_reader.duration = pulseIn(ECHO_PIN, HIGH);
        
        // Calculate the distance in cm
        ultra_sonic_reader.distance = ultra_sonic_reader.duration * SOUND_SPEED/2;
        
        vTaskDelay(DELAY_FOR_READ_VALUE/portTICK_PERIOD_MS);
    }
    
}