"""
Super simple python script to convert the index.html file into a const char array
"""


def convert_html(input_file, output_file, array_name):
    f = open(input_file, "r").readlines()

    new_index = open(output_file, "w")

    f[0] = f[0].strip()
    new_index.write(f"const char *{array_name} =  \"" + f[0])

    for line in f[1::]:
        """
        Initially was using .strip() remove whitespace but that blows up the ascii art.
        So now have to insert the \ character BEFORE the newline for each line.
        More memory required... but preserves the ascii art in the html file
        """
        line = list(line.replace("\"", "'"))
        line.insert(-1, '\\\n\\')
        new_index.write("".join(line))

    # Have to add the newlone to avoid undefiend behavior without ending in newline
    new_index.write("\";\n")


if __name__ == '__main__':

    convert_html("index.html", "index_html.h", "index_html_code")
    convert_html("blueprint.html", "blueprint_html.h", "blueprint_html_code")
