#include <Wire.h>
#include "MAX30105.h"
#include <LoRa.h>
#include <SPI.h>
#include "heartRate.h"
#include "mickey.h"
//// Pin LoRa
#define ss 5
#define rst 14
#define dio0 2

#define LENGTH_CHAR 7
#define LED_BUILTIN 9
#define INTERVAL_RF 200 // Interval for sending data
unsigned long oldtime=0; // timer memory
const int LENGTH_DATA=35; // jumlah character
// max30102 i2c
MAX30105 particleSensor;

const byte RATE_SIZE = 5; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
const int RESET_BEAT_TIME = 5000 ; // time to 0 beat variable when no finger detected
unsigned long oldtime_beat_time=0; // timer memory

//// mickey conversion

uint8_t key[] = {0x01, 0x02, 0x03, 0x04, 0x05};
uint8_t iv[] = {0x06, 0x07, 0x08, 0x09, 0x0A};

// uint8_t key[] = {1, 2, 3, 4, 5,6,7};
// uint8_t iv[] = {8, 9, 10,11,12,13,14};
// Mickey mickey(key, sizeof(key), iv, sizeof(iv));
// Mickey mickey2(key, sizeof(key), iv, sizeof(iv));

void mickey_encrypt(uint8_t *key,uint8_t *iv,uint8_t *data, uint8_t dataLen){
  
  Mickey mickey(key, sizeof(key)+1, iv, sizeof(iv)+1);
  // Serial.println(sizeof(key));
  for (int i = 0; i < dataLen; i++) {
    uint8_t keystream = mickey.generateKeystreamByte();
    data[i] = data[i] ^ keystream;
    // Serial.print("Keystream[");
    // Serial.print(i);
    // Serial.print("] = ");
    // Serial.println(keystream);
    // Serial.print("data[");
    // Serial.print(i);
    // Serial.print("] = ");
    // Serial.println(data[i]);
  }
}

void mickey_decrypt(uint8_t *key,uint8_t *iv,uint8_t *data, uint8_t dataLen){
  mickey_encrypt(key,iv,data,dataLen);
}
void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Initializing...");
  LoRa.setPins(ss, rst, dio0);    //setup LoRa transceiver module
  while (!LoRa.begin(433E6))     //433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  LoRa.setTxPower(18);
  Serial.println("LoRa Initializing OK!");
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void loop()
{
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  char radiopacket[LENGTH_DATA];  // Reserve space data
  if (irValue < 50000){
    //  Serial.print(" No finger?");
    // sprintf(radiopacket, "{message:\"No Finger\"}");
    if(millis()-oldtime_beat_time>=RESET_BEAT_TIME){
      beatsPerMinute=0;
      beatAvg=0;
      for (byte i = 0; i < RATE_SIZE; i++) {
        rates[i] = 0;
      }
      sprintf(radiopacket, "{message:\"Reset Beat\"}");
      oldtime_beat_time=millis();
    }
    
  }
  else{
    // Serial.print("IR:");
    // Serial.print(irValue/1000);
    // Serial.print(", BPM:");
    // Serial.print(beatsPerMinute);
    // Serial.print(", AvgBPM:");
    // Serial.println(beatAvg);
    sprintf(radiopacket, "{\'IR\':%d,\'BPM\':%.1f,\'avg\':%d}", irValue,beatsPerMinute,beatAvg);
    // sprintf(radiopacket, "{message:\"Hello World!\"}");
 
    // Serial.println();
    // sending data as INTERVAL_RF loop
    
    

    // menghitung panjang string
    int len = strlen(radiopacket);
    // int messageLength  = LENGTH_DATA - len;
    // menghapus karakter NULL (\0) pada akhir string
    memset(&radiopacket[len], 0, LENGTH_DATA - len);
    
    
    if(millis()-oldtime >= INTERVAL_RF)
    { 
        // LoRa.beginPacket();   //Send LoRa packet to receiver
        // // LoRa.write((uint8_t *)radiopacket, len);
        // LoRa.write(data_encrypt, messageLength + 1);
        // LoRa.endPacket();
        // Serial.println(radiopacket);
        Serial.print("Original Hex: ");
        for (int i = 0; i < len+1; i++) {
          Serial.print(radiopacket[i], HEX);
        }
        Serial.println();
        uint8_t* data_encrypt = new uint8_t[len];
        
        memcpy(data_encrypt, radiopacket, len);
        // data_encrypt[len] = '\0';
        Serial.print("Encrypted Hex: ");
        mickey_encrypt(key,iv, data_encrypt, len);
        // Serial.println((char*)radiopacket);
        for (int i = 0; i < len; i++) {
          Serial.print(data_encrypt[i], HEX);
        }
        Serial.println();
        LoRa.beginPacket();   //Send LoRa packet to receiver
        // LoRa.write((uint8_t *)radiopacket, len);
        LoRa.write(data_encrypt, len);
        LoRa.endPacket();
        // Serial.println(radiopacket);        
        // Serial.print("Decrypted Hex: ");
        // mickey_decrypt(key,iv,data_encrypt, len);
        // for (int i = 0; i < len; i++) {
        //   Serial.print(data_encrypt[i], HEX);
        // }
        // Serial.println();
        // Serial.write(data_encrypt, len);
        // Serial.println();
        // char convertedChar[len + 1];
        // for (size_t i = 0; i < len + 1; i++) {
        //   convertedChar[i] = (char)data_encrypt[i];
        // }
        // Serial.println(convertedChar);
        oldtime=millis();
    }

  }
  

  
}