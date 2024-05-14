import matplotlib.pyplot as plt

file_path = 'pointsX.txt'
try:
    with open(file_path, 'r') as file:

        pointsX = eval(file.read())
except FileNotFoundError:
    print("The file '{file_path}' does not exist")
except IOError as e:
    print("An error occurred while reading the file: {e}")

# file_path = 'pointsY.txt'
# try:
#     with open(file_path, 'r') as file:
#
#         pointsY = eval(file.read())
# except FileNotFoundError:
#     print("The file '{file_path}' does not exist")
# except IOError as e:
#     print("An error occurred while reading the file: {e}")

file_path = 'fileN.txt'
try:
    with open(file_path, 'r') as file:

        filename = file.read()
except FileNotFoundError:
    print("The file '{file_path}' does not exist")
except IOError as e:
    print("An error occurred while reading the file: {e}")

# plt.xscale('log')
plt.plot(pointsX)
plt.title(filename)
plt.grid(True)
plt.savefig(".//images//"+filename+".png")
# plt.show()
