/********************************************/
/*PROYECTO CONECTA-4

Autor: ArduMyth - https://forum.arduino.cc/index.php?action=profile;u=892050
                - ardumyth@gmail.com
                - https://github.com/ArduMyth

Componentes usados: 
  - 1 protoboard
  - 2 resistencias 220Ω
  - 8 resistencias 10kΩ
  - 2 leds
  - 8 pulsadores
  - 1 Pantalla LCD - https://www.arduino.cc/en/Guide/TFT

/********************************************/
/* LCD */
//Librerías para ROBOT LCD 
#include <TFT.h>
#include <SPI.h>

//Pins LCD para ARDUINO UNO
#define cs   10
#define dc   9
#define rst  8

TFT TFTscreen = TFT(cs, dc, rst);

/********************************************/
/* ELEMENTOS EN ARRAYS */
#define SIZE(x) (sizeof(x) / sizeof(x[0]))

/********************************************/
/* CONFIGURACION */

/*
DEBUG en true muestra en consola los
movimientos y la posición final de las 
fichas en el array bidimensional.
(Esto realentizará la animación).
*/

#define DEBUG false 
#define FILAS 6
#define COLUMNAS 7
#define retardo_ficha 20
#define rotar_turno_finpartida true 

/********************************************/
/* COMPONENTES DEL JUEGO */
#define led_Amarillo A0
#define led_Rojo     A1
#define btn_reset    A2

struct Botones{
  byte pin;
  unsigned int intervalo;
  unsigned long tiempo;
};
Botones Btn[8] = {
  /* 
     Los pulsadores están ordenados por columna dejando al final el reset;
     Desde: A3 => columna 0 Hasta: D6 => columna 6
     De esta manera el índice del array corresponde con la columna del tablero. 
     Si btn_reset estuviese al inicio deberíamos restar 1 a la columna 
     en mover_ficha(byte columna) 
  */
  {A3, 500, 0}, {A4, 500, 0},       //columna 1 y 2
  {A5, 500, 0}, {2, 500, 0},        //columna 3 y 4
  {3, 500, 0} , {5, 500, 0},        //columna 5 y 6
  {6, 500, 0} , {btn_reset, 200, 0} //columna 7 y reset
};

/********************************************/
/* VARIABLES PARA LA PARTIDA */
byte
valorFicha[] = {1,2}, 
fichas = FILAS * COLUMNAS,
tablero[FILAS][COLUMNAS] = {};

bool
turno_jugador = false,
turno_activo  = false;

/********************************************/
/* INICIO */
void setup() {
  Serial.begin(9600);
  TFTscreen.begin();
  pinMode(led_Amarillo, OUTPUT);
  pinMode(led_Rojo,     OUTPUT);
  for(byte i = 0 ; i < SIZE(Btn); i++){
    pinMode(Btn[i].pin, INPUT);
  }
  dibujar_presentacion();
}

/********************************************/
/* LOOP */
void loop() {
  if(fichas > 0){
    for(byte i = 0; i < SIZE(Btn); i++){
      /*Si hemos presionado un pulsador*/
      if(valor_pulsador(&Btn[i])){
        if(Btn[i].pin == btn_reset){
          reiniciar();
        }else if(!turno_activo){ /*Hasta que no termine la ficha de caer no se activará el pulsador*/
          mover_ficha(i); /*el índice será la columna importante el orden de lso botones*/
        }
        /*Desde que se pulse un botón dejamos de revisar el resto*/
        break;
      }
    }
  }else{
    /*Se ha completado el tablero sin conectar 4*/
    dibujar_empate();
  }
  digitalWrite(led_Rojo,    (!turno_jugador) ? true : false);
  digitalWrite(led_Amarillo,(turno_jugador)  ? true : false);
}

/********************************************/
/* ESTADO Y DEBOUNCE DE LOS BOTONES */
bool valor_pulsador(Botones *b){  
  if (digitalRead(b->pin) && millis() - b->tiempo >= b->intervalo) {
    b->tiempo = millis();
    return true; 
  }
  return false;
}

/********************************************/
/* RESTABLECER VARIABLES */
void reiniciar(){
  memset(tablero,0,sizeof(tablero));
  turno_jugador = (rotar_turno_finpartida) ? !turno_jugador : turno_jugador; 
  turno_activo  = false;
  fichas = FILAS * COLUMNAS;
  dibujar_reset();
  dibujar_tablero();
}

/********************************************/
/* ANIMACIÓN Y POSICIONAMIENTO DE LAS FICHAS*/
void mover_ficha(byte columna){    
  /*Posición donde acabará finalmente la ficha 
    y desde la cual se buscará si hay victoria.*/
  byte posicion_final = 0;

  /*Movimientos de la ficha hasta "caer"
    en la correspondiente posición. */
  for(byte fila = 0; fila < SIZE(tablero); fila++){
    if(tablero[fila][columna] == 0 ){
      if(turno_activo){
        tablero[fila-1][columna] = 0;
        /*Eliminamos el movimiento anterior para crear la animación*/
        TFTscreen.stroke(255,255,255);
        TFTscreen.fill(20,190,190);
        TFTscreen.circle(columna * 20 + 20, (fila-1) * 20 + 14, 8);
      }
      /*posicion libre, comienza el turno.*/
      turno_activo = true;
      tablero[fila][columna] = valorFicha[turno_jugador];
      
      TFTscreen.stroke(255,255,255);
      TFTscreen.fill(200, (!turno_jugador) ? 0 : 200 ,0);
      TFTscreen.circle(columna * 20 + 20, fila * 20 + 14, 8);
      delay(retardo_ficha);
    }
    else{
      posicion_final = fila-1;
      break;
    }
    /*Mostrar los pasos de las fichas*/
    if(DEBUG){
      mostrar_debug();
    }
    /*Llega a la última posición sin que hubiese
      alguna ficha por medio*/
    posicion_final = fila;
  }
  /* Si se produjo un movimiento posible, el turno está
     activo. Al finalizar de colocar la ficha:
     1. Cambiamos el turno del jugador.
     2. Descontamos una ficha
     3. Revisamos si hay victoria
  */
  if(turno_activo){ 
    fichas--;
    //revisar cuando el número de fichas TOTALES colocadas sea 7 (minimo de fichas para ganar)
    if( fichas <= FILAS * COLUMNAS - 7 ) { 
      if( revisar_victoria(posicion_final,columna) ){
        dibujar_ganador();
      }
    }
    turno_activo = false;
    turno_jugador = !turno_jugador;
  }
}

/********************************************/
/* COMPROBAMOS LAS POSICIONES DE LAS FICHAS */

bool revisar_victoria(int fila, int columna){
  String finales = "";  
  /**********************************/
  /*REVISAMOS LAS OPCIONES VERTICALES 
  Y LUEGO LAS HORIZONTALES*/
  finales += comprobar_recto(fila,columna);

  /**********************************/
  /*REVISAMOS LAS DIAGONALES*/
  String diagonal_1 = "", diagonal_2 = "";

  diagonal_1 = eliminar_repetidos(
                  comprobar_diagonal(fila,columna,"inferior_izq") +
                  comprobar_diagonal(fila,columna,"superior_drcha")
               );            
  if(diagonal_1.length() >= 8){
    finales += diagonal_1; 
  }

  diagonal_2 = eliminar_repetidos(
                  comprobar_diagonal(fila,columna,"superior_izq") +
                  comprobar_diagonal(fila,columna,"inferior_drcha")
               );           
  if(diagonal_2.length() >= 8){
    finales += diagonal_2; 
  }

  //Comprobación final de todas las posibles
  finales = eliminar_repetidos(finales);
  if(finales.length() >= 8){
    dibujar_jugada(finales);
    return true;       
  }
  return false;
}

/*
ELIMINAMOS LAS POSICIONES REPETIDAS AL REVISARLAS
POR EJEMPLO SI COINCIDE EN UNA JUGADA UNA DIAGONAL 
JUNTO A UNA HORIZONTAL Y TAMBIÉN ELIMINAMOS LA
POSICIÓN INICIAL REPETIDA DESDE LA QUE SE REVISA
*/
String eliminar_repetidos(String posiciones){
  for(byte i = 0; i < posiciones.length(); i+=2){
    for(byte j = i+2; j < posiciones.length(); j+=2){
      String busqueda  = String(posiciones[i]) + String(posiciones[i+1]);
      String siguiente = String(posiciones[j]) + String(posiciones[j+1]);
      if( busqueda.equals(siguiente) ){
        String str1 = posiciones.substring(0,j);
        String str2 = posiciones.substring(j+2,posiciones.length());
        posiciones = str1 + str2;
      }
    }
  }
  return posiciones;
}

