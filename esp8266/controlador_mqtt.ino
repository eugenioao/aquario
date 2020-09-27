/*****************************************************************
   Projeto para luminaria de um aquario marinho
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

 *****************************************************************/

// Bibliotecas usadas no projeto
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <FS.h>

// Define TimeZone e NTP para atualizar data/hora
#define time_zone -3

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.br.pool.ntp.org");


// GPIO com driver LDD para o canal
#define GPIO_BRANCO        D1
#define GPIO_AZUL_ROYAL    D2
#define GPIO_AZUL          D3
#define PORTA_SENSOR_TEMP  D4  // Senor DS18B20 - Só funciona na porta GPIO02 (D4)
#define GPIO_VIOLETA       D5

// GPIO com relê solicido para o canal
#define GPIO_COLLER_L      D7
#define GPIO_COLLER_A      D8

// Definições para o site Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "USERNAME" // Nome de login do site io.adafruit.com
#define AIO_KEY         "aio_CODE" // Chave criada no site io.adafruit.com

WiFiClient client;

// Definições para Sensor(es) de temperatura
OneWire oneWire(PORTA_SENSOR_TEMP);
DallasTemperature Sensores(&oneWire);

// Configuração do cliente MQTT que serão passadas para o servidor do MQTT
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// CANAIS
Adafruit_MQTT_Subscribe canal_branco     = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/canal-branco");
Adafruit_MQTT_Subscribe canal_azul       = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/canal-azul");
Adafruit_MQTT_Subscribe canal_azul_royal = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/canal-azul-royal");
Adafruit_MQTT_Subscribe canal_violeta    = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/canal-violeta");

// Temperatura da Luminaria e Aquario
Adafruit_MQTT_Subscribe temp_luminaria  = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/temp-luminaria");
Adafruit_MQTT_Subscribe temp_arquario   = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/temp-aquario");

// Publicar percentual dos canais, temperatura da luminaria, arquario e ph
Adafruit_MQTT_Publish tempo_real = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/tempo-real.canais-pwm");

// Testes dos canais
Adafruit_MQTT_Subscribe testes_canais = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/testes.canais");

Adafruit_MQTT_Subscribe *subscription;

// Valor inicial do PWM para o canal. Os valores serão multiplicados por 10 na função f_configura_pwm
int pwm_canal_branco = 0;
int pwm_canal_azul = 0;
int pwm_canal_azul_royal = 0;
int pwm_canal_violeta = 0;

// Definição padrão de iniciar/parar de cada canal
// BRANCO

String SEL_branco_inicial = "13:00"; // Guarda a hora selecionada para iniciar
String SEL_branco_parar   = "20:00"; // Guarda a hora selecionada para parar

long SS_branco_inicial = ((13 * 3600) + (0 * 60));   // Transforma a Hora de ligar dos LEDs em segundos
long SS_branco_parar   = ((20 * 3600) + (0 * 60));    // Transforma a Hora de desligar dos LEDs em segundos
int TempoEfeito_branco = 3000; // Valor para o calculo do incremento do FADE definido em segundos
int MaxPWM_branco      = 100; // Valor que será incrementado até maximo definido para canal.

byte IncEfeito_branco  = 1;  // Calcula o incremento para a porta do PWM do canal branco.
unsigned long ultimo_valor_branco; // Guarda o valor anterior em milisegundos para o fade do canal branco

// AZUL

String SEL_azul_inicial = "9:30"; // Guarda a hora selecionada para iniciar
String SEL_azul_parar   = "21:00"; // Guarda a hora selecionada para parar

long SS_azul_inicial = ((9 * 3600) + (30 * 60));   // Transforma a Hora de ligar dos LEDs em segundos
long SS_azul_parar   = ((21 * 3600) + (0 * 60));    // Transforma a Hora de desligar dos LEDs em segundos
int TempoEfeito_azul = 2400; // Valor para o calculo do incremento do FADE definido em segundos
int MaxPWM_azul      = 100; // Valor que será incrementado até maximo definido para canal.

byte IncEfeito_azul  = 1;  // Calcula o incremento para a porta do PWM do canal azul.
unsigned long ultimo_valor_azul; // Guarda o valor anterior em milisegundos para o fade do canal azul

// AZUL ROYAL

String SEL_azul_royal_inicial = "10:00"; // Guarda a hora selecionada para iniciar
String SEL_azul_royal_parar   = "20:30"; // Guarda a hora selecionada para parar

long SS_azul_royal_inicial = ((10 * 3600) + (0 * 60));   // Transforma a Hora de ligar dos LEDs em segundos
long SS_azul_royal_parar   = ((20 * 3600) + (30 * 60));    // Transforma a Hora de desligar dos LEDs em segundos
int TempoEfeito_azul_royal = 1800; // Valor para o calculo do incremento do FADE definido em segundos
int MaxPWM_azul_royal      = 100; // Valor que será incrementado até maximo definido para canal.

byte IncEfeito_azul_royal  = 1;  // Calcula o incremento para a porta do PWM do canal azul_royal.
unsigned long ultimo_valor_azul_royal; // Guarda o valor anterior em milisegundos para o fade do canal azul_royal

// VIOLETA

String SEL_violeta_inicial = "9:00"; // Guarda a hora selecionada para iniciar
String SEL_violeta_parar   = "20:00"; // Guarda a hora selecionada para parar

long SS_violeta_inicial = ((9 * 3600) + (0 * 60));   // Transforma a Hora de ligar dos LEDs em segundos
long SS_violeta_parar   = ((20 * 3600) + (0 * 60));    // Transforma a Hora de desligar dos LEDs em segundos
int TempoEfeito_violeta = 1800; // Valor para o calculo do incremento do FADE definido em segundos
int MaxPWM_violeta      = 100; // Valor que será incrementado até maximo definido para canal.

byte IncEfeito_violeta  = 1;  // Calcula o incremento para a porta do PWM do canal violeta.
unsigned long ultimo_valor_violeta; // Guarda o valor anterior em milisegundos para o fade do canal violeta


