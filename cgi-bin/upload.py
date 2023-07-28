import cgi, os

path = os.environ['UPLOAD_DIR']
form = cgi.FieldStorage()
fi = form['filename']

if fi.filename:
	if not os.path.exists(path):
		os.makedirs(path)
	fn = os.path.basename(fi.filename)
	open(path + fn, 'wb').write(fi.file.read())
	print("""\
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Upload success</title>
    </head>
    <body>
        <h1>Your file has been successfully uploaded to {}</h1>
        <a href="../">go back</a>
    </body>
    </html>
	""".format(path))