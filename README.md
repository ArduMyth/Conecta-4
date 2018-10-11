# Proyecto Arduino CONECTA-4:

## Introducción:
Este proyecto es un ejemplo para realizar un conecta 4 con pulsadores y una pantalla TFT.<br />
Dibujaremos un tablero de 6 filas por 7 columnas y utilizaremos un pulsador por columna para añadir una ficha al juego.<br />
Cada ficha caerá por la columna correspondiente hasta encontrar una posición ocupada o llegar al final del tablero.<br />
Una vez la ficha se detenga comprobamos si se ha ganado el juego, en dicho caso mostramos una animación con las fichas que han conseguido coenctar 4 (o más). En el caso contrario cambiaremos el turno del jugador.<br />
Opcionalmente se incluye un pulsador más que sirve para reiniciar el juego y dos leds con los colores de los jugadores, estando "ON" cuando sea su turno y un modo DEBUG para visualizar por Serial la matriz bidimensional con las fichas.

## Librería:

- TFTLibrary: https://www.arduino.cc/en/Reference/TFTLibrary

## Componentes:

- 1 breadboard
- 2 resistencias 220Ω
- 8 resistencias 10kΩ
- 2 leds
- 8 pulsadores
- 1 Pantalla - https://www.arduino.cc/en/Guide/TFT
- 1 Arduino UNO (en caso de usar otro modelo revisar las conexiones de la pantalla TFT)

## Conexión TFT según Modelos de Arduino:

- TFTtoBoards: https://www.arduino.cc/en/Guide/TFTtoBoards

## Conexiones y ejemplo:

*Led Amarillo*: **A0**<br/>
*Led Rojo*: **A1**<br/>

*Pulsador columna 1*: **A3**<br/>
*Pulsador columna 2*: **A4**<br/>
*Pulsador columna 3*: **A5**<br/>
*Pulsador columna 4*: **D2**<br/>
*Pulsador columna 5*: **D3**<br/>
*Pulsador columna 6*: **D5**<br/>
*Pulsador columna 7*: **D6**<br/>
*Pulsador reset*: **A2**<br/>

*+5V*:	**5V**<br/>
*MISO*:	**D12**<br/>
*SCK*:	**D13**<br/>
*MOSI*:	**D11**<br/>
*LCD CS*:	**D10**<br/>
*SD CS*:	**D4**<br/>
*D/C*: **D9**<br/>
*RESET*: **D8**<br/>
*GND*: **GND**<br/>

### Más:
- Diagrama: https://github.com/ArduMyth/conecta-4/blob/master/20181011_123447.pdf<br />
- Fotografía: https://github.com/ArduMyth/conecta-4/blob/master/20181011_123410.pdf

## Vídeo:

- (Añadir)

## Publicación:

- (Añadir)