// OUTRAS VARIAVEIS
bool DEBUG = true;                // Definie se mostra ou não mensagens na serial

int pwm_periodo_inicial = 90000;  // Valor inicial do horario para o modo automatico
int pwm_periodo_final = 210000;   // Valor final do horario para o modo automatico
int pwm_periodo = 80000;          // Valor total para o horario que a luz deverá ficar ligada para o modo automatico
int temp_luminaria_lc = 47;       // Valor inicial para ligar os coolers da luminaria
int temp_aquario_lc = 27;         // Valor inicial para ligar os coolers do aquario

long TempoEsperaEfeito = 59000;   // Tempo de espera para o proximo fade em millisegundos.
long ultimo_TempoEsperaEfeito = -59000;

long SegundosAtuais = 0;          // Valor dos segundos atuais
int minPWM = 0;                   // Valor minimo do PWM dos LEDs
int i_ph = 0;                     // Valor inicial do projeto para notificação

int ultimo_valor_ph = 0;

int temp_luminaria_atual = 0;     // Valor da temperatuda da luminaria
int ultimo_temp_luminaria = 0;
int temp_aquario_atual = 0;       // Valor da temperatuda do aquario
int ultimo_temp_aquario = 0;

int TempqtdSensores;
DeviceAddress TempEndSensores;    // Endereços dos sensores conectados

bool b_conecta_MQTT = true;     // Define se esta conectado ao MQTT Server
bool b_canais_testes = false;   // Define se esta na aba de testes dos canais
int  max_canais_testes = 180;   // Tempo maximo em segundos para os canais de testes ficarem ligados sem receber informações
int  ultimo_canais_testes = 0;

String arqConfigCanais = "/propriedades.config";
String arqConfigWifi = "/wifi.config";

// Define dados para conexão WIFI
String WLAN_SSID = "ESP_WIFI_SETUP";
String WLAN_PASS = "";
bool b_salvar_wifi = false;


// Declaração de funções
void f_ler_propriedades();
void f_salvar_propriedades();
void f_conecta_MQTT();
void f_ler_fila();
void f_ajustar_canais();

//WiFiServer server(80);
ESP8266WebServer server(80);

// Inicia a Configuração do Controlar ESP8266
void setup()
{
  // Configuração dos canais Branco, Azul, Azul Royal e Violeta para sinal de saida
  pinMode(GPIO_BRANCO, OUTPUT);
  pinMode(GPIO_AZUL, OUTPUT);
  pinMode(GPIO_AZUL_ROYAL, OUTPUT);
  pinMode(GPIO_VIOLETA, OUTPUT);

  // Liga o LED para mostrar que o controlador ESP esta trabalhando
  //pinMode(D0,OUTPUT);
  //digitalWrite(D0, LOW);
  Serial.begin(115200);
  delay(2000);


  // Para formatar o flash. Necessário somente uma vez
  //SPIFFS.format();

  SPIFFS.begin();
  if (!SPIFFS.exists(arqConfigWifi))
  {
    if (WiFi.status() == WL_NO_SHIELD) {
      f_mostra_msg(1,  "Falha no Wifi do ESP!!", true);
      while (true);
    }
    delay(1000);
    f_mostra_msg(1, "", true);
    f_mostra_msg(1, "ATENÇÃO!!!", true);
    f_mostra_msg(1, "", true);
    f_mostra_msg(1, "A configuração do WIFI não existe e precisa ser feita para continuar.", true);
    f_mostra_msg(1, "Criando o Access Point de configuração...", true);

    bool status = WiFi.softAP(WLAN_SSID);
    if (status == false) {
      f_mostra_msg(1,  "Falhou para criar o SSID.", true);
      while (true);
    }

    f_mostra_msg(1,  "Endereço do servidor Web: ", false);
    Serial.println( WiFi.softAPIP() );

    f_mostra_msg(1, "Aguardando configurar o WIFI pela Web....", true);

    server.on("/", f_WebServerRoot);
    server.on("/salvar", f_WebServerForm);
    server.onNotFound(f_WebServerNaoEncontrado);
    server.begin();

    while (!b_salvar_wifi) {
      server.handleClient();
      delay(200);
    }
    f_salvar_wifi();
    delay(2000);
    WiFi.softAPdisconnect(WLAN_SSID);
  }
  f_mostra_msg(DEBUG,  "Arquivo " + arqConfigWifi + " encontrado!", true);
  f_ler_wifi();

  f_mostra_msg(DEBUG,  "", true);
  f_mostra_msg(DEBUG,  "Conectando ao WIFI", false);

  // Conexão com a rede WiFi. Mostra um "." na console até conectar ou dar erro
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    f_mostra_msg(DEBUG, ".", false);
    delay(200);
  }
  f_mostra_msg(DEBUG, "", true);
  f_mostra_msg(DEBUG, "Conectado a ", false);
  f_mostra_msg(DEBUG, WLAN_SSID, true);
  f_mostra_msg(DEBUG, "Endereço IP: ", false);
  if ( DEBUG ) {
    Serial.println(WiFi.localIP());
  }
  delay(200);

  f_mostra_msg(DEBUG, "Hora atual: ", false);

  timeClient.begin();
  timeClient.setTimeOffset(-10800);
  timeClient.update();
  delay(200);

  f_mostra_msg(DEBUG, String(timeClient.getHours()), false);
  f_mostra_msg(DEBUG, ":", false);
  f_mostra_msg(DEBUG, String(timeClient.getMinutes()), false);
  f_mostra_msg(DEBUG, ":", false);
  f_mostra_msg(DEBUG, String(timeClient.getSeconds()), true);

  // Inicia biblioteca dos sensores
  Sensores.begin();

  // Pesquisa pelos sensores conectados na GPIO02 (D4)
  f_mostra_msg(DEBUG, "", true);
  f_mostra_msg(DEBUG, "Pesquisando sensores de temperatura...", true);

  // Pega o numero de sensores conectados
  TempqtdSensores = Sensores.getDeviceCount();

  // O primeiro sempre será o da luminaria
  f_mostra_msg(DEBUG, "[", false);
  f_mostra_msg(DEBUG, String(TempqtdSensores), false);
  f_mostra_msg(DEBUG, "] sensor(es) encontrado(s).", true);

  if (TempqtdSensores > 0) {
    pinMode(GPIO_COLLER_L, OUTPUT);
    digitalWrite(GPIO_COLLER_L, LOW);
    if (TempqtdSensores > 0) {
      pinMode(GPIO_COLLER_A, OUTPUT);
      digitalWrite(GPIO_COLLER_A, LOW);
    }
  }

  // Conexão com os canais do MQTT
  mqtt.subscribe(&canal_branco);
  mqtt.subscribe(&canal_azul);
  mqtt.subscribe(&canal_azul_royal);
  mqtt.subscribe(&canal_violeta);

  mqtt.subscribe(&temp_arquario);
  mqtt.subscribe(&temp_luminaria);

  mqtt.subscribe(&testes_canais);

  if (SPIFFS.exists(arqConfigCanais))
  {
    f_mostra_msg(DEBUG, "Arquivo " + arqConfigCanais + " encontrado!", true);
    f_ler_propriedades();
  }
  else
  {
    f_salvar_propriedades();
  }

}

