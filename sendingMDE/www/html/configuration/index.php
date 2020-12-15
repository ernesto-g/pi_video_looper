<?php
session_start();

ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);

include_once("Model.php");
include_once("View.php");
include_once("DataBaseManager.php");
include_once("InterfacesManager.php");
include_once("SysInfo.php");


if (!isset($_SERVER['PHP_AUTH_USER'])) {
    header('WWW-Authenticate: Basic realm=""');
    header('HTTP/1.0 401 Unauthorized');
    echo 'ERROR';
    exit;
}


//$dname = '/var/www/html/dbcontroladora.db';
$dname = '/opt/remote_video_player/commService/dbntp.db';
$dbm = new DataBasemanager($dname);
if($dbm->getError()==true) {
	echo("ERROR");
	exit();
}


$model = new Model($dbm);
if($model->getError()==true) {
	echo("ERROR");
	exit();
}
$signSettings = $model->getSignSettings();

//Login
if(isset($_SESSION['timeLogin']))
	$timeLogin = $_SESSION['timeLogin'];
else
	$timeLogin = time();

$inactivity = time() - $timeLogin;
if($_SERVER['PHP_AUTH_USER']!=$signSettings->logName || sha1($_SERVER['PHP_AUTH_PW'])!=$signSettings->logPass || $inactivity>=($signSettings->timeoutLogin*60))
{
	$_SESSION['timeLogin'] = time();
	header('WWW-Authenticate: Basic realm=""');
    header('HTTP/1.0 401 Unauthorized');
	exit();
}
$_SESSION['timeLogin'] = time();
//_______

$im = new InterfacesManager();

// Update values
$flagUpdate=false;
$flagReset=false;
$flagFreeDisk=false;
$flagSetDate=false;
$error="";
if(isset($_POST["update"]))
{
    $error=$model->updateValues($_POST,$im);
    $flagUpdate=true;
    //$signSettings->signName=$_POST["signName"];
}
elseif(isset($_POST["reset"]))
{
    $model->reset();
    $flagReset=true;
}
if(isset($_POST["freeDisk"]))
{
    $model->freeDisk();
    $flagFreeDisk=true;
}
if(isset($_POST["setDate"]))
{
    if($model->setDateTime($_POST["date"]))
        $flagSetDate=true;
    else
        $error="Error cargando fecha y hora.";
}
//______________


//$signSettings = $model->getSignSettings(); // vuelvo a leer las settings para mostrar los cambios recientes
//echo("nombre:".$signSettings->signName);
//echo("flag update:".$flagUpdate);

// Print web page
$interfaces = $im->getNetworkSettings();
$sysinfo = new SysInfo();

$view = new View();
$view->printOpenHtmlTag();
	$view->printHeader();
    $view->printMessages($flagUpdate,$flagReset,$error,$flagFreeDisk,$flagSetDate);
    $view->printSysInfo($sysinfo->getAvalDisk(),$sysinfo->getDate(),$signSettings->signModel,$signSettings->pitch,$signSettings->modulesW,$signSettings->modulesH);
	$view->printForms($signSettings,$interfaces[0]["ip"],$interfaces[0]["netmask"],$interfaces[0]["gateway"],$interfaces[0]["dhcp"],$signSettings->signName,
			$signSettings->ntpOnOff,$signSettings->ntpServer,$signSettings->ntpTime,$signSettings->ntpLastUpdate,
			$interfaces[0]["ssid"],$interfaces[0]["wifiPass"],$interfaces[0]["wifiAp"]);
    $view->printFooter();
$view->printCloseHtmlTag();
//_______________
?>
