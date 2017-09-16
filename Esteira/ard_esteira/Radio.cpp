#include "Arduino.h"
#include "Radio.h"
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

void InicializaRadio()
{
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"serv1");
  Mirf.payload = 10*sizeof(char);
  Mirf.config();
  
  //Serial.println("Listening..."); 
}

void RecebeDados(char* dta){  
  //char dta[10];
  byte data[Mirf.payload];

  while(!Mirf.dataReady()){}
  
  if(!Mirf.isSending() && Mirf.dataReady()){
    //Serial.println("Got packet");   
    Mirf.getData(data);
    Mirf.setTADDR((byte *)"clie1"); 
    Mirf.send(data);
    
    while(Mirf.isSending()){}
    //Serial.println("Reply sent.");
  }
  
  for(int i = 0 ; i < 10 ; i++)
      dta[i] = data[i];
  
}