// Fica no loop infinito
void loop()
{

  // Função para conectar e reconectar no servidor MQTT
  f_conecta_MQTT();

  if ( b_conecta_MQTT ) {
    while (subscription = mqtt.readSubscription(300)) // Aguardando por informações
    {
      f_ler_fila();
    }
  }

  SegundosAtuais = ((timeClient.getHours() * 3600) + (timeClient.getMinutes() * 60) + timeClient.getSeconds());

  if (b_canais_testes) {
    if (SegundosAtuais >= (ultimo_canais_testes + max_canais_testes)) {
      b_canais_testes          = false;
      pwm_canal_branco         = 0;
      pwm_canal_azul           = 0;
      pwm_canal_azul_royal     = 0;
      pwm_canal_violeta        = 0;
      ultimo_TempoEsperaEfeito = 0;
    }
  } else {
    unsigned long MiliSegundosAtuais = millis();

    if (MiliSegundosAtuais > (ultimo_TempoEsperaEfeito + TempoEsperaEfeito)) {
      f_ajustar_canais();
      ultimo_TempoEsperaEfeito = MiliSegundosAtuais;

      if (TempqtdSensores > 0)
      {
        f_ler_temperatura();
        if (temp_luminaria_atual > temp_luminaria_lc) {
          f_mostra_msg(DEBUG, "Ligando coolers da luminaria.", true);
          digitalWrite(GPIO_COLLER_L, HIGH);
        } else {
          f_mostra_msg(DEBUG, "Desligando coolers da luminaria.", true);
          digitalWrite(GPIO_COLLER_L, LOW);
        }

        if (temp_aquario_atual > temp_aquario_lc) {
          f_mostra_msg(DEBUG, "Ligando coolers do aquario.", true);
          digitalWrite(GPIO_COLLER_A, HIGH);
        } else {
          f_mostra_msg(DEBUG, "Desligando coolers do aquario.", true);
          digitalWrite(GPIO_COLLER_A, LOW);
        }
      }
    }
  }
  f_configura_pwm(pwm_canal_branco, pwm_canal_azul, pwm_canal_azul_royal, pwm_canal_violeta);


  //i_ph = readPh
  //if (i_ph != ultimo_valor_ph)
  //{
  //  if (! p_ph.publish(i_ph)) {
  //    f_mostra_msg(DEBUG, "Falha ao enviar o valor do sensor.", true);
  //  }
  //}

}

// Função para conectar e reconectar (se necessário) ao servidor de MQTT
void f_conecta_MQTT() {
  int8_t ret;
  b_conecta_MQTT = true;

  // Retuna se ja estiver conectado.
  if (mqtt.connected()) {
    return;
  }
  f_mostra_msg(DEBUG, "", true);
  f_mostra_msg(DEBUG, "Conectando/Reconectando ao MQTT Server:", false);

  uint8_t tentativas = 3;
  while ((ret = mqtt.connect()) != 0) { // Retorna 0 quando conectar
    mqtt.disconnect();
    f_mostra_msg(DEBUG, ".", false);
    delay(5000);  // Aguarda 5 segundos antes de tentar novamente
    tentativas--;
    if (tentativas == 0) {
      // Se for a tentativa 3, aguarda o processo ser finalizado automaticamente
      f_mostra_msg(DEBUG, " ", true);
      f_mostra_msg(DEBUG, "Problemas para conectar ao MQTT Server!", true);
      //while (1);
      b_conecta_MQTT = false;
      return;
    }
  }
  f_mostra_msg(DEBUG, " OK", true);
  return;
}

// Função para Ler os valores do canal do servidor MQTT
void f_ler_fila()
{
  bool b_salvar = false;

  if (subscription == &canal_branco)
  {
    char *value = (char *)canal_branco.lastread;
    String message = String(value);
    message.trim();

    String tmpLigar    = f_seperar_string(message, ';', 0);
    String tmpDesligar = f_seperar_string(message, ';', 1);
    int    tmpEfeito   = f_seperar_string(message, ';', 2).toInt();
    int    tmpPWM      = f_seperar_string(message, ';', 3).toInt();

    int    tmpSSInicial  = ((f_seperar_string(tmpLigar, ':', 0).toInt() * 3600) + (f_seperar_string(tmpLigar, ':', 1).toInt() * 60));
    int    tmpSSParar    = ((f_seperar_string(tmpDesligar, ':', 0).toInt() * 3600) + (f_seperar_string(tmpDesligar, ':', 1).toInt() * 60));

    SEL_branco_inicial = tmpLigar;
    SEL_branco_parar   = tmpDesligar;
    SS_branco_inicial  = tmpSSInicial;
    SS_branco_parar    = tmpSSParar;
    TempoEfeito_branco = tmpEfeito;

    MaxPWM_branco = tmpPWM;
    b_salvar = true;
    f_mostra_msg(DEBUG, "Chegou mensagem do MQTT Server - Branco: " + message, true);
  }

  if (subscription == &canal_azul)
  {
    char *value = (char *)canal_azul.lastread;
    String message = String(value);
    message.trim();

    String tmpLigar    = f_seperar_string(message, ';', 0);
    String tmpDesligar = f_seperar_string(message, ';', 1);
    int    tmpEfeito   = f_seperar_string(message, ';', 2).toInt();
    int    tmpPWM      = f_seperar_string(message, ';', 3).toInt();

    int    tmpSSInicial  = ((f_seperar_string(tmpLigar, ':', 0).toInt() * 3600) + (f_seperar_string(tmpLigar, ':', 1).toInt() * 60));
    int    tmpSSParar    = ((f_seperar_string(tmpDesligar, ':', 0).toInt() * 3600) + (f_seperar_string(tmpDesligar, ':', 1).toInt() * 60));

    SEL_azul_inicial = tmpLigar;
    SEL_azul_parar   = tmpDesligar;
    SS_azul_inicial  = tmpSSInicial;
    SS_azul_parar    = tmpSSParar;
    TempoEfeito_azul = tmpEfeito;

    MaxPWM_azul = tmpPWM;
    b_salvar = true;
    f_mostra_msg(DEBUG, "Chegou mensagem do MQTT Server - Azul: " + message, true);
  }

  if (subscription == &canal_azul_royal)
  {
    char *value = (char *)canal_azul_royal.lastread;
    String message = String(value);
    message.trim();

    String tmpLigar    = f_seperar_string(message, ';', 0);
    String tmpDesligar = f_seperar_string(message, ';', 1);
    int    tmpEfeito   = f_seperar_string(message, ';', 2).toInt();
    int    tmpPWM      = f_seperar_string(message, ';', 3).toInt();

    int    tmpSSInicial  = ((f_seperar_string(tmpLigar, ':', 0).toInt() * 3600) + (f_seperar_string(tmpLigar, ':', 1).toInt() * 60));
    int    tmpSSParar    = ((f_seperar_string(tmpDesligar, ':', 0).toInt() * 3600) + (f_seperar_string(tmpDesligar, ':', 1).toInt() * 60));

    SEL_azul_royal_inicial = tmpLigar;
    SEL_azul_royal_parar   = tmpDesligar;
    SS_azul_royal_inicial  = tmpSSInicial;
    SS_azul_royal_parar    = tmpSSParar;
    TempoEfeito_azul_royal = tmpEfeito;

    MaxPWM_azul_royal = tmpPWM;
    b_salvar = true;
    f_mostra_msg(DEBUG, "Chegou mensagem do MQTT Server - Azul Royal: " + message, true);
  }

  if (subscription == &canal_violeta)
  {
    char *value = (char *)canal_violeta.lastread;
    String message = String(value);
    message.trim();

    String tmpLigar    = f_seperar_string(message, ';', 0);
    String tmpDesligar = f_seperar_string(message, ';', 1);
    int    tmpEfeito   = f_seperar_string(message, ';', 2).toInt();
    int    tmpPWM      = f_seperar_string(message, ';', 3).toInt();

    int    tmpSSInicial  = ((f_seperar_string(tmpLigar, ':', 0).toInt() * 3600) + (f_seperar_string(tmpLigar, ':', 1).toInt() * 60));
    int    tmpSSParar    = ((f_seperar_string(tmpDesligar, ':', 0).toInt() * 3600) + (f_seperar_string(tmpDesligar, ':', 1).toInt() * 60));

    SEL_violeta_inicial = tmpLigar;
    SEL_violeta_parar   = tmpDesligar;
    SS_violeta_inicial  = tmpSSInicial;
    SS_violeta_parar    = tmpSSParar;
    TempoEfeito_violeta = tmpEfeito;

    MaxPWM_violeta = tmpPWM;
    b_salvar = true;
    f_mostra_msg(DEBUG, "Chegou mensagem do MQTT Server - Violeta: " + message, true);
  }

  if (subscription == &testes_canais)
  {
    ultimo_canais_testes = ((timeClient.getHours() * 3600) + (timeClient.getMinutes() * 60) + timeClient.getSeconds());

    char *value = (char *)testes_canais.lastread;
    String message = String(value);
    message.trim();

    if (!b_canais_testes) {
      ultimo_valor_branco     = pwm_canal_branco;
      ultimo_valor_azul       = pwm_canal_azul;
      ultimo_valor_azul_royal = pwm_canal_azul_royal;
      ultimo_valor_violeta    = pwm_canal_violeta;
    }

    b_canais_testes = true;

    pwm_canal_branco     = f_seperar_string(message, ';', 0).toInt();
    pwm_canal_azul       = f_seperar_string(message, ';', 1).toInt();
    pwm_canal_azul_royal = f_seperar_string(message, ';', 2).toInt();
    pwm_canal_violeta    = f_seperar_string(message, ';', 3).toInt();

    b_salvar = false;
    f_mostra_msg(DEBUG, "Chegou mensagem do MQTT Server - TESTES: " + message, true);
    if ((pwm_canal_branco + pwm_canal_azul + pwm_canal_azul_royal + pwm_canal_violeta) <= 0 ) {
      b_canais_testes = false;
      ultimo_TempoEsperaEfeito = 0;
    }
  }

  if ( b_salvar )
  {
    f_salvar_propriedades();
    b_salvar = false;

  }

}


