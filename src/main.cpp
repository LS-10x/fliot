#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include "hardware/pwm.h"

// ------------------------- RF24 Konfiguration -------------------------

// Definieren der CE und CSN Pins
#define CE_PIN 22   // GP12 auf Pico (Physischer Pin 17)
#define CSN_PIN 17  // GP17 auf Pico (Physischer Pin 22)

// Erstellen eines RF24 Objekts mit den definierten Pins
RF24 radio(CE_PIN, CSN_PIN);

// Adresse für die Kommunikation
const byte address[6] = "00001";

// Struktur zur Speicherung der übertragenen Daten (6 Kanäle)
struct transfer_data {
  byte ch1; // Roll input
  byte ch2; // Pitch input
  byte ch3; // Yaw input
  byte ch4; // Pitch adjustment
  byte ch5; // Button ch5
  byte ch6; // Button ch6
};

// Variable für die Struktur
transfer_data data;

// Empfangene Werte
int ch1_value = 90;  // Roll input
int ch2_value = 90;  // Pitch input
int ch3_value = 90;  // Yaw input
int ch4_value = 90;  // Pitch adjustment value
int ch5_value = 1;   // Button ch5
int ch6_value = 1;   // Button ch6

// ------------------------- Servo Konfiguration -------------------------

// Definieren der Servo-Pins
const uint8_t servoPins[] = {4, 5, 6, 7}; // GP4, GP5, GP6, GP7
const uint8_t numServos = 4;

// PWM-Konfigurationsparameter
const float pwmFrequency = 50.0f;          // 50 Hz für Servos (20 ms Periode)
const uint16_t pwmWrap = 2500;              // Wrap-Wert für 50 Hz (125 MHz / (250 * 2500) = 50 Hz)

// Pulsbreiten in Mikrosekunden für Standard SG90 Servo
const uint16_t minPulse = 500; // 1 ms für 0 Grad (500 * 2 µs = 1000 µs)
const uint16_t maxPulse = 1000; // 2 ms für 180 Grad (1000 * 2 µs = 2000 µs)

// Aktuelle Winkel der Servos
int servoAngles[numServos] = {90, 90, 90, 90}; // Startwinkel für alle vier Servos

// Funktion zur Initialisierung der PWM für einen Servo
void initServoPWM(uint8_t pin) {
    // Bestimmen des PWM-Slice und -Kanals
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);

    // GPIO-Funktion auf PWM setzen
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // PWM-Konfiguration
    pwm_set_wrap(slice_num, pwmWrap - 1);    // Setzen des Wrap-Wertes für 20 ms Periode
    pwm_set_clkdiv(slice_num, 250.0f);       // Clock-Divisor auf 250 setzen (125 MHz / (250 * 2500) = 1)
    pwm_set_chan_level(slice_num, channel, 0); // Initialer PWM-Level
    pwm_set_enabled(slice_num, true);           // PWM-Slice aktivieren
}

// Funktion zur Einstellung des Servo-Winkels
void setServoAngle(uint8_t pin, int angle_deg) {
    // Begrenzen des Winkels auf 0-180 Grad
    angle_deg = constrain(angle_deg, 0, 180);

    // Berechnung der Pulsbreite basierend auf dem Winkel
    uint16_t pulseWidth = map(angle_deg, 0, 180, minPulse, maxPulse);

    // Berechnung des PWM-Level (Anzahl der Ticks)
    // Jeder Tick entspricht 2 µs (clkdiv=250 / 125MHz)
    uint16_t pwmLevel = pulseWidth; // pulseWidth ist bereits in Ticks (500-1000)

    // Setzen des PWM-Level für den entsprechenden Slice und Kanal
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);
    pwm_set_chan_level(slice_num, channel, pwmLevel);

    // Serieller Output zur Überwachung
    Serial.print("Servo Pin: ");
    Serial.print(pin);
    Serial.print(" | Angle: ");
    Serial.print(angle_deg);
    Serial.print("° | Pulse Width: ");
    Serial.print(pulseWidth * 2); // Umrechnung in µs
    Serial.print(" µs | PWM Level: ");
    Serial.println(pwmLevel);
}

void setup() {
    // Initialisierung der seriellen Kommunikation
    Serial.begin(115200);
    while (!Serial); // Optional: Warten, bis der serielle Monitor verbunden ist

    // Initialisierung der RF24
    if (!radio.begin()) {
        Serial.println("Setup, Radio hardware not responding!");
        while (1);  // Halt, wenn die RF24 Hardware nicht reagiert
    } else {
        Serial.println("SETUP, Radio hardware initialized!");
    }

    // Setzen der Radio-Parameter
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_LOW);
    radio.setChannel(50);               // Auf denselben Kanal setzen
    radio.setDataRate(RF24_250KBPS);    // Optional: Niedrigere Datenrate für Zuverlässigkeit
    radio.startListening();

    // Initialisierung der PWM für jeden Servo
    for (uint8_t i = 0; i < numServos; i++) {
        initServoPWM(servoPins[i]);
        setServoAngle(servoPins[i], servoAngles[i]); // Setzen auf die neutrale Position (90 Grad)
    }
}

void loop() {
    // RADIO COMMUNICATION
    if (radio.available()) {
        radio.read(&data, sizeof(transfer_data));

        // Map the received data to control values
        ch1_value = map(data.ch1, 0, 254, 10, 170);  // Roll input
        ch2_value = map(data.ch2, 0, 254, 10, 170);  // Pitch input
        ch3_value = map(data.ch3, 0, 254, 25, 155);  // Yaw input
        ch4_value = map(data.ch4, 0, 254, 10, 170);  // Pitch adjustment value
        ch5_value = data.ch5;
        ch6_value = data.ch6;

        // Für Debugging: Empfangene Werte drucken
        Serial.print("Received data: ");
        Serial.print("ch1: "); Serial.print(ch1_value);
        Serial.print(", ch2: "); Serial.print(ch2_value);
        Serial.print(", ch3: "); Serial.print(ch3_value);
        Serial.print(", ch4: "); Serial.print(ch4_value);
        Serial.print(", ch5: "); Serial.print(ch5_value);
        Serial.print(", ch6: "); Serial.println(ch6_value);

        // Setzen der Servo-Winkel basierend auf den empfangenen Kanälen
        // Zuordnung: ch1 -> Servo1 (GP4), ch2 -> Servo2 (GP5), ch3 -> Servo3 (GP6), ch4 -> Servo4 (GP7)
        setServoAngle(servoPins[0], ch1_value); // Servo1
        setServoAngle(servoPins[1], ch2_value); // Servo2
        setServoAngle(servoPins[2], ch3_value); // Servo3
        setServoAngle(servoPins[3], ch4_value); // Servo4
    } else {
        Serial.println("No message received");
        //delay(500);  // Kurze Pause, um den seriellen Monitor nicht zu überfluten
    }
}
