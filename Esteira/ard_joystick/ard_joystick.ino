#include <SPI.h>
#include <string.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

const int X = A1;
const int Y = A2;

void setup(){
  Serial.begin(9600);  
  Mirf.cePin = 8;
  Mirf.csnPin = 7;
  
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
 
  Mirf.setRADDR((byte *)"clie1");
  
   
  Mirf.payload = 10*sizeof(char);
   
  Mirf.config();
  
  Serial.println("Beginning ... "); 
}

void loop(){
  int x,y;
  char c[10];
  x = analogRead(X);
  y = analogRead(Y);
  sprintf(c,"%d,%d",x,y);
  Serial.println(c);
  unsigned long time = millis();
  
  Mirf.setTADDR((byte *)"serv1");
  
  Mirf.send((byte *)c);
  
  while(Mirf.isSending()){}
  
  //Serial.println("Finished sending");
  
  while(!Mirf.dataReady()){
    //Serial.println("Waiting");
    if ( ( millis() - time ) > 500 ) {
      Serial.println("Timeout on response from server!");
      return;
    }
  }
  //Serial.print("Ping: ");
  //Serial.println((millis() - time));
} 
  
  
  
