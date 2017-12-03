#include <Wire.h> 
#include "U8glib.h"
#include <VirtualWire.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);

const byte TRIGGER_PIN = 2; 
const byte ECHO_PIN = 3;   
const byte TRIGGER_PIN2 = 4; 
const byte ECHO_PIN2 = 5;  

const unsigned long DelaiMax = 25000UL;

float MaxValue1;
float MaxValue2;

const float VitesseSon = 340.0 / 1000;

void setup(void) {
  Serial.begin(9600);
  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW); 
  pinMode(ECHO_PIN, INPUT);
  
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  Calibration();
  vw_setup(2000);
}

void EnvoyerMessage(String msg) {

  char message[100];
  msg.toCharArray(message, strlen(message) - 1);
  
  vw_send((uint8_t *)message, strlen(message)+1);
  vw_wait_tx();
}

void loop(void) {
  // put your main code here, to run repeatedly:
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  delay(200);
}

String Mesurer() {
  float b1 = Capteur1();
  float b2 = Capteur2();

  
  
  float d1 = map(b1,60,MaxValue1,0,100);
  float d2 = map(b2,60,MaxValue2,0,100);
  if(b1 > MaxValue1)
  {
    d1 = 2003;
  }
  if(b2 > MaxValue2)
  {
    d2 = 2003;
  }
  
  return String((int)d1)+ "/" + String((int)d2);
}

float Capteur1(){
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  long measure = pulseIn(ECHO_PIN, HIGH, DelaiMax);
  return measure / 2.0 * VitesseSon;
}

float Capteur2(){
  digitalWrite(TRIGGER_PIN2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN2, LOW);
  
  long measure = pulseIn(ECHO_PIN2, HIGH, DelaiMax);
  return measure / 2.0 * VitesseSon;
}


void Calibration(void){
  u8g.firstPage();  
  do {
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr( 0, 22, "Calibration ...");
  } while( u8g.nextPage() );
  
  
  delay(1000);
  
  MaxValue1 = Capteur1();
  MaxValue2 = Capteur2();
  
  
}

float volumePrec = 0;

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  String Mesure = Mesurer();
  Serial.println(Mesure);
  
  float volume = Split(Mesure,'/',0).toFloat();
  if(volume == 2003){
    volume = volumePrec;
  }
  volumePrec = volume;
  char outstr[15];
  dtostrf(volume,7, 0, outstr);
  
  u8g.drawStr( 0, 22, "Volume ");
  u8g.drawStr( 50, 22, outstr);
}

 String Split(String string, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = string.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(string.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? string.substring(strIndex[0], strIndex[1]) : "";
}

