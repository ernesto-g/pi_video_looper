import sqlite3

class DisplayDAO:

    def __init__(self,fileDbPath):
        self.path = fileDbPath
        self.con = sqlite3.connect(self.path)
    
    #def endTransaction(self):
    #    self.con.commit()

    def close(self):
        self.con.close()

    
    def getBright(self):
        cur = self.con.cursor()   
        #cur.execute("SELECT Displays.idDisplay,bright,x,y,w,h FROM DisplaySettings INNER JOIN Displays ON Displays.idDisplay=DisplaySettings.idDisplay")
        cur.execute("SELECT bright,flagVirtual FROM bright")
        displays = []
        for row in cur:
            displays.append(row)

        cur.close()
        return displays

