import numpy as np
# np.float_ = np.float64 # to fix the error: AttributeError: module 'numpy' has no attribute 'float_'
from stl import mesh
import sys

# Align rounding function with the c program
# From `rotate_image.c`:
# int my_round(float x)
# {
#     return (int)(x + 0.5);
# }

def my_round(x:float)->int:
    return int(x+0.5)

# get first argument as filename
if len(sys.argv) < 2:
    print("Please provide the filename of the stl file")
    print("Usage: python stl_generate_array.py <filename> <arguments>")
    print("--help: for more information")
    sys.exit(1)
if sys.argv[1] == "--help":
    print("Usage: python stl_generate_array.py <filename> <arguments>")
    print("--demension NxMxO")
    print("N: x dimension")
    print("M: y dimension")
    print("O: z dimension")
    print("default dimension is 5x5x5")
    print("--margin x")
    print("x: margin between object and the grid")
    print("default scale margin is 0")
    sys.exit(0)

arg_mode = ""
filename = ""
dimension = [5, 5, 5]
margin = 0
for arg in sys.argv:
    if arg == "--dimension":
        arg_mode = "dimension"
        continue
    if arg == "--margin":
        arg_mode = "margin"
        continue
    if arg_mode == "dimension":
        dimension = arg.split('x')
        if len(dimension) != 3:
            print("Invalid dimension format")
            sys.exit(1)
        dimension = [int(x) for x in dimension]
        arg_mode = ""
        continue
    if arg_mode == "margin":
        margin = float(arg)
        arg_mode = ""
        continue
    if arg_mode == "":
        # if not in any mode, then it is the filename
        filename = arg
print(f"Filename: {filename}")
print(f"Dimension: {dimension}")
print(f"Margin: {margin}")
# filename = input("Enter the filename of the stl file: ")
# filename = "basic_sphere.stl"
mesh_data = mesh.Mesh.from_file(filename)
max_x = np.max(mesh_data.x)
min_x = np.min(mesh_data.x)
max_y = np.max(mesh_data.y)
min_y = np.min(mesh_data.y)
max_z = np.max(mesh_data.z)
min_z = np.min(mesh_data.z)
print(f"max_x: {max_x}, min_x: {min_x}, max_y: {max_y}, min_y: {min_y}, max_z: {max_z}, min_z: {min_z}")

# map mesh coords to dimension x dimension x dimension grid
def get_grid_coords_from_mesh_coords(x, y, z):
    # scale to 0 to dimension and also add margin
    # fit xyz scale with same ratio
    rx = (max_x - min_x) / (dimension[0] - 1 - margin)
    ry = (max_y - min_y) / (dimension[1] - 1 - margin)
    rz = (max_z - min_z) / (dimension[2] - 1 - margin)
    max_r = max(rx, ry, rz)
    nx,ny,nz = dimension
    if max_r == rx:
        ny = int((max_y - min_y) / rx + 1)
        nz = int((max_z - min_z) / rx + 1)
    elif max_r == ry:
        nx = int((max_x - min_x) / ry + 1)
        nz = int((max_z - min_z) / ry + 1)
    else:
        nx = int((max_x - min_x) / rz + 1)
        ny = int((max_y - min_y) / rz + 1)
    # scale to 0 to dimension
    grid_x = (x - min_x) / (max_x - min_x) * (nx - 1)
    grid_y = (y - min_y) / (max_y - min_y) * (ny - 1)
    grid_z = (z - min_z) / (max_z - min_z) * (nz - 1)
    # add margin
    grid_x += margin
    grid_y += margin
    grid_z += margin

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

# customize from raster_geometry for float
def bresenham_line(coord_a, coord_b,endpoint=False):
    n_dim = len(coord_a)
    diffs = [abs(b - a) for a, b in zip(coord_a, coord_b)]
    steps = [1 if a < b else -1 for a, b in zip(coord_a, coord_b)]
    max_diff = max(diffs)
    updates = [max_diff / 2] * n_dim
    coord = list(coord_a)
    for i in range(int(max_diff)):
        for j, (x, d, s, u) in enumerate(
                zip(coord, diffs, steps, updates)):
            updates[j] -= d
            if u < 0:
                coord[j] += s
                updates[j] += max_diff
        yield tuple(coord)
    if endpoint:
        yield tuple(coord_b)
    

def full_triangle(a, b, c):
    ab = bresenham_line(a, b, endpoint=True)
    for x in set(ab):
        yield from bresenham_line(c, x, endpoint=True)

coords_count = 0
precision = 1
for vector in mesh_data.vectors:
    a,b,c = vector
    a = get_grid_coords_from_mesh_coords(a[0], a[1], a[2])
    a = (a[0]*precision, a[1]*precision, a[2]*precision )
    b = get_grid_coords_from_mesh_coords(b[0], b[1], b[2])
    b = (b[0]*precision, b[1]*precision, b[2]*precision )
    c = get_grid_coords_from_mesh_coords(c[0], c[1], c[2])
    c = (c[0]*precision, c[1]*precision, c[2]*precision)
    coords = set(full_triangle(a, b, c))
    coords_count += len(coords)
    for x, y, z in coords:
        x= float(x)/precision
        y= float(y)/precision
        z= float(z)/precision
        query = f"{my_round(x)},{my_round(y)},{my_round(z)}"
        if(query in cache):
            cache[query][0] += x
            cache[query][1] += y
            cache[query][2] += z
            cache[query][3] += 1
            continue
        cache[query] = [x, y, z, 1]
        nodes_count += 1
for key, value in cache.items():
    x = value[0]/value[3]
    y = value[1]/value[3]
    z = value[2]/value[3]
    data += f"{{.x = {x:.1f}, .y = {y:.1f}, .z = {z:.1f}}},\n"
    # old algorithm
    # just write the 3 points in the triangle to the file
    # for i in range(3):
    #     x, y, z = get_grid_coords_from_mesh_coords(vector[i][0], vector[i][1], vector[i][2])
    #     # 1dp
    #     x = round(x, 1)
    #     y = round(y, 1)
    #     z = round(z, 1)
    #     # use int(.) so that only one point is save for each render point
    #     if(f"{my_round(x)},{my_round(y)},{my_round(z)}" in cache):
    #         continue
    #     cache[f"{my_round(x)},{my_round(y)},{my_round(z)}"] = True
    #     nodes_count += 1
    #     data += f"{{.x = {x:.1f}, .y = {y:.1f}, .z = {z:.1f}}},\n"
data += "};\n"
data = data.replace("TO_BE_REPLACE_WITH_NUM_NODES", str(nodes_count))
print(f"Number of vectors: {len(mesh_data.vectors)}")
print(f"Number of coords: {coords_count}")
print(f"Number of nodes: {nodes_count}")
# calculate Center x,y,z
data = data.replace("TO_BE_REPLACE_WITH_CENTER_X",str((dimension[0] - 1)/2))
data = data.replace("TO_BE_REPLACE_WITH_CENTER_Y",str((dimension[1] - 1)/2))
data = data.replace("TO_BE_REPLACE_WITH_CENTER_Z",str((dimension[2] - 1)/2))


with open('stl_data.h', 'w') as file:
    file.write(data)