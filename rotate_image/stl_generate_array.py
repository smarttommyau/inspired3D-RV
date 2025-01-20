import numpy
from stl import mesh
import sys
from mpl_toolkits import mplot3d
from matplotlib import pyplot
# get first argument as dimension if it exists NxMxO
# otherwise use 5x5x5
if len(sys.argv) > 1:
    dimension = sys.argv[1].split('x')
    if len(dimension) != 3:
        print("Invalid dimension format")
        sys.exit(1)
    dimension = [int(x) for x in dimension]
else:
    dimension = [5, 5, 5]

filename = input("Enter the filename of the stl file: ")
# filename = "basic_sphere.stl"
mesh_data = mesh.Mesh.from_file(filename)
max_x = numpy.max(mesh_data.x)
min_x = numpy.min(mesh_data.x)
max_y = numpy.max(mesh_data.y)
min_y = numpy.min(mesh_data.y)
max_z = numpy.max(mesh_data.z)
min_z = numpy.min(mesh_data.z)
print(f"max_x: {max_x}, min_x: {min_x}, max_y: {max_y}, min_y: {min_y}, max_z: {max_z}, min_z: {min_z}")

# map mesh coords to dimension x dimension x dimension grid
def get_grid_coords_from_mesh_coords(x, y, z):
    grid_x = (x - min_x) * (dimension[0] - 1) / (max_x - min_x)
    grid_y = (y - min_y) * (dimension[1] - 1) / (max_y - min_y)
    grid_z = (z - min_z) * (dimension[2] - 1) / (max_z - min_z)
    return grid_x, grid_y, grid_z


# convert and truncate mesh into a 5x5x5 grid
# and generate stl_data.h file

# open stl_data_template.h
data = ""
with open('stl_data_template.h', 'r') as file:
    data = file.read()

# By this format
# each points
# {.x = 0, .y = 0, .z = 0},
# and end with };
# set NUM_NODES by replacing TO_BE_REPLACE_WITH_NUM_NODES to the number of nodes
cache = {} # skip duplicate points
nodes_count = 0
for vector in mesh_data.vectors:
    # just write the 3 points in the triangle to the file
    for i in range(3):
        x, y, z = get_grid_coords_from_mesh_coords(vector[i][0], vector[i][1], vector[i][2])
        # 1dp
        x = round(x, 1)
        y = round(y, 1)
        z = round(z, 1)
        # use int(.) so that only one point is save for each render point
        if(f"{int(x)},{int(y)},{int(z)}" in cache):
            continue
        cache[f"{int(x)},{int(y)},{int(z)}"] = True
        nodes_count += 1
        data += f"{{.x = {x:.1f}, .y = {y:.1f}, .z = {z:.1f}}},\n"
data += "};\n"
data = data.replace("TO_BE_REPLACE_WITH_NUM_NODES", str(nodes_count))
print(f"Number of nodes: {nodes_count}")
with open('stl_data.h', 'w') as file:
    file.write(data)