# Proyecto Arduino CONECTA-4:

## Introducción:
Este proyecto es un ejemplo para realizar un conecta 4 con pulsadores y una pantalla TFT.<br />
Dibujaremos un tablero de 6 filas por 7 columnas y utilizaremos un pulsador por columna para añadir una ficha al juego.<br />
Cada ficha caerá por la columna correspondiente hasta encontrar una posición ocupada o llegar al final del tablero.<br />
Una vez la ficha se detenga comprobamos si se ha ganado el juego, en dicho caso mostramos una animación con las fichas que han conseguido coenctar 4 (o más). En el caso contrario cambiaremos el turno del jugador.<br />
Opcionalmente se incluye un pulsador más que sirve para reiniciar el juego y dos leds con los colores de los jugadores, estando en ON cuando sea su turno y un modo DEBUG para visualizar por Serial la matriz bidimensional con las fichas.

## Componentes:

- 1 protoboard
- 2 resistencias 220Ω
- 8 resistencias 10kΩ
- 2 leds
- 8 pulsadores
- 1 Pantalla - https://www.arduino.cc/en/Guide/TFT
- 1 Arduino UNO (en caso de usar otro modelo revisar las conexiones de la pantalla TFT)

## Conexiones:

-Diagrama:
-Fotografía:


