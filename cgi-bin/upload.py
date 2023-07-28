import cgi, os

path = os.environ['UPLOAD_DIR']
form = cgi.FieldStorage()
fi = form['filename']

# for field in form.keys():
#     if form[field].file:
#         print(f"{field} (filename): {form[field].filename}")
#         # Access the file data using form[field].file.read()
#     else:
#         print(f"{field}: {form[field].value}")

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