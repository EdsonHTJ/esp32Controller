#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Arduino.h>
#include <WiFi.h>

Adafruit_MPU6050 mpu;

float prevY;
float prevZ;
unsigned long int prevTime;

const char* ssid = "D&D";
const char* password =  "29091999";
 
WiFiServer wifiServer(80);
 
void clientLoop(WiFiClient* p_client);
int getSignal(int a);
void setup(void) {

  Serial.begin(115200);
  pinMode(2,OUTPUT);
  delay(1000);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting..");
  }
 
  Serial.print("Connected to WiFi. IP:");
  Serial.println(WiFi.localIP());
 
  wifiServer.begin();
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  //setupt motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);

  Serial.println("");
  delay(100);
}

void loop() {

    WiFiClient client = wifiServer.available();
    /* Get new sensor events with the readings */
    if(client) {
      while (client.connected())
      {
        clientLoop(&client);
      }
      
    }
}

void clientLoop(WiFiClient* p_client){
    WiFiClient c = *p_client;
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);



    if(abs(prevY - a.acceleration.y) > 9){
      Serial.println("Attack");
      c.println("ATK 1");
      
      prevY = a.acceleration.y;
      delay(100);
      return;
    }
    int actual = getSignal(a.acceleration.z);
    if (actual == 0) {
      return;
    }
    if(prevZ != actual){
      Serial.printf("MOV %d\n", actual);
      c.printf("MOV %d\n", actual);
      prevZ = actual;
      delay(100);
    }

    prevTime = millis();


    delay(10);
}

int getSignal(int a){
  if (a > 8) {
    return 1;
  }else if( a < -8) {
    return -1;
  }else {
    return 0;
  }
}