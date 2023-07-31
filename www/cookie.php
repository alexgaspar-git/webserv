<!DOCTYPE html>
<html>
<head>
    <title>Print Environment Variable</title>
</head>
<body>
    <h1>Environment Variable "NUMBER":</h1>
    <?php
    // Get the value of the "NUMBER" environment variable
    $number = getenv('NUMBER');

    // Check if the variable is set and not empty
    if ($number !== false && $number !== '') {
        echo "<p>The value of the \"NUMBER\" environment variable is: $number</p>";
    } else {
        echo "<p>The \"NUMBER\" environment variable is not set or is empty.</p>";
    }
    ?>
</body>
</html>
