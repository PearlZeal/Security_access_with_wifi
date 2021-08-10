<?php
$servername = "localhost";  // replace with servername
$username = "username";     // replace with username
$password = "password";     // replace with password of the server
$db="db";                   // replace with database name

// Create connection
$conn = new mysqli($servername, $username, $password,$db);


if ($conn->connect_error)  // Check for connection Error
{
   die("Connection failed: " . $conn->connect_error);
}

else
{
    
        $user = $_GET["user"];
        $status = $_GET["status"];
       
        date_default_timezone_set("Asia/Calcutta");
        $date= date("Y-m-d");
        $time= date("H:i:s");   
        
         //based on the status from the api link from the arduino code the updating of data is done
        if( $status=="Opening_Door")
        {
             $sql="SELECT * FROM `attendance_data` WHERE `user`='$user' AND `date`='$date'";
             //Check if any row present with given user name
            if(mysqli_num_rows(($conn->query($sql))) > 0) //already logged in
            {
                echo "already logged in    " ;
                echo $user ." ";
                echo  $status;
            }
            else  //new login event
            {
              $sql = "INSERT INTO `attendance_data`(`user`, `date`, `in_time`) VALUES ('$user', '$date', '$time')";
              $run_sql=mysqli_query($conn,$sql);
                    if ($run_sql )
                    {
                        echo "Inserted    " ;
                    }
                    else
                    {
                        echo mysqli_errorno($conn);  
                    }
             }
             
        }
        else if( $status=="Closing_Door")
        {
             $sql="SELECT * FROM `attendance_data` WHERE `user`='$user' AND `date`='$date' ";
             //Check if any row present with given user name
            if(mysqli_num_rows(($conn->query($sql))) > 0) //already logged in
            {
              
              $sql  = "UPDATE `attendance_data` SET   `out_time`= '$time', `comment`= 'SUCCESS' WHERE `user` = '$user' AND `date` = '$date' ";
              $run_sql=mysqli_query($conn,$sql);
                    if ($run_sql )
                    {
                        echo "Updated logout    " ;
                        echo $user;
                        echo  $status;
                    }
                    else
                    {
                        echo mysqli_errorno($conn);  
                    }
            }
            else  //logout without a login event... error condetion
            {
              $sql = "INSERT INTO `attendance_data`(`user`, `date`,`out_time`, `comment`) VALUES ('$user', '$date','$time', 'INVALID LOGIN ENTRY')";
              $run_sql=mysqli_query($conn,$sql);
                    if ($run_sql )
                    {
                        echo "Invalid login entry  " ;

                    }
                    else
                    {
                        echo mysqli_errorno($conn);  
                    }
            }
                    
        }
            
       
        $conn->close();
}
?>
