"""
Super simple python script to convert the index.html file into a const char array
"""


def convert_html(input_file, output_file, array_name):
    f = open(input_file, "r").readlines()

    new_index = open(output_file, "w")

    f[0] = f[0].strip()
    new_index.write(f"const char *{array_name} =  \"" + f[0])

    for line in f[1::]:
        line = line.replace("\"", "'").strip()
        new_index.write(line)

    new_index.write("\";")


if __name__ == '__main__':

    convert_html("index.html", "index_html.h", "index_html_code")
    convert_html("blueprint.html", "blueprint_html.h", "blueprint_html_code")
