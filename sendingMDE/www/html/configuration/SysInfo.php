<?php

class SysInfo
{

    function getDate()
    {
        exec("date", $data);
        $data = implode($data, "\n");
        return $data;
    }

    function getAvalDisk()
    {
        exec("df -h | grep 'root[ \t]' | awk '{print $4}'",$data);
        $data = implode($data, "\n");
        return $data;
    }


}

?>
