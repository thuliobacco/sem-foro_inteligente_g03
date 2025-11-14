#include "UbidotsEsp32Mqtt.h"

/****************************************
 * Define Constants
 ****************************************/
const char *WIFI_SSID = "Inteli.Iot";
const char *WIFI_PASS = "%(Yk(sxGMtvFEs.3";

const char *UBIDOTS_TOKEN = "";
const char *DEVICE_LABEL = "esp32Semaforo";
const char *VARIABLE_LABEL1 = "LDR";
const char *VARIABLE_LABEL2 = "distancia";
const char *UBIDOTS_VARIABLE_LABEL = "botao_ubi";
const char *CLIENT_ID = "santana";

Ubidots ubidots(UBIDOTS_TOKEN, CLIENT_ID);

const int PUBLISH_FREQUENCY = 3000;
const int SUBSCRIBER_FREQUENCY = 2000;

unsigned long timer;
unsigned long timerSubscriber;
int valor_ldr;
float distancia_cm;

// Pinos do SEMÁFORO 1 (Rua principal)
#define LED_RED_1 17
#define LED_YELLOW_1 15
#define LED_GREEN_1 4

// Pinos do SEMÁFORO 2 (Rua secundária)
#define LED_RED_2 27
#define LED_YELLOW_2 26
#define LED_GREEN_2 25

#define LDR_PIN 34

// Pinos do sensor HC-SR04
#define TRIG_PIN 22
#define ECHO_PIN 21

// Distância de detecção (em cm)
const float DIST_DETECCAO = 3.0;

bool modoNoturnoAtivo = false;

class SistemaSemaforos
{
private:
  const int THRESHOLD_LUZ = 2000;
  bool noite = false;

  // Estados do sistema
  enum Estado
  {
    SEM1_VERDE_SEM2_VERMELHO, // Semáforo 1 verde, Semáforo 2 vermelho
    SEM1_VERMELHO_SEM2_VERDE,
    SEM1_AMARELO_SEM2_VERDE,
    SEM1_VERMELHO_SEM2_AMARELO,
    DETECCAO_AMBOS_VERMELHOS, // Modo noturno: detecção ativa
  };

  Estado estadoAtual = SEM1_VERDE_SEM2_VERMELHO;
  unsigned long tempoEstado = 0;
  unsigned long tempoDeteccao = 0;
  bool deteccaoAtiva = false;

public:
  // Controle Semáforo 1
  void semaforo1_red()
  {
    digitalWrite(LED_RED_1, HIGH);
    digitalWrite(LED_YELLOW_1, LOW);
    digitalWrite(LED_GREEN_1, LOW);
  }

  void semaforo1_yellow()
  {
    digitalWrite(LED_RED_1, LOW);
    digitalWrite(LED_YELLOW_1, HIGH);
    digitalWrite(LED_GREEN_1, LOW);
  }

  void semaforo1_green()
  {
    digitalWrite(LED_RED_1, LOW);
    digitalWrite(LED_YELLOW_1, LOW);
    digitalWrite(LED_GREEN_1, HIGH);
  }

  void semaforo1_off()
  {
    digitalWrite(LED_RED_1, LOW);
    digitalWrite(LED_YELLOW_1, LOW);
    digitalWrite(LED_GREEN_1, LOW);
  }

  // Controle Semáforo 2
  void semaforo2_red()
  {
    digitalWrite(LED_RED_2, HIGH);
    digitalWrite(LED_YELLOW_2, LOW);
    digitalWrite(LED_GREEN_2, LOW);
  }

  void semaforo2_yellow()
  {
    digitalWrite(LED_RED_2, LOW);
    digitalWrite(LED_YELLOW_2, HIGH);
    digitalWrite(LED_GREEN_2, LOW);
  }

  void semaforo2_green()
  {
    digitalWrite(LED_RED_2, LOW);
    digitalWrite(LED_YELLOW_2, LOW);
    digitalWrite(LED_GREEN_2, HIGH);
  }

  void semaforo2_off()
  {
    digitalWrite(LED_RED_2, LOW);
    digitalWrite(LED_YELLOW_2, LOW);
    digitalWrite(LED_GREEN_2, LOW);
  }

  // Ambos desligados
  void ambos_off()
  {
    semaforo1_off();
    semaforo2_off();
  }

  // Ambos vermelhos
  void ambos_vermelhos()
  {
    semaforo1_red();
    semaforo2_red();
  }

  void ldr()
  {
    valor_ldr = analogRead(LDR_PIN);
    if (valor_ldr >= THRESHOLD_LUZ)
    {
      noite = false;
    }
    else
    {
      noite = true;
    }
  }

  float medirDistancia()
  {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duracao = pulseIn(ECHO_PIN, HIGH, 30000);
    float dist = (duracao * 0.034) / 2.0;

    if (dist == 0 || dist > 400)
    {
      dist = 400;
    }

    return dist;
  }

