/*  HAMS.h
 *
 *  Classe para facilitar o acionamento de motores CC em códigos .ino
 *  utilizando o H.A.M.S. (Hybrid Awesome Motion Sistem) ou similares
 */

#ifndef HAMS_h
#define HAMS_h

#include "Arduino.h"

class HAMS
{
private:

    int PWM;
    int dir;

public:

  void Pinagem(int pino_dir, int pino_pwm);
  void Mover(int vel_pwm);
};

#endif
