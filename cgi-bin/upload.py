import cgi, os

path = os.environ.get('UPLOAD_DIR', '')
if not path:
    print("""\
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta http-equiv="X-UA-Compatible" content="IE=edge">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>DELETED</title>
        </head>
        <body>
            <h1>Error path not found.</h1>
            <a href="../"> go back </a>
        </body>
        </html>
    """)

form = cgi.FieldStorage()
if 'filename' in form:
    fi = form['filename']
else:
    fi = None  # or any other appropriate action if the key is not present.


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
        <a href="/gallery">go to gallery</a>
    </body>
    </html>
	""".format(path))
else:
    print("""\
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Upload failed</title>
    </head>
    <body>
        <h1>Upload failed.</h1>
    </body>
    </html>
    """)