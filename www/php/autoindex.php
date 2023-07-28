<!DOCTYPE html>
<html>
<head>
    <title>File Listing</title>
</head>
<body>
    <h1>List of Files in /uploads/ Directory</h1>
    <ul>
        <?php
        $directory = './www/pages/';
        $files = scandir($directory);

        foreach ($files as $file) {
            // Exclude '.' and '..' directories and list only files
            if ($file !== '.' && $file !== '..' && is_file($directory . $file)) {
                $file_url = htmlspecialchars($directory . $file);
                $file_name = htmlspecialchars($file);
                echo "<li><a href=\"$file_url\">$file_name</a></li>";
            }
        }


        if (empty($files)) {
            echo "<li>No files found in the directory.</li>";
        }
        ?>
    </ul>
</body>
</html>
