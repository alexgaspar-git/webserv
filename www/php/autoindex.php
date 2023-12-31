<?php
function indexDirectory($directory)
{
    if (!is_dir($directory)) {
        die("$#404");
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

$noindex = getenv('NOINDEX');

if ($noindex == "TRUE")
    die ("$#403");

$directoryToIndex = getenv('INDEXPATH');

$index = indexDirectory($directoryToIndex);

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
