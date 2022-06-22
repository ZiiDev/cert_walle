<?php
require_once './db.php';


$granttoken = isset($_GET['granttoken']) ? $_GET['granttoken'] : '';
$token = isset($_GET['token']) ? $_GET['token'] : '';
$cURLConnection = curl_init();

curl_setopt($cURLConnection, CURLOPT_URL, "http://localhost/cert/nadra.verify.php??granttoken={$granttoken}&token={$token}");
curl_setopt($cURLConnection, CURLOPT_RETURNTRANSFER, true);

$response = curl_exec($cURLConnection);

curl_close($cURLConnection);
print_r($response);
if($response!='ACCOUNT_VERIFICATION_FAILED'){
	$data=json_decode($response,TRUE);
	
	$status = $db->insert("UPDATE hblid SET  token='{$token}' where granttoken = '{$granttoken}'");
	if($status == true) {
	    header("x-custom-header:{$granttoken}");
	    echo 'ACCOUNT_REGISTRATION_SUCCESSFULL';
	}
	else {
	    echo 'ACCOUNT_REGISTRATION_FAILED';
	}
}
