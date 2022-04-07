#include <Arduino.h>
#include <WiFi.h>

#define LED_BLUE 2
#define BUTTON_UP 33
#define BUTTON_DOWN 25
#define POTENTIOMETERX 34
#define POTENTIOMETERY 35

#define P2_ANALOG 1

bool led_val = 0;
short prevMovP1 = 0;
short prevMovP2 = 0;
int prevAnalog = 0;
int maxYval = 4095;
int maxXval = 4095;

#define ssid "Eds"
#define password "aaaabbbb"
 
WiFiServer wifiServer(80);

typedef struct userState {
    int X;
    int Y;
    int b1;
    int b2;
} userState_t;


bool compareUserState(userState_t u1, userState_t u2);
void sendUserState(userState_t u, WiFiClient client);
void readUs(userState_t* u);
void caliBrate();

userState_t prevUserState;

void setup() {
    Serial.begin(115200);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(BUTTON_UP, INPUT_PULLUP);
    pinMode(BUTTON_DOWN, INPUT_PULLUP);

    WiFi.begin(ssid, password);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting..");
    }
    
    Serial.print("Connected to WiFi. IP:");
    Serial.println(WiFi.localIP());
    
    wifiServer.begin();
    caliBrate();
    Serial.println("Calibrate");
}

void loop() {

    userState_t actualUserState;
    WiFiClient client = wifiServer.available();

    if (client) {
 
        while (client.connected()) { // O que vai fazer enquanto está conectado, porém não recebeu mensagem
      //aqui

            readUs(&actualUserState);
            bool res = compareUserState(actualUserState, prevUserState);
            
            if (!res) {
                sendUserState(actualUserState, client);
                prevUserState = actualUserState;
            }
            

            delay(10);
        }

        client.stop();

    }
}

void caliBrate() {
    userState_t u;
    for(;;) {
        readUs(&u);
        if(u.X > 50) {
            maxXval++;
        } 
        else if(u.X < 50) {
            maxXval--;
        }
        else if(u.Y > 50) {
            maxYval++;
        } 
        else if(u.Y < 50) {
            maxYval--;
        }
        else {
            break;
        }

    }
}

void readUs(userState_t* u) {
    u->b1 = !digitalRead(BUTTON_UP);
    u->b2 = !digitalRead(BUTTON_DOWN);

    int pxRead = analogRead(POTENTIOMETERX);
    u->X = map(pxRead, 0, maxXval, 0, 100);
    if (u->X > 100) {
        u->X = 100;
    }

    int pyRead = analogRead(POTENTIOMETERY);
    u->Y = map(pyRead, 0, maxYval, 0, 100);
    if (u->Y > 100) {
        u->Y = 100;
    }
}

void sendUserState(userState_t u, WiFiClient client) {
    char toSend[100];
    memset(toSend, 0, sizeof(toSend));
    sprintf(toSend, "{ \"b1\":%i, \"b2\": %i, \"X\": %i, \"Y\": %i }", u.b1, u.b2, u.X, u.Y);
    Serial.println(toSend);
    
    if (!client.connected()) {
        return;
    }
    Serial.println("Send to client");
    client.println(toSend);
}

bool compareUserState(userState_t u1, userState_t u2) {
    if(u1.b1 != u2.b1) {
        return false;
    }

    if(u1.b2 != u2.b2) {
        return false;
    }

    int absXdif = abs(u1.X - u2.X);
    if (absXdif > 5) {
        return false;
    }

    int absYdif = abs(u1.Y - u2.Y);
    if (absYdif > 2) {
        return false;
    }

    return true;
}


