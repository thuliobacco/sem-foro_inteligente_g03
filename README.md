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

Nesta etapa, montamos dois semáforos controlados por um ESP32 e adicionamos um sensor LDR para que o sistema consiga reagir à luminosidade do ambiente. A ideia é que, quando escurece, os semáforos mudem automaticamente do modo “diurno” para o modo “noturno”.
Funcionamento do LDR e integração ao sistema
O LDR está ligado ao pino analógico 34 do ESP32. Ele mede a quantidade de luz no ambiente: quanto mais claro, maior o valor lido; quanto mais escuro, menor o valor. No código, esse valor é comparado com um limite (THRESHOLD_LUZ).

- Se a leitura fica acima do limite, o sistema entende que é “dia” e os semáforos seguem o ciclo normal (verde–amarelo–vermelho entre as duas vias).

- Se a leitura fica abaixo do limite, o sistema entende que é “noite” e entra no modo noturno, em que os semáforos passam a piscar amarelo e, quando há detecção de veículo pelo sensor de distância, ficam ambos vermelhos por alguns segundos.

Dessa forma, o LDR é o responsável por informar ao sistema em qual modo (diurno ou noturno) os dois semáforos devem operar.

**LINK DO DRIVE COM O VÍDEO DE DEMONSTRAÇÃO (OBS: caso apareça a mensagem "Este arquivo de vídeo ainda está sendo processado para reprodução. Tente de novo mais tarde." clique em *'download'* e depois em *'fazer o download mesmo assim'*):**
https://drive.google.com/file/d/1zj4RKUmdVISI1NEAsFDGtwgOMOaGHaX2/view?usp=sharing

Segue a imagem da plataforma Ubidots, que demonstra os dados de forma interativa. 

<img width="2936" height="1678" alt="imagem ubidots" src="https://github.com/user-attachments/assets/543e886d-93dc-49a1-8aac-8fc7aacc00de" />
