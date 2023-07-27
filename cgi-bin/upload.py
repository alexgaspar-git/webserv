#!/usr/bin/env python3

import cgi
import os

UPLOAD_DIR = '/Users/algaspar/Desktop/'  # Change this to your desired upload directory

def save_uploaded_file(fileitem, upload_dir):
    if not os.path.exists(upload_dir):
        os.makedirs(upload_dir)

    filename = os.path.join(upload_dir, os.path.basename(fileitem.filename))
    with open(filename, 'wb') as f:
        while True:
            chunk = fileitem.file.read(8192)
            if not chunk:
                break
            f.write(chunk)
    return filename

print("Content-type: text/html")
print()

print("<html><body>")
print("<h2>File Upload</h2>")

form = cgi.FieldStorage()

if 'file' in form:
    fileitem = form['file']
    if fileitem.filename:
        try:
            uploaded_file = save_uploaded_file(fileitem, UPLOAD_DIR)
            print(f"<p>File '{fileitem.filename}' uploaded successfully to '{uploaded_file}'</p>")
        except Exception as e:
            print(f"<p>Error uploading file: {e}</p>")
    else:
        print("<p>No file selected.</p>")
else:
    print("""
        <form enctype="multipart/form-data" method="post">
        <input type="file" name="file" />
        <input type="submit" value="Upload" />
        </form>
    """)

print("</body></html>")
