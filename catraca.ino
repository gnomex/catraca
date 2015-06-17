/*
 * Pin layout should be as follows:
 * Signal     Pin           Pin
 *            Arduino Uno   MFRC522 board
 * ---------------------------------------
 * Reset      9             RST
 * SPI SS     10            SDA
 * SPI MOSI   11            MOSI
 * SPI MISO   12            MISO
 * SPI SCK    13            SCK
 * IRQ        2             IRQ

 * LCD RS pin to digital pin 12 ->7
 * LCD Enable pin to digital pin 11 ->6
 * LCD D4 pin to analog pin A2
 * LCD D5 pin to analog pin A3
 * LCD D6 pin to analog pin A4
 * LCD D7 pin to analog pin A5
 * LCD R/W pin to ground
 * 10K resistor:
   * ends to +5V and ground
   * wiper to LCD VO pin (pin 3)

  Wheel
    Radius ~= 4,5cm (0.045m)
    middle radius ~= 3,5 cm
    Diameter ~= 0,09m
    Area ~= 0.00636173 m2
    Circumference ~= 0.282743 m
    iman: {NN: 0º, NN: 120º, NN: 240º}
  Hall shield
    sensor a3144
    Vcc +5V
    GND
    D0 pin to analog pin A0
 */

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define NOFIELD 505L
#define TOMILLIGAUSS 1.953125

// LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
LiquidCrystal lcd(7, 6, A2, A3, A4, A5);

// String id3="9955204222";
// boolean avlimpiar=false;
// int contLimpiar=0;
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

#define HALL_PINOUT A0

#define RELAYIN4 5  // Nothing yet
#define RELAY_RELEASE 4  //Inductor 3 - Release: NC - no current(default), NO current and release the iron rod
#define RELAY_LEFT 3  //Inductor 2 - Left: NC - current and active
#define RELAY_RIGHT 2  //Inductor 1 - Right: NC - current and active

void
setup()
{
  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);
  SPI.begin();    // Init SPI bus
  lcd.begin(20, 4);
  mfrc522.PCD_Init(); // Init MFRC522 card

  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(8, HIGH);
  digitalWrite(7, HIGH);

  pinMode(RELAY_RIGHT, OUTPUT);
  pinMode(RELAY_LEFT, OUTPUT);
  pinMode(RELAY_RELEASE, OUTPUT);
  pinMode(RELAYIN4, OUTPUT);
  lock_relays();

  ShowReaderDetails();

  Serial.println("Leitor RFID");
  lcd.setCursor(5,0);
  lcd.print("LEITOR RFID");
  delay(1000);
  lcd.clear();
}

void
ShowReaderDetails()
{
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  }
}

void
noise(int pin,int seg)
{
  digitalWrite(pin, LOW);
  delay(seg);
  digitalWrite(pin, HIGH);
  delay(seg);
}

int
do_hall_measurement()
{
  return (int) ( analogRead(HALL_PINOUT) - NOFIELD) * TOMILLIGAUSS;
}

// pass1: not moving
//   current negative
//   rotate_right();
//   while negative
// pass2: moving
//   middle positive
//   while positive
// pass3: moved
//   negative again
//   lock_all();
void
wait_for_wheel_spin()
{

  rotate_right();
  pong("You can pass now");
  while( do_hall_measurement() < 0 )              { delay(100); }
  while( do_hall_measurement() > 0 ){  }
  if   (do_hall_measurement() < 0)    { pong("Spin completed"); }
  lock_relays();
}

void
pong(String received)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(received);
  Serial.println(received);
}

void
default_message()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Passe o cracha");
}

/*
  NC
*/
void
relay_closed(int pin)
{
  digitalWrite(pin, HIGH);
}

/*
  NO
*/
void
relay_opened(int pin)
{
  digitalWrite(pin, LOW);
}

void
lock_relays()
{
  relay_closed(RELAY_RIGHT);
  relay_closed(RELAY_LEFT);
  relay_closed(RELAY_RELEASE);
  relay_closed(RELAYIN4);
}

void
rotate_right()
{
  relay_opened(RELAY_RIGHT);
}

void
rotate_left()
{
  relay_opened(RELAY_LEFT);
}

String id2="";

void
loop()
{

  default_message(); delay(1000);

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    // Serial.print(mfrc522.uid.uidByte[i], DEC);
    String id1 = String(mfrc522.uid.uidByte[i], DEC);
    id2 = id2 + id1;
  }

  do_hall_measurement();

  Serial.println("VERIFY-UID:" + id2);

  while(!(Serial.available() > 0) ){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Waiting for a server response");
    delay(500);
  }

  String incomingBytes; // Serial Event
  Serial.flush(); // Waits for the transmission of outgoing serial data to complete

  if(Serial.available() > 0)  {
    incomingBytes = Serial.readString();

    // if(String("auth").equalsIgnoreCase(incomingBytes)){
    if( incomingBytes.startsWith("auth") ){

    }
    // if(String("fail").equalsIgnoreCase(incomingBytes)){
    if( incomingBytes.startsWith("fail") ){

    }
    // if(String("error").equalsIgnoreCase(incomingBytes)){
    if( incomingBytes.startsWith("error") ){

    }
    if( incomingBytes.startsWith("print<")){
      int at = incomingBytes.lastIndexOf('<');
      int end = incomingBytes.lastIndexOf('>');

      if( at < end ){
        pong( incomingBytes.substring(at + 1, end - 1) );
      }

      wait_for_wheel_spin();

      // delay(2000);

      // pong("Hall:" + String( do_hall_measurement(), DEC ));

    }
    if( incomingBytes.startsWith("readhall") ){
      pong("Hall:" + String( do_hall_measurement(), DEC ));
    }
  }

  delay(1000);

  lock_relays();

  id2 = "";
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

