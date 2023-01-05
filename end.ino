#include <deprecated.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <require_cpp11.h>

#include <dht11.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Stepper.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Servo.h>

#include <Adafruit_NeoPixel.h>
#define PIN 24                          // input pin Neopixel is attached to
#define NUMPIXELS      12     // number of neopixels in strip

#define DHT11PIN 11

dht11 DHT11;

const int tilt =12;
int tiltstate = 0;

LiquidCrystal lcd(46, 47, 48, 42, 43, 44, 45);

#define   SS_PIN  53  //  칩셋핀(데이터 핀)
#define   RST_PIN 5   //  리셋핀
#define   SERVO_PIN 23
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo servo;
int angle = 0;
bool motorState = false;

const int g_Card_UID[4] = {0x61, 0x8B, 0x02, 0x27}; //  흰 카드의 UID번호


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 100; // timing delay in milliseconds
int redColor = 0;
int greenColor = 0;
int blueColor = 0;

Stepper step1(2028, 2,3,4,22);
int lap = 2048;
int btn1 = 6;

int piezo = 13;
int numTones = 4;
int tones[]={261,294,330,349};

void setup() {
  // put your setup code here, to run once:
  step1.setSpeed(10);
  lcd.begin(16,2);
  pinMode(btn1, INPUT); //버튼1

  pinMode(tilt,INPUT); //기울기
  pinMode(13,OUTPUT); //부저
  Serial.begin(9600);  //시리얼 모니터 출력
  SPI.begin();            // Init SPI bus
  mfrc522.PCD_Init();     // Init mfrc522
  mfrc522.PCD_DumpVersionToSerial();  //  RFID의 펌웨어 버전 확인

  servo.attach(SERVO_PIN);  //  6번핀
  servo.write(0);
  pixels.begin();
}

bool CheckCardUID() // rfid
{
  if (mfrc522.uid.uidByte[0] == g_Card_UID[0] && 
      mfrc522.uid.uidByte[1] == g_Card_UID[1] && 
      mfrc522.uid.uidByte[2] == g_Card_UID[2] &&
      mfrc522.uid.uidByte[3] == g_Card_UID[3])
      return true;

  return false;
}

void setColor(){
  redColor = random(0, 255);
  greenColor = random(0,255);
  blueColor = random(0, 255);
}

void loop() {
  unsigned long time = millis();
  // 버튼 1을 누르면 장식품이 360도로 돌아간다.
  if(time <= millis()){
    
  
  if(digitalRead(btn1) == LOW){
    
    step1.step(lap);
    delay(500);
    step1.step(-lap);
    delay(500);
  }
  }
  setColor();
  if(time <= millis()){
    for (int i=0; i < NUMPIXELS; i++) {
                 // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(redColor, greenColor, blueColor));
                 // This sends the updated pixel color to the hardware.
      pixels.show();
               // Delay for a period of time (in milliseconds).
      delay(delayval);
  }
  }
  //온습도 측정후 LCD표시
  int chk = DHT11.read(DHT11PIN);
  if(time <= millis()){
 // lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("TEMP:     ");
  lcd.print((float)DHT11.temperature, 2);
  
  lcd.setCursor(0,1);
  lcd.print("HUMIDITY: ");
  lcd.print((float)DHT11.humidity, 2);
  delay(200); 
  }
  //기울기
  tiltstate = digitalRead(tilt);
  if(time <= millis()){
  if (tiltstate == 0){
    noTone(piezo);
  }
  else{
    for(int i=0; i<numTones; i++){
      tone(piezo,tones[i]);
      delay(500);
    }
  }
  }

  if(time <= millis()){
  //  카드가 읽혀지지 않은 상태(찍히지 않으면)이면 리턴
  if (!mfrc522.PICC_IsNewCardPresent())   return;

  if (!mfrc522.PICC_ReadCardSerial())     return;

  //  uid카드를 출력하기. 각 자리는 16진수로 출력
  Serial.println(mfrc522.uid.uidByte[0],HEX);
  Serial.println(mfrc522.uid.uidByte[1],HEX);
  Serial.println(mfrc522.uid.uidByte[2],HEX);
  Serial.println(mfrc522.uid.uidByte[3],HEX);

  if (CheckCardUID())
  {
    if (!motorState)    angle = 90;
    else                angle = 0;

    servo.write(angle);
    
    motorState = !motorState;

    delay(1000);
  }
  
  //  delay함수 대신에 아래의 두 함수를 호출.
  mfrc522.PICC_HaltA();

  mfrc522.PCD_StopCrypto1();
  }

}
