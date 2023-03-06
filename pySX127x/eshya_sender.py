from time import sleep
from SX127x.LoRa import *
from SX127x.board_config import BOARD

BOARD.setup()

class LoRaSender(LoRa):
    def __init__(self, frequency, verbose=False):
        super(LoRaSender, self).__init__(verbose)
        self.set_mode(MODE.TX)
        self.set_pa_config(pa_select=1)
        self.frequency = frequency

    def send_message(self, message):
        payload = list(bytes(message, "utf-8"))
        self.write_payload(payload)
        self.set_mode(LoRa.TX)
        sleep(1)
        self.set_mode(LoRa.SLEEP)

    def on_tx_done(self):
        self.clear_irq_flags(TxDone=1)
        print("Packet sent")
        sys.stdout.flush()

lora = LoRaSender(433E6, verbose=False)
message_counter = 0
while True:
    message = "Hello {}".format(message_counter)
    print(message)
    lora.send_message(message.encode('utf-8'))
    message_counter += 1
    sleep(1)
