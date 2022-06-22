<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

require_once './db.php';
$granttoken = isset($_GET['granttoken']) ? $_GET['granttoken'] : '';
$token = isset($_GET['token']) ? $_GET['token'] : '';
$is_signed = false;

if($granttoken != '' && $token!= '') {
  $data = $db->get("SELECT * from nadraid where granttoken = '{$granttoken}' and token ='{$token}'");
  if($data) {
  //print_r($data);
    echo json_encode($data);
    echo "ACCOUNT_VERIFICATION_SUCCESS";
    }
    else{
    echo "ACCOUNT_VERIFICATION_FAILED";
    }
}
else{
echo "ACCOUNT_VERIFICATION_FAILED";
}


?>
