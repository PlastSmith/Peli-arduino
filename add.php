<?php
   	include("connect.php");
   	
   	$link=Connection();

	$nikki=$_GET["nikki"];
	$tulos=$_GET["tulos"];

	$query = "INSERT INTO `DBtulokset` (`tulos`, `nikki`) VALUES ('".$tulos."','".$nikki."')"; 
	echo "sql hakukomento: " + $query;
   	
   	mysql_query($query,$link);
	mysql_close($link);

   	header("Location: index.php");
?>