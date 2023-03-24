#!/usr/bin/env python3

import asyncio
import json
import sys
from time import sleep

from SX127x.LoRa import *
from SX127x.board_config import BOARD

import paho.mqtt.client as mqtt
from mickey import *


BOARD.setup()

# Define MQTT broker host and port
broker_host = "localhost"
broker_port = 1883

# Dictionary to store client sids
clients = {}
# key & iv for mickey 2.0

key = bytes([0x01, 0x02, 0x03, 0x04, 0x05])
iv = bytes([0x06, 0x07, 0x08, 0x09, 0x0A])

# Define MQTT on_connect callback function
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker")
    else:
        print(f"Connection to MQTT broker failed with code {rc}")

# Create MQTT client instance and connect to broker
mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect
mqtt_client.connect(broker_host, broker_port)

# Define a function to publish message to MQTT broker
def publish_mqtt(topic, message):
    mqtt_client.publish(topic, json.dumps(message))

class LoRaRcvCont(LoRa):

    def __init__(self, verbose=False):
        super(LoRaRcvCont, self).__init__(verbose)
        self.set_mode(MODE.SLEEP)
        self.set_dio_mapping([0] * 6)
        self.set_lna_gain(6)
        self.set_freq(433.0)
        self.set_sync_word(0xA5)
        print("Hello")
        # print(self.version())    
        
    async def start(self):
        # self.set_freq(433.0)
        self.reset_ptr_rx()
        self.set_mode(MODE.RXCONT)
        print("Start")
        print(self.get_freq())
        while True:
            await asyncio.sleep(.5)
            rssi_value = self.get_rssi_value()
            status = self.get_modem_status()
            sys.stdout.flush()
            await self.on_rx_done()
            
    async def on_rx_done(self):
        print("\nReceived: ")
        self.clear_irq_flags(RxDone=1)
        
        payload = self.read_payload(nocheck=True)
        mickey = Mickey(key, len(key), iv, len(iv))
        decrypt_result = mickey.decrypt(payload)
        #print(bytes(payload).decode("utf-8",'ignore'))
        # print(f"data:{bytes(decrypt_result).decode('utf-8','ignore')}\nrssi:{self.get_rssi_value()}")
        try:
            if payload:
                data = {
                    "payload": bytes(decrypt_result).decode("utf-8",'ignore'),
                    "rssi_value": self.get_rssi_value(),
                    "status": self.get_modem_status()
                }
                # 
                print(data)
                # json_data = json.dumps(data)
                publish_mqtt("share-data", data)
                print("Payload sent to MQTT broker")
        except:
            print("Failed to send payload to MQTT broker")
        
        self.set_mode(MODE.SLEEP)
        self.reset_ptr_rx()
        self.set_mode(MODE.RXCONT) 

lora = LoRaRcvCont(verbose=False)
lora.set_mode(MODE.STDBY)
# lora.set_freq(433.0)
# Medium Range Defaults after init are 434.0MHz, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on 13 dBm
lora.set_pa_config(pa_select=1)

try:
    asyncio.run(lora.start())
except KeyboardInterrupt:
    sys.stdout.flush()
    print("")
    sys.stderr.write("KeyboardInterrupt\n")
finally:
    sys.stdout.flush()
    print("")
    lora.set_mode(MODE.SLEEP)
    BOARD.teardown()
