<?php

class DataBaseManager
{
	private $db;
	private $flagError;
	
	//public function DataBaseManager($dbName)
	public function __construct($dbName)
	{
		$this->flagError=true;
		try{
			$this->db = new SQLite3($dbName,SQLITE3_OPEN_READWRITE);
			if($this->db!=null)
			{
				$this->flagError=false;
			}
		}
		catch(Exception $e) {
		
		}
	}
	
	public function getDb()
	{
		return $this->db;
	}
	
	public function getError()
	{
		return $this->flagError;
	}

    public function reload($dbName)
    {
        $this->db->close();
        $this->flagError=true;
        try{
            $this->db = new SQLite3($dbName,SQLITE3_OPEN_READWRITE);
            if($this->db!=null)
            {
                $this->flagError=false;
            }
        }
        catch(Exception $e) {

        }
    }

}
?>
