#include <LoRa.h>
#include <SPI.h>
 
#define ss 5
#define rst 14
#define dio0 2
#define L0_NEGATIF 21
#define L0_POSITIF 22
#define AD8232_ANALOG 4


#define LENGTH_CHAR 7
#define LED_BUILTIN 9
#define INTERVAL_RF 50 // Interval for sending data
char  counter = 48;
bool LOOP_CYCLE=true;
unsigned long oldtime=0; // timer memory
uint8_t leads_off = 0;
const int LENGTH_DATA=23;
void setup() 
{
  pinMode(2, OUTPUT);
  Serial.begin(115200); 
  while (!Serial);
  Serial.println("LoRa Sender");
  pinMode(L0_NEGATIF, INPUT_PULLUP); // Setup for leads off detection LO +
  pinMode(L0_POSITIF, INPUT_PULLUP); // Setup for leads off detection LO -
  LoRa.setPins(ss, rst, dio0);    //setup LoRa transceiver module
  
  while (!LoRa.begin(433E6))     //433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  LoRa.setTxPower(18);
  Serial.println("LoRa Initializing OK!");
}

void loop() 
{
  
  if((digitalRead(L0_NEGATIF) == 1)||(digitalRead(L0_POSITIF) == 1)){
    // Serial.println('!');
    leads_off=1;   
  }
  int beatAnalog= analogRead(AD8232_ANALOG);
  
  char radiopacket[LENGTH_DATA];  // Reserve space data
  sprintf(radiopacket, "{data:%d,leadsoff:%d}", beatAnalog,leads_off);
  
  // menghitung panjang string
  int len = strlen(radiopacket);
  // Serial.println(len);
  // menghapus karakter NULL (\0) pada akhir string
  memset(&radiopacket[len], 0, LENGTH_DATA - len);
  
  if(millis()-oldtime >= INTERVAL_RF)
  { 
    
    LoRa.beginPacket();   //Send LoRa packet to receiver
    LoRa.write((uint8_t *)radiopacket, len);
    LoRa.endPacket();
    Serial.println(radiopacket);

    oldtime=millis();
  }  
  
  digitalWrite(2,LOW);
  // digitalWrite(2, LOOP_CYCLE ? (LOOP_CYCLE=false, HIGH) : (LOOP_CYCLE=true, LOW));
  LOOP_CYCLE=!LOOP_CYCLE; // merubah kondisi 
  leads_off=0;
  delay(1);
}