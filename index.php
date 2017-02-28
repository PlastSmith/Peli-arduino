<?php

	include("connect.php"); 	
	
	// muodostetaan yhteys tamkin tietokantaan connection on erillinen tiedosto
	// joka sisaltaa tunnukseni ja sita ei tassa ole esitetty.
	$link=Connection();

	// haetaan tietokannassa olevat tulokset sql kyselylla ja tallennetaan ne muuttujaan
	$result=mysql_query("SELECT * FROM `DBtulokset` ORDER BY `tulos` DESC",$link);

?>

<html>

<head>
	<title>Meteori</title>
</head>

<body>
	<h1>Meteori pelin tulokset kautta aikojen! =)</h1>

		<table style='width:20%'>
			<tr>
				<th>Nikki</th>
				<th>Tulos</th>
			</tr>
			<?php 
			// niin kauan kuin on tuloksia luettavaksi tulostetaan nikki ja vastaava tulos
			if($result!==FALSE){
				while ($row = mysql_fetch_array($result)) {
					echo " <tr> <td> {$row['nikki']} </td> <td> {$row['tulos']} </td> </tr>";
				}
			}
			?>
		</table>

</body>

</html>