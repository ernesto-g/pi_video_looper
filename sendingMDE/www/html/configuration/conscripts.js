function showLoadFile()
{
    $('#btnLoadFile').css('display', 'none');
    $('#btnUploadFile').css('display', 'block');
}

function fileSelected()
{
    $('#btnSaveFile').css('display', 'block');    
}

function sendConfigEvent()
{
    // griso boton
    $("#btnSendConfig").prop( "disabled", true );
    
    if(confirm("Se enviará la configuración a la pantalla. ¿Está seguro?."))
    {
        $.ajax({
            url: 'ConManager.php',
            type: 'POST',
            data: 'SEND',
            success: function(result) {
                $("#btnSendConfig").prop( "disabled", false );
                console.log(result);
                if(result.includes("OK"))
                    alert("Envio correcto");
                else{
                    alert(result);
                }
            }
        });
    }
    else
    {
        $("#btnSendConfig").prop( "disabled", false );
    }
}