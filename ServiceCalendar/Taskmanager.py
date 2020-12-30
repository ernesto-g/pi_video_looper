import datetime

class TaskManager:

    def __init__(self):
        pass

    def analize_calendar(self,data):
        power = 1
        bright = 100

        for item in data:
            print(item)
            item_hr = item["hr"]
            now_hr = datetime.datetime.now().time()
            if now_hr>item_hr:
                print("la hora actual es mayor que "+str(item_hr))
                if item["type"]=="POWER":
                    print("seteo power a:"+item["val"])
                    if item["val"] =="ON":
                        power=1
                    else:
                        power=0
                else:
                    print("seteo brillo a:"+str(item["val"]))
                    bright = item["val"]