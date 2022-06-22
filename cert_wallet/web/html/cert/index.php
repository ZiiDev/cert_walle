<?php
require_once './db.php';
$email = isset($_GET['email']) ? $_GET['email'] : '';

$is_signed = false;
if($email != '') {
  $data = $db->get("SELECT * from signatures where email = '{$email}'");
  if($data) {
    $is_signed = true;
  }
}


?>

<!DOCTYPE html>
<html style="height:100%">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <title> Signature Check </title>
  </head>
  <body style="color: #444; margin:0;font: normal 14px/20px Arial, Helvetica, sans-serif; height:100%; background-color: #fff;">
    <div style="height:auto; min-height:100%; ">
      <div style="text-align: center; width:800px; margin-left: -400px; position:absolute; top: 30%; left:50%;">
        <h1 style="margin:0; font-size:50px; font-weight:bold;">Signature Check</h1>
        <h2 style="margin-top:50px; font-size: 30px;">Hello <?php echo $email == '' ? 'Guest' : $email ?> </h2>
        <p>
        <?php if($email !== '') { ?>
            <?php if($is_signed) { ?>
            Congradulations !!! You are already registered and signed !!!
            <?php } else { ?>
            <a href='app://<?php echo $email ?>'>Open in App to create your register your signature</a>
            <?php } ?>
        <?php } else { ?>    
        <form method = 'get'>
        <input type='email' name='email' />
        <input type='submit' value='Request link'/>
        </form>
        <?php } ?>
        </p>
      </div>
    </div>
  </body>
</html>