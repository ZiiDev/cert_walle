<?php
//phpinfo();
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);
//die('here 123');
require_once './db.php';
require_once './helper.php';
$username = isset($_GET['username']) ? $_GET['username'] : '';
$signature = isset($_GET['signature']) ? $_GET['signature'] : '';
$granttoken = isset($_GET['granttoken']) ? $_GET['granttoken'] : '';
echo "<pre>username : {$username}<br/>signature : {$signature}</pre>";  
if($username != '' && $signature != '' && $granttoken!= '' ) {
    $data = $db->get("SELECT * from nadraid where username = '{$username}' and signature = '{$signature}' and token IS NOT NULL" );
    if($data) {
        echo 'ACCOUNT_ALREADY_REGISTERED';
        echo '1';
    }
    else {
    echo '2';
        $data = $db->get("SELECT * from nadraid where username = '{$username}' and signature = '{$signature}'" );
	if($data) { 
		$token = RandomString(6);
		$status = $db->insert("UPDATE nadraid SET  token='{$token}', granttoken='{$granttoken}' where username = '{$username}' and signature = '{$signature}'" );
		if($status == true) {
		        header("x-accesstoken:{$token}");
		        header("x-granttoken:{$granttoken}");
		        echo 'ACCOUNT_REGISTRATION_SUCCESSFULL';
		    
		}
		else {
		        echo 'ACCOUNT_REGISTRATION_FAILED';
		}
	}
	else {
	        echo 'ACCOUNT_NOT_FOUND';
	}
    
    
    }
}
else {
    echo 'MISSING_PARAMETERS';
}
?>
