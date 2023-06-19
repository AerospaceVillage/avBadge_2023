"""
Super simple python script to convert the index.html file into a const char array
"""


f = open("index.html", "r").readlines()

new_index = open("index_html.h", "w")

f[0] = f[0].strip()
new_index.write("const char *index_html_code =  \"" + f[0])

for line in f[1::]:
    line = line.replace("\"", "'").strip()
    new_index.write(line)

new_index.write("\";")
