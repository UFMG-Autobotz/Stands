#include "Radio.h"
#include "HAMS.h"

int x, y;
int vel_esq, vel_dir;
HAMS motor_e, motor_d;

void obter_xy();

// ________________________________________

void setup()
{
  InicializaRadio();
  
  motor_e.Pinagem(5,6);
  motor_d.Pinagem(9,10);
  Serial.begin(9600);
}

void loop()
{
  obter_xy();

  switch(x + 3*y)
  {
  case 0:           // x=0,  y=0
    vel_esq = 0;
    vel_dir = 0;
    break;

  case 1:           // x=1,  y=0
    vel_esq = 255;
    vel_dir = 255;
    break;

  case -1:          // x=-1, y=1
    vel_esq = -255;
    vel_dir = -255;
    break;

  case 3:           // x=0,  y=1
    vel_esq = 255;
    vel_dir = -255;
    break;

  case -3:          // x=0, y=-1
    vel_esq = -255;
    vel_dir = 255;
    break;

  case 4:           // x=1,  y=1
    vel_esq = 180;
    vel_dir = 255;
    break;

  case -2:          // x=1,  y=-1
    vel_esq = 255;
    vel_dir = 180;
    break;

  case 2:           // x=-1, y=1
    vel_esq = -180;
    vel_dir = -255;
    break;

  case -4:          // x=-1, y=-1
    vel_esq = -255;
    vel_dir = -180;
    break;
  }

  motor_e.Mover(vel_esq);
  motor_d.Mover(vel_dir);

  Serial.print(vel_esq);
  Serial.print(",");
  Serial.print(vel_dir);
  Serial.print("   -----   ");
  
  delay(5);
}

// ________________________________________

void obter_xy()
{
  char *pch;
  char dados[10];

  //Serial.println("Recebendo dados...");
  RecebeDados(dados);
  //Serial.print("recebido: ");
  Serial.print(dados);
  Serial.print("   -----   ");  
  delay(50);
  
  pch = strtok (dados,",");
  x = atoi(pch);
  pch = strtok (NULL, ",");
  y = atoi(pch);
  
  if(x < 50)
    x = -1;
  else if(x > 800)
    x = 1;
  else
    x = 0;

  if(y < 50)
    y = -1;
  else if(y > 900)
    y = 1;
  else
    y = 0;
    

  Serial.print(x);
  Serial.print(",");
  Serial.println(y);  
}

