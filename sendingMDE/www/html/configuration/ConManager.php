<?php
session_start();

ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);

include_once("Model.php");
include_once("DataBaseManager.php");
include_once("ViewConManager.php");
include_once("ConDAO.php");
include_once("ConfigSender.php");


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

// Web service
if(isset($_POST["SEND"]))
{
	// Envio configuracion a servicio de brillo para que le llegue a la FPGA
	$dbconfig = new DataBasemanager('/var/www/html/dbconfig.db3');
	$conDao = new ConDAO($dbconfig->getDb());
	$sender = new ConfigSender($conDao);
	if($sender->send())
		echo("OK");
	else
		echo("ERROR");

	exit();
}
//___________


// Print web page
$view = new ViewConManager();
$view->printOpenHtmlTag();
	$view->printHeader();


// Veo si vino un archivo nuevo y lo guardo
if(isset($_FILES["newFile"]))
{
	$ext = pathinfo($_FILES["newFile"]["name"],PATHINFO_EXTENSION);

	if($ext=="db3" && move_uploaded_file($_FILES["newFile"]["tmp_name"],"/var/www/html/dbconfig.db3"))
	{
		$view->printMessages("Archivo de configuración subido correctamente","");
	}
	else
	{
		$view->printMessages("","Error subiendo archivo de configuración");
	}
}
//_________________________________________

// CON MODEL
$dbconfig = new DataBasemanager('/var/www/html/dbconfig.db3');
$conDao = new ConDAO($dbconfig->getDb());

$screenData = $conDao->getScreenData();
$version = $screenData["version"];
$modulesW = $screenData["w_mod"];
$modulesH = $screenData["h_mod"];
$fileData= $conDao->getModulesData(); //array( array("id"=>0,"X"=>0) ,  array("id"=>0,"X"=>0) );


// Print web page
$view->printConInfo($version,$modulesW,$modulesH,$fileData);
$view->printFooter();
$view->printCloseHtmlTag();
//_______________

// cierro dbs
$dbm->getDb()->close();
$dbconfig->getDb()->close();

?>
