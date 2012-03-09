--TEST--
Check for sixplz data consistency
--SKIPIF--
<?php if (!extension_loaded("sixplz")) print "skip"; ?>
--FILE--
<?php
$BLOCK=1024;
$length=1000;
$data = array();
for($x=0;$x<($length/$BLOCK);$x++) {
        $data=$data . substr(str_shuffle(str_repeat('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789',5)),0,$BLOCK);
}
$sdata=serialize($data);
$md5_sdata = md5($sdata);
$cdata = sixplz_pack($sdata);
$sdata = sixplz_unpack($cdata);
$md5_sdata2 = md5($sdata);
if ($md5_sdata === $md5_sdata2) echo "TRUE";
?>
--EXPECT--
TRUE
