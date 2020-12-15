#!/usr/bin/python3
import os
import pwd
import sys

print("Installing support for MDE Sending on Raspi...")

if os.getuid() != 0:
    print("This command must be executed as sudo")
    exit(1)

sendingType="1" # 1: HDMI . 2: SENDING MDE
try:
    sendingType = sys.argv[1]
    if sendingType!="1" and sendingType!="2":
        raise Exception
except:
    print("Invalid argument:. 1:HDMI 2:SENDING MDE")
    exit(1)

if sendingType=="1":
    print("Video OUT: HDMI")
else:
    print("Video OUT: SENDING MDE")

#creo directorio para instalar servicios
os.system("mkdir /opt/remote_video_player")


#Instalo soporte NTP 
os.system("apt install ntpdate")
os.system("apt install fake-hwclock")
os.system("apt install aptitude")

#instalo webserver para pagina de configuracion en raspberry
os.system("aptitude install php")
os.system("aptitude install php-sqlite3")
os.system("aptitude install dnsmasq")
os.system("aptitude install hostapd")


#configuracion del sistema
os.system("cp scripts/rc.local /etc/")
os.system("cp scripts/boot/dt-blob.bin /boot/")
os.system("cp hardware/uart/driver_uart/soft_uart.ko /boot/")

# copio sudoers con www-data con permiso para reboot
os.system("cp scripts/sudoers /etc/")
os.system("chmod 440 /etc/sudoers")
# permiso de www-data para configurar la red
os.system("chmod o+r /etc/network")
os.system("chmod 666 /etc/network/interfaces")
os.system("chmod 666 /etc/dhcpcd.conf")

#configuracion de wifi como ap
os.system("cp scripts/dnsmasq.conf /etc/dnsmasq.conf")
os.system("systemctl reload dnsmasq")
os.system("cp scripts/hostapd.conf /etc/hostapd/hostapd.conf")
os.system("cp scripts/hostapd /etc/default/hostapd")
os.system("chmod 666 /etc/hostapd/hostapd.conf")
os.system("chmod 666 /etc/wpa_supplicant/wpa_supplicant.conf")

os.system("systemctl unmask hostapd")
os.system("systemctl enable hostapd")
os.system("systemctl start hostapd")
#_____________________________


os.system("cp scripts/start_config.sh /opt/remote_video_player/")
os.system("chmod 777 /opt/remote_video_player/start_config.sh")

#copio web de configuracion
os.system("cp -r www/html/* /var/www/html/")
os.system("rm /var/www/html/index.html")
#__________________________

"""
print("Indique la salida de video a utilizar:")
print("1) HDMI")
print("2) Sending MDE")
sendingType = input("Seleccione una opcion:")
"""
if sendingType=="2":    
    os.system("cp scripts/boot/config_1280x1024_LCD.txt /boot/config.txt")
else:
    os.system("cp scripts/boot/config_HDMI.txt /boot/config.txt")


print("Installing services...")

#instalo servicio de WDT
os.system("mkdir /opt/remote_video_player/wdtService")
os.system("cp ServiceWatchdog/serviceWatchdog /opt/remote_video_player/wdtService/")
os.system("chmod 777 /opt/remote_video_player/wdtService/serviceWatchdog")
os.system("cp ServiceWatchdog/dhcpcd.conf /opt/remote_video_player/wdtService/")
os.system("cp ServiceWatchdog/hostapd.conf /opt/remote_video_player/wdtService/")
os.system("cp scripts/remote_player_wdt.service  /lib/systemd/system/")
os.system("chmod 644 /lib/systemd/system/remote_player_wdt.service")

#instalo servicio de comunicaciones
os.system("mkdir /opt/remote_video_player/commService")
os.system("cp ServiceComm/serviceComm /opt/remote_video_player/commService/")
os.system("cp ServiceComm/dbntp.db /opt/remote_video_player/commService/")
os.system("cp scripts/internetCheck.sh /opt/remote_video_player/commService/")
os.system("chmod 777 /opt/remote_video_player/commService/serviceComm")
os.system("chmod 666 /opt/remote_video_player/commService/dbntp.db")
os.system("chmod 777 /opt/remote_video_player/commService")
os.system("chmod 777 /opt/remote_video_player/commService/internetCheck.sh")
os.system("cp scripts/remote_player_comm.service  /lib/systemd/system/")
os.system("chmod 644 /lib/systemd/system/remote_player_comm.service")

#instalo servicio de brillo para sending MDE
if sendingType=="2":
    os.system("rm -rf /opt/service_bright/*")
    os.system("cp -r ServiceBrightMDE/* /opt/service_bright/")
    os.system("mv /opt/service_bright/serviceBright.py /opt/service_bright/serviceBright")
    os.system("chmod 777 /opt/service_bright/serviceBright")


#Deshabilito NTP automatico
os.system("systemctl disable systemd-timesyncd.service")
os.system("sudo systemctl disable systemd-timedated")
os.system("cp /usr/sbin/ntpdate /opt/remote_video_player/commService/ntpdate_manual")
os.system("chmod 777 /opt/remote_video_player/commService/ntpdate_manual")


#Habilito nuevos servicios
print("Enabling services...")
os.system("systemctl daemon-reload")
os.system("systemctl enable remote_player_wdt")
os.system("systemctl enable remote_player_comm")


print("Sending MDE support installation finished: OK")
