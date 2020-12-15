<?php
class MyByteArray
{
    public function __construct()
    {
        $this->data="";
    }
    public function append($byteValue)
    {
        $this->data.= pack("C",$byteValue);
    }
    public function appendByteArray($ba)
    {
        for($i=0; $i<$ba->len(); $i++)
        {
            $this->append(ord($ba->getByte($i)));
        }
    }
    public function getByte($index)
    {
        return $this->data[$index];
    }
    public function get()
    {
        return $this->data;
    }
    public function len()
    {
        return strlen($this->get());
    }
}

class ConfigSender
{
    private $conDao;

	public function __construct($conDao)
	{
        $this->conDao = $conDao;
	}

    public function send()
    {
        $addr = "127.0.0.1";
        $port = 8000;

        $socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);

        $data = $this->createPacket();
        if($data!=null)
        {
            // create the request packet
            $packet = $data->get(); 
            // UDP is connectionless, so we just send on it.
            socket_sendto($socket, $packet, strlen($packet), 0, $addr, $port);
            print_r(bin2hex($packet));
            
            socket_set_option($socket,SOL_SOCKET,SO_RCVTIMEO,array("sec"=>2,"usec"=>0));
            $buffer="";
            socket_recvfrom($socket, $buffer, 516, 0, $addr, $port);
            //print_r("response:".$buffer."|");
            if($buffer=="OK")
                return true;
            else{
                echo("NO HAY RESPUESTA. ");
                return false;            
                }        
        }
        else
        {
            echo("ARCHIVO INVALIDO. ");
            return false;        
        }
    }

    private function createPacket()
    {
        $segments = new MyByteArray();

        $screen = $this->conDao->getScreenData();
        $modules = $this->conDao->getModulesData();
        $submodData = $this->conDao->getSubModulesData();

        $this->calculateHeights($modules,$submodData);// calcula los px de altura de cada linea de modulos
        //print_r($this->heights);
        $xPx=0;
        foreach($modules as $mod)
        {
            $wPx=$this->calculateWidthPxOneModule($mod["w_subm"],$mod["pitch"],$submodData);
            if($wPx==0)
                return null; // error, no esta la info de la baldosa en el archivo

            $lineNumber=$this->calculateLineNumber($mod["y_mod"]);

            $xPx = $wPx * ((int)$mod["x_mod"]);
            $pagMem = (int)($xPx / 256);
            $offsetMem = (int)( $xPx % 256 );

            $segments->append($mod["x_mod"]);
            $segments->append($mod["y_mod"]);
            $segments->append($screen["w_mod"]);
            $segments->append($screen["h_mod"]);
            $segments->append($mod["w_subm"]);
            $segments->append($mod["h_subm"]);
            $segments->append(0x00); // fijo
            $segments->append(0x00); // fijo
            $segments->append($mod["pitch"]); 
            $segments->append($pagMem);
            $segments->append($offsetMem);
            $segments->append( ($lineNumber>>8)&0xFF ); // parte alta
            $segments->append( ($lineNumber>>0)&0xFF ); // parte baja
            $segments->append(0xEE);
        }

        // Packet
        $data = new MyByteArray();
        $data->append(0x80);
        $data->append(0xD0);
        $data->append($segments->len());
        $data->append(0xFF);
        $data->appendByteArray($segments);
        $data->append(0xC2);
        $data->append(0xC4);
        $data->append(0xC5);

        return $data;
    }

    private function calculateLineNumber($y_mod)
    {
        try
        {
            $y_mod=(int)$y_mod;
            if($y_mod==0)
                return 0;

            $acc=0;
            while($y_mod>0)
            {
                $acc = $acc + $this->heights[$y_mod-1];
                $y_mod=$y_mod-1;
            }
            return $acc;
        }
        catch(Exception $e)
        {
            return 0;
        }
    }

    private function calculateHeights($modules,$submodData)
    {
        $this->heights=array();
        foreach($modules as $mod)
        {
            $lineNumber = (int)$mod["y_mod"];
            $this->heights[$lineNumber] = $this->calculateHeightPxOneModule($mod["h_subm"],$mod["pitch"],$submodData);
        }
        
    }

    private function calculateWidthPxOneModule($w_subm,$pitch,$submodData)
    {
        $pitch = (int)$pitch;
        $w_subm = (int)$w_subm;

        foreach($submodData as $row)
        {
            if( ((int)$row["pitchCode"]) == $pitch) // busco la baldosa
            {
                $wPx = (int)$row["wPx"]; // leo ancho en Px de la baldosa
                return $wPx * $w_subm; // devuelve ancho px 1 baldosa por cantidad de baldosas de ancho
            }
        }
        /*
        if($pitch==8)
            return 40 * $w_subm;
        if($pitch==5)
            return 64 * $w_subm;
        if($pitch==25)
            return 128 * $w_subm;
        // TODO agregar los demas pitchs
        */
        return 0;
    }
    private function calculateHeightPxOneModule($h_subm,$pitch,$submodData)
    {
        $pitch = (int)$pitch;
        $h_subm = (int)$h_subm;

        foreach($submodData as $row)
        {
            if( ((int)$row["pitchCode"]) == $pitch) // busco la baldosa
            {
                $hPx = (int)$row["hPx"]; // leo ancho en Px de la baldosa
                return $hPx * $h_subm; // devuelve alto px 1 baldosa por cantidad de baldosas de alto
            }
        }
        return 0;
    }

}


?>