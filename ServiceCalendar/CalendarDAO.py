import functools
import datetime

class CalendarDAO:



    def __init__(self,path):
        self.__path = path


    def read_calendar(self):
        data = []
        with open(self.__path,"r") as f:
            for line in f:
                try:
                    parts = line.split("=")
                    hr = parts[0].strip()
                    hr = datetime.datetime.strptime(hr, '%H:%M').time()
                    val = parts[1].strip()
                    if val=="ON" or val=="OFF":
                        t="POWER"
                    else:
                        t="BRIGHT"
                        val = int(val)

                    item = {"type":t,"hr":hr,"val":val}
                    data.append(item)
                except:
                    pass

        #order by hour asc
        return sorted(data,key=functools.cmp_to_key(self.__sortFn))
        


    def __sortFn(self,item1,item2):
        comp = item1["hr"] > item2["hr"]
        if comp:
            return 1
        else:
            return -1