// Função para inserir os valores no PWM
void f_configura_pwm(int valor_branco, int valor_azul, int valor_azul_royal, int valor_violeta)
{

  bool publicar = false;

  if (valor_branco != ultimo_valor_branco) //Atualiza o PWM (Branco) se for diferente do ultimo valor definido
  {
    f_mostra_msg(DEBUG, "Ajustando o canal Branco " + String(valor_branco) + " - " + String(ultimo_valor_branco), true);
    analogWrite(GPIO_BRANCO, valor_branco * 10);
    ultimo_valor_branco = valor_branco;
    publicar = true;
  }

  if (valor_azul != ultimo_valor_azul) //Atualiza o PWM (Azul) se for diferente do ultimo valor definido
  {
    f_mostra_msg(DEBUG, "Ajustando o canal Azul " + String(valor_azul) + " - " + String(ultimo_valor_azul), true);
    analogWrite(GPIO_AZUL, valor_azul * 10);
    ultimo_valor_azul = valor_azul;
    publicar = true;
  }

  if (valor_azul_royal != ultimo_valor_azul_royal) //Atualiza o PWM (Azul Royal) se for diferente do ultimo valor definido
  {
    f_mostra_msg(DEBUG, "Ajustando o canal Azul Royal " + String(valor_azul_royal) + " - " + String(ultimo_valor_azul_royal), true);
    analogWrite(GPIO_AZUL_ROYAL, valor_azul_royal * 10);
    ultimo_valor_azul_royal = valor_azul_royal;
    publicar = true;
  }

  if (valor_violeta != ultimo_valor_violeta) //Atualiza o PWM (Violeta) se for diferente do ultimo valor definido
  {
    f_mostra_msg(DEBUG, "Ajustando o canal Violeta " + String(valor_violeta) + " - " + String(ultimo_valor_violeta), true);
    analogWrite(GPIO_VIOLETA, valor_violeta * 10);
    ultimo_valor_violeta = valor_violeta;
    publicar = true;
  }

  if (temp_luminaria_atual != ultimo_temp_luminaria) //Envia msg se for diferente do ultimo valor definido
  {
    f_mostra_msg(DEBUG, "A temperatura da Luminaria mudou: " + String(temp_luminaria_atual), true);
    ultimo_temp_luminaria = temp_luminaria_atual;
    publicar = true;
  }

  if (temp_aquario_atual != ultimo_temp_aquario) //Envia msg se for diferente do ultimo valor definido
  {
    f_mostra_msg(DEBUG, "A temperatura do Aquario mudou: " + String(temp_luminaria_atual), true);
    ultimo_temp_aquario = temp_aquario_atual;
    publicar = true;
  }

  if (! b_canais_testes && publicar && b_conecta_MQTT) {

    String tmpPublicar = String(valor_branco) + ";" + String(valor_azul) + ";" + String(valor_azul_royal) + ";" + String(valor_violeta) + ";" + String(temp_luminaria_atual)  + ";" + String(temp_aquario_atual) + ";" + String(ultimo_valor_ph);

    f_mostra_msg(DEBUG, "Publicando [" + tmpPublicar + "] no canal de Tempo Real", true);
    int tmpTamanhoArray = tmpPublicar.length() + 1;
    char tmpTexto[tmpTamanhoArray];
    tmpPublicar.toCharArray(tmpTexto, tmpTamanhoArray);
    tempo_real.publish(tmpTexto);

  }

}


// Função para ler a temperatura dos sensores e se for diferente do ultimo, envia para a fila.
void f_ler_temperatura() {

  // Pegar a temperatura atual
  Sensores.requestTemperatures();
  for (int i = 0; i < TempqtdSensores; i++) {

    if (Sensores.getAddress(TempEndSensores, i)) {
      // Mostrando os dados do sensor
      f_mostra_msg(DEBUG, "Temperatura do Sensor: ", false);
      f_mostra_msg(DEBUG, String(i), true);
      float tempC = Sensores.getTempC(TempEndSensores);
      f_mostra_msg(DEBUG, "Temp C: ", false);
      f_mostra_msg(DEBUG, String(tempC), true);
      if (i = 0) {
        temp_luminaria_atual = tempC;
      } else if (i = 1) {
        temp_aquario_atual = tempC;
      } else {
        f_mostra_msg(DEBUG, "ATENCAO!!! Nao existe definicao para este sensor.", false);
      }
    }
  }

}

// Função para ler os valores no arquivo de propriedades
void f_ler_propriedades() {

  //Faz a leitura do arquivo de configuração
  File tmpArquivo = SPIFFS.open(arqConfigCanais, "r");
  f_mostra_msg(DEBUG, "\nLendo arquivo " + arqConfigCanais + "...", true);
  while (tmpArquivo.available()) {
    String tmpLinha     = tmpArquivo.readStringUntil('\n');
    String tmpCanal     = f_seperar_string(tmpLinha, ';', 0);
    String tmpLigar     = f_seperar_string(tmpLinha, ';', 1);
    String tmpDesligar  = "";
    int    tmpEfeito    = 0;
    int    tmpSSInicial = 0;
    int    tmpSSParar   = 0;
    int    tmpPWM       = 0;

    if (tmpCanal == "1" || tmpCanal == "2" || tmpCanal == "3" || tmpCanal == "4")
    {

      tmpDesligar = f_seperar_string(tmpLinha, ';', 2);
      tmpEfeito   = f_seperar_string(tmpLinha, ';', 3).toInt();
      tmpPWM      = f_seperar_string(tmpLinha, ';', 4).toInt();

      tmpSSInicial  = ((f_seperar_string(tmpLigar, ':', 0).toInt() * 3600) + (f_seperar_string(tmpLigar, ':', 1).toInt() * 60));
      tmpSSParar    = ((f_seperar_string(tmpDesligar, ':', 0).toInt() * 3600) + (f_seperar_string(tmpDesligar, ':', 1).toInt() * 60));
    }

    //f_mostra_msg(DEBUG, tmpLinha, true);

    if (tmpCanal == "1")
    {

      SEL_branco_inicial = tmpLigar;
      SEL_branco_parar   = tmpDesligar;
      SS_branco_inicial  = tmpSSInicial;
      SS_branco_parar    = tmpSSParar;
      TempoEfeito_branco = tmpEfeito;

      MaxPWM_branco = tmpPWM;

    } else if (tmpCanal == "2")
    {

      SEL_azul_inicial = tmpLigar;
      SEL_azul_parar   = tmpDesligar;
      SS_azul_inicial  = tmpSSInicial;
      SS_azul_parar    = tmpSSParar;
      TempoEfeito_azul = tmpEfeito;

      MaxPWM_azul = tmpPWM;

    } else if (tmpCanal == "3")
    {

      SEL_azul_royal_inicial = tmpLigar;
      SEL_azul_royal_parar   = tmpDesligar;
      SS_azul_royal_inicial  = tmpSSInicial;
      SS_azul_royal_parar    = tmpSSParar;
      TempoEfeito_azul_royal = tmpEfeito;

      MaxPWM_azul_royal = tmpPWM;

    } else if (tmpCanal == "4")
    {

      SEL_violeta_inicial = tmpLigar;
      SEL_violeta_parar   = tmpDesligar;
      SS_violeta_inicial  = tmpSSInicial;
      SS_violeta_parar    = tmpSSParar;
      TempoEfeito_violeta = tmpEfeito;

      MaxPWM_violeta = tmpPWM;

    } else if (tmpCanal == "5")
    {
      temp_luminaria_lc = tmpLigar.toInt();

    } else if (tmpCanal == "6")
    {
      temp_aquario_lc = tmpLigar.toInt();
    }

    f_mostra_msg(DEBUG, tmpCanal + ";" + tmpLigar + ";" + tmpDesligar + ";" + String(tmpEfeito) + ";" + String(tmpSSInicial) + ";" + String(tmpSSParar) + ";" + String(tmpPWM), true);

  }
  tmpArquivo.close();

  // Define valor para forçar entrar na função f_ajustes_canais()
  ultimo_TempoEsperaEfeito = -59000;

  // Define o ultimo valor para forçar entrar na f_configura_pwm() para quando ligar o controlador ou chegar valores do MQTT.
  ultimo_valor_branco = 200;
  ultimo_valor_azul = 200;
  ultimo_valor_azul_royal = 200;
  ultimo_valor_violeta = 200;

}

