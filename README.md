# SEMÁFORO INTELIGENTE 
## Ponderada de programação - semana 05 - Grupo 03
### Participantes:

Antonio Cillo

Caue Taddeo

Carlos Eduardo

Gabriel Willian

Rafael Santana

Thúlio Bacco

---


# **Projeto – Semáforo Inteligente com ESP32**

Este projeto tem como objetivo simular o funcionamento de um **semáforo inteligente** instalado na interseção entre uma rua e uma avenida principal. O sistema opera em dois modos,  **diurno** e **noturno**, determinados automaticamente por um **sensor LDR**, e conta também com um **sensor ultrassônico** utilizado como detector de presença (simulando pedestres).



## **Descrição Geral do Sistema**

* **Modo Diurno:**
  Ativado quando o LDR detecta luminosidade suficiente. Nesse modo, os dois semáforos (um para cada via) operam normalmente, alternando seus ciclos entre **vermelho → amarelo → verde**.

* **Modo Noturno:**
  Quando o ambiente fica escuro (o LDR registra baixa luminosidade), o sistema entra no modo noturno. Neste estado:

  * Ambos os semáforos passam a **piscar amarelo**.


## **Integração dos Sensores**

### **LDR – Sensor de Luminosidade**

O LDR está conectado ao **pino analógico 34** do ESP32 e é responsável por medir a quantidade de luz no ambiente:

* Quanto **mais claro**, maior o valor lido.
* Quanto **mais escuro**, menor o valor.

No código, esse valor é comparado com uma constante (`THRESHOLD_LUZ`), que define o limite entre modo diurno e noturno:

* **Leitura acima do limite:** Modo **diurno**, semáforos seguem o ciclo normal.
* **Leitura abaixo do limite:** Modo **noturno**, semáforos piscam amarelo e podem ser interrompidos caso o sensor ultrassônico detecte movimento.

Assim, o LDR atua como componente central para definir automaticamente o modo de operação do sistema.



### **Sensor Ultrassônico – Detector de Proximidade**

Esse sensor foi adicionado para simular a detecção de pedestres ou veículos próximos ao cruzamento.
Quando ativado (objeto dentro da distância configurada), o sistema:

* Interrompe o funcionamento atual (tanto no modo noturno quanto no diurno),
* Coloca ambos os semáforos no **vermelho** por alguns segundos.


## **Demonstração em Vídeo**

**Link para o vídeo no Google Drive:**
*(Caso apareça a mensagem “Este arquivo de vídeo ainda está sendo processado…”, selecione **Download** → **Fazer download mesmo assim**)*
👉 [https://drive.google.com/file/d/1zj4RKUmdVISI1NEAsFDGtwgOMOaGHaX2/view?usp=sharing](https://drive.google.com/file/d/1zj4RKUmdVISI1NEAsFDGtwgOMOaGHaX2/view?usp=sharing)



## **Dashboard – Plataforma Ubidots**

Abaixo está a imagem do dashboard montado na Ubidots, exibindo dados de luminosidade coletados. 


<img width="2936" height="1678" alt="imagem ubidots" src="https://github.com/user-attachments/assets/543e886d-93dc-49a1-8aac-8fc7aacc00de" />


O código do projeto está localizado no arquivo `semaforo.ino` na raiz do projeto. 
