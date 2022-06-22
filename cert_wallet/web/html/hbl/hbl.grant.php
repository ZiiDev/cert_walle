<?php
require_once './db.php';
require_once './helper.php';



$grantToken = RandomString(6);
$status = $db->insert("INSERT INTO hblid(granttoken) VALUES ('{$grantToken}')");
        if($status == true) {
            header("x-custom-header:{$grantToken}");
            echo 'ACCOUNT_REGISTRATION_SUCCESSFULL';
        }
        else {
            echo 'ACCOUNT_REGISTRATION_FAILED';
        }


?>