// Função para gravar os valores no arquivo de propriedades
void f_salvar_propriedades() {

  f_mostra_msg(DEBUG, "Abrir arquivo" + arqConfigCanais + " para gravação!", true);
  //Abre o arquivo para escrita ("w")
  File tmpArquivo = SPIFFS.open(arqConfigCanais, "w+");
  if (!tmpArquivo) {
    f_mostra_msg(DEBUG, "Erro ao abrir arquivo" + arqConfigCanais + "!", true);
  } else {

    String tmpLinhas = "";

    // CANAIS
    tmpLinhas += "1;" + SEL_branco_inicial + ";" + SEL_branco_parar + ";" + TempoEfeito_branco + ";" + String(MaxPWM_branco) + "\n";
    tmpLinhas += "2;" + SEL_azul_inicial + ";" + SEL_azul_parar + ";" + TempoEfeito_azul + ";" + String(MaxPWM_azul) + "\n";
    tmpLinhas += "3;" + SEL_azul_royal_inicial + ";" + SEL_azul_royal_parar + ";" + TempoEfeito_azul_royal + ";" + String(MaxPWM_azul_royal) + "\n";
    tmpLinhas += "4;" + SEL_violeta_inicial + ";" + SEL_violeta_parar + ";" + TempoEfeito_violeta + ";" + String(MaxPWM_violeta) + "\n";
    tmpLinhas += "5;" + String(temp_luminaria_lc) + "\n";
    tmpLinhas += "6;" + String(temp_aquario_lc);

    tmpArquivo.println(tmpLinhas);
    f_mostra_msg(DEBUG, "Arquivo gravado :" + arqConfigCanais, true );
    f_ler_propriedades();

  }
  tmpArquivo.close();

}

// Função para fazer split de dados de uma string
String f_seperar_string(String dados, char separador, int indice)
{
  int encontrado = 0;
  int strIndice[] = { 0, -1 };
  int maxIndice = dados.length() - 1;

  for (int i = 0; i <= maxIndice && encontrado <= indice; i++) {
    if (dados.charAt(i) == separador || i == maxIndice) {
      encontrado++;
      strIndice[0] = strIndice[1] + 1;
      strIndice[1] = (i == maxIndice) ? i + 1 : i;
    }
  }
  return encontrado > indice ? dados.substring(strIndice[0], strIndice[1]) : "";
}


