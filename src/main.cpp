#include <Arduino.h>
#include <WiFi.h>

#define LED_BLUE 2
#define BUTTON_UP 33
#define BUTTON_DOWN 32
#define POTENTIOMETERX 34
#define POTENTIOMETERY 35

#define P2_ANALOG 1

bool led_val = 0;
short prevMovP1 = 0;
short prevMovP2 = 0;
int prevAnalog = 0;

#define ssid "D&D"
#define password "29091999"
 
WiFiServer wifiServer(80);

typedef struct userState {
    int X;
    int Y;
    int b1;
    int b2;
} userState_t;


bool compareUserState(userState_t u1, userState_t u2);
void sendUserState(userState_t u, WiFiClient client);

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
}

void loop() {

    userState_t actualUserState;

    actualUserState.b1 = !digitalRead(BUTTON_UP);
    actualUserState.b2 = !digitalRead(BUTTON_DOWN);

    int pxRead = analogRead(POTENTIOMETERX);
    actualUserState.X = map(pxRead, 0, 4095, 0, 100);

    int pyRead = analogRead(POTENTIOMETERY);
    actualUserState.Y = map(pyRead, 0, 4095, 0, 100);

    bool res = compareUserState(actualUserState, prevUserState);
    
    WiFiClient client = wifiServer.available();
    if (!res) {
        sendUserState(actualUserState, client);
        prevUserState = actualUserState;
    }
    

    if(client && client.connected() && client.available()>0) {

    }

    delay(50);
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
    if (absYdif > 5) {
        return false;
    }

    return true;
}


