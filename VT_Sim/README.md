# Dependências
* ROS Kinetic
* PyQT4

# Instruçōes para rodar
## 1. Inicializar ROS:
```
roscore
```

## 2. Abrir simulação:
Na pasta Bash:
```
// Terreno plano
bash run_flatfield.sh
```
ou
```
// Terreno com inclinaçōes
bash run_field.sh
```

## 3. Abrir interface grafica:
Em Modules/Plugin_GUI
```
python main.py
```

## 4. Preparar interface grafica:
No menu superior, clicar em Controller e em Nova Aba

## 5. Enviar comandos:
Ajustar os valores de velocidade das rodas esquerdas e direitas e clicar em Publish para publicar.
Observaçoes:
* Quando Once esta marcado o valor selecionado é enviado uma vez, quando Stream está marcado o valor é reenviado a cada modificação. Independentemente da opção escolhida, é  necessário clicar em Publish para iniciar o envio. Para parar o envio caso o inicie em Stream, basta clicar na opção Once.
* Lembrar que a simumaçao inicia pausada, é preciso dar play.
* Atualmente a GUI envia valores de velocidade entre -10 e 10 rad/s, esses valores sao razoaveis para o que queremos mostrar, mas é possivel modificar em Modules/Plugin_GUI/topics_vt.yaml

# Comportamento
* Velocidades iguais e positivas: andar para a frente
* Velocidades iguais e negativas: andar para trás
* Rodas esquerdas com velocidade mais alta que rodas direitas: girar para a direita
* Rodas direitas com velocidade mais alta que rodas esquerdas: girar para a esquerda
* Rodas de lados opostos girando com velocidades de mesmo módulo e sentido contrario: girar sem sair do lugar
