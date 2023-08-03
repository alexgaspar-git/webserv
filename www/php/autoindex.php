<?php
function indexDirectory($directory)
{
    if (!is_dir($directory)) {
        die("Error: Not a valid directory.");
    }

    $files = scandir($directory);
    $index = array();

    foreach ($files as $file) {
        if ($file !== '.' && $file !== '..') {
            if (is_dir($directory . '/' . $file)) {
                $index['directories'][] = $file;
            } else {
                $index['files'][] = $file;
            }
        }
    }

    return $index;
}

// Usage example:
$directoryToIndex = getenv('INDEXPATH');

$index = indexDirectory($directoryToIndex);

// Display the results
echo "<h2>Autoindex of $directoryToIndex :</h2>";
if (isset($index['directories'])) {
    foreach ($index['directories'] as $dir) {
        echo $dir . '<br>';
    }
}
if (isset($index['files'])) {
    foreach ($index['files'] as $file) {
        echo $file . '<br>';
    }
}
?>
