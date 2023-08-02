<!DOCTYPE html>
<html>
<head>
    <title>File Listing</title>
</head>
<body>
    <h1>DELETUMS</h1>
    <ul>
        <?php
        $directory = getenv('UPLOAD_DIR');
        $files = scandir($directory);
        foreach ($files as $file) {
            if ($file !== '.' && $file !== '..' && is_file($directory . $file)) {
                $file_url = htmlspecialchars($directory . $file);
                $file_name = htmlspecialchars($file);
                echo "<li><a href=\"./delete?file=$file\"><img src=\"./images/$file\"></a></li>";
            }
        }
        if (empty($files)) {
            echo "<li>No files found in the directory.</li>";
        }
        ?>
    </ul>
</body>
</html>
