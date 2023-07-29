<!DOCTYPE html>
<html>
<head>
    <title>File Listing</title>
</head>
<body>
    <h1>DELETUMS</h1>
    <ul>
        <?php
        $directory = './www/uploads/';
        $files = scandir($directory);
        foreach ($files as $file) {
            if ($file !== '.' && $file !== '..' && is_file($directory . $file)) {
                $file_url = htmlspecialchars($directory . $file);
                $file_name = htmlspecialchars($file);
                echo "<li><a href='./cgi-bin/delete.py?file=$file'><img src=\"./uploads/$file_name\"></a></li>";
            }
        }
        if (empty($files)) {
            echo "<li>No files found in the directory.</li>";
        }
        ?>
    </ul>
</body>
</html>
