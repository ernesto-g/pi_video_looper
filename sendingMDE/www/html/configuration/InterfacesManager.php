<?php
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);


class InterfacesManager
{
    //function InterfacesManager()
    function __construct()
    {

    }

    function setNetworkSettingsDhcp()
    {
        //exec("cp /var/www/Controladora/pages/configuration/interfaces_default_dhcp.dat /etc/network/interfaces", $data);
        exec("cp /var/www/html/configuration/dhcpcd_default_dhcp.conf /etc/dhcpcd.conf", $data);
        $data = implode($data, "\n");
        if($data=="")
            return true;
        else
            return false;
    }


    function getNetworkSettings()
    {
        //exec("/sbin/ifconfig", $data);
        //$data = implode($data, "\n");

        $interfaces = array();
	/*
        foreach (preg_split("/\n\n/", $data) as $int)
        {
            preg_match("/^([A-z]*\d)\s+Link\s+encap:([A-z]*)\s+HWaddr\s+([A-z0-9:]*).*" .
            "inet addr:([0-9.]+).*Bcast:([0-9.]+).*Mask:([0-9.]+).*" .
            "MTU:([0-9.]+).*Metric:([0-9.]+).*" .
            "RX packets:([0-9.]+).*errors:([0-9.]+).*dropped:([0-9.]+).*overruns:([0-9.]+).*frame:([0-9.]+).*" .
            "TX packets:([0-9.]+).*errors:([0-9.]+).*dropped:([0-9.]+).*overruns:([0-9.]+).*carrier:([0-9.]+).*" .
            "RX bytes:([0-9.]+).*\((.*)\).*TX bytes:([0-9.]+).*\((.*)\)" .
            "/ims", $int, $regex);

            if (!empty($regex))
            {
                $interface['ip'] = $regex[4];
                $interface['netmask'] = $regex[6];
            }

            exec("route -n | grep 'UG[ \t]' | awk '{print $2}'",$data);
            $data = implode($data, "\n");
            $interface['gateway'] = $data;

            exec("cat /etc/network/interfaces | grep dhcp",$data);
            $data = implode($data, "\n");
            if($data=="")
                $interface['dhcp'] = false;
            else
                $interface['dhcp'] = true;

            $interfaces[] = $interface;
        }
	*/


	$interface = array();
	exec("/sbin/ip -o -4 addr list eth0 | awk '{print $4}' | cut -d/ -f1", $data);
	if(count($data)>0)
		$interface['ip'] = $data[0];
	else
	{
		//echo("Error leyendo IP");
		$interface['ip']=""; // la debo leer de /etc/dhcpcd.conf
	}

	exec("/sbin/ifconfig eth0 | awk '/netmask/{ print $4;} '",$data2);
	if(count($data2)>0)
		$interface['netmask'] = $data2[0];
	else
	{
		$interface['netmask'] = ""; // la debo leer de /etc/dhcpcd.conf
	}

        exec("route -n | grep 'UG[ \t]' | awk '{print $2}'",$data3);
        if(count($data3)>0)
        	$interface['gateway'] = $data3[0];
	else
	{
		$interface['gateway'] = ""; // la debo leer de /etc/dhcpcd.conf
	}



        exec("cat /etc/dhcpcd.conf | grep \"^static\"",$data4);
	//print_r($data4);
        $data4 = implode($data4, "\n");
        if($data4=="")
             $interface['dhcp'] = true;
        else
             $interface['dhcp'] = false;


	// leo datos Wifi
        exec("cat /etc/dhcpcd.conf | grep \"^interface wlan0\"",$data7);
        $data7 = implode($data7, "\n");
        if($data7=="")
             $interface['wifiAp'] = false;
        else
             $interface['wifiAp'] = true;


	if($interface['wifiAp'])
	{
	   // MODO AP
	   try{
		exec("cat /etc/hostapd/hostapd.conf | grep ^ssid=.*", $data5);
		$interface['ssid'] = explode("=",$data5[0])[1];
	   }
	   catch(Exception $e)
	   {
		$interface['ssid']="";
	   }

	   try{
        	exec("cat /etc/hostapd/hostapd.conf | grep ^wpa_passphrase=.*", $data6);
        	$interface['wifiPass'] = explode("=",$data6[0])[1];
	   }
	   catch(Exception $e)
	   {
		$interface['wifiPass'] = "";
	   }
	}
	else
	{
	    // MODO STATION
	    try{
		exec("cat /etc/wpa_supplicant/wpa_supplicant.conf | grep ssid=\\\"", $data5);
		$data5 = explode("=",$data5[0]);
		$data5 = explode("\"",$data5[1]);
		//print_r($data5[1]);
		$interface['ssid']=$data5[1];
            }catch(Exception $e){
		$interface['ssid']="";
            }


             try{
                 exec("cat /etc/wpa_supplicant/wpa_supplicant.conf | grep psk=\\\"", $data6);
                 $data6 = explode("=",$data6[0]);
                 $data6 = explode("\"",$data6[1]);
                 //print_r($data6[1]);
                 $interface['wifiPass']=$data6[1];
             }catch(Exception $e){
                 $interface['wifiPass']="";
             }

	}
	//_________________________________________________________________________________



	$interfaces[] = $interface;

        return $interfaces;
    }

