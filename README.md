## install broker for mqtt for raspi
`wget http://repo.mosquitto.org/debian/mosquitto-repo.gpg.key`
`sudo apt-key add mosquitto-repo.gpg.key`
`sudo apt-get install mosquitto`
# Start Mosquitto Broker for MQTT
`sudo service mosquitto start`

## Listen all topic
`mosquitto_sub -v -h localhost -p 1883 -t '#'`