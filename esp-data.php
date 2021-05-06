<html><body>
<?php

$servername = "localhost";

// REPLACE with your Database name
$dbname = "esp_data";
// REPLACE with Database user
$username = "admin";
// REPLACE with Database user password
$password = "cemmacemma";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT id, ringnumber, latitude, longitude, temperature, humidity, reading_time FROM SensorData ORDER BY id DESC";

echo '<table cellspacing="5" cellpadding="5">
      <tr> 
        <td>ID</td> 
        <td>Ringnumber</td> 
        <td>Latitude</td> 
        <td>Longitude</td> 
        <td>Temperature</td>
        <td>Humidity</td> 
        <td>Timestamp</td> 
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_ringnumber = $row["ringnumber"];
        $row_latitude = $row["latitude"];
        $row_longitude = $row["longitude"];
        $row_temperature = $row["temperature"]; 
        $row_humidity = $row["humidity"]; 
        $row_reading_time = $row["reading_time"];
        // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time - 1 hours"));
      
        // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 4 hours"));
      
        echo '<tr> 
                <td>' . $row_id . '</td> 
                <td>' . $row_ringnumber . '</td> 
                <td>' . $row_latitude . '</td> 
                <td>' . $row_longitude . '</td> 
                <td>' . $row_temperature . '</td>
                <td>' . $row_humidity . '</td> 
                <td>' . $row_reading_time . '</td> 
              </tr>';
    }
    $result->free();
}

$conn->close();
?> 
</table>
</body>
</html>
