#!/usr/bin/env python3

from time import sleep
from SX127x.LoRa import *
from SX127x.board_config import BOARD
import asyncio
import socketio
import json

BOARD.setup()

# Dictionary to store client sids
clients = {}

# Create a Socket.IO server instance
sio = socketio.Client()

@sio.event
def connect():
    print('WebSocket connected')

@sio.event
def disconnect():
    print('WebSocket disconnected')

# Define a custom event to register clients and save their sids
@sio.on('register')
def register(sid, data):
    print(f"Client registered: {data}")
    clients[data['client_id']] = sid

# Send payload to the client using the WebSocket connection
@sio.on('message')
def send_message(data):
  sio.emit('message', {'payload': data['payload']})

# Define a function to handle the 'client_found' event
@sio.event
def client_found(data):
    print(f"Client {data['client_id']} found in room {data['room_sid']}")

# Define a function to handle the 'client_not_found' event
@sio.event
def client_not_found(data):
    print(f"Client {data['client_id']} not found")

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
        #print(bytes(payload).decode("utf-8",'ignore'))
        print(f"data:{bytes(payload).decode('utf-8','ignore')}\nrssi:{self.get_rssi_value()}")
        try:
            data = {
                "payload": bytes(payload).decode("utf-8",'ignore'),
                "rssi_value": self.get_rssi_value(),
                "status": self.get_modem_status()
            }
            if 'client1' in clients:
                await sio.emit('message', data, room=clients['client1'])
                print("Payload sent to WebSocket")
            else:
                print("Client1 not found")
                sio.emit('find_client', 'client1')
        except:
            print("Failed to send payload to WebSocket")
        
        self.set_mode(MODE.SLEEP)
        self.reset_ptr_rx()
        self.set_mode(MODE.RXCONT) 

lora = LoRaRcvCont(verbose=False)
lora.set_mode(MODE.STDBY)
# lora.set_freq(433.0)
# Medium Range Defaults after init are 434.0MHz, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on 13 dBm
lora.set_pa_config(pa_select=1)

try:
    sio.connect('http://localhost:8000')
    sio.emit('find_client', 'client1')
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
    sio.disconnect()
