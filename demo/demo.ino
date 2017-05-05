#include <HSBColor.h>
#include <Adafruit_DotStar.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>  

#define BAUD 9600

//******************** DEVICE DEFINITIONS *************************//
//ENCODER
#define ENCODER_PIN_A 2 // MUST BE 2
#define ENCODER_PIN_B 3 // MUST BE 3
#define ENCODER_BUTTON_PIN 4

// SWITCH
#define SWITCH_RIGHT_PIN 7
#define SWITCH_CENTER_PIN 6
#define SWITCH_LEFT_PIN 5

// LUMINAIRES
#define EARRING_PIXELS 12 // Number of LEDs in strip
#define EARRING_LEFT_PIN 9
#define EARRING_RIGHT_PIN 8

#define CITY_PIXELS 16 // Number of LEDs in strip
#define CITY_DATAPIN    13
#define CITY_CLOCKPIN   12

#define SUNMOON_PIXELS 25 // Number of LEDs in strip
#define SUNMOON_DATAPIN    11
#define SUNMOON_CLOCKPIN   10

//********************* Luminaires *************************//

Adafruit_DotStar city = Adafruit_DotStar(CITY_PIXELS, CITY_DATAPIN, CITY_CLOCKPIN, DOTSTAR_BGR);
Adafruit_DotStar sunmoon = Adafruit_DotStar(SUNMOON_PIXELS, SUNMOON_DATAPIN, SUNMOON_CLOCKPIN, DOTSTAR_BGR);
Adafruit_NeoPixel r_earring = Adafruit_NeoPixel(EARRING_PIXELS, EARRING_LEFT_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel l_earring = Adafruit_NeoPixel(EARRING_PIXELS, EARRING_RIGHT_PIN, NEO_GRB + NEO_KHZ800);
  

//********************* Rotary Encoder *************************//
// From bildr article: http://bildr.org/2012/08/rotary-encoder-arduino/
// datasheet: http://cdn.sparkfun.com/datasheets/Components/Switches/EC12PLRGBSDVBF-D-25K-24-24C-6108-6HSPEC.pdf
// Sparkfun product info: https://www.sparkfun.com/products/10982

//these pins can not be changed 2/3 are special pins
int encoderPin1 = ENCODER_PIN_A;
int encoderPin2 = ENCODER_PIN_B;
const int button = ENCODER_BUTTON_PIN;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;
long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;

//********************* 2P3T SWITCH *************************//
// http://www.oddwires.com/content/2P3Tswitch.pdf
// http://www.oddwires.com/slide-switch/
const int switchRight = SWITCH_RIGHT_PIN;
const int switchCenter = SWITCH_CENTER_PIN;
const int switchLeft = SWITCH_LEFT_PIN;




//********************* DEVICE TYPES *************************//

enum mode {
  CITY,
  SUNMOON,
  EARRINGS
};

//***************** SETUP AND INITIALIZATION *********************//


mode currentMode;

void setup() {
  Serial.begin (9600);
  Serial.println("CHI DEMO");
  initDevices();
  currentMode = SUNMOON;
}


void initDevices(){
  Serial.println("INIT DEVICES");
  // LUMINAIRES 
    city.begin(); // Initialize pins for output
    city.show();  // Turn all LEDs off ASAP
    sunmoon.begin(); // Initialize pins for output
    sunmoon.show();  // Turn all LEDs off ASAP
  // EARRINGS
    r_earring.begin();
    r_earring.show();
    l_earring.begin();
    l_earring.show();

  // BUTTON
    pinMode(button, INPUT);
    digitalWrite(button, LOW);

  // SWITCH
    pinMode(switchRight, INPUT);
    pinMode(switchCenter, INPUT);
    pinMode(switchLeft, INPUT);

    digitalWrite(switchRight, HIGH);
    digitalWrite(switchCenter, HIGH);
    digitalWrite(switchLeft, HIGH);


  // ENCODER
    pinMode(encoderPin1, INPUT); 
    pinMode(encoderPin2, INPUT);

    digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
    digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

    //call updateEncoder() when any high/low changed seen
    //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
    attachInterrupt(0, updateEncoder, CHANGE); 
    attachInterrupt(1, updateEncoder, CHANGE);
}


//********************* SENSING ROUTINES *************************//

// SENSING VARIABLES
int buttonVal, rightVal, centerVal = 0; 
int leftVal = 0;

void sense(){
  buttonVal = digitalRead(button);
  rightVal = digitalRead(switchRight);
  centerVal = digitalRead(switchCenter);
  leftVal = digitalRead(switchLeft);
//  Serial.print("R:");
//  Serial.println(rightVal);
//  Serial.print("C:");
//  Serial.println(centerVal);
//  Serial.print("L:");
//  Serial.println(leftVal);
//  Serial.print("Button:");
//  Serial.println(buttonVal);
  updateEncoder();
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


//********************* LOGIC ROUTINES *************************//

bool buttonPressed = false;
int hueValue = 0;
bool hueChanged = false;
int lastHue = 0;
void logic(){
  if (rightVal == 0) { currentMode = CITY; }
  if (centerVal == 0) { currentMode = EARRINGS; }
  if (leftVal == 0)  { currentMode = SUNMOON; }
  buttonPressed = buttonVal == 1;
  
  hueValue = constrain(encoderValue % 360, 0, 360);
  hueChanged = hueValue != lastHue;
  lastHue = hueValue;
  
  Serial.print("Mode: ");
  Serial.println(currentMode);
  Serial.print("Button Pressed: ");
  Serial.println(buttonPressed);
  Serial.print("Hue value: ");
  Serial.println(hueValue);
}

void act(){
  switch (currentMode) {
    case CITY:
      Serial.println("City mode");
      if(buttonPressed) city_colored();
      if(hueChanged) dotstar_rainbow(&city, CITY_PIXELS, hueValue);
      dotstar_update(&city);
      break;
    case SUNMOON:
      Serial.println("Sunmoon mode");
      if(buttonPressed) sunmoon_colored();
      if(hueChanged) dotstar_rainbow(&sunmoon, SUNMOON_PIXELS, hueValue);
      dotstar_update(&sunmoon);
      break;
    case EARRINGS:
      Serial.println("Earrings mode");
      if(buttonPressed) earring_colored();
      if(hueChanged){
        neopixel_rainbow(&r_earring, EARRING_PIXELS, hueValue);
        neopixel_rainbow(&l_earring, EARRING_PIXELS, hueValue);
      }
      neopixel_update(&r_earring);
      neopixel_update(&l_earring);
      break;
  }
}



//********************* API *************************//

// API PROCESSING
char prefix = 0;
char buffer = ' ';



void findCommandEnd(){
  buffer = ' ';
  while(buffer != '\n'){
    if(Serial.available() > 0){
      buffer = Serial.read();
    } 
  }
}



//********************* LIGHTING BEHAVIORS *************************//
int int_buffer[] = {0, 0, 0};
void dotstar_colorRGB(Adafruit_DotStar* strip, uint16_t n, uint8_t r, uint8_t g, uint8_t b){
  strip->begin();
  for(int i = 0; i < n; i++) strip->setPixelColor(i, r, g, b);
}
void dotstar_color(Adafruit_DotStar* strip, uint16_t n, uint32_t color){
  strip->begin();
  for(int i = 0; i < n; i++) strip->setPixelColor(i, color);
}
void dotstar_update(Adafruit_DotStar* strip){  strip->show(); strip->show(); }
void dotstar_off(Adafruit_DotStar* strip, uint16_t n){ dotstar_color(strip, n, 0x000000);  }
void dotstar_on(Adafruit_DotStar* strip, uint16_t n){  dotstar_color(strip, n, 0xFFFFFF);  }
void dotstar_rainbow(Adafruit_DotStar* strip, uint16_t n, int hue){
  hue = constrain(hue, 0, 360);
  H2R_HSBtoRGB(hue, 100, 100, int_buffer);
  strip->begin();
  dotstar_colorRGB(strip, n, int_buffer[0], int_buffer[1], int_buffer[2]);
}
//********************* LIGHTING BEHAVIORS (NEOPIXEL) *************************//
void neopixel_colorRGB(Adafruit_NeoPixel* strip, uint16_t n, uint8_t r, uint8_t g, uint8_t b){
  strip->begin();
  for(int i = 0; i < n; i++) strip->setPixelColor(i, r, g, b);
}
void neopixel_color(Adafruit_NeoPixel* strip, uint16_t n, uint32_t color){
  strip->begin();
  for(int i = 0; i < n; i++) strip->setPixelColor(i, color);
}
void neopixel_update(Adafruit_NeoPixel* strip){  strip->show(); strip->show(); }
void neopixel_off(Adafruit_NeoPixel* strip, uint16_t n){ neopixel_color(strip, n, 0x000000);  }
void neopixel_on(Adafruit_NeoPixel* strip, uint16_t n){  neopixel_color(strip, n, 0xFFFFFF);  }
void neopixel_rainbow(Adafruit_NeoPixel* strip, uint16_t n, int hue){
  hue = constrain(hue, 0, 360);
  H2R_HSBtoRGB(hue, 100, 100, int_buffer);
  strip->begin();
  neopixel_colorRGB(strip, n, int_buffer[0], int_buffer[1], int_buffer[2]);
}


void api_call(char prefix){
   switch (prefix) {
    case 'u': 
      Serial.println("UPDATE");
      dotstar_update(&sunmoon);
      findCommandEnd();
      break;
    case 'o': 
      Serial.println("ON");
      dotstar_on(&sunmoon, SUNMOON_PIXELS);
      dotstar_update(&sunmoon);
      findCommandEnd();
      break;
    case 'h': 
      Serial.println("HUE_CHANGE");
      hue_change();
      findCommandEnd();
      break;
    case 'p': 
      Serial.println("OFF");
      dotstar_off(&sunmoon, SUNMOON_PIXELS);
      dotstar_update(&sunmoon);
      findCommandEnd();
      break;
    case 'c': 
      Serial.println("COLORED");
      sunmoon_colored();
      break;
    case 'd': 
      Serial.println("DEBUG:");
      for(int i = 0; i < SUNMOON_PIXELS; i++){
        Serial.print(sunmoon.getPixelColor(i), HEX);
        Serial.print(",");
      }
      Serial.println("");  
      dotstar_off(&sunmoon, SUNMOON_PIXELS);
      dotstar_update(&sunmoon);
      findCommandEnd();
      break;
    default: 
      Serial.print(prefix);
      Serial.println("API command does not exist");
      findCommandEnd(); 
    break;
  }
}



// DEVICE SPECIFIC BEHAVIORS
void sunmoon_colored(){
  sunmoon.begin(); // Initialize pins for output
  sunmoon.setPixelColor(0,0xe65e19);
  sunmoon.setPixelColor(1,0xe65e19);
  sunmoon.setPixelColor(2,0xe65e19);
  sunmoon.setPixelColor(4,0xe65e19);
  sunmoon.setPixelColor(3,0xe65e19);
  sunmoon.setPixelColor(6,0xe65e19);
  sunmoon.setPixelColor(5,0xe65e19);
  sunmoon.setPixelColor(7,0xe65e19);
  sunmoon.setPixelColor(8,0xe65e19);
  sunmoon.setPixelColor(9,0xe65e19);
  sunmoon.setPixelColor(10,0xe65e19);
  sunmoon.setPixelColor(11,0x19a1e6);
  sunmoon.setPixelColor(12,0x19a1e6);
  sunmoon.setPixelColor(13,0x19a1e6);
  sunmoon.setPixelColor(14,0xe5e619);
  sunmoon.setPixelColor(15,0xe5e619);
  sunmoon.setPixelColor(16,0xe5e619);
  sunmoon.setPixelColor(17,0xe5e619);
  sunmoon.setPixelColor(18,0xe5e619);
  sunmoon.setPixelColor(19,0xe5e619);
  sunmoon.setPixelColor(20,0x195de6);
  sunmoon.setPixelColor(21,0x195de6);
  sunmoon.setPixelColor(22,0x195de6);
  sunmoon.setPixelColor(23,0xffffff);
  sunmoon.setPixelColor(24,0xffffff);
  sunmoon.show();
  sunmoon.show();
}

void earring_colored(){
  neopixel_on(&r_earring, EARRING_PIXELS);
  neopixel_update(&r_earring);
  neopixel_on(&l_earring, EARRING_PIXELS);
  neopixel_update(&l_earring);
  
}
void city_colored(){
  city.begin(); // Initialize pins for output
  city.show();  // Turn all LEDs off ASAP
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

int hue;
void hue_change(){
  hue = Serial.parseInt();
  Serial.print("HUE: ");
  Serial.println(hue);
  dotstar_rainbow(&sunmoon, SUNMOON_PIXELS, hue);
}

void enable_api(){
  if (Serial.available() > 0) {
      prefix = Serial.read();
      api_call(prefix);
  }
}

void loop() {
//  enable_api(); // always on
  sense();
  logic();
  act();
  delay(200);
}