// Função para ajustar os valores do PWM dos canais de LEDs
void f_ajustar_canais() {

  SegundosAtuais = ((timeClient.getHours() * 3600) + (timeClient.getMinutes() * 60) + timeClient.getSeconds());
  int MinutosDiferencaEfeito;

  f_mostra_msg(DEBUG, "\nCalculando os valores para cada canal de acordo com a hora atual", true);

  if ((pwm_canal_branco >= MaxPWM_branco) || (SegundosAtuais >= (SS_branco_inicial + TempoEfeito_branco) && SegundosAtuais < (SS_branco_parar - TempoEfeito_branco))) {
    f_mostra_msg(DEBUG, "Branco     => Maximo do PWM: " + String(MaxPWM_branco), true);
    ultimo_valor_branco = pwm_canal_branco;
    pwm_canal_branco = MaxPWM_branco;
  }
  if (SegundosAtuais >= SS_branco_inicial && SegundosAtuais < (SS_branco_inicial + TempoEfeito_branco)) {

    MinutosDiferencaEfeito = (SegundosAtuais - SS_branco_inicial) / 60;
    IncEfeito_branco = MaxPWM_branco / (TempoEfeito_branco / 60);
    pwm_canal_branco = MinutosDiferencaEfeito * (IncEfeito_branco > 1 ? IncEfeito_branco : (IncEfeito_branco + 1));
    if (pwm_canal_branco > MaxPWM_branco) {
      pwm_canal_branco = MaxPWM_branco;
    }
    f_mostra_msg(DEBUG, "Branco     => Com Hora de Subida: " + String(pwm_canal_branco), true);

  }
  if (SegundosAtuais > (SS_branco_parar - TempoEfeito_branco) && SegundosAtuais < SS_branco_parar) {

    MinutosDiferencaEfeito = (SS_branco_parar - SegundosAtuais) / 60;
    IncEfeito_branco = MaxPWM_branco / (TempoEfeito_branco / 60);
    pwm_canal_branco = MaxPWM_branco - (MaxPWM_branco - (MinutosDiferencaEfeito * (IncEfeito_branco > 1 ? IncEfeito_branco : (IncEfeito_branco + 1))));
    if (pwm_canal_branco < minPWM) {
      pwm_canal_branco = minPWM;
    }
    f_mostra_msg(DEBUG, "Branco     => Com Hora de Descida: " + String(pwm_canal_branco), true);

  }

  if ((pwm_canal_azul >= MaxPWM_azul) || (SegundosAtuais >= (SS_azul_inicial + TempoEfeito_azul) && SegundosAtuais < (SS_azul_parar - TempoEfeito_azul))) {
    f_mostra_msg(DEBUG, "Azul       => Maximo do PWM: " + String(MaxPWM_azul), true);
    ultimo_valor_azul = pwm_canal_azul;
    pwm_canal_azul = MaxPWM_azul;
  }
  if (SegundosAtuais >= SS_azul_inicial && SegundosAtuais < (SS_azul_inicial + TempoEfeito_azul)) {

    MinutosDiferencaEfeito = (SegundosAtuais - SS_azul_inicial) / 60;
    IncEfeito_azul = MaxPWM_azul / (TempoEfeito_azul / 60);
    pwm_canal_azul = MinutosDiferencaEfeito * (IncEfeito_azul > 1 ? IncEfeito_azul : (IncEfeito_azul + 1));
    if (pwm_canal_azul > MaxPWM_azul) {
      pwm_canal_azul = MaxPWM_azul;
    }
    f_mostra_msg(DEBUG, "Azul       => Com Hora de Subida: " + String(pwm_canal_azul), true);

  }
  if (SegundosAtuais > (SS_azul_parar - TempoEfeito_azul) && SegundosAtuais < SS_azul_parar) {

    MinutosDiferencaEfeito = (SS_azul_parar - SegundosAtuais) / 60;
    IncEfeito_azul = MaxPWM_azul / (TempoEfeito_azul / 60);
    pwm_canal_azul = MaxPWM_azul - (MaxPWM_azul - (MinutosDiferencaEfeito * (IncEfeito_azul > 1 ? IncEfeito_azul : (IncEfeito_azul + 1))));
    if (pwm_canal_azul < minPWM) {
      pwm_canal_azul = minPWM;
    }
    f_mostra_msg(DEBUG, "Azul       => Com Hora de Descida: " + String(pwm_canal_azul), true);

  }

  if ((pwm_canal_azul_royal >= MaxPWM_azul_royal) || (SegundosAtuais >= (SS_azul_royal_inicial + TempoEfeito_azul_royal) && SegundosAtuais < (SS_azul_royal_parar - TempoEfeito_azul_royal))) {
    f_mostra_msg(DEBUG, "Azul Royal => Maximo do PWM: " + String(MaxPWM_azul_royal), true);
    ultimo_valor_azul_royal = pwm_canal_azul_royal;
    pwm_canal_azul_royal = MaxPWM_azul_royal;
  }
  if (SegundosAtuais >= SS_azul_royal_inicial && SegundosAtuais < (SS_azul_royal_inicial + TempoEfeito_azul_royal)) {

    MinutosDiferencaEfeito = (SegundosAtuais - SS_azul_royal_inicial) / 60;
    IncEfeito_azul_royal = MaxPWM_azul_royal / (TempoEfeito_azul_royal / 60);
    pwm_canal_azul_royal = MinutosDiferencaEfeito * (IncEfeito_azul_royal > 1 ? IncEfeito_azul_royal : (IncEfeito_azul_royal + 1));
    if (pwm_canal_azul_royal > MaxPWM_azul_royal) {
      pwm_canal_azul_royal = MaxPWM_azul_royal;
    }
    f_mostra_msg(DEBUG, "Azul Royal => Com Hora de Subida: " + String(pwm_canal_azul_royal), true);

  }
  if (SegundosAtuais > (SS_azul_royal_parar - TempoEfeito_azul_royal) && SegundosAtuais < SS_azul_royal_parar) {

    MinutosDiferencaEfeito = (SS_azul_royal_parar - SegundosAtuais) / 60;
    IncEfeito_azul_royal = MaxPWM_azul_royal / (TempoEfeito_azul_royal / 60);
    pwm_canal_azul_royal = MaxPWM_azul_royal - (MaxPWM_azul_royal - (MinutosDiferencaEfeito * (IncEfeito_azul_royal > 1 ? IncEfeito_azul_royal : (IncEfeito_azul_royal + 1))));
    if (pwm_canal_azul_royal < minPWM) {
      pwm_canal_azul_royal = minPWM;
    }
    f_mostra_msg(DEBUG, "Azul Royal => Com Hora de Descida: " + String(pwm_canal_azul_royal), true);

  }

  if ((pwm_canal_violeta >= MaxPWM_violeta) || (SegundosAtuais >= (SS_violeta_inicial + TempoEfeito_violeta) && SegundosAtuais < (SS_violeta_parar - TempoEfeito_violeta))) {
    f_mostra_msg(DEBUG, "Violeta    => Maximo do PWM: " + String(MaxPWM_violeta), true);
    ultimo_valor_violeta = pwm_canal_violeta;
    pwm_canal_violeta = MaxPWM_violeta;
  }
  if (SegundosAtuais >= SS_violeta_inicial && SegundosAtuais < (SS_violeta_inicial + TempoEfeito_violeta)) {

    MinutosDiferencaEfeito = (SegundosAtuais - SS_violeta_inicial) / 60;
    IncEfeito_violeta = MaxPWM_violeta / (TempoEfeito_violeta / 60);
    pwm_canal_violeta = MinutosDiferencaEfeito * (IncEfeito_violeta > 1 ? IncEfeito_violeta : (IncEfeito_violeta + 1));
    if (pwm_canal_violeta > MaxPWM_violeta) {
      pwm_canal_violeta = MaxPWM_violeta;
    }
    f_mostra_msg(DEBUG, "Violeta    => Com Hora de Subida: " + String(pwm_canal_violeta), true);

  }
  if (SegundosAtuais > (SS_violeta_parar - TempoEfeito_violeta) && SegundosAtuais < SS_violeta_parar) {

    MinutosDiferencaEfeito = (SS_violeta_parar - SegundosAtuais) / 60;
    IncEfeito_violeta = MaxPWM_violeta / (TempoEfeito_violeta / 60);
    pwm_canal_violeta = MaxPWM_violeta - (MaxPWM_violeta - (MinutosDiferencaEfeito * (IncEfeito_violeta > 1 ? IncEfeito_violeta : (IncEfeito_violeta + 1))));
    if (pwm_canal_violeta < minPWM) {
      pwm_canal_violeta = minPWM;
    }
    f_mostra_msg(DEBUG, "Violeta    => Com Hora de Descida: " + String(pwm_canal_violeta), true);

  }

  f_mostra_msg(DEBUG, "", false);
}