  void modoNoturno()
  {
    unsigned long now = millis();
    distancia_cm = medirDistancia();

    // Se detectou objeto próximo
    if (distancia_cm < DIST_DETECCAO)
    {
      if (!deteccaoAtiva)
      {
        Serial.println("*** MODO NOTURNO: Objeto detectado! Ambos VERMELHOS ***");
        deteccaoAtiva = true;
        tempoDeteccao = now;
      }

      // Mantém ambos vermelhos por 5 segundos
      ambos_vermelhos();

      if (now - tempoDeteccao >= 5000)
      {
        deteccaoAtiva = false;
        Serial.println("*** Voltando ao pisca amarelo ***");
      }
    }
    else
    {
      deteccaoAtiva = false;

      // Pisca amarelo em ambos os semáforos
      static bool ledState = false;
      static unsigned long lastBlink = 0;

      if (now - lastBlink >= 500)
      {
        ledState = !ledState;
        if (ledState)
        {
          semaforo1_yellow();
          semaforo2_yellow();
        }
        else
        {
          ambos_off();
        }
        lastBlink = now;
      }
    }
  }

  void modoDiurno()
  {
    unsigned long now = millis();
    distancia_cm = medirDistancia();

    // Se detectou objeto próximo
    if (distancia_cm < DIST_DETECCAO)
    {
      if (!deteccaoAtiva)
      {
        Serial.println("*** MODO DIURNO: Objeto detectado! Ambos VERMELHOS ***");
        deteccaoAtiva = true;
        tempoDeteccao = now;
      }

      // Mantém ambos vermelhos por 5 segundos
      ambos_vermelhos();

      if (now - tempoDeteccao >= 5000)
      {
        deteccaoAtiva = false;
        Serial.println("*** Voltando ao funcionamento normal ***");
      }
    }
    else
    {

      // Ciclo normal do semáforo
      switch (estadoAtual)
      {
      case SEM1_VERDE_SEM2_VERMELHO:
        semaforo1_green();
        semaforo2_red();
        if (now - tempoEstado >= 5000)
        { // 5 segundos verde
          estadoAtual = SEM1_AMARELO_SEM2_VERDE;
          tempoEstado = now;
        }
        break;

      case SEM1_AMARELO_SEM2_VERDE:
        semaforo1_yellow();
        semaforo2_green();
        if (now - tempoEstado >= 5000)
        { // 5 segundos verde
          estadoAtual = SEM1_VERMELHO_SEM2_VERDE;
          tempoEstado = now;
        }
        break;

      case SEM1_VERMELHO_SEM2_VERDE:
        semaforo1_red();
        semaforo2_green();
        if (now - tempoEstado >= 2000)
        { // 2 segundos amarelo
          estadoAtual = SEM1_VERMELHO_SEM2_AMARELO;
          tempoEstado = now;
        }
        break;

      case SEM1_VERMELHO_SEM2_AMARELO:
        semaforo1_red();
        semaforo2_yellow();
        if (now - tempoEstado >= 1000)
        { // 1 segundo ambos vermelhos
          estadoAtual = SEM1_VERDE_SEM2_VERMELHO;
          tempoEstado = now;
        }
        break;

      case DETECCAO_AMBOS_VERMELHOS:
        // Não usado no modo diurno
        break;
      }
    }
  }

  void working()
  {
    ldr();

    // Verifica se deve ativar modo noturno
    if (modoNoturnoAtivo || noite)
    {
      modoNoturno();
    }
    else
    {
      modoDiurno();
    }
  }

  void inicializar()
  {
    estadoAtual = SEM1_VERDE_SEM2_VERMELHO;
    tempoEstado = millis();
  }
};

// Instância da classe
SistemaSemaforos sistema;

/****************************************
 * Callback: Recebe dados do Ubidots
 ****************************************/
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println("Calling Callback...");
  String message = "";
  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  if (message == "1")
  {
    modoNoturnoAtivo = true;
    Serial.println("*** Modo NOTURNO ativado via Ubidots! ***");
  }
  else if (message == "0")
  {
    modoNoturnoAtivo = false;
    Serial.println("*** Modo DIURNO ativado via Ubidots! ***");
  }
}

/****************************************
 * Main Functions
 ****************************************/
void setup()
{
  Serial.begin(115200);
  ubidots.setDebug(true);
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  // Configura pinos Semáforo 1
  pinMode(LED_RED_1, OUTPUT);
  pinMode(LED_YELLOW_1, OUTPUT);
  pinMode(LED_GREEN_1, OUTPUT);

  // Configura pinos Semáforo 2
  pinMode(LED_RED_2, OUTPUT);
  pinMode(LED_YELLOW_2, OUTPUT);
  pinMode(LED_GREEN_2, OUTPUT);

  pinMode(LDR_PIN, INPUT);

  // Configura pinos do sensor HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  timer = millis();
  timerSubscriber = millis();

  sistema.inicializar();

  Serial.println("==============================================");
  Serial.println("Sistema de 2 Semáforos Inteligentes");
  Serial.println("Semáforo 1: Pinos 4(R), 2(Y), 15(G)");
  Serial.println("Semáforo 2: Pinos 5(R), 18(Y), 19(G)");
  Serial.println("==============================================");
}

void loop()
{
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }

  // Executa lógica do sistema
  sistema.working();

  // Publica valores no Ubidots periodicamente
  if (millis() - timer > PUBLISH_FREQUENCY)
  {
    ubidots.add(VARIABLE_LABEL1, valor_ldr);
    ubidots.add(VARIABLE_LABEL2, distancia_cm);
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }

  // Atualiza valor do botão (assinatura MQTT)
  if (millis() - timerSubscriber > SUBSCRIBER_FREQUENCY)
  {
    ubidots.subscribeLastValue(DEVICE_LABEL, UBIDOTS_VARIABLE_LABEL);
    timerSubscriber = millis();
  }

  ubidots.loop();
}