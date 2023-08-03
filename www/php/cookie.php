<!DOCTYPE html>
<html>
<head>
    <title>Cookies</title>
</head>
<body>
    <h1>Ghetto Cookie Clicker</h1>
    <?php
    $number = getenv('NUMBER');

    echo "You have $number cookies."

    ?>
    <button onClick="window.location.reload();">I require more.</button>
    <a href="/">go back</a>
</body>
</html>
