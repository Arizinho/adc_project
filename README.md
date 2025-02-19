# Conversor Analógico Digital
Este projeto implementa um sistema de controle de LEDs RGB e display OLED utilizando um joystick analógico na BitDogLab. O sistema permite o ajuste da intensidade dos LEDs com base nos valores analógicos do joystick e exibe um ponto no display OLED correspondente à posição do joystick. Além disso, o botão do joystick e o botão A permitem alterar estados dos LEDs e desativar o PWM dos LEDs.

---

## **Configuração dos Pinos GPIO**

Os pinos GPIO da Raspberry Pi Pico estão configurados conforme a tabela abaixo:

| GPIO   | Componente     | Função                                                                           |
| ------ | -------------- | -------------------------------------------------------------------------------- |
| **27** | Joystick X     | Entrada analógica para leitura do eixo horizontal                                |
| **26** | Joystick Y     | Entrada analógica para leitura do eixo vertical                                  |
| **22** | Joystick Botão | Entrada digital para alterar estado do LED verde e estilo da borda do display    |
| **11** | LED Verde      | Saída digital para indicação do estado do botão do joystick                      |
| **12** | LED Azul       | Saída PWM para controle de brilho                                                |
| **13** | LED Vermelho   | Saída PWM para controle de brilho                                                |
| **5**  | Botão A        | Entrada digital para desativar o PWM dos LEDs                                    |
| **14** | I2C SDA        | Linha de dados para comunicação I2C com o display OLED                           |
| **15** | I2C SCL        | Linha de clock para comunicação I2C com o display OLED                           |

---

## **Funcionamento do Código**

1. **Inicialização:**

   - Configura os pinos GPIO para os LEDs, botões e entrada analógica do joystick.
   - Inicializa a comunicação I2C para o display OLED.
   - Configura o PWM para controle dos LEDs vermelho e azul.
   - Configura interrupções nos botões do joystick e Botão A.

2. **Leitura do Joystick:**

   - O sistema realiza a leitura analógica dos eixos X e Y do joystick.
   - Os valores são usados para ajustar o brilho dos LEDs vermelho e azul.
   - O ponto no display OLED é atualizado de acordo com a posição do joystick.

3. **Controle dos LEDs:**

   - O **Botão do Joystick** alterna o estado do LED verde e estilo da borda do display.
   - O **Botão A** desativa o PWM dos LEDs, desligando-os.
   - Se o Botão A for pressionado, os LEDs não respondem ao joystick.

4. **Exibição no Display OLED:**

   - O display OLED exibe um ponto cuja posição é definida pelos valores analógicos do joystick.
   - Se o Botão do Joystick for pressionado, a borda do display é alterada.

5. **Tratamento de Interrupção:**

   - Os botões do joystick e Botão A são configurados para gerar interrupção ao serem pressionados.
   - Um debounce de 200 ms é aplicado para evitar leituras erradas.

---

## **Link do Vídeo**

- Youtube: https://youtu.be/mJ3kyfn0cyg

