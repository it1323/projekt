#include <Password.h>
#include <Keypad.h>
#include <EEPROM.h>
#define GREEN 10
#define BLUE 9
#define RED 11

Password password = Password("1111");
char noveHeslo[5];
String heslo = "";
String pomocna;
char pasvord[5];
int prihlasen;
int moznost = 0; //proměna užitá jako paramert pro změnu hesla
int normal = 1; //
int klik = 0; //proměná pro počítání kliknutí
int led = 13; //led na pinu 13
int getImpuls = 0; //uchovává jestli byl otřes
int led2 = 12;
int ledState = LOW;

unsigned long currentMillis;
unsigned long previousMillis = 0;
unsigned long previousMil = 0;
unsigned long pre = 0;
const long interval = 1500;


const byte ROWS = 4; // Four rows
const byte COLS = 4; // columns
// Define the Keymap
char keys[ROWS][COLS] = {
    {'1', '2', '3', '4'}
};
int pocet = 0;
int pocetNaNew = 0;
int zmena = 0;
int sensor;

byte rowPins[ROWS] = {2};
byte colPins[COLS] = {3, 4, 5, 6};

// Create the Keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
    Serial.begin(9600);
    Serial.println("test klavesnice");
    for (int i = 0; i < 4; i++) {
        heslo += String(EEPROM.read(i));
    }

    Serial.println(heslo);
    keypad.addEventListener(keypadEvent); //add an event listener for this keypad
    heslo.toCharArray(pasvord, 5);
    password.set(pasvord);
    pocet = 0;
    prihlasen = 0;
    klik = 0;
    pinMode(led, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    pinMode(RED, OUTPUT);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, HIGH);
    digitalWrite(RED, HIGH);
}
void(*resetFunc)(void) = 0;

void loop() {
    sensor = analogRead(A0);
    keypad.getKey();

    if (klik == 1) {
        prihlasen = 1;
        Serial.println("zamknuto");
        digitalWrite(led, HIGH);
        delay(3000);
        klik = 0;
    }

    if (prihlasen == 1) {
        if (sensor > 1002) {
            //digitalWrite(8, HIGH);
            delay(200);
            getImpuls == 1;

        } else digitalWrite(8, LOW);
    }
    blika();
}

void keypadEvent(KeypadEvent eKey) {
    switch (keypad.getState()) {
        case PRESSED:

            //Serial.print("Enter:");
            // Serial.println(eKey);
            // delay(10);
            if (normal == 1) {
                if (prihlasen == 1) {
                    if (pocet == 3) {
                        password.append(eKey);
                        checkPassword();
                        password.reset();
                        pocet = 0;
                    } else {
                        switch (eKey) {


                            default: Serial.print("Enter:");
                                Serial.println(eKey);
                                password.append(eKey);
                                pocet++;
                                delay(100);
                        }
                    }
                } else {
                    if (eKey == '3') {
                        currentMillis = millis();
                        if (currentMillis - previousMillis < interval) {
                            klik++;
                            previousMillis = currentMillis;
                            Serial.println("klikni");
                            Serial.print("Enterlock:");
                            Serial.println(eKey);
                        } else {
                            klik = 0;
                            previousMillis = currentMillis;
                        }
                    }
                }
            } else {
                if (pocet == 3) {
                    noveHeslo[pocet] = eKey;
                    uloz(noveHeslo);
                    pocet = 0;
                } else {
                    switch (eKey) {

                        default: Serial.print("Enter:");
                            Serial.println(eKey);
                            noveHeslo[pocet] = eKey;
                            pocet++;
                            delay(100);
                    }
                }
            }
        case HOLD: if (moznost == 1 && eKey == '1') {

                currentMillis = millis();
                normal = 0;
                moznost = 0;
                Serial.println(eKey);
                digitalWrite(led2, HIGH);
                pocet = 0;
            }
    }
}

void checkPassword() {//kontrola hesla

    if (password.evaluate()) { //if password is right open
        Serial.println("deaktivováno");
        prihlasen = 0;
        moznost = 1;
        digitalWrite(led, LOW);

    } else {
        Serial.println("špatné heslo");
        moznost = 0;
        digitalWrite(led, LOW);
        delay(400);
        digitalWrite(led, HIGH);
        delay(400);
        digitalWrite(led, LOW);
        delay(400);
        digitalWrite(led, HIGH);

    }

}

void blika() {//pokud byl impuls bliká 10s pak prestane jestli nebude otřes
    unsigned long Millis = millis();

    if (getImpuls) {
        if (Millis - previousMil <= 10000) {
            pre = Millis;
            if (Millis - previousMil >= 500) {
                // save the last time you blinked the LED
                previousMil = Millis;

                // if the LED is off turn it on and vice-versa:
                if (ledState == LOW) {
                    ledState = HIGH;
                }
                else {
                    ledState = LOW;
                }

                // set the LED with the ledState of the variable:
                digitalWrite(8, ledState);
            }
        } else getImpuls = 0;

    }

}

void uloz(char *newh) {

    for (int i = 0; i < 4; i++) {
        int mezi = newh[i] - '0';
        EEPROM.write(i, mezi);
        Serial.print(newh[i]);
    }
    digitalWrite(led2, LOW);
    resetFunc();
}
