## install broker for mqtt for raspi
`wget http://repo.mosquitto.org/debian/mosquitto-repo.gpg.key`
`sudo apt-key add mosquitto-repo.gpg.key`
`sudo apt-get install mosquitto`
# Start Mosquitto Broker for MQTT
`sudo service mosquitto start`

## Listen all topic
`mosquitto_sub -v -h localhost -p 1883 -t '#'`



## Folder Description
- Project_Faldo_ESP32 : Kode untuk ESP32 transceiver
- micket : cpp kode untuk enkripsi
- pySX127x : library python untuk lora
- pySX127x/LoRates-mqtt/py : code receiver lora
- Login-Authorize : Simple Backend untuk login system ke web server
- IOT_Faldo : directory static Web Server