file = input("File name: ")

text = open(file).read()
text = text.split("\n")

text_out = ""

vertices = []

for line in text:
    if(line != ""):
        if(line[0] == "f"):
            string = "tr "


            line_split = line[2:].split(" ")
            line_split = [int(numeric_string) for numeric_string in line_split]

            for l in line_split:
                string+= vertices[l - 1]

            string += "0.8 1 0.4 0.4\n"
            text_out += string
        elif (line[0] == "v"):
            line_split = line[2:].split(" ");

            string = ""
            string += str(float(line_split[2])*10) + " "
            string += str(float(line_split[0])*10) + " "
            string += str(float(line_split[1])*10) + " "

            vertices.append(string);

open(file.replace(".obj", ".scene"), "w").write(text_out)
