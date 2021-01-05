<?php

include_once("SignSettingsDAO.php");
include_once("SignSettings.php");

class Model
{
	private $dbmProgram;
	private $signSettings;
	private $signSettingsDao;
	private $error;

    	//public function Model($dataBaseManagerProgram)
    	public function __construct($dataBaseManagerProgram)
	{
		$error=false;
		$this->dbmProgram=$dataBaseManagerProgram;
		$this->signSettingsDao = new SignSettingsDAO($this->dbmProgram->getDb());
		$this->signSettings = $this->signSettingsDao->getSettings();
		if($this->signSettings==null)
			$this->error=true;
	}

	public function getError()
	{
		return $this->error;
	}

	public function getSignSettings()
	{
		return $this->signSettings;
	}


	public function updateValues($fields,$im)
	{
        	$flagNtpOnOff=false;
        	if(array_key_exists("flagNtpOnOff",$fields))
            		$flagNtpOnOff=true;

            $error="";
            $this->signSettingsDao->updateValues($fields["logName"],$fields["logTimeout"],$flagNtpOnOff,$fields["ntpServer"]);

            if($fields["logPass"]!="")
            {
                if($fields["logPass"]==$fields["logPass2"])
                    $this->signSettingsDao->updatePassword(sha1($fields["logPass"]));
                else
                    $error="Los passwords son diferentes.";
            }

	    /*
            $im->setNetworkSettingsStatic($fields["ip"],$fields["netmask"],$fields["gateway"]);
            $flagDhcp = isset($fields['flagDhcp']) && $fields['flagDhcp']  ? "1" : "0";
            if($flagDhcp)
            {
                $im->setNetworkSettingsDhcp();
            }
	    */

           $flagDhcp = isset($fields['flagDhcp']) && $fields['flagDhcp']  ? "1" : "0";
	   //$flagAp = isset($fields['flagDhcp']) && $fields['flagDhcp']  ? "1" : "0";

	   $im->configureNetworks($fields["ip"],$fields["netmask"],$fields["gateway"],$flagDhcp,$fields["ssid"],$fields["wifiPass"],$fields["wifiMode"]);

	   $this->signSettingsDao->updateSize($fields["wPx"],$fields["hPx"]);	


            return $error;
	}

    public function reset()
    {
        shell_exec("sudo /sbin/reboot");
    }

    public function freeDisk()
    {
		shell_exec("sudo rm /var/log/syslog.*");
		shell_exec("sudo rm /var/log/daemon.log.*");
		shell_exec("sudo rm /var/log/kern.log.*");
		shell_exec("sudo rm /var/log/messages.*");
		shell_exec("sudo rm /var/log/user.log.*");
    }

    public function setDateTime($datetime)
    {
        return $this->signSettingsDao->setDateTime($datetime);
    }

}
?>
