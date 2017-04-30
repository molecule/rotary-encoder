//******************** DOTSTAR *************************//
#include <Adafruit_DotStar.h>
#include <SPI.h>  

#define CITYPIXELS 16 // Number of LEDs in strip
#define CITY_DATAPIN    11
#define CITY_CLOCKPIN   10 
Adafruit_DotStar city = Adafruit_DotStar(
  CITYPIXELS, CITY_DATAPIN, CITY_CLOCKPIN, DOTSTAR_BGR);

//********************* Rotary Encoder *************************//
// From bildr article: http://bildr.org/2012/08/rotary-encoder-arduino/
// datasheet: http://cdn.sparkfun.com/datasheets/Components/Switches/EC12PLRGBSDVBF-D-25K-24-24C-6108-6HSPEC.pdf
// Sparkfun product info: https://www.sparkfun.com/products/10982

//these pins can not be changed 2/3 are special pins
int encoderPin1 = 2;
int encoderPin2 = 3;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;

const int button = 4;


//********************* 2P3T SWITCH *************************//
// http://www.oddwires.com/content/2P3Tswitch.pdf
// http://www.oddwires.com/slide-switch/
const int switchRight = A0;
const int switchCenter = A1;
const int switchLeft = A2;

enum mode {
  CITY,
  SUNMOON,
  EARRINGS
};

mode currentMode = SUNMOON;

void setup() {
  Serial.begin (9600);

  city.begin(); // Initialize pins for output
  city.show();  // Turn all LEDs off ASAP
  initializeCity();

  pinMode(button, INPUT);
  digitalWrite(button, LOW);

  pinMode(switchRight, INPUT);
  pinMode(switchCenter, INPUT);
  pinMode(switchLeft, INPUT);

  digitalWrite(switchRight, HIGH);
  digitalWrite(switchCenter, HIGH);
  digitalWrite(switchLeft, HIGH);

  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(0, updateEncoder, CHANGE); 
  attachInterrupt(1, updateEncoder, CHANGE);

}

void loop() { 
  
  int buttonVal = digitalRead(button);
  if (buttonVal == 1) { Serial.println("button!"); }

  int rightVal = digitalRead(switchRight);
  int centerVal = digitalRead(switchCenter);
  int leftVal = digitalRead(switchLeft);
  if (rightVal == 0) { currentMode = CITY; }
  if (centerVal == 0) { currentMode = EARRINGS; }
  if (leftVal == 0)  { currentMode = SUNMOON; }

  switch (currentMode) {
    case CITY:
      Serial.println("city mode");
      initializeCity();
      break;
    case SUNMOON:
      Serial.println("sunmoon mode");
      cityOff();
      break;
    case EARRINGS:
       Serial.println("earrings mode");
       cityOff();
       break;
  }

  /*
  Serial.print("right: ");
  Serial.println(rightVal);
  Serial.print("center: ");
  Serial.println(centerVal);
  Serial.print("left: ");
  Serial.println(leftVal);
  */

  Serial.print("encoderVal: ");
  Serial.println(encoderValue);
  delay(1000); //just here to slow down the output, and show it will work  even during a delay
}


void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time
}

void initializeCity() {
  city.setPixelColor(0,0xe5e619);
  city.setPixelColor(1,0x19e65d);
  city.setPixelColor(2,0xe619a1);
  city.setPixelColor(3,0x19e5e6);
  city.setPixelColor(4,0xe6a219);
  city.setPixelColor(5,0x19e680);
  city.setPixelColor(6,0xe61919);
  city.setPixelColor(8,0xffffff);
  city.setPixelColor(7,0x19e65d);
  city.setPixelColor(9,0xffffff);
  city.setPixelColor(10,0xe6a219);
  city.setPixelColor(11,0x3b19e6);
  city.setPixelColor(12,0xe61919);
  city.setPixelColor(13,0xe63b19);
  city.setPixelColor(14,0xe5e619);
  city.setPixelColor(15,0x19e6a2);
  city.show();
  city.show();
}

void cityOff() {
  for (int i = 0; i < CITYPIXELS; i++) {
    city.setPixelColor(i, 0);
  }
  city.show();
}

