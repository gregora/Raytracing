file = input("File name: ")

text = open(file).read()
text = text.split("\n")

text_out = ""

for line in text:
    if(line != ""):
        if(line[0] == "f"):
            string = "tr "

            line_split = line[2:].split(" ")
            line_split = [int(numeric_string) for numeric_string in line_split]

            for v in line_split:
                vertex = text[v - 1][2:].split(" ")

                if(len(vertex) == 3):
                    vertex = [float(numeric_string) for numeric_string in vertex]
                else:
                    vertex = [0, 0, 0]


                for i in vertex:
                    i = i*10
                    string += str(i) + " "


            string += "1 1 1 0\n"
            text_out += string

open(file.replace(".obj", ".scene"), "w").write(text_out)
