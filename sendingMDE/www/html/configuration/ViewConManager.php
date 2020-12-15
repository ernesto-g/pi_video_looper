<?php
class ViewConManager
{
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

            <script src='conscripts.js'></script>
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

    public function printConInfo($version,$modulesW,$modulesH,$fileData)
    {
        echo("
            <div id='tableContainer' class='table-responsive'>
                <h3>Configuración de módulos</h3>
                <table class='table table-hover'>
                    <tr>
                        <td>Archivo de configuración:</td>
                        <td>V$version - $modulesW x $modulesH</td>
                        <td>
                            <input id='btnLoadFile' type='submit' value='Cargar nuevo archivo' name='' class='btn btn-default' onclick='showLoadFile()'></input>
                            <form action='' method='POST' class='form-horizontal' enctype='multipart/form-data'>
                                <input id='btnUploadFile' type='file' value='Cargar nuevo archivo' name='newFile' class='btn btn-default' onchange='fileSelected()' style='display:none;'></input>
                                <input id='btnSaveFile' type='submit' value='Subir' name='upload' class='btn btn-primary' style='display:none;'></input>
                            </form>
                        </td>
                        <td><a href='http://".$_SERVER['SERVER_NAME']."/dbconfig.db3'><input type='submit' value='Bajar' name='downloadFile' class='btn btn-default' ></input></a>
                    </tr>
                </table>

                <h3>Configuración del archivo</h3>
                <table class='table table-hover'>
                    <tr>
                        <th>Módulo</th>
                        <th>X</th>
                        <th>Y</th>
                        <th>Submódulos W</th>
                        <th>Submódulos H</th>
                        <th>Pitch</th>
                    </tr>");

        foreach($fileData as $row)
        {
            echo("
                    <tr>
                        <td>".$row["id"]."</td>
                        <td>".$row["x_mod"]."</td>
                        <td>".$row["y_mod"]."</td>
                        <td>".$row["w_subm"]."</td>
                        <td>".$row["h_subm"]."</td>
                        <td>".$row["name"]."</td>
                    </tr>
                ");
        }
        echo("        
                </table>
                <input id='btnSendConfig' type='submit' value='REENVIAR CONFIGURACIÓN' name='sendData' class='btn btn-primary' onclick='sendConfigEvent()' ></input>
                <p style='font-weight: bold;'> Atención: Reenviar la configuración incorrecta puede hacer que el equipo deje de funcionar.</p>
            </div>
            </br>
            ");
    }

 
    function printMessages($msg,$error)
    {
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

