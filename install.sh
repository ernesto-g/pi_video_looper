#!/bin/sh

# Error out if anything fails.
set -e

# Make sure script is run as root.
if [ "$(id -u)" != "0" ]; then
  echo "Must be run as root with sudo! Try: sudo ./install.sh"
  exit 1
fi


echo "Installing dependencies..."
echo "=========================="
apt update && apt -y install python3 python3-pip python3-pygame supervisor omxplayer ntfs-3g exfat-fuse

if [ "$*" != "no_hello_video" ]
then
	echo "Installing hello_video..."
	echo "========================="
	apt -y install git build-essential python3-dev
	git clone https://github.com/adafruit/pi_hello_video
	cd pi_hello_video
	./rebuild.sh
	cd hello_video
	make install
	cd ../..
	rm -rf pi_hello_video
else
    echo "hello_video was not installed"
    echo "=========================="
fi

echo "Installing video_looper program..."
echo "=================================="

# change the directoy to the script location
cd "$(dirname "$0")"

mkdir -p /mnt/usbdrive0 # This is very important if you put your system in readonly after
mkdir -p /home/pi/video # create default video directory

pip3 install setuptools
python3 setup.py install --force

cp ./assets/video_looper.ini /boot/video_looper.ini

echo "Configuring video_looper to run on start..."
echo "==========================================="

cp ./assets/video_looper.conf /etc/supervisor/conf.d/


echo "Configuring service_bright to run on start..."
echo "==========================================="
cp ./assets/service_bright.conf /etc/supervisor/conf.d/
mkdir /opt/service_bright
cp ./ServiceBright2/serviceBright /opt/service_bright/serviceBright
chmod 777 /opt/service_bright/serviceBright

echo "Configuring service_calendar to run on start..."
echo "==========================================="
cp ./assets/service_calendar.conf /etc/supervisor/conf.d/
mkdir /opt/service_calendar
cp ./ServiceCalendar/* /opt/service_calendar/
chmod 777 /opt/service_calendar/serviceCalendar.py


echo -n "Se utilizara sending NOVA ? (y/n)? "
read answer
if echo "$answer" | grep -iq "^y" ;then
    echo Yes
    cp ./ServiceBrightNova/serviceBright /opt/service_bright/serviceBright
    chmod 777 /opt/service_bright/serviceBright
fi



service supervisor restart

echo "Finished!"
