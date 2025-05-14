# EmbarcaTech_ComunicaçãoIOT.
<p align="center">
  <img src="Group 658.png" alt="EmbarcaTech" width="300">
</p>

## Projeto: Simulação de AMR para Logística 4.0 com Servidor Web

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)
![Raspberry Pi](https://img.shields.io/badge/-Raspberry_Pi-C51A4A?style=for-the-badge&logo=Raspberry-Pi)
![HTML](https://img.shields.io/badge/HTML-%23E34F26.svg?style=for-the-badge&logo=html5&logoColor=white)
![CSS](https://img.shields.io/badge/CSS-1572B6?style=for-the-badge&logo=css3&logoColor=fff)
![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)
![Windows 11](https://img.shields.io/badge/Windows%2011-%230079d5.svg?style=for-the-badge&logo=Windows%2011&logoColor=white)

## Descrição do Projeto

Este projeto tem como objetivo desenvolver um servidor web embarcado utilizando o Raspberry Pi Pico W para controlar um robô móvel autônomo (AMR) em um ambiente simulado de logística 4.0. O sistema permite o controle e o monitoramento remoto do robô por meio de uma interface web acessível por dispositivos como smartphones e computadores conectados à mesma rede Wi-Fi.

A aplicação demonstra conceitos de robótica móvel, redes embarcadas e algoritmos de otimização aplicados à logística, sendo especialmente relevante para a indústria (aplicação escolhida para o Webserver).

## Componentes Utilizados

- **Joystick (ADC nos eixos X e Y)**: Captura dos valores analógicos de movimentação.
- **Microcontrolador Raspberry Pi Pico W (RP2040)**: Responsável pelo controle dos pinos GPIO.
- **LED RGB**: Com os pinos conectados às GPIOs 11, 12 e 13.
- **Botão A**: Conectado à GPIO 5.
- **Botão B**: Conectado à GPIO 6
- **Botão do Joystick**: Conectado à GPIO 22.
- **Display SSD1306**: Conectado via I2C nas GPIOs 14 e 15.
- **Matriz de LEDs WS2812B**: Conectada à GPIO 7.
- **Buzzer 1 e Buzzer 2**: Emitindo alertas sonoros, conectados às GPIOs 10 e 21.
- **Potenciômetro do Joystick**: Conectado às entradas analógicas GPIO 26 (eixo X) e GPIO 27 (eixo Y).

## Ambiente de Desenvolvimento

- **VS Code**: Ambiente de desenvolvimento utilizado para escrever e debugar o código.
- **Linguagem C**: Linguagem de programação utilizada no desenvolvimento do projeto.
- **Pico SDK**: Kit de Desenvolvimento de Software utilizado para programar a placa Raspberry Pi Pico W.
- **Biblioteca LwIP**: Implementação leve do protocolo IP para sistemas embarcados e dispositivos com recursos limitados;
- **HTML e CSS**: Tecnologias essenciais para o desenvolvimento de interfaces web.

## Guia de Instalação

1. Clone o repositório:
2. Importe o projeto utilizando a extensão da Raspberry Pi.
3. Compile o código utilizando a extensão da Raspberry Pi.
4. Caso queira executar na placa BitDogLab, insira o UF2 na placa em modo bootsel.
5. Para a simulação, basta executar pela extensão no ambiente integrado do VSCode.

## Guia de Uso
O projeto possui três funcionalidades principais:

- **Movimentação Manual**:

  O AMR pode ser controlado por meio do joystick ou pelos direcionais da interface web, 
  permitindo que o usuário o direcione no ambiente de simulação.
  A posição do robô é representada graficamente em uma matriz de LEDs, onde:
    1. Vermelho: Representa o robô.
    2. Azul: Representa as paredes da fábrica.
    3. Verde: Indica as cargas a serem transportadas.
    4. Magenta: Indica o ponto de distribuição.
  
  Se o robô colidir com uma parede, um LED RGB e um buzzer fornecem um alerta sonoro e visual.
  Ao coletar ou entregar uma carga, um feedback luminoso e sonoro é acionado.

- **Navegação Automática**:

  O AMR é capaz de se movimentar de forma autônoma, sem a necessidade de controle pelo usuário.
  Algoritmos de Busca em Espaços de Estados são utilizados para otimizar o trajeto do robô.
  O usuário define um ponto de destino e o robô se move até ele automaticamente.

  O display auxiliar exibe:
    1. O setor da fábrica onde o robô está localizado.
    2. A capacidade atual de carga do robô.
    3. A quantidade de cargas ainda pendentes para transporte.

- **Entregas Automáticas (CVRP)**:

  Algoritmos de Busca em Espaços de Estados e Capacitated Vehicle Routing Problem (CVRP) são utilizados para otimizar o trajeto do robô.
  O robô automaticamente coleta e transporta cargas até o ponto de distribuição de maneira eficiente.

- **Monitoramento do Sistema**:

  O display auxiliar fornece informações sobre o funcionamento do sistema em tempo real.
  Dados relevantes sobre o estado do robô e da simulação são atualizados dinamicamente.

  O display auxiliar exibe:
    1. O setor da fábrica onde o robô está localizado.
    2. A capacidade atual de carga do robô.
    3. A quantidade de cargas ainda pendentes para transporte.

## Testes

Testes básicos foram implementados para garantir que cada componente está funcionando corretamente, incluindo: 

  1. Leitura dos valores do joystick e resposta dos LEDs RGB.
  2. Verificação do correto acionamento dos buzzers.
  3. Renderização precisa da posição do robô no display e na matriz de LEDs.
  4. Execução da lógica de movimentação manual e automática.

## Desenvolvedor

[Lucas Gabriel Ferreira](https://github.com/usuario-lider)

## Vídeo da Solução

[Link do YouTube](https://www.youtube.com/watch?v=w6TDg70J72w)

