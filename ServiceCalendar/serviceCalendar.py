#!/usr/bin/python3
import time
from CalendarDAO import CalendarDAO
from Taskmanager import TaskManager

print("Service Calendar TOTEM")

calDao = CalendarDAO("/home/pi/calendar.txt")
taskMan = TaskManager()

while True:

    time.sleep(1)

    #chequeo si cambio minuto
    data = calDao.read_calendar()
    taskMan.analize_calendar(data)
        