    //function setNetworkSettingsStatic($ip,$netmask,$gateway)



    // se usa este metodo que configura los dos eth0 y wifi
    function configureNetworks($ip,$netmask,$gateway,$flagDhcp,$ssid,$wifiPass,$wifiMode)
    {
        $out="";
        $lineNumber=0;
        $fh = fopen('/var/www/html/configuration/dhcpcd_default_static.conf','r');

        while ($line = fgets($fh))
        {
            $out=$out.$line;
            //echo($line);
            $lineNumber++;
        }
        fclose($fh);

	// agrego datos de ip estatica al final del archivo
	$netmask_to_cidr = array(
    		'255.255.255.255' => 32,
    		'255.255.255.254' => 31,
    		'255.255.255.252' => 30,
    		'255.255.255.248' => 29,
    		'255.255.255.240' => 28,
    		'255.255.255.224' => 27,
    		'255.255.255.192' => 26,
    		'255.255.255.128' => 25,

    		'255.255.255.0' => 24,
    		'255.255.254.0' => 23,
    		'255.255.252.0' => 22,
    		'255.255.248.0' => 21,
    		'255.255.240.0' => 20,
    		'255.255.224.0' => 19,
    		'255.255.192.0' => 18,
    		'255.255.128.0' => 17,

    		'255.255.0.0' => 16,
    		'255.254.0.0' => 15,
    		'255.252.0.0' => 14,
    		'255.248.0.0' => 13,
    		'255.240.0.0' => 12,
    		'255.224.0.0' => 11,
    		'255.192.0.0' => 10,
    		'255.128.0.0' => 9,

    		'255.0.0.0' => 8,
    		'254.0.0.0' => 7,
    		'252.0.0.0' => 6,
    		'248.0.0.0' => 5,
    		'240.0.0.0' => 4,
    		'224.0.0.0' => 3,
    		'192.0.0.0' => 2,
    		'128.0.0.0' => 1,
    		'0.0.0.0' => 0);

	$cidr="24";
	try{
		$cidr = "".$netmask_to_cidr[$netmask];
	}catch(Exception $e){
		$cidr="24";
	}

	// ethernet
	if($flagDhcp)
	{
	}
	else
	{
		$out=$out."interface eth0\n";
		$out=$out."static ip_address=".$ip."/".$cidr."\n";
		$out=$out."static routers=".$gateway."\n";
		$out=$out."static domain_name_servers=8.8.8.8\n";
	}
	//_________________________________________________

	// wifi
	if($wifiMode=="0") // modo AP
	{
		$out=$out."interface wlan0\n";
		$out=$out."\tstatic ip_address=192.168.4.1/24\n"; // importante dejar el tab
		$out=$out."\tnohook wpa_supplicant\n";
	}
	//_________________________________________________


        $file = fopen('/tmp/dhcpcd_default_static.conf.tmp',"w");
        fwrite($file,$out);
        fclose($file);

        exec("cp /tmp/dhcpcd_default_static.conf.tmp /etc/dhcpcd.conf", $data);
        $data = implode($data, "\n");
        //echo($data);


	// configuro SSID y password en el archivo que corresponde segun si soy station o AP
	if($wifiMode=="0") // modo AP
	{
		// AP
		$fileData="interface=wlan0
driver=nl80211
ssid=$ssid
hw_mode=g
channel=7
wmm_enabled=0
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=2
wpa_passphrase=$wifiPass
wpa_key_mgmt=WPA-PSK
wpa_pairwise=TKIP
rsn_pairwise=CCMP";

	         $file = fopen('/tmp/hostapd.conf.tmp',"w");
         	 fwrite($file,$fileData);
         	 fclose($file);

		 exec("cp /tmp/hostapd.conf.tmp /etc/hostapd/hostapd.conf", $data);
	}
	else
	{
		// STATION
		$fileData="
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
country=AR

network={
    ssid=\"$ssid\"
    psk=\"$wifiPass\"
    scan_ssid=1
}
";

                  $file = fopen('/tmp/wpa_supplicant.conf.tmp',"w");
                  fwrite($file,$fileData);
                  fclose($file);

                  exec("cp /tmp/wpa_supplicant.conf.tmp /etc/wpa_supplicant/wpa_supplicant.conf", $data);
	}



    }
    


    

}


// prueba
//$im = new InterfacesManager();
//$im->setNetworkSettingsStatic("192.168.0.127","255.255.255.0","192.168.0.1");
//$interfaces = $im->getNetworkSettings();
//echo "ip:". $interfaces[0]["ip"];
//echo "netmask:". $interfaces[0]["netmask"];
//echo "gateway:". $interfaces[0]["gateway"];

?>
