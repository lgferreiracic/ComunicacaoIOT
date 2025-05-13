#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/timer.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"   
#include "hardware/gpio.h"
#include "hardware/pwm.h"

#include "pico/cyw43_arch.h"       
#include "lwip/pbuf.h"           
#include "lwip/tcp.h"            
#include "lwip/netif.h"         

#include "./lib/button.h"
#include "./lib/buzzer.h"
#include "./lib/joystick.h"
#include "./lib/led_rgb.h"
#include "./lib/matrix.h"
#include "./lib/factory.h"

#define WIFI_SSID "A35 de Lucas"
#define WIFI_PASSWORD "lucaslucas"
#define LED_PIN CYW43_WL_GPIO_LED_PIN   

ssd1306_t ssd; // Variável para o display OLED SSD1306
volatile uint32_t last_time_button_a = 0; // Variável para debounce do botão A
volatile uint32_t last_time_button_b = 0; // Variável para debounce do botão B
volatile uint32_t last_time_joystick_button = 0; // Variável para debounce do botão do joystick
volatile int option_selected = 1; // Variável para armazenar a opção selecionada
uint16_t joystick_x, joystick_y; // Variáveis para armazenar a leitura do joystick em relação aos eixos x e y
uint8_t sector = 0; // Variável para armazenar o setor atual
Robot objectives[NUM_LOADS]; // Vetor para armazenar os objetivos do robô
int distances[NUM_LOADS]; // Vetor para armazenar as distâncias entre o robô e os objetivos
bool delivered[NUM_LOADS] = {false, false, false, false, false}; // Vetor para armazenar se os objetivos foram entregues
char html[1024]; // Buffer para armazenar a resposta HTML
Robot destination; // Variável para armazenar o destino do robô

// Estrutura responsável por armazenar a fábrica e o robô
Factory factory = {
    .sectors[0] = {
        2, 2, 2, 2, 2,
        2, 1, 0, 0, 2,
        2, 0, 0, 0, 0,
        2, 0, 0, 0, 2,
        2, 2, 0, 2, 2
    },
    .sectors[1] = {
        2, 2, 2, 2, 2,
        2, 0, 0, 0, 2,
        0, 0, 0, 0, 0,
        2, 0, 0, 0, 2,
        2, 2, 0, 2, 2
    },
    .sectors[2] = {
        2, 2, 2, 2, 2,
        2, 0, 0, 0, 2,
        0, 0, 0, 0, 2,
        2, 0, 0, 0, 2,
        2, 2, 0, 2, 2
    },
    .sectors[3] = {
        2, 2, 0, 2, 2,
        2, 0, 0, 0, 2,
        2, 0, 0, 0, 0,
        2, 0, 0, 0, 2,
        2, 2, 0, 2, 2
    },
    .sectors[4] = {
        2, 2, 0, 2, 2,
        2, 0, 0, 0, 2,
        0, 0, 0, 0, 0,
        2, 0, 0, 0, 2,
        2, 2, 0, 2, 2
    },
    .sectors[5] = {
        2, 2, 0, 2, 2,
        2, 0, 0, 0, 2,
        0, 0, 0, 0, 2,
        2, 0, 0, 0, 2,
        2, 2, 0, 2, 2
    },
    .sectors[6] = {
        2, 2, 0, 2, 2,
        2, 0, 0, 0, 2,
        2, 0, 0, 0, 0,
        2, 0, 0, 0, 2,
        2, 2, 2, 2, 2
    },
    .sectors[7] = {
        2, 2, 0, 2, 2,
        2, 0, 0, 0, 2,
        0, 0, 0, 0, 0,
        2, 0, 0, 0, 2,
        2, 2, 4, 2, 2
    },
    .sectors[8] = {
        2, 2, 0, 2, 2,
        2, 0, 0, 0, 2,
        0, 0, 0, 0, 2,
        2, 0, 0, 0, 2,
        2, 2, 2, 2, 2
    },
    .robot = {
        .position = {
            .x = 1,
            .y = 1
        },
        .sector = 0,
        .current_capacity = 0,
        .missing_deliverables = 5
    }
};

