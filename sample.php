<?php
$handle = fopen("php://input", "r");
echo "_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/\n";
while($row = fread($handle, 1024)) {
    echo $row."\n";
}
echo "_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/\n";
fclose($handle);

print_r($_SERVER);
echo "\n";
