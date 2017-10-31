/*  esteira_comando
 *  Versão 1.0
 *
 *  Nó que envia os comandos de movimento para o nó do Arduino que acionará os
 *	motores da esteira da Autobotz, através do tópico esteira/movimento.
 *	Os comandos são obtidos através de informações enviadas pelo código de visão,
 *	a fim de fazer com que a esteira siga a bolinha laranja ou algum outro objeto.
 *  ________________________________________________________________________________
 */

#include <stdlib.h>
#include <iostream>
#include "ros/ros.h"
#include "../lib/rs232.h"
#include "../lib/rs232_class.hpp"
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Point.h>

#define VEL_LIN_MAX 30
#define VEL_ANG_MAX .4

float x = 0, y = 0;

// Subscriber que recebe as mensagens da visão
void cb_visao(const geometry_msgs::Point coordenadas)
{
  x = coordenadas.x;
  y = coordenadas.y;
}

int main(int argc, char **argv)
{
  // Inicializa nó do ROS e o subscriber
  ros::init(argc, argv, "esteira_comando");
  ros::NodeHandle nh;
  ros::Subscriber sub_visao = nh.subscribe<geometry_msgs::Point>("esteira/visao",100,&cb_visao);

  // Define a taxa de execução do programa
  ros::Rate loop_rate(3);

  // Cria e inicializa as variáveis de comando da esteira
  float vel_linear, vel_angular;
  int FLAG_esteira;
  char comando[15];
  vel_linear = vel_angular = 0;

  // Parâmetros utilizados na detecção
  float perto, limiar_y, limiar_x;
  int id_porta;
  nh.getParam("visao/ref_perto",perto);
  nh.getParam("visao/limiar_x",limiar_x);
  nh.getParam("visao/limiar_y",limiar_y);
  nh.getParam("visao/porta_serial",id_porta);

  // Inicializa a porta seria escolhida
  Rs232_class serial(id_porta);

  while(ros::ok())
  {
    // Dadas as mensagens recebidas da visão, setar a flag e as velocidades:

    if(y < perto)         // Bola suficientemente perto da esteira:
      FLAG_esteira = 2;   // Flag que manda a esteira parar
    else if(x == -200 )   // Bola saiu da tela para a esquerda:
      FLAG_esteira = -1;  // Flag que faz a esteira dar um giro para a esquerda em torno de seu próprio eixo
    else if(x == 200)     // Bola saiu da tela para a direita:
      FLAG_esteira = 1;   // Flag análoga à anterior, porém para a direita
    else
    {
      FLAG_esteira = 0;           // Flag para movimento normal da esteira

      if(y - perto > limiar_y)    // Bola muito longe da esteira:
        vel_linear = VEL_LIN_MAX; // Andar com velocidade máxima
      else                        // Se estiver a uma distância razoável,
        vel_linear = VEL_LIN_MAX * (y - perto) / limiar_y + 5;	// andar com velocidade proporcional à distância.

      if(abs(x) < limiar_x || y < perto)    // Se a bola estiver suficientemente centralizada na horizontal,
        vel_angular = 0;                    // manter a esteira reta
      else
        vel_angular = x * VEL_ANG_MAX/100;  // Senão, velocidade angular proporcional ao deslocamento horizontal.  
    }

    // Cria a string de comando para o arduino e a envia
    sprintf(comando,"%3.1f,%2.3f,%i",vel_linear,vel_angular,FLAG_esteira);
    std::string send(comando);
    serial.send_string(send);

    // Recebe um sinal possivelmente enviado pelo arduino
    serial.receive();
    
    // Espera a taxa de execução e trata os callbacks
    loop_rate.sleep();
    ros::spinOnce();
  }
}