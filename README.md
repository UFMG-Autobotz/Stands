# Esteira

## Operação com joystick

### Pré-requisitos
Ter a [IDE] do Arduíno instalada no computador

### Instalação
Carregue o código em `Esteira/ard_esteira` para o Arduíno da esteira e o código `Esteira/ard_joystick` para o Arduino do joystick.

### Operação
* Alimente o Arduíno do Joystick com as baterias. Pode ser necessário usar jumpers. Recomenda-se fixar a bateria na parte de baixo do Arduíno com durex, fita crepe, fita isolante ou algo do tipo.
* Às vezes a comunicação entre os Arduínos é perdida (razões para tal são até então desconhecidas). Caso isso aconteça, pressione o botão de reset do Arduíno do joystick.  
**Atenção**: quando isso acontece, o movimento trava no último comando recebido, isto é, a esteira pode começar a andar indefinidamente para frente sem que nada possa ser feito para pará-la até que o Arduíno resete.

## Seguindo bolinhas

### Pré-requisitos
* Sistema operacional Linux
* Ter o [ROS] Kinetic instalado no computador
* Ter a [IDE] do Arduíno instalada no computador
* Configure seu computador para jamais entrar em espera, nem por tempo de inatividade nem quando fechar a tampa

### Instalação
* Abra o terminal na pasta `Esteira/ros` e rode o comando `catkin_make`
* Carregue o código em `Esteira/ard_bolinha` para o Arduíno da esteira

### Operação
* Conecte o Arduíno da esteira no computador
* Com o terminal na pasta `Esteira/ros`, rode o comando `source devel/setup.bash`
  * Só é necessário executar este passo uma vez, sempre que uma nova janela do terminal for aberta
* Verifique em qual porta o Arduíno foi conectado através da IDE ou pelo comando `ls /dev | grep ACM`
  * Caso a porta seja `ttyACM0`, nada precisa ser feito
  * Caso seja `ttyACM1`, os comandos a seguir devem vir seguidos de `porta:=25`:
* Para inicializar a esteira no modo de seguir a bolinha laranja, rode o comando `roslaunch esteira bola.launch`
* Para inicializar a esteira no modo de seguir o cubo amarelo (?), rode o comando `roslaunch esteira cubo.launch`
* Para encerrar a execução, pressione `Ctrl+C` no terminal e, em seguida, abra a janela da câmera e pressione `Q`
* OBS: mantenha seu computador carregando na tomada enquanto a esteira não estiver sendo usada


Qualquer dúvida que surgir, por menor que seja, não hesite em perguntar. Estarei disposto a ajudar.  
Daniel L.

[ROS]: https://www.ros.org
[IDE]: https://www.arduino.cc/en/Guide/Linux
