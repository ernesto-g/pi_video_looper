import sqlite3

class DisplayDAO:

    def __init__(self,fileDbPath):
        self.path = fileDbPath
        #self.con = sqlite3.connect(self.path)
    
    #def endTransaction(self):
    #    self.con.commit()

    def close(self):
        #self.con.close()
        pass

    
    def getBright(self):
        """
        cur = self.con.cursor()   
        cur.execute("SELECT bright,flagVirtual FROM bright")
        displays = []
        for row in cur:
            displays.append(row)

        cur.close()
        return displays
        """
        try:
            with open(self.path,"r") as f:
                bright = f.read()
                bright = bright.strip()
                bright = int(bright)
                return bright
        except:
            return 128
