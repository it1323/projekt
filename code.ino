
#include <Password.h>
#include <Keypad.h>
#include <EEPROM.h>



Password password = Password( "1111" ); 
char noveHeslo[5];
String heslo = "";
String pomocna;
char pasvord[5];
int prihlasen;
int klik=0;//pro měná pro počítání kliknutí
int led=13;//led na pinu 13


unsigned long currentMillis;
unsigned long previousMillis = 0;   

const long interval = 3000;


const byte ROWS = 4; // Four rows
const byte COLS = 4; // columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1', '2', '3', '4'}
};
int pocet=0;
int pocetNaNew=0;
int zmena=0;

byte rowPins[ROWS] = { 2 };
byte colPins[COLS] = { 3, 4, 5, 6 };

// Create the Keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup(){
  Serial.begin(9600);
  Serial.println("test klavesnice");
  for(int i=0; i<4; i++){
    heslo+= String(EEPROM.read(i));
    
    }

    Serial.println(heslo);
  
  keypad.addEventListener(keypadEvent); //add an event listener for this keypad
  heslo.toCharArray(pasvord, 5);
  password.set(pasvord);
  pocet=0;
  prihlasen=0;
  klik=0;
  pinMode(led ,OUTPUT);
  }
void(*resetFunc)(void)=0;

void loop(){
  
 keypad.getKey();

  if(klik==1){
    prihlasen=1;
    Serial.println("zamknuto");
    digitalWrite(led, HIGH);
    klik=0;
    
    }
  }
  
  
void keypadEvent(KeypadEvent eKey){
  switch (keypad.getState()){
  case PRESSED:
  
  //Serial.print("Enter:");
 // Serial.println(eKey);
 // delay(10);
  
      if(prihlasen==1){
        if(pocet==3){
            password.append(eKey);
            checkPassword();
            password.reset();
            pocet=0;
          }
          else {
           switch (eKey){              
               default: Serial.print("Enter:");Serial.println(eKey);password.append(eKey); pocet++; delay(1);
                }
              
          }
      }else{
                     currentMillis = millis();
                    if(currentMillis - previousMillis < interval){
                      klik++;
                      previousMillis = currentMillis;
                      Serial.println("klikni");
                      Serial.print("Enterlock:");
                      Serial.println(eKey);
                      }else {klik=0; previousMillis = currentMillis;}
        }
     
      }
    
}


void checkPassword(){//kontrola hesla
  
if (password.evaluate()){  //if password is right open
   Serial.println("deaktivováno");
   prihlasen=0;
   
   digitalWrite(led,LOW);
   
}else {
    Serial.println("špatné heslo");
    
      digitalWrite(led,LOW);
      delay(400);
      digitalWrite(led,HIGH);
      delay(400);
      digitalWrite(led,LOW);
      delay(400);
      digitalWrite(led,HIGH);
      
  }

}



/*void uloz(char * newh){
  for(int i=0; i<4; i++){
    EEPROM.write(i, newh[i] - '0');
    Serial.print(newh[i]);
    }
    resetFunc();
  }*/
