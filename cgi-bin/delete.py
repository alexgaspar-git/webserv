import cgi, os

form = cgi.FieldStorage()

if "file" in form:
	if os.path.exists("./www/images/" + form['file'].value):
		os.remove("./www/images/" + form['file'].value)

print("""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>DELETED</title>
</head>
<body>
    <h1>Image successfully deleted.</h1>
	<a href="../"> go back </a>
</body>
</html>""")