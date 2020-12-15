function eventDhcp(cb) 
{
    //console.log("Clicked, new value = " + cb.checked);
    var ip = document.getElementById("ip");
    var netmask = document.getElementById("netmask");
    var gateway = document.getElementById("gateway");

    if(cb.checked)
    {
        ip.readOnly = true;
        netmask.readOnly = true;
        gateway.readOnly = true;
    }
    else
    {
        ip.readOnly = false;
        netmask.readOnly = false;
        gateway.readOnly = false;

    }
};

function eventSetHour()
{
    d = getDateTime();
    console.log("dia:"+d);

    var date = document.getElementById("date");    
    date.value = d

}

function getDateTime() 
{
    var now     = new Date(); 
    var year    = now.getFullYear();
    var month   = now.getMonth()+1; 
    var day     = now.getDate();
    var hour    = now.getHours();
    var minute  = now.getMinutes();
    var second  = now.getSeconds(); 

    if(month.toString().length == 1) {
        var month = '0'+month;
    }
    if(day.toString().length == 1) {
        var day = '0'+day;
    }   
    if(hour.toString().length == 1) {
        var hour = '0'+hour;
    }
    if(minute.toString().length == 1) {
        var minute = '0'+minute;
    }
    if(second.toString().length == 1) {
        var second = '0'+second;
    }   

    var dateTime = (''+year)+(''+month)+(''+day)+(''+hour)+(''+minute)+(''+second);   
    return dateTime;

};
