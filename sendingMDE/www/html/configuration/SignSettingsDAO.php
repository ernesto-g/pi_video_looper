<?php
class SignSettingsDAO
{
	private $db;

	//public function SignSettingsDAO($db)
	public function __construct($db)
	{
		$this->db=$db;
	}

	public function getSettings()
	{
        $r=5;
        while($r>0)
        {
            try
            {
		        $results = $this->db->query('SELECT * FROM SignSettings LIMIT 1');
		        if($results!=null && $row = $results->fetchArray())
		        {
			        $vws = new SignSettings();
			        $vws->logName = $row['webLogName'];
			        $vws->logPass = $row['webLogPass'];
                    $vws->timeoutLogin = $row['timeoutLogin'];

                    //consulto nombre del cartel y modelo
		    /*
                    $results = $this->db->query('SELECT name,model FROM HardwareInfo LIMIT 1');
                    if($row = $results->fetchArray())
                    {
                        $vws->signName = $row['name'];
                        $vws->signModel = $row['model'];
                    }*/
		    $vws->signName = ""; //$row['name'];
 	            $vws->signModel = "TOTEM"; //$row['model'];

                    //consulto cantidad de modulos
                    $results = $this->db->query('SELECT modulesW,modulesH,pitch FROM DisplaySettings WHERE idDisplay=1 LIMIT 1');
                    if($row = $results->fetchArray())
                    {
                        $vws->modulesW = $row['modulesW'];
                        $vws->modulesH = $row['modulesH'];
                        $vws->pitch = $row['pitch'];
                    }

                    // consulto datos NTP
                    $results = $this->db->query('SELECT url,onOff,refreshTime FROM NtpSettings LIMIT 1');
                    if($row = $results->fetchArray())
                    {
                        $vws->ntpServer = $row['url'];
                        //echo("lei:".$row['onOff']);
                        if( ((int)$row['onOff']) == 1)
                            $vws->ntpOnOff = true;
                        else
                            $vws->ntpOnOff = false;
                        $vws->ntpTime = $row['refreshTime'];
                        $vws->ntpLastUpdate = 0; //$row['lastUpdate']; // no lo uso mas
                    }


                    $vws->maxBright = $this->getMaxBright();

                    $size  = $this->readPlayerSize();
                    //echo("lei size: ".$size["w"]." ".$size["h"]);
                    $vws->wPx = (int)$size["w"];
                    $vws->hPx = (int)$size["h"];

 			        return $vws;
		        }
		        return null;
            }catch(Exception $e){}
            $r--;
            usleep(100000);
        }
        return null;
    }

    public function updateSize($wPx,$hPx)
    {
        file_put_contents("/home/pi/size.txt","$wPx\n$hPx"); 
    }
    
    private function readPlayerSize()
    {
        if(file_exists("/home/pi/size.txt"))
        {
            $handle = fopen("/home/pi/size.txt", "r");
            if ($handle) {
                $w = fgets($handle);
                $h = fgets($handle);
                fclose($handle);
                return array("w"=>$w,"h"=>$h);
            } else {
                // error opening the file.
            }
        } 
        return array("w"=>0,"h"=>0);
    }

    public function updateValues($user,$timeout,$ntpOnOff,$ntpService)
    {
        if($timeout<60)
            $timeout=60;

        $ntpOnOffN="0";
        if($ntpOnOff)
            $ntpOnOffN="1";

        $this->db->query("UPDATE SignSettings SET webLogName='".$user."', timeoutLogin=".$timeout);
	$this->db->query("UPDATE NtpSettings SET url='".$ntpService."', onOff=".$ntpOnOffN);

    }

    public function updatePassword($sha1Pass)
    {
        $this->db->query("UPDATE SignSettings SET webLogPass='".$sha1Pass."'");
    }


    public function setDateTime($datetime)
    {
        if($this->db->query("UPDATE DateTime SET flagUpdate=1, datetime='".$datetime."'"))
        {
            return true;
        }
        return false;
    }

    private function getMaxBright()
    {
        //$ini_array = parse_ini_file("/var/www/Controladora/ServiceCalendar/brightConfig.conf", true);
        //$ini_array = parse_ini_file("/opt/service_calendar/brightConfig.conf", true);
        //print_r($ini_array);
        //$dic = $ini_array["Configuration"];
        //return (int)$dic["maxbright"];
	return 0;
    }

    private function setBrightMax($maxBright)
    {
        //$ini_array = parse_ini_file("/var/www/Controladora/ServiceCalendar/brightConfig.conf", true);
        //$ini_array = parse_ini_file("/opt/service_calendar/brightConfig.conf", true);
        //$ini_array["Configuration"]["maxbright"] = $maxBright;
        //$this->write_ini_file($ini_array,"/var/www/Controladora/ServiceCalendar/brightConfig.conf", true);
        //$this->write_ini_file($ini_array,"/opt/service_calendar/brightConfig.conf", true);
    }


    function write_ini_file($assoc_arr, $path, $has_sections=FALSE) 
    { 
    $content = ""; 
    if ($has_sections) { 
        foreach ($assoc_arr as $key=>$elem) { 
            $content .= "[".$key."]\n"; 
            foreach ($elem as $key2=>$elem2) { 
                if(is_array($elem2)) 
                { 
                    for($i=0;$i<count($elem2);$i++) 
                    { 
                        $content .= $key2."[] = ".$elem2[$i]."\n"; 
                    } 
                } 
                else if($elem2=="") $content .= $key2." = \n"; 
                else $content .= $key2." = ".$elem2."\n"; 
            } 
        } 
    } 
    else { 
        foreach ($assoc_arr as $key=>$elem) { 
            if(is_array($elem)) 
            { 
                for($i=0;$i<count($elem);$i++) 
                { 
                    $content .= $key."[] = \"".$elem[$i]."\"\n"; 
                } 
            } 
            else if($elem=="") $content .= $key." = \n"; 
            else $content .= $key." = \"".$elem."\"\n"; 
        } 
    } 

    if (!$handle = fopen($path, 'w')) { 
        return false; 
    }

    $success = fwrite($handle, $content);
    fclose($handle); 

    return $success; 
    }
}
?>