String comprobar_recto(int fila, int columna){
  String 
  posiciones = "",
  finales = "";
  
  byte 
  indice = 0,
  desde_fila    = (fila-3 < 0)    ? 0 : fila-3,
  desde_columna = (columna-3 < 0) ? 0 : columna-3,
  hasta_fila    = (fila+3 >= SIZE(tablero))       ? SIZE(tablero) - 1    : fila+3, 
  hasta_columna = (columna+3 >= SIZE(tablero[0])) ? SIZE(tablero[0]) - 1 : columna+3;  

  for(byte veces = 0; veces < 2; veces++){
    bool es_vertical = (veces == 0) ? true : false;
    
    //Recorremos el bucle sólo si hay posiciones disponibles para 4 fichas
    if((es_vertical && hasta_fila - desde_fila >= 3) || ( !es_vertical && hasta_columna - desde_columna >= 3)){
      for(byte opcion = ((es_vertical) ? fila : desde_columna) ; opcion <= ((es_vertical) ? hasta_fila : hasta_columna); opcion++){
        if(tablero[((es_vertical) ? opcion : fila)][((es_vertical) ? columna : opcion)] == valorFicha[turno_jugador] ){
          posiciones +=  String( ((es_vertical) ? opcion : fila) ) + String( ((es_vertical) ? columna : opcion) );
          indice++;
          if(indice >= 4){
            finales += posiciones;
          }
        }
        else{
          indice = 0;
        }
      }
    }
    indice = 0;
    posiciones = "";
  }  
  return finales; 
}

String comprobar_diagonal(int fila, int columna, String opcion){
  String posiciones = "";
  while(fila >= 0 && fila < FILAS && columna >= 0 && columna < COLUMNAS){
    if(tablero[fila][columna] == valorFicha[turno_jugador]){
      posiciones +=  String(fila) + String(columna);
    }else{
      columna = -1; //salir
    }
    if(opcion == "inferior_izq")  {fila++; columna--;}
    if(opcion == "superior_drcha"){fila--; columna++;}
    if(opcion == "superior_izq")  {fila--; columna--;}
    if(opcion == "inferior_drcha"){fila++; columna++;}
  }
  return posiciones;    
}

/********************************************/
/* MOSTRAR ARRAY BIDIMENCIONAL CON LOS DATOS*/
void mostrar_debug(){
  Serial.println("---------------");
  for(byte fila = 0; fila < SIZE(tablero); fila++){
    Serial.print("|");
    for(byte columna = 0; columna < SIZE(tablero[0]); columna++){
      Serial.print(String(tablero[fila][columna]) + "|");
    }
    Serial.println();
  }
  Serial.println("---------------");
}

/********************************************/
/* FONDOS DE LA PANTALLA */
void dibujar_fondo(){
  TFTscreen.setTextSize(2);
  TFTscreen.stroke(255,255,255);
  TFTscreen.fill(0,80,80);
  TFTscreen.rect(0,0,TFTscreen.width(),TFTscreen.height());
}

void dibujar_presentacion(){
  dibujar_fondo();
  TFTscreen.text("Conecta 4.", 22, 50);
  delay(1000);
  dibujar_tablero();
}

void dibujar_tablero(){
  dibujar_fondo();
  TFTscreen.fill(20,190,190);
  for(byte fila = 0; fila < SIZE(tablero); fila++){
    for(byte columna = 0; columna < SIZE(tablero[0]); columna++){
      TFTscreen.circle(columna * 20 + 20, fila * 20 + 14, 8);
    }
  }
}

void dibujar_reset(){
  digitalWrite(led_Rojo, false);
  digitalWrite(led_Amarillo, false);
  dibujar_fondo();
  TFTscreen.text("Reiniciando.", 10, 50);
  delay(1000);
}

void dibujar_empate(){
  turno_jugador = (rotar_turno_finpartida) ? !turno_jugador : turno_jugador; 
  dibujar_fondo();
  TFTscreen.text("Empate.", 38, 50);
  delay(2000);
  reiniciar();
}

void dibujar_ganador(){
  delay(2000);
  dibujar_fondo();
  TFTscreen.setTextSize(2);
  String jugador = "Rojo";
  byte margin = 26;
  
  if(turno_jugador){
    jugador = "Amarillo"; 
    margin = 3;
  }
  turno_jugador = (rotar_turno_finpartida) ? !turno_jugador : turno_jugador; 
    
  String str = "Gana "+ jugador;
  char texto[20];
  str.toCharArray(texto,20);
  TFTscreen.text(texto, margin, 50);
  delay(1000);

  reiniciar();
}

void dibujar_jugada(String posiciones){
  /*Al ganar hacemos parpadear las fichas que
  forman el conecta cuatro (o más) entre el color
  del jugador y el color verde*/
  TFTscreen.stroke(255,255,255);
  for(byte i = 0; i < 6; i++){ //veces
    if(i % 2 == 0){
      TFTscreen.fill(0,200,0);
      digitalWrite((!turno_jugador) ? led_Rojo : led_Amarillo , true);
    }
    else{
      TFTscreen.fill(200, (!turno_jugador) ? 0 : 200 ,0);
      digitalWrite((!turno_jugador) ? led_Rojo : led_Amarillo , false);
    }
    for(byte j = 0; j < posiciones.length()-1; j+=2){ //posiciones
      TFTscreen.circle(String(posiciones[j+1]).toInt() * 20 + 20, String(posiciones[j]).toInt() * 20 + 14, 8);
    }
    delay(100);
  }
}
