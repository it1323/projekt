#include <Password.h>
#include <Keypad.h>
#include <EEPROM.h>
#define buzerPin 8
//#define DEBUG

Password password = Password( "1111" ); 
char noveHeslo[5];
String heslo = "";
String pomocna;
char pasvord[5];
int prihlasen;
int moznost=0;//proměna užitá jako paramert pro změnu hesla
int normal=1; 
int klik=0;//proměná pro počítání kliknutí
int led=13;//led na pinu 13
int getImpuls=0; //uchovává jestli byl otřes
int led2=12;
int ledState=LOW;
int kolikrat=6; //počítá počet probliknutí

unsigned long currentMillis;
unsigned long previousMillis = 0;   
unsigned long previousMil = 0; 
unsigned long previous = 0; 
unsigned long pre = 0; 
const long interval = 1500;

const byte ROWS = 4; // Four rows
const byte COLS = 4; // columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1', '2', '3', '4'}
};
int pocet=0;
int pocetNaNew=0;
int zmena=0;
int sensor;

byte rowPins[ROWS] = { 2 };
byte colPins[COLS] = { 3, 4, 5, 6 };
// Create the Keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup(){
  #ifdef DEBUG
  Serial.begin(9600);
  Serial.println("test klavesnice");
  #endif
  //čtení hesla z EEPROM
  for(int i=0; i<4; i++){
    heslo+= String(EEPROM.read(i));    
  }
  #ifdef DEBUG
  Serial.println(heslo);
  #endif  
  keypad.addEventListener(keypadEvent); //add an event listener for this keypad
  heslo.toCharArray(pasvord, 5);
  password.set(pasvord);
  pocet=0;
  prihlasen=0;
  klik=0;
  pinMode(led ,OUTPUT);
  pinMode(led2, OUTPUT);
}
  
void(*resetFunc)(void)=0;
void loop(){
  sensor = analogRead(A0);
  keypad.getKey();
  looped();   
}  
  
void keypadEvent(KeypadEvent eKey){
  switch (keypad.getState()){
  case PRESSED: pres(eKey);  //funkce je vyvolaná, když je zmáčknuté tlačítko
  case HOLD: holded(eKey);   //funkce se vyvolá, když se přidrží tlačítko
  }
}

void checkPassword(){//kontrola hesla
  if (password.evaluate()){  //if password is right open
    #ifdef DEBUG
    Serial.println("deaktivováno");
    #endif
    prihlasen=0;
    moznost=1;
    digitalWrite(led,LOW); 
  }
  else {
    #ifdef DEBUG
    Serial.println("špatné heslo");
    #endif
    moznost=0;
    kolikrat=0;//spouští blikání led diody  
  }
}

void blika(){ //pokud byl impuls bliká 10s pak prestane jestli nebude otřes
  if(getImpuls){  //kontroluje, zda byl zaznamenán otřes
    unsigned long Millis = millis();
    //kontrola doby blikání -> když bliká déle než 10s tak tak přestane pokud nebyl zaznamenán otřes
    if (Millis - previousMil >= 10000) {
      previousMil= Millis;
      getImpuls=0;
    }
    if (Millis - previous >= 500) {
      // save the last time you blinked the LED
      previous = Millis;
      // pokud je LEDka vypnutá, tak se zapne, a naopak
      if (ledState == LOW) {
        ledState = HIGH;
       tone(buzerPin, 2200, 600);  
      } 
      else {
        noTone(buzerPin);
        ledState = LOW;      }
      // set the LED with the ledState of the variable:
      digitalWrite(13, ledState);
    }
  }
 }
//funkce je vyvolaná když se ukládá nové heslo
void uloz(char *newh){
  //ukládá heslo do EEPROM
  for(int i=0; i<4; i++){
    int mezi=newh[i]-'0';
    EEPROM.write(i, mezi);    
    #ifdef DEBUG
    Serial.print(newh[i]);
    #endif    
  }
  digitalWrite(led2,LOW);
  resetFunc();
  }
//funkce je vyvolaná, když je zmáčknuté tlačítko
void pres(char eKey){
  //kontroluje zda se má měnit heslo nebo ne
  //když je proměnná nastavená na 0, tak se vyvolá funkce ke změně hesla
  if(normal==1){
    //kontrola, zda je alarm zapnutý
    if(prihlasen==1){
      //kontroluje se, zda už bylo zadané celé heslo
      if(pocet==3){
        password.append(eKey);
        //kontrola hesla
        checkPassword();
        password.reset();
        pocet=0;
      }
      else {
        #ifdef DEBUG
        Serial.print("Enter:");
        Serial.println(eKey);
        #endif
        password.append(eKey); 
        pocet++;  
      }
    }
    else{
      //když alarm není zapnutý, tak se kontroluje, zda bylo stisknuto tlačítko č. 3 2x
      //když ano, tak se alarm zapne
      if(eKey=='3'){
        currentMillis = millis();
        if(currentMillis - previousMillis < interval){
          klik++;
          previousMillis = currentMillis;          
         #ifdef DEBUG
         Serial.println("klikni");
         Serial.print("Enterlock:");
         Serial.println(eKey);
         #endif
       }
       else {
        klik=0; 
        previousMillis = currentMillis;
       }
     }
   }
  }
  else{
    //zadávání nového hesla
    if(pocet==3){
      noveHeslo[pocet]=eKey; 
      //vyvolání funkce k uložení hesla do EEPROM
      uloz(noveHeslo);
      pocet=0;
    }
    else {
      #ifdef DEBUG
      Serial.print("Enter:");
      Serial.println(eKey); 
      #endif        
      noveHeslo[pocet]=eKey; 
      pocet++; 
      delay(100);         
    }
  }
}
//funkce se vyvolá, když se přidrží tlačítko  
void holded(char eKey){
  //kontrola, zda se může měnit heslo (jestli je proměnná možnost nastavená na 1), a jestli bylo stisknuté tlačítko č. 1, když ne, tak se funkce přeskočí
  //proměnná možnost tady je pro ochranu před tím, aby heslo nemohl změnit kdokoliv, ale jen ten kdo zná heslo
  //tzn. že prom. moznost se nastaví na 1, jen když je alarm odemknut správným heslem
  if(moznost==1 && eKey=='1'){
    currentMillis = millis();
    normal=0;
    moznost=0;    
    #ifdef DEBUG
    Serial.println(eKey);
    #endif    
    digitalWrite(led2,HIGH);
    pocet=0;
  }
}

void looped(){
  //když se proměnná klik rovná 1, tak tzn. že bylo 2x zmáčknuté 3. tlačítko, a že se má alarm zapnout
  if(klik==1){
    prihlasen=1;
    moznost=0;    
    #ifdef DEBUG
    Serial.println("zamknuto");
    #endif    
    digitalWrite(led, HIGH);
    delay(4000);
    klik=0;
  }
  if(prihlasen==1){
    #ifdef DEBUG
    Serial.println(sensor);
    #endif
    //kontrola, zda bylo s alarmem pohnuto, pokud ano, tak je proměná getimpuls nastavena na 1
    if(sensor>300){
      getImpuls=1;
    }
    //vyvolání funkce blika
    blika();
  }
  if(kolikrat < 6){
    unsigned long Millis = millis();
    if (Millis - prev >= 500) {
      prev = Millis;
      if (ledState == LOW) {
        ledState = HIGH;
      } 
      else {
        ledState = LOW;      }
        kolikrat++;
        digitalWrite(13, ledState);
    }
  }
}
