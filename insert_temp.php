<?php

if(isset($_GET["resval"])) {
   $resval = $_GET["resval"]; // get resval value from HTTP GET

   $servername = "localhost";
   $username = "ESP32";
   $password = "esp32io.com";
   $database_name = "db_esp32";

   // Create MySQL connection fom PHP to MySQL server
   $connection = new mysqli($servername, $username, $password, $database_name);
   // Check connection
   if ($connection->connect_error) {
      die("MySQL connection failed: " . $connection->connect_error);
   }

   $sql = "INSERT INTO tbl_temp (resval,time) VALUES ($resval, NOW())";

   if ($connection->query($sql) === TRUE) {
      echo "New record created successfully";
   } else {
      echo "Error: " . $sql . " => " . $connection->error;
   }

   $connection->close();
} else {
   echo "resval is not set in the HTTP request";
}
?>