// Função para gerenciar a interrupção dos botões
void irq_handler(uint gpio, uint32_t events){
    if (gpio == BUTTON_A_PIN && debounce(&last_time_button_a)){
        clear_matrix();
        clear_display(&ssd);
        reset_usb_boot(0,0);
    }
    else if (gpio == BUTTON_B_PIN && debounce(&last_time_button_b)){
        option_selected = 1;
        clear_matrix();
        randomize_objectives(objectives, &factory);
        reset_delivered(delivered);
        for(int i = 0; i < NUM_SECTORS; i++){
            for(int j = 0; j < NUM_PIXELS; j++){
                if(factory.sectors[i][j] == 3){
                    factory.sectors[i][j] = 0;
                }
            }
        }
    }
}

// Função para gerar o HTML da página
void user_request(char *html, size_t html_size) {
    snprintf(html, html_size,
    "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
    "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<style>body{font-family:sans-serif;text-align:center;background:#eee;margin:10px}"
    "form{margin:5px}input,button{padding:8px;margin:3px;font-size:1em;border:0;border-radius:5px;width:130px;height:95px}"
    "button{background:#4CAF50;color:#fff}button.c{width:100px;height:100px;margin:2px}"
    "</style></head><body><h1>AMR</h1>"

    "<form method='GET' action='/move'>"  // Definido o action corretamente
    "<input name='a' placeholder='Setor' required>"
    "<input name='b' placeholder='Pos' required>"
    "<button style='background:#29f'>Mover</button></form>"

    "<form method='GET' action='/cvrp'><button style='background:#29f'>Entregas</button></form>"

    "<form method='GET' action='/manual'>"
    "<div><button class='c' name='dir' value='up'>C</button></div>"
    "<div><button class='c' name='dir' value='left'>E</button>"
    "<button class='c' name='dir' value='right'>D</button></div>"
    "<div><button class='c' name='dir' value='down'>B</button></div>"
    "</form>"

    "</body></html>");
}

// Função de callback para receber dados TCP
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }
    char *request = (char *)p->payload;
    int a = 0, b = 0;

    if (strncmp(request, "GET /cvrp", 9) == 0) { // Verifica se é a requisição CVRP
        printf("Botão CVRP pressionado!\n");
        reset_delivered(delivered);
        option_selected = 2;
        user_request(html, sizeof(html));
    }else if (strstr(request, "GET /move?a=")) { // Verifica se é a requisição de movimentação
        sscanf(request, "GET /move?a=%d&b=%d", &a, &b);
        printf("Parâmetros recebidos: Setor = %d, Posicao = %d\n", a, b);
        destination.sector = a - 1;
        destination.position.x = b / 5;
        destination.position.y = b % 5;
        destination.current_capacity = factory.robot.current_capacity;
        destination.missing_deliverables = factory.robot.missing_deliverables;
        option_selected = 3;
        user_request(html, sizeof(html));
    }else if(strncmp(request, "GET /manual?dir=up", 18) == 0){
        manual_mode_movimentation(&factory, &sector, 2048, 3500, &ssd, delivered, objectives);
    }else if(strncmp(request, "GET /manual?dir=down", 20) == 0){
        manual_mode_movimentation(&factory, &sector, 2048, 500, &ssd, delivered, objectives);
    }else if(strncmp(request, "GET /manual?dir=left", 20) == 0){
        manual_mode_movimentation(&factory, &sector, 500, 2048, &ssd, delivered, objectives);
    }else if(strncmp(request, "GET /manual?dir=right", 21) == 0){
        manual_mode_movimentation(&factory, &sector, 3500, 2048, &ssd, delivered, objectives);
    }else{
        user_request(html, sizeof(html));
    }
    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    pbuf_free(p);
    return ERR_OK;
}

// Função de callback ao aceitar conexões TCP
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err){
    tcp_recv(newpcb, tcp_server_recv);
    return ERR_OK;
}

