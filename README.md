#  Projeto para luminaria de um aquario marinho
   ---------------------------------------------

   Detalhes do Projeto:
   Este projeto foi desenvolvido para ser usado com uma luminaria
   de 4 canais para um aquario marinho.
   O kit contém um controlador ESP8266 para gerenciar 4(quatro)
   canais de LED, 2 sensores de temperatura DS18B20, 2 relês de
   estado solido para os canais de coolers (1º para a Luminaria e
   2º para o Aquario) e 1 probe de PH.
   Também foi desenvolvido um aplicativo para Android que ajudará
   na configuração do controlador (não é obrigatorio usa-lo).

   Este projeto foi construido usando o protocolo MQTT com as
   bibliotecas disponibilizadas pelo site Adafruit.com
   https://learn.adafruit.com/mqtt-adafruit-io-and-you/overview

   Autor: Eugenio Oliveira
   Data : 01/08/2020

   Itens:
   1) 1 x Controlador ESP68266 NodeMCU v3;
      https://www.espressif.com/en/products/socs/esp8266

   2) 4 x LDD MaanWell 1000H com placa;
      https://www.meanwell.com/Upload/PDF/LDD-H/LDD-H-SPEC.PDF

   3) 2 x Sensores DS18B20 a prova de agua;
      https://playground.arduino.cc/Learning/OneWire/

   4) 1 x Relê duplo de estado solido (G3MB202P);
   5) 1 x Probe de ph compativel com o ESP8266;
   6) 1 x StepDown para 5v ou fonte externa;
   7) 1 x Conta no site https://io.adafruit.com.

 *****************************************************************

  Definição padrão para cada canal
  --------------+-------+----------+--------+--------
      Canal     | ligar | desligar | efeito | valor
  --------------+-------+----------+--------+--------
   1-Branco     |  13   |    20    |   50   |  95
  --------------+-------+----------+--------+--------
   2-Azul       | 9:30  |    21    |   40   |  95
  --------------+-------+----------+--------+--------
   3-Azul Royal |  10   |   20:30  |   30   |  95
  --------------+-------+----------+--------+--------
   4-Violeta    |   9   |    20    |   30   |  85
  --------------+-------+----------+--------+--------

 *****************************************************************

  Definição dos canais do servidor MQTT
  ----------------------+----------------------------------------
  CHAVE                 |  DESCRIÇÃO
  ----------------------+----------------------------------------
  canal-branco          | Serie do Canal Branco
  canal-azul            | Serie do Canal Azul
  canal-azul-royal      | Serie do canal Azul Royal
  canal-violeta         | Serie do Canal Violeta
  ----------------+----------------------------------------------
  temp-luminaria        | Temperatura Ligar Coolers Luminaria
  temp-aquario          | Temperatura Ligar Coolers Aquario
  ----------------------+----------------------------------------
  tempo-real.canais-pwm | Valores dos Canais PWM
  ----------------------+----------------------------------------
  testes.canais         | Testar os canais individualmente
  ----------------------+----------------------------------------
  OBS: Os canais no MQTT Server irão receber os valores separados
       por ponto e virgula. Foi feito desta forma para não
       ultrapassar a quantiade de filas permitida pelo site
       Adafruit.com para uma conta gratuita.

 *****************************************************************
  Definição das Portas do controlador ESP8266
 *****************************************************************

  GPIO5   (D1) - Branco
  GPIO4   (D2) - Azul Royal
  GPIO0   (D3) - Azul
  GPIO2   (D4) - Senores de temperatura
  3V3     (3V) - Positivo Senores de temperatura
  GND      (G) - Negativo Senores de temperatura
  GPIO14  (D5) - Violeta
  GPIO13  (D7) - Cooler Luminaria
  GPIO15  (D8) - Cooler Aquario
  GPIO9  (SD2) -
  GPIO10 (SD3) -
