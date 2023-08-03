import cgi

form = cgi.FieldStorage()

if "ccnumber" not in form:
    cc = ":("
else:
    cc = form['ccnumber'].value

if "cvv" not in form:
    cvv = ":("
else:
    cvv = form['cvv'].value

print("""\
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Free Vbux</title>
</head>
<body>
    <h2>Your credit card number is %s</h2>
    <h2>3 numbers in the back are %s</h2>
    <h2>Congratulations on the free fortnite gift card, the police has been called to your address.</h2>
    <a href="/"> go back </a>
</body>
</html>
""" % (cc, cvv))
