

# Instalation of software
## install docker
```
sudo apt update
sudo apt-get install ca-certificates\
 curl\
 vim -y
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh get-docker.sh
sudo usermod -aG docker ${USER}

```

### install log2ram
```
echo "deb [signed-by=/usr/share/keyrings/azlux-archive-keyring.gpg] http://packages.azlux.fr/debian/ bookworm main" | sudo tee /etc/apt/sources.list.d/azlux.list
sudo wget -O /usr/share/keyrings/azlux-archive-keyring.gpg  https://azlux.fr/repo.gpg
sudo apt update
sudo apt install log2ram
```

### RS485 module
Follow the [guide](https://www.waveshare.com/wiki/RS485_RS232_HAT) from manufacturer.


### BME680


Follow the [guide](https://www.waveshare.com/wiki/BME680_Environmental_Sensor) from manufacturer.



