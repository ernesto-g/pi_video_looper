#instalo modulo para uart
insmod /boot/soft_uart.ko gpio_tx=19 gpio_rx=18


#gpio 26 como salida para WDT
echo 26 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio26/direction

#gpio 25 como entrada para SW
echo 25 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio25/direction

#gpio 27 libre
echo 27 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio27/direction

#cambio permisos de log para que la pagina web los pueda borrar
chmod 666 /var/log/syslog.*
chmod 666 /var/log/daemon.log.*
chmod 666 /var/log/kern.log.*
chmod 666 /var/log/messages.*
chmod 666 /var/log/user.log.*

chmod 666 /home/pi/size.txt