void f_WebServerRoot() {
  //  urlResposta += "<head><link rel=\"stylesheet\" href=\"styles.css\">";

  String urlResposta, urlCabecalho;

  urlResposta  = "<!DOCTYPE html><html lang=\"pt-BR\">";
  urlResposta += "<head>";
  urlResposta += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  urlResposta += "<title>Configura&ccedil;&atilde;o do WiFi</title></head>";
  urlResposta += "<body style=\"font-size: 14px; padding: 20px; line-height: 1.42857142857143;\">";
  urlResposta += "<div style=\"margin: 0 auto; max-width: 400px;\">";
  urlResposta += "<h1 style=\"text-align: center;\">Configura&ccedil;&atilde;o do WiFi</h1>";
  urlResposta += "<p style=\"text-align: center;\"><b>ATEN&Ccedil;&Atilde;O!!! Se errar as configura&ccedil;&otilde;es, ser&aacute; necess&aacute;rio apagar toda a flash do controlador.</b></p>";
  urlResposta += "<form method=\"post\" action=\"/salvar\">";
  urlResposta += "<div style=\"box-sizing: border-box; clear: both; padding: 4px 0; position: relative; margin: 1px 0; width: 100%;\">";
  urlResposta += "<label>SSID: </label>";
  urlResposta += "<input name=\"SSID\" type=\"text\" size=\"32\">";
  urlResposta += "</div>";
  urlResposta += "<div style=\"box-sizing: border-box; clear: both; padding: 4px 0; position: relative; margin: 1px 0; width: 100%;\">";
  urlResposta += "<label>Senha: </label>";
  urlResposta += "<input name=\"PASSWORD\" type=\"text\" size=\"35\">";
  urlResposta += "</div>";
  urlResposta += "<div style=\"text-align: center; box-sizing: border-box; clear: both; padding: 4px 0; position: relative; margin: 1px 0 0; width: 100%;\">";
  urlResposta += "<button style=\"position: absolute; top: 50%; left: 50%;\" type=\"submit\">Salvar</button>";
  urlResposta += "</div></form></div></body></html>";

  urlCabecalho  = "HTTP/1.1 200 OK\r\n";
  urlCabecalho += "Content-Length: ";
  urlCabecalho += urlResposta.length();
  urlCabecalho += "\r\n";
  urlCabecalho += "Content-Type: text/html\r\n";
  urlCabecalho += "Connection: close\r\n";
  urlCabecalho += "\r\n";

  server.send(200, "text/html", urlResposta);

}


// Função para configuração do SSID
void f_WebServerForm() {

  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Metodo n&atilde;o permitido!");
  } else {
    String message = "Dados enviados para gravacao:\n";
    String tmpGrava = "";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
      if (server.argName(i) == "SSID" ) {
        WLAN_SSID = server.arg(i);
      }
      if (server.argName(i) == "PASSWORD" ) {
        WLAN_PASS = server.arg(i);
      }
    }
    message += "\n\nSe os dados estiverem corretos, o controlador ira se conectar ao roteador WiFi e este SSID nao funcionara mais. ";
    server.send(200, "text/plain", message);
    b_salvar_wifi = true;

  }

}

void f_WebServerNaoEncontrado() {
  String message = "URI n&atilde;o encontrada\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


// Le os valores para conexão com o WIFI
void f_ler_wifi() {

  //Faz a leitura do arquivo de configuração
  File tmpArquivo = SPIFFS.open(arqConfigWifi, "r");
  f_mostra_msg(DEBUG, "\nLendo arquivo " + arqConfigWifi + "...", true);
  while (tmpArquivo.available()) {
    String tmpLinha = tmpArquivo.readStringUntil('\n');
    String tmpItem = f_seperar_string(tmpLinha, ';', 0);
    if ( tmpItem == "1" ) {
      WLAN_SSID = f_seperar_string(tmpLinha, ';', 1);
      WLAN_PASS = f_seperar_string(tmpLinha, ';', 2);
    }
  }
  tmpArquivo.close();

  f_mostra_msg(1, "", true);
}

// Salva os valores para conexão com o WIFI
void f_salvar_wifi() {

  File tmpArquivo = SPIFFS.open(arqConfigWifi, "w+");
  if (!tmpArquivo) {
    f_mostra_msg(DEBUG, "Erro ao abrir arquivo: " + arqConfigWifi + "!", true);
  } else {
    f_mostra_msg(DEBUG, "Arquivo gravado: " + arqConfigWifi, true );
    tmpArquivo.print("1;" + WLAN_SSID + ";" + WLAN_PASS + "\n");
  }
  tmpArquivo.close();

  f_mostra_msg(1, "", true);
}

// Função para mostrar ou não as mensagens na serial (DEBUG)
void f_mostra_msg(bool Debug, String Mensagem, bool NovaLinha) {

  if (Debug > 0) {
    if ( NovaLinha ) {
      Serial.println( Mensagem );
    }
    else {
      Serial.print( Mensagem );
    }
  }

}
