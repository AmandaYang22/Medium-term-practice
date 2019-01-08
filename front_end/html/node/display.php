<?php

$conn=mysqli_connect("localhost","test","123456");
if(!$conn)
{
	die("could not connect".mysqli_error());
}
	$stime=$_GET['time_start'];	
	$etime=$_GET['time_end'];
	$type=$_GET['type'];
	$area=$_GET['area'];
	$node=$_GET['node'];
	
/*	echo "!!stime";
	echo $stime;
	echo $etime;
	echo "type";
	echo $ty;
 */

	mysqli_select_db($conn,"test");
	if($type!="all"&&$area=="all"&&$node=="all"){
	//$sql="select * from test where type='".$ty."' and  time>='".$stime."'and time<='".$etime."'";
	//$result=mysqli_query($conn,"select * from test where time>='".$stime."'");
		$result=mysqli_query($conn,"select * from data where type='".$type."' and  ((time>='".$stime."'and time<='".$etime."') or time like('%".$etime."%'))order by time");
		if(!$result){
	   		printf("Error:%s\n",mysqli_error($conn));
	   		exit();
		}
	}
	else if($type=="all"&&$area!="all"&&$node=="all"){
		$result=mysqli_query($conn,"select * from data where area='".$area."'and ((time>='".$stime."'and time<='".$etime."') or time like('%".$etime."%'))order by time");
		if(!$result){
	   		printf("Error:%s\n",mysqli_error($conn));
	   		exit();
		}
	}
	else if($type=="all"&&$area=="all"&&$node!="all"){
		$result=mysqli_query($conn,"select * from data where  node='".$node."' and ((time>='".$stime."'and time<='".$etime."') or time like('%".$etime."%'))order by time");
		if(!$result){
	   		printf("Error:%s\n",mysqli_error($conn));
	   		exit();

		}
	}
	else if($type!="all"&&$area!="all"&&$node=="all"){
		$result=mysqli_query($conn,"select * from data where  area='".$area."' and type='".$type."' and ((time>='".$stime."'and time<='".$etime."') or time like('%".$etime."%'))order by time");
		if(!$result){
	   		printf("Error:%s\n",mysqli_error($conn));
	   		exit();

		}
	}
	else if($type!="all"&&$area=="all"&&$node!="all"){
		$result=mysqli_query($conn,"select * from data where type='".$type."' and node='".$node."' and ((time>='".$stime."'and time<='".$etime."') or time like('%".$etime."%')) order by time");
		if(!$result){
	   		printf("Error:%s\n",mysqli_error($conn));
	   		exit();

		}
	}
	else if($type=="all"&&$area!="all"&&$node!="all"){
		$result=mysqli_query($conn,"select * from data where area='".$area."' and node='".$node."' and ((time>='".$stime."'and time<='".$etime."') or time like('%".$etime."%')) order by time");
		if(!$result){
	   		printf("Error:%s\n",mysqli_error($conn));
	   		exit();

		}
	}
	else if($type=="all"&&$area=="all"&&$node=="all"){
		$result=mysqli_query($conn,"select * from data where((time>='".$stime."'and time<='".$etime."') or time like('%".$etime."%')) order by time");
		if(!$result){
	   		printf("Error:%s\n",mysqli_error($conn));
	   		exit();

		}

	}
	else {
		$result=mysqli_query($conn,"select * from data where area='".$area."' and type='".$type."'and node='".$node."' and ((time>='".$stime."'and time<='".$etime."') or time like('%".$etime."%'))order by time");
		if(!$result){
	   		printf("Error:%s\n",mysqli_error($conn));
	   		exit();

		}
	}

echo "<table border='1' >
<tr>
<td align=center> <b>Type</b></td>
<td align=center><b>Time</b></td>
<td align=center><b>Area</b></td>
<td align=center><b>Node</b></td></td>
<td align=center><b>Data</b></td>";

while($data = mysqli_fetch_row($result))
{   
    echo "<tr>";
    echo "<td align=center>$data[4]</td>";
    echo "<td align=center>$data[5]</td>";
    echo "<td align=center>$data[0]</td>";
    echo "<td align=center>$data[1]</td>";
    echo "<td align=center>$data[3]</td>";
    echo "</tr>";
}
echo "</table>";
?>
