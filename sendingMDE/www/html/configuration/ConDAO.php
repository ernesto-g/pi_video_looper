<?php
class ConDAO
{
	private $db;

	public function __construct($db)
	{
		$this->db=$db;
	}

    public function getScreenData()
    {
        $out=array();
        $results = $this->db->query('SELECT * FROM screen LIMIT 1');
        if($results!=null && $row = $results->fetchArray())
        {
            $out["w_mod"] = $row["w_mod"];
            $out["h_mod"] = $row["h_mod"];
        }
        $results = $this->db->query('SELECT * FROM version LIMIT 1');
        if($results!=null && $row = $results->fetchArray())
        {
            $out["version"] = $row["version"];
        }
        return $out;
    }

    public function getModulesData()
    {
        $out=array();
        //$results = $this->db->query('SELECT * FROM modules ORDER BY id ASC');
        $results = $this->db->query('SELECT * FROM modules LEFT JOIN submodules ON modules.pitch=submodules.pitchCode ORDER BY id ASC');
        if($results!=null)
        {
            while($row = $results->fetchArray()) 
            {
                $out[] = $row;
            }
        }
        return $out;
    }

    public function getSubModulesData()
    {
        $out=array();
        $results = $this->db->query('SELECT * FROM submodules');
        if($results!=null)
        {
            while($row = $results->fetchArray()) 
            {
                $out[] = $row;
            }
        }
        return $out;
    }

    public function getSubModuleName($pitchCode)
    {
        $out="?";
        $results = $this->db->query('SELECT name FROM submodules WHERE pitchCode=$pitchCode');
        if($results!=null)
        {
            while($row = $results->fetchArray()) 
            {
                $out = $row["name"];
                break;
            }
        }
        return $out;
    }
}
?>
