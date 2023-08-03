import cgi, os

form = cgi.FieldStorage()
path = os.environ["UPLOAD_DIR"]
if path == "":
    print("""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Error</title>
    </head>
    <body>
        <h1>Error: Path not found.</h1>
    </body>
    </html>
    """)

if "file" in form:
    if os.path.exists("./www/images/" + form['file'].value):
        os.remove("./www/images/" + form['file'].value)
        print("""
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Delete</title>
        </head>
        <body>
            <h1>File deleted.</h1>
            <a href="/"> go back </a>
        </body>
        </html>
        """)
    else:
        print("""
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Error</title>
        </head>
        <body>
            <h1>An error has occured.</h1>
        </body>
        </html>
        """)
else:
    print("""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Error</title>
    </head>
    <body>
        <h1>An error has occured.</h1>
    </body>
    </html>
    """)
