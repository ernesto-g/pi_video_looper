<?php
include_once("Tabs.php");
class View
{
	//public function View()
	public function __construct()
	{

	}

	public function printOpenHtmlTag()
	{
		echo("<!DOCTYPE html><html>");
	}
	public function printCloseHtmlTag()
	{
        echo("</body>");
        //echo("<script src='scripts.js' />");
		echo("</html>");
	}

	public function printHeader()
	{
        echo("
        <head>
            <meta charset='utf-8'>
            <meta name='viewport' content='width=device-width, initial-scale=1.0'>
            <title>Configuration</title>

            <link rel='stylesheet' href='dist/css/bootstrap.min.css'/>
            <!-- Custom styles for this template -->
            <link href='dist/css/dashboard.css' rel='stylesheet'>

            <script src='scripts.js'></script>
        </head>
        <body>
            <div class='container-fluid'>
              <div class='row'>
                <div class='col-sm-9 col-md-12 main'><!--col-sm-offset-3 col-md-offset-2 -->
            <div role='navigation' class='navbar navbar-inverse navbar-fixed-top'>
              <div class='container-fluid'>
                <div class='navbar-header'>
                    <a href='#' class='navbar-brand'>
                        <img class='full' alt='multiled' src='header-object.png'>
                        <img class='min' alt='multiled' src='header-object-min.png'>
                    </a>
                </div>
              </div>
            </div>");

            if(count(Tabs::$TABS)>0)
            {
                echo('
                <nav class="navbar navbar-default" style="min-height: 60px;">
                    <div class="container-fluid">
                        <div class="navbar-header">
                            <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#navbar" aria-expanded="false" aria-controls="navbar">
                                <span class="sr-only">Toggle navigation</span>
                                <span class="icon-bar"></span>
                                <span class="icon-bar"></span>
                                <span class="icon-bar"></span>
                            </button>
                            <!-- <a class="navbar-brand" href="#">Project name</a> -->
                        </div>
                        <div id="navbar" class="navbar-collapse collapse">
                            <ul class="nav navbar-nav">
                                <li class="active"><a href="#">Configuraci&oacute;n</a></li>
                                ');
                foreach(Tabs::$TABS as $name => $url)
                {                
                    echo('<li><a href="'.$url.'">'.$name.'</a></li>');
                }
                echo('
                            </ul>
                        </div><!--/.nav-collapse -->
                    </div><!--/.container-fluid -->
                </nav>
                ');  
            }          

	}

    public function printFooter()
    {
        echo('
        <!-- Bootstrap core JavaScript
        ================================================== -->
        <!-- Placed at the end of the document so the pages load faster -->
        <script src="dist/js/jquery.min.js"></script>
        <script src="dist/js/bootstrap.min.js"></script>
        <script src="dist/js/docs.min.js"></script>
        </div>
        </div>
        </div>
        ');
    }

    public function printSysInfo($disco,$hora,$model,$pitch,$modulesW,$modulesH)
    {
        echo("
            <div id='tableContainer' class='table-responsive'>
                <table class='table table-hover'>
                    <tr>
                        <td>Modelo:</td>
                        <td>$model <a href='ConManager.php'>Config</a></td>
                    </tr>

                    <tr>
                        <td>Memoria libre:</td>
                        <td>$disco</td>
                        <td>
                            <form action='' method='POST' class='form-horizontal' onsubmit='return confirm(\"¿Está seguro?\");'>
                                <input type='submit' value='Liberar' name='freeDisk' class='btn btn-default' ></input>
                            </form>
                        </td>
                    </tr>
                    <tr>
                        <td>Hora:</td>
                        <td>$hora</td>
                        <td>
                            <form action='' method='POST' class='form-horizontal'>
                                <input type='submit' value='Poner en hora' onclick='eventSetHour();' name='setDate' class='btn btn-default'></input>
                                <input value='sarsa' type='hidden' id='date' name='date'></input>
                            </form>
                        </td>
                    </tr>
                </table>
            </div>
            </br>
            ");
    }

    public function printForms($signSettings,$ip,$netmask,$gateway,$flagDhcp,$signName,$flagNtpOnOff,$ntpServer,$ntpTime,$ntpLastUpdate,$ssid,$wifiPass,$wifiAp)
	{
	//echo("dhcp vino con:");
	//print_r("+".$flagDhcp."+");
        //$signSettings->wPx

        $checked="";
        if($flagDhcp)
            $checked="checked";

        $checkedNtp="";
        if($flagNtpOnOff)
            $checkedNtp="checked";

	$checkedAp="";
	$checkedSt="checked";
	if($wifiAp){
	    $checkedAp="checked";
	    $checkedSt="";
	}


		echo("
            <div class='col-sm-offset-10 col-sm-2'>
                <form action='' method='POST'>
                    <input id='btnEnvio' class='btn btn-primary btn-block' type='submit' name='reset' value='Reset equipo'/>
                </form>
            </div>
            <form action='' method='POST' role='form'>
                <div class='col-sm-12'>
                    <div class='form-group'>
                    <div class='col-sm-offset-2 col-sm-10'>
                        <div class='checkbox'>
                            <input type='checkbox' name='flagDhcp' $checked id='checkDhcp' onclick='eventDhcp(this);'/>DHCP
                        </div>
                    </div>
                    </div>
		   <!--
                    <div class='form-group'>
                        <label for='signName' class='control-label col-sm-2'>Nombre:</label>
                        <div class='col-sm-10'>
                            <input type='text' class='form-control' name='signName' value='".$signName."'/>
                        </div>
                    </div>
		   -->
                    <div class='form-group'>
                        <label for='ip' class='control-label col-sm-2'>IP:</label>
                        <div class='col-sm-10'>
                            <input type='text' class='form-control' class='form-control' pattern='((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$' value='".$ip."' name='ip' id='ip' placeholder='Direcci&oacute;n IP del equipo (ej.: 192.168.0.2)'/>
                        </div>
                    </div>
                    <div class='form-group'>
                        <label for='netmask' class='control-label col-sm-2'>Netmask:</label>
                        <div class='col-sm-10'>
                            <input type='text' class='form-control' value='".$netmask."' name='netmask' id='netmask' pattern='((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$' placeholder='Máscara de subred (ej.: 255.255.255.0)'/>
                        </div>
                    </div>
                    <div class='form-group'>
                        <label for='gateway' class='control-label col-sm-2'>Gateway:</label>
                        <div class='col-sm-10'>
                            <input type='text' class='form-control' value='".$gateway."' name='gateway' id='gateway' pattern='((^|\.)((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]?\d))){4}$' placeholder='Puerta de enlace (ej.: 192.168.0.1)'/>
                        </div>
                    </div>
                    <div class='form-group'>
                        <label for='logName' class='control-label col-sm-2'>Usuario:</label>
                        <div class='col-sm-10'>
                            <input type='text' class='form-control' value='".$signSettings->logName."' id='logName' name='logName' placeholder='Nombre de usuario para acceder a este panel'/>
                        </div>
                    </div>
                    <div class='form-group'>
                        <label for='logPass' class='control-label col-sm-2'>Password:</label>
                        <div class='col-sm-10'>
                            <input type='password' class='form-control' value='' id='logPass' name='logPass' placeholder='Clave'/>
                        </div>
                    </div>
                    <div class='form-group'>
                        <label for='logPass2' class='control-label col-sm-2'>Confirmar:</label>
                        <div class='col-sm-10'>
                            <input type='password' class='form-control' value='' id='logPass2' name='logPass2' placeholder='Repita clave'/>
                        </div>
                    </div>
                    <div class='form-group'>
                        <label for='logTimeout' class='control-label col-sm-2'>Timeout Login:</label>
                        <div class='col-sm-10'>
                            <input type='number' class='form-control' value='".$signSettings->timeoutLogin."' id='logTimeout' name='logTimeout' placeholder='Duracion de la sesion en segundos)'/>
                        </div>
                    </div>
		   <!--
                    <div class='form-group'>
                        <label for='maxBright' class='control-label col-sm-2'>Limite brillo (0-100):</label>
                        <div class='col-sm-10'>
                            <input type='number' class='form-control' value='".$signSettings->maxBright."' id='maxBright' name='maxBright' placeholder='Limite de brillo' />
                        </div>
                    </div>
		    -->

                <div class='form-group'>
                    <label for='wPx' class='control-label col-sm-2'>Width (px):</label>
                    <div class='col-sm-10'>
                        <input type='number' class='form-control' value='".$signSettings->wPx."' id='wPx' name='wPx' placeholder='width' />
                    </div>
                </div>
                <div class='form-group'>
                    <label for='hPx' class='control-label col-sm-2'>Height (px):</label>
                    <div class='col-sm-10'>
                        <input type='number' class='form-control' value='".$signSettings->hPx."' id='hPx' name='hPx' placeholder='height' />
                    </div>
                </div>

                    <br>


                    <!-- NTP -->
                    <div class='form-group'>
                    <div class='col-sm-offset-2 col-sm-10'>
                        <div class='checkbox'>
                            <input type='checkbox' name='flagNtpOnOff' $checkedNtp id='flagNtpOnOff' onclick='eventNtp(this);'/>NTP
                        </div>
                    </div>
                    </div>

                    <div class='form-group'>
                        <label for='ntpServer' class='control-label col-sm-2'>NTP Server:</label>
                        <div class='col-sm-10'>
                            <input type='text' class='form-control' class='form-control' value='".$ntpServer."' id='ntpServer' name='ntpServer' placeholder='time.nist.gov'></input>
                        </div>
                    </div>
		    <!--
                    <div class='form-group'>
                        <label for='ntpTime' class='control-label col-sm-2'>NTP Hora refresco:</label>
                        <div class='col-sm-10'>
                            <input type='time' class='form-control' class='form-control' value='".$ntpTime."' id='ntpTime' name='ntpTime' ></input>
                        </div>
                    </div>

                    <div class='form-group'>
                        <label for='ntpTime' class='control-label col-sm-2'>Ultima actualizacion:</label>
                        <div class='col-sm-10'>
                            <input type='text' disabled value='".$ntpLastUpdate."'></input>
                        </div>
                    </div>
		    -->
                    <!-- _____________ -->

		    <!-- WIFI -->
		    <div class='form-group'>
			<label  class='control-label col-sm-12' style='margin-top: 40px;margin-bottom: 20px;font-size: 16px;color: gray;'>Configuracion WIFI</label>
		    </div>

                     <div class='form-group'>
			<label for='maxBright' class='control-label col-sm-2'>Modo Wifi:</label>
                        <div class='checkbox col-sm-10'>
                             <input type='radio' name='wifiMode' value='1' $checkedSt > Station 
                             <input type='radio' name='wifiMode' value='0' $checkedAp > AP 
                        </div>
                    </div>

                     <div class='form-group'>
                         <label for='ssid' class='control-label col-sm-2'>SSID:</label>
                         <div class='col-sm-10'>
                             <input type='text' value='$ssid' name='ssid' class='form-control' placeholder='Nombre red Wifi'></input>
                         </div>
                     </div>

                      <div class='form-group'>
                          <label for='ssid' class='control-label col-sm-2'>Pass:</label>
                          <div class='col-sm-10'>
                              <input type='password' value='$wifiPass' name='wifiPass' class='form-control' placeholder='Clave'></input>
                          </div>
                      </div>


                    <div class='col-sm-2'>
                        <input id='btnEnvio' class='btn btn-primary btn-block' type='submit' name='update' value='Guardar'/>
                    </div>
                </div>
            </form>
			");
	}

    function printMessages($flagUpdates,$flagReset,$error="",$flagFreeDisk,$flagSetDate)
    {
        $msg="";
        if($flagUpdates)
            $msg="Se guardaron los cambios, reiniciar equipo para aplicarlos.";

        if($flagReset)
            $msg="Se va a reiniciar el equipo...";

        if($flagFreeDisk)
            $msg="Se libero espacio.";

        if($flagSetDate)
            $msg="Se configuro la hora actual.";


        if($error!="")
        {
            echo("
                <div class='bg-danger'>
                    <span>$error</span>
                </div>
                ");
        }
        else
        {
            echo("
                <div class='bg-success'>
                    <span>$msg</span>
                </div>
                ");
        }
        echo("<div class='clearfix'></div>");

    }


}
?>

