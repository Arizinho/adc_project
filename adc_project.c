#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include <stdlib.h>
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/timer.h"

#define JOYSTICK_X 27
#define JOYSTICK_Y 26
#define JOYSTICK_BUTTON 22

#define BLUE_LED 12
#define RED_LED 13
#define GREEN_LED 11

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

//booleano para indicar que botão do joystick foi pressionado
volatile bool joystick_pressed = 0;

ssd1306_t ssd; // Inicializa a estrutura do display

//função para inicializar pinos do joystick
void joystick_init(){
    //inicializa analógico
    adc_init();
    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y);

    //inicializa botão do joystick
    gpio_init(JOYSTICK_BUTTON);
    gpio_set_dir(JOYSTICK_BUTTON, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON);
}

//função para fazer leitura analógica do joystick
void joystick_read_value(uint16_t *analog_x, uint16_t *analog_y){
    //lê movimento em horizontal
    adc_select_input(1);
    *analog_x = adc_read();

    //lê movimento vertical
    adc_select_input(0);
    *analog_y = adc_read();
}


//função para inicializar led como PWM
void pwm_led_init(uint gpio){
    uint8_t slice;
    gpio_set_function(gpio, GPIO_FUNC_PWM); //configura como PWM
    slice = pwm_gpio_to_slice_num(gpio); //número do slice ligado ao pino
    pwm_set_clkdiv(slice, 125); //divide clock por 125
    pwm_set_wrap(slice, 999); //configura contagem até 1000 (999 + 1)
    pwm_set_gpio_level(gpio, 0); //inicializa duty cycle em 0 (led desligado)
    pwm_set_enabled(slice, true);  //habilita PWM no slice
}

//função para inicializar led verde como saída GPIO
void green_led_init(){
    gpio_init(GREEN_LED);
    gpio_set_dir(GREEN_LED, GPIO_OUT);
    gpio_put(GREEN_LED, 0);
}

//função para configurar brilho do led de acordo com leituras do joystick
void led_config(uint16_t analog_x, uint16_t analog_y){
    uint16_t red, blue;

    //faz mapeamento dos valores analógicos para níveis do PWM:
    //2048 ~ 0 para 0 ~ 1000 (com saltos de 50 em 50)
    //2048 ~ 4095 para 0 ~ 1000 (com saltos de 50 em 50)
    red = (abs(analog_x-2048)*1000)/2048 - ((abs(analog_x-2048)*1000)/2048)%50; 
    blue = (abs(analog_y-2048)*1000)/2048 - ((abs(analog_y-2048)*1000)/2048)%50;
    pwm_set_gpio_level(RED_LED, red);
    pwm_set_gpio_level(BLUE_LED, blue);
}

//função para inicializar comunicação I2C com display oled
void i2c_oled_init(){
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

//função para tratar interrupção no botão do joystick
void gpio_irq_handler(uint gpio, uint32_t events){
    //variável para guardar tempo da ocorrência da última interrupção
    static uint32_t last_time = 0;

    //compara diferença entre tempo atual e da última interrupção efetiva (debounce por delay de 0,2 s) 
    if(to_ms_since_boot(get_absolute_time())-last_time > 200){
        last_time = to_ms_since_boot(get_absolute_time()); //salva valor do tempo de ocorrência da última interrupção
        joystick_pressed = !joystick_pressed; //muda valor de booleano que indica se o botão foi pressionado
        //alterna estado do led verde de acordo com o pressionamento do botão
        gpio_put(GREEN_LED, joystick_pressed);
        

    }
}

int main()
{
    //inicializa periféricos
    uint16_t analog_x, analog_y, oled_x, oled_y;
    pwm_led_init(BLUE_LED);
    pwm_led_init(RED_LED);
    green_led_init();
    joystick_init();
    i2c_oled_init();
    //stdio_init_all();

    //habilita interrupção por borda de descida no botão do joystick
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON, GPIO_IRQ_EDGE_FALL, 1, gpio_irq_handler);

    while (true) {
        //lê valor do analógico
        joystick_read_value(&analog_x, &analog_y);

        //converte valor analógico em posição x,y do display
        oled_x = (analog_x)*119/4095 + 1;
        oled_y = (4095-analog_y)*55/4095 + 1;

        //se o botão for pressionado, desliga pwm
        if (joystick_pressed){
            led_config(2048, 2048);
        }
        //caso contrário, ajusta intensidade dos leds azul e vermelho de acordo com leitura analógica
        else {
            led_config(analog_x, analog_y);
        }
        ssd1306_fill(&ssd, false); //limpa tela oled
        ssd1306_rect(&ssd, 0, 0, 128, 64, true, joystick_pressed); //desenha borda da tela -> se botão do joystick for pressionado, altera o estilo da borda
        ssd1306_draw_dot(&ssd, oled_x, oled_y); //desenha ponto de acordo com movimento do joystick
        ssd1306_send_data(&ssd); //envia dados para tela
    }
}
