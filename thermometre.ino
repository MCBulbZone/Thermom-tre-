/***************************************************************************************
Code exemple pour le thermomètre imprimable.
Ce code permet d'allumer avec des intervalles définis (permet d'economiser la batterie)
les leds afin d'indiquer la température ambiante avec une précision de 5 degrés.

cette précision est due au nombre de LED sur la face avant, mais rien ne vous empeche
de creer des effets lumineux afin d'augmenter la précision.

Vous pouvez définir une température de seuil minimum et maximum grace aux variables 
"tempmin" et "temp Max", cela activera un effet lumineux vous avertissant que le seuil est dépassé.

une fonction bonus permet de faire un chenillard avec les leds afin de vous prévenir 
qu'il est temps de vous lever de votre chaise pour prendre un café, marcher un peu ou 
tout simplement éviter les courbatures dans le dos.

Les leds sont connectées à la PIN 3 du pro Trinket et le capteur à la PIN 4.

code écrit par Michael Captant 
Bulb Zone - 2015
***************************************************************************************/

//importer les librairies pour NeoPixel, OneWire et capteur de température
#include <Adafruit_NeoPixel.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// définir la PIN pour les données du capteur de température
#define ONE_WIRE_BUS 4
// mettre en place le protocole oneWire
OneWire oneWire(ONE_WIRE_BUS);
// passer sur le mode capteur DallasTemperature
DallasTemperature sensors(&oneWire);

// PIN de connexion des NEOPIXELS
#define PIN            3
// nombre de NEOPIXELS connectés
#define NUMPIXELS      9
// Définir les paramètres des NEOPIXEL
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//Définir les variables pour la température
float tempC;//variable de température avec virgule
int tempInt;//variable de température sans virgule
int tempDiv;//variable nombre de led à allumer
boolean tempOK;// variable confirmation de non-depassement de seuil

int tempMin = 0;//variable de température minimum
int tempMax = 40;//variable de température maximum

//créer une variable pour inverser la numérotation des NEOPIXELS
int ledMap;

//créer des variable afin de définir rapidement les couleurs de base
uint32_t color;
uint32_t red =pixels.Color( 255,0,0);
uint32_t blue =pixels.Color( 0,0,255);
uint32_t green =pixels.Color( 0,255,0);

//stocker la valeur en milli pour le temps entre chaque affichage de la température
long previousMillis = 0;        // stock la derniere valeur milli pour la température
long previousMillis2 = 0;        // stock la dernière valeur milli pour la fonction bonus
long interval = 120000;           // temps entre chaque coupure et la durée totale d'allumage
long interval2 =1800000;           // bonus "STAND UP"

void setup() {
//initialiser les NEOPIXELS
pixels.begin();
//initialiser le capteur de température
sensors.begin();
}

void loop() {
  // créer un delai pour économiser la batterie:
  unsigned long currentMillis = millis();

  //créer la condition si la différence entre la valeur actuelle et
  //la valeur stockée du temps et superieur à l'intervalle de temps:
  if(currentMillis - previousMillis > interval) {
    //on stock la nouvelle valeur de temps
    previousMillis = currentMillis;   
    //lancer le sous programme getTemp
    getTemp();
    //afficher l'information pendant 3 secondes
    delay(3000);
    //pour les NEOPIXELS de 0 à 8(0 correspond a la premiere led)
        for(int z = 0;z <= 8;z++){
          //on éteint les leds avec un delai de 100ms entre chacunes
          pixels.setPixelColor(z,0,0,0);
          pixels.show();
          delay(100);
        }
    }
  //créer la condition si la différence entre la valeur actuelle et
  //la valeur stockée du temps et supérieur à l'intervalle de temps:
  if(currentMillis - previousMillis2 > interval2) {
    //on stock la nouvelle valeur de temps
    previousMillis2 = currentMillis;
    //on répète dix fois la fonction:   
    for(int x=0 ;x<=10;x++){
          //pour les leds de 8 a 0:
          for(int z = 8;z >= 0;z--){
          //on allume les leds en bleu
          pixels.setPixelColor(z,blue);
          //on éteint les leds progressivement
          pixels.setPixelColor(z+1,0,0,0);
          pixels.show();
          delay(50);
          }
          //puis on éteint la dernière led
          pixels.setPixelColor(0,0,0,0);
          pixels.show();
        }
    }
}

//Cette fonction permet d'obtenir la température et de traiter l'information
//afin de l'afficher sous forme de graduations et de codes couleurs
void getTemp(){
  sensors.requestTemperatures(); // envoi la commande pour obtenir la température
  tempC = sensors.getTempCByIndex(0); //stock la température dans la variable tempC

  if (tempC >= tempMin && tempC <= tempMax){// vérifie si la température est bien comprise entre tempMin et tempMax
    tempOK = true;  //si oui : le drapeau tempOK est mis à 1
  }
  else{  //sinon le drapeau tempOK est mis à zéro
    tempOK = false;
  }
  if (tempC < 0){ //vérifie si la température est négatif
  color = blue;
  tempC = tempC * -1.0; //si oui mettre la couleur à bleu et transformer la valeur en positive
  }
  else{
  color = red; // sinon la couleur est rouge
  }
  if(!tempOK){ // si la température dépasse les températures Min et Max:
    //on répète la fonction suivante 3 fois:
    for(int cpt = 0; cpt <= 3; cpt++){
      for(int x = 0;x <=8; x++){  //on allume toutes les leds en vert
        pixels.setPixelColor(x,green);
        pixels.show();
      }
      delay(500);
      for(int x = 8;x >=0; x--){  //on éteint toutes les leds
        pixels.setPixelColor(x,0,0,0);
        pixels.show();
      }
      delay(500);// on ajoute un intervalle de 500ms
    }
  }
 
 tempInt = (int)tempC;// obtiens la valeur integrale de la température
 tempDiv = tempInt /5; //tempDiv contient le nombre de led à allumer
 ledMap = map(tempDiv,0,8,8,0);//permet d'inverser la numérotation des LEDS

 //Si nous sommes en dessous des seuils de température:
 if(tempOK){
  for(int y = ledMap;y >= 0;y--){// éteindre toutes les leds progressivement
    pixels.setPixelColor(y,0,0,0);
    pixels.show();
    delay(100);
  }
  delay(100);//marquer une pause
  for(int x =8;x>=ledMap;x--){ //allumer les leds jusqu'à la température actuelle
    pixels.setPixelColor(x,color);
    pixels.show();
    delay(100);
  }
 }
}



