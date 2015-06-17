/*
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital 6
 * LCD D4 pin to analog pin A2
 * LCD D5 pin to analog pin A3
 * LCD D6 pin to analog pin A4
 * LCD D7 pin to analog pin A5
 * LCD R/W pin to ground
 * 10K resistor:
   * ends to +5V and ground
   * wiper to LCD VO pin (pin 3)
*/

#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, A2, A3, A4, A5);

void setup()
{
  Serial.begin(9600);
  lcd.begin(20,4);

  Serial.println("Serial echo");

  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(8, HIGH);
  digitalWrite(7, HIGH);

  lcd.setCursor(0,0);
  lcd.print("Serial echo");
  delay(1000);
}

void pong(String received)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(received);

  Serial.println(received);
}

void loop()
{
  byte incomingByte = 0;

  if(Serial.available() > 0)  {
    incomingByte = Serial.read();

    switch (incomingByte) {
      case 0xFF:
        pong("Workando");
        break;
      default:
        pong(String(incomingByte, DEC));
        Serial.println(incomingByte);
    }
  }
}
