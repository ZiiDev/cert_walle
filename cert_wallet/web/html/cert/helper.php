<?php
function RandomString($size)
{
    $chars    = "abcdefghijklmnopqrstuwxyzABCDEFGHIJKLMNOPQRSTUWXYZ0123456789";
    $string        = array();
    $alphaLength = strlen($chars) - 1;
    for ($i = 0; $i < $size; $i++) {
        $n      = rand(0, $alphaLength);
        $string[] = $chars[$n];
    }
    return implode($string);
}
?>
