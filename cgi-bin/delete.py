import cgi, os

form = cgi.FieldStorage()

if "file" in form:
	if os.path.exists("./www/uploads/" + form['file'].value):
		os.remove("./www/uploads/" + form['file'].value)
		