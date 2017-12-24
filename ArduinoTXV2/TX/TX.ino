#include <Wire.h> 
#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);

const byte TRIGGER_PIN = 2; 
const byte ECHO_PIN = 3;   
const byte PinProx1 = 4;
const byte PinProx2 = 5;

const unsigned long DelaiMax = 25000UL;

float MaxValue1;

const float VitesseSon = 340.0 / 1000;

void setup(void) {
  Serial.begin(1200);
  
  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW); 
  pinMode(ECHO_PIN, INPUT);

  pinMode(PinProx1,INPUT);
  pinMode(PinProx2,INPUT);
  
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
}


void loop(void) {
  // put your main code here, to run repeatedly:
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  delay(200);
}

String Etat = "";

String Mesurer() {
  float b1 = Capteur1();
  float d1 = map(b1,60,MaxValue1,0,100);

 
  Etat = "";
  
  if(digitalRead(PinProx1) == LOW){
    if(digitalRead(PinProx2) == LOW){
    Etat = "---";
    delay(500);
    return "!---";
    }
    Etat = ">>>";
    return "!>>>";
  }
  else if(digitalRead(PinProx2) == LOW){
    Etat = "<<<";
    return "!<<<";
  }

   if(b1 > MaxValue1)
  {
    return "";
  }
  
  return "!" + String((int)d1);
}

float Capteur1(){
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  long measure = pulseIn(ECHO_PIN, HIGH, DelaiMax);
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
  
}

float volumePrec = 0;

void draw(void) {
  
  u8g.setFont(u8g_font_unifont);
  
  String Mesure = Mesurer();
  
  if(Mesure != ""){
    Serial.println(Mesure);
  }
  Mesure.remove(0,1);
  if(Mesure.toFloat()!= 0){
    float volume = Mesure.toFloat();
    volumePrec = volume;
    
  }
  
  char outstr[15];
  dtostrf(volumePrec,7, 0, outstr);
  
  u8g.drawStr( 0, 22, "Volume ");
  u8g.drawStr( 50, 22, outstr);
  char Copie[15];
  Etat.toCharArray(Copie,15);
  u8g.drawStr( 50, 50, Copie);
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

