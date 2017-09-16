/*  HAMS.cpp
 *
 *  Classe para facilitar o acionamento de motores CC em códigos .ino
 *  utilizando o H.A.M.S. (Hybrid Awesome Motion Sistem) ou similares
 */

#include "HAMS.h"

void HAMS::Pinagem(int pino_dir, int pino_pwm)
{
   PWM = pino_pwm;
   dir = pino_dir;

   pinMode(PWM,OUTPUT);
   pinMode(dir,OUTPUT);
}

void HAMS::Mover(int vel_pwm)
{
   if(vel_pwm > 0)
      digitalWrite(dir,HIGH);
   else
      digitalWrite(dir,LOW);

   analogWrite(PWM,abs(vel_pwm));
}
