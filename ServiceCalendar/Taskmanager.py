import datetime
#from datetime import timezone,timedelta

class TaskManager:

    def __init__(self):
        pass


    def __read_bright(self):
        with open("/home/pi/bright.txt","r") as f:
            d = f.read()
        try:
            br = int(d)
        except:
            return -1
        return br

    def __write_bright(self,bright):
        with open("/home/pi/bright.txt","w") as f:
            f.write()

    def analize_calendar(self,data):
        power = 1
        bright = self.__read_bright()
        print("Brillo leido:{}".format(bright))

        hr_0000 = datetime.datetime.strptime("00:00", '%H:%M').time()

        hr_on = self.__search_on_hr(data)
        hr_off = self.__search_off_hr(data)
        hr_now = datetime.datetime.now().time()
        print("hr on:{} hr off:{}".format(hr_on,hr_off))

        if hr_on!=None and hr_off!=None:
            if hr_off > hr_on:
                if hr_now >=hr_0000 and hr_now <=hr_on:
                    power=0
                if hr_now > hr_on and hr_now <=hr_off:
                    power=1
                if hr_now >= hr_off:
                    power=0
            else:
                if hr_now >=hr_0000 and hr_now <=hr_off:
                    power=1
                if hr_now > hr_off and hr_now <=hr_on:
                    power=0
                if hr_now >= hr_on:
                    power=1


        #brilllo
        bright_items = self.__search_brights(data)
        print(bright_items)
        if len(bright_items)==2:
            hr_b1 = bright_items[0]["hr"]
            hr_b2 = bright_items[1]["hr"]
            b1 = bright_items[0]["val"]
            b2 = bright_items[1]["val"]

            if hr_now >=hr_0000 and hr_now <=hr_b1:
                bright = b2
            if hr_now > hr_b1 and hr_now <=hr_b2:
                bright = b1
            if hr_now >= hr_b2:
                bright = b2


        print("Valores finales: power:{} bright:{}".format(power,bright))

    def __search_on_hr(self,data):
        for item in data:
            if item["type"]=="POWER":
                if item["val"] =="ON":
                    return item["hr"]

        return None

    def __search_off_hr(self,data):
        for item in data:
            if item["type"]=="POWER":
                if item["val"] =="OFF":
                    return item["hr"]

        return None

    def __search_brights(self,data):
        items = []
        count=0
        for item in data:
            if item["type"]!="POWER":
                items.append(item)
                count+=1
                if count==2:
                    break

        return items #devuelvo dos brillos con su hora y valor

