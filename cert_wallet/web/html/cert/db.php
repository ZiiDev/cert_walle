<?php
$db = new db;
class db {
    private $conn = null;

    function __construct() {
        $this->connect();
    }

    public function connect() {
        $this->conn = new mysqli("localhost","root","phpmyadmin","nadra");
        if (mysqli_connect_errno()) {
            echo "Failed to connect to MySQL: " . mysqli_connect_error();
            exit();
        }
    }
    
    public function get($sql) {
        $records = $this->conn->query($sql);
        
        if ($records && $records->num_rows > 0) {
            $data = [];
            // output data of each row
            while($row = $records->fetch_assoc()) {
              $data[] = $row;
            }
            return $data;
        } 
        else {
            if($this->conn -> error)
            {
                echo $this->conn -> error;
            }
            return false;
        }
    }

    public function insert($sql) {
        if($this->conn->query($sql) === TRUE) {
            return true;
        }
        else {
            echo $this->conn -> error;
            return false;
        }
    }
}
