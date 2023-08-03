<!DOCTYPE html>
<html>
<head>
    <title>Gallery</title>
    <style>
        body {
            display: flex;
            flex-direction: column;
        }
        a {
            margin: 5px 0;
        }
    </style>
</head>
<body>
    <h1>Gallery</h1>
    <a href="/">go back</a>
    <?php
    $directory = getenv('UPLOAD_DIR');
    $files = scandir($directory);
    foreach ($files as $file) {
        if ($file !== '.' && $file !== '..' && is_file($directory . $file)) {
            echo "<a href=\"/delete?file=$file\"><img src=\"/images/$file\"></a>";
        }
    }
    ?>
</body>
</html>