// Função para inicializar o servidor TCP
int server_init(void) {
    //Inicializa a arquitetura do cyw43
    while (cyw43_arch_init()){
        printf("Falha ao inicializar Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }

    // GPIO do CI CYW43 em nível baixo
    cyw43_arch_gpio_put(LED_PIN, 0);

    // Ativa o Wi-Fi no modo Station, de modo a que possam ser feitas ligações a outros pontos de acesso Wi-Fi.
    cyw43_arch_enable_sta_mode();

    // Conectar à rede WiFI - fazer um loop até que esteja conectado
    printf("Conectando ao Wi-Fi...\n");
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 180000)){
        printf("Falha ao conectar ao Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }
    printf("Conectado ao Wi-Fi\n");

    // Caso seja a interface de rede padrão - imprimir o IP do dispositivo.
    if (netif_default){
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }

    // Configura o servidor TCP - cria novos PCBs TCP. É o primeiro passo para estabelecer uma conexão TCP.
    struct tcp_pcb *server = tcp_new();
    if (!server){
        printf("Falha ao criar servidor TCP\n");
        return -1;
    }

    //vincula um PCB (Protocol Control Block) TCP a um endereço IP e porta específicos.
    if (tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK){
        printf("Falha ao associar servidor TCP à porta 80\n");
        return -1;
    }

    // Coloca um PCB (Protocol Control Block) TCP em modo de escuta, permitindo que ele aceite conexões de entrada.
    server = tcp_listen(server);

    // Define uma função de callback para aceitar conexões TCP de entrada. É um passo importante na configuração de servidores TCP.
    tcp_accept(server, tcp_server_accept);
    printf("Servidor ouvindo na porta 80\n");
}

// Função para inicializar todo o hardware
void hardware_init_all(){
    stdio_init_all(); // Inicializa a comunicação padrão
    buzzer_init_all(); // Inicializa todos os buzzers
    matrix_init(); // Inicializa a matriz de LEDs RGB
    joystick_init(); // Inicializa o joystick
    led_init_all(); // Inicializa todos os LEDs
    button_init_all(); // Inicializa todos os botões
    display_init(&ssd); // Inicializa o display OLED SSD1306
    start_display(&ssd); // Inicia o display
    server_init(); // Inicializa o servidor TCP

    // Configuração dos pinos de interrupção
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &irq_handler); // Configura interrupção para o botão A
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &irq_handler); // Configura interrupção para o botão B

}

int main(){
    hardware_init_all(); // Inicializa o hardware
    randomize_objectives(objectives, &factory); // Posiciona os objetivos aleatoriamente
    reset_delivered(delivered); // Reinicializa o vetor que responsável por armazenar os objetivos entregues
    manual_mode_display(&ssd); // Exibe a tela de movimentação manual
    while (true) {
        cyw43_arch_poll(); // Necessário para manter o Wi-Fi ativo
        
        switch(option_selected){
            case 1:
                reading_joystick(&joystick_x, &joystick_y); // Leitura do joystick
                manual_mode_movimentation(&factory, &sector, joystick_x, joystick_y, &ssd, delivered, objectives); // Movimentação manual
                break;
            case 2:
                clear_display(&ssd); // Limpa o display
                clear_matrix(); // Limpa a matriz de LEDs RGB1
                solve_capacitated_vrp(&factory, objectives, distances, delivered, &sector, &ssd); // Resolve o problema do CVRP
                manual_mode_display(&ssd); // Exibe a tela de movimentação manual 
                option_selected = 1; // Retorna para o modo manual
                break;
            case 3:
                clear_display(&ssd); // Limpa o display
                clear_matrix(); // Limpa a matriz de LEDs RGB
                find_path(destination, &factory, &sector, delivered, objectives, &ssd); // Encontra o caminho e movimenta o robô
                manual_mode_display(&ssd); // Exibe a tela de movimentação manual
                option_selected = 1; // Retorna para o modo manual
                break;
            default:
                break;
        }
        sleep_ms(200);
    }
    cyw43_arch_deinit(); // Desativa o CYW43
    return 0;
}
