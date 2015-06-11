/*
----------------------------------------------------------------------------- Nicola Coppola
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                5                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               51                MOSI
 * SPI MISO   12               50                MISO
 * SPI SCK    13               52                SCK
 *
 */
/*

 The circuit:
 * LCD RS pin to digital pin 12 ->7
 * LCD Enable pin to digital pin 11 ->6
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
   * ends to +5V and ground
   * wiper to LCD VO pin (pin 3)
 */

 /*
  Wheel
    Radius ~= 4,5cm (0.045m)
    Diameter ~= 0,09m
    Area ~= 0.00636173 m2
    Circumference ~= 0.282743 m
  Hall shield
    sensor a3144
    Vcc +5V
    GND
    D0 pin to analog pin A0
 */

#include <SPI.h>
#include "MFRC522.h"
#include <LiquidCrystal.h>

#define NOFIELD 505L    // Analog output with no applied field, calibrate this
#define TOMILLIGAUSS 1.953125

// LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
LiquidCrystal lcd(7, 6, A2, A3, A4, A5);
long conS=50;
String id2="";
String id3="9955204222";
boolean avlimpiar=false;
int contLimpiar=0;
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

int hall_pin = A0;
int hall_value = 0;

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();    // Init SPI bus
  lcd.begin(20, 4);

  // pinMode(hall_pin, INPUT);

  mfrc522.PCD_Init(); // Init MFRC522 card
  Serial.println("Hey!!!");
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(8, HIGH);
  digitalWrite(7, HIGH);
  Serial.println("Leitor RFID");
  lcd.setCursor(8,0);
  lcd.print("LEITOR RFID");
  delay(1000);
  lcd.clear();
}

void sonido(int pin,int seg){
  digitalWrite(pin, LOW);
  delay(seg);
  digitalWrite(pin, HIGH);
  delay(seg);
}

void rele(int pin,int seg){
  digitalWrite(pin, LOW);
  delay(seg);
  digitalWrite(pin, HIGH);
  delay(seg);
}

void DoMeasurement()
{
  hall_value = analogRead(hall_pin);   // Range : 0..1024

  Serial.print("Raw hall reading: ");
  Serial.println(hall_value);

  float gauss = (hall_value - NOFIELD) * TOMILLIGAUSS;

  Serial.print(gauss, 2);
  Serial.print(" Gauss ");

  if (gauss > 0)     Serial.println("(South pole)");
  else if(gauss < 0) Serial.println("(North pole)");
  else               Serial.println();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hall: ");
  lcd.print(hall_value);
  lcd.setCursor(0,1);
  lcd.print("Gauss: ");
  lcd.print(gauss);

  delay(1000);
}


void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("Card UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], DEC);
    String id1 = String(mfrc522.uid.uidByte[i], DEC);
    id2 = id2+id1;
  }

  Serial.println();
  Serial.print("--Card IDD:");
  Serial.println(id2);

  DoMeasurement();

  if(id2 == id3){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("UID:");
    lcd.print(id2);
    sonido(8,10);
    delay(1000);
  }else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ERRO");
    lcd.setCursor(0,1);
    lcd.print("UID:");
    lcd.print(id2);
    sonido(8,70);
    delay(1000);
  }
  id2="";

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Passe o cracha");

  Serial.println("Lol");

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
