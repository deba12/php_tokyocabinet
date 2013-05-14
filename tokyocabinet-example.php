<?php

echo "tokyocabinet Hash DB\n";
$hdb = new tokyocabinet_hdb("./hash_test.db", TOKYOCABINET_HDB_HDBOWRITER | TOKYOCABINET_HDB_HDBOCREAT);

$hdb->put("name", "lijinxing");
$hdb->put("key", "value");

$hdb->put("function", "NB");

$hdb->delete("name");

$hdb->iterinit();


while(($key = $hdb->iternext()))
{
    $data = $hdb->get($key);
    echo $key, " => ", $data, "\n";
}

echo "filesize =>". $hdb->fsize(), "\n";
echo "rnum     =>". $hdb->rnum(), "\n";

//$data = $db->get("name");

//echo $data;

echo "tokyocabinet B+ Tree DB\n";
$bdb = new tokyocabinet_bdb("./btree_test.db", TOKYOCABINET_BDB_BDBOWRITER | TOKYOCABINET_BDB_BDBOCREAT);

$bdb->put("name", "lijinxing");
$bdb->put("age",  "26");
$bdb->put("nickname", "qiye");

echo $bdb->get("age");
echo "\n";

$bdb->curfirst();

while(($key = $bdb->curkey()))
{
	echo $key, " => ", $bdb->curval(), "\n";
	$bdb->curnext();
}
$bdb->curdel();

echo "filesize =>". $bdb->fsize(), "\n";
echo "rnum     =>". $bdb->rnum(), "\n";

$arr = array();
$arr["name"]  = "mikio";
$arr["age"]   = "30";
$arr["lang"]  = "ja,en,c";

$data = array();
$data["name"]  = "falcon";
$data["age"]   = "31";
$data["lang"]  = "ja";


$rs = array();
$rs["name"]  = "joker";
$rs["age"]   = "19";
$rs["lang"]  = "en,es";

echo "tokyocabinet Table Database\n";
$tdb = new tokyocabinet_tdb("./table_test.db", TOKYOCABINET_TDB_TDBOWRITER | TOKYOCABINET_TDB_TDBOCREAT);
$tdb->setindex("lang", TOKYOCABINET_TDB_TDBITLEXICAL);

$tdb->put("123", $arr);


$tdb->put("abcde", $data);
$tdb->put("ddff", $rs);


$tdb->qrynew();

$tdb->qryaddcond("age", TOKYOCABINET_TDB_TDBQCNUMGE, "20");
$tdb->qryaddcond("lang", TOKYOCABINET_TDB_TDBQCSTROR, "ja,en");
$tdb->qrysetorder("name", TOKYOCABINET_TDB_TDBQOSTRASC);
$tdb->qrysetlimit(10, 0);

echo $tdb->qrysearch_count();
echo "\n";
$list = $tdb->qrysearch();
if(is_array($list)) print_r($list);

$tdb->qrydel();

echo $tdb->makeid(), "\n";
/*
$tdb->delete("123");
$list = $tdb->get("123");
if(is_array($list)) print_r($list);
*/

?>
