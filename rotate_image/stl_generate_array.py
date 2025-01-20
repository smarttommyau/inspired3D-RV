import numpy as np
# np.float_ = np.float64 # to fix the error: AttributeError: module 'numpy' has no attribute 'float_'
from stl import mesh
import sys
import raster_geometry as rg

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
    print("Usage: python stl_generate_array.py <filename> <dimension>")
    sys.exit(1)
filename = sys.argv[1]
print(f"Filename: {filename}")

# get second argument as dimension if it exists NxMxO
# otherwise use 5x5x5
if len(sys.argv) > 2:
    dimension = sys.argv[2].split('x')
    if len(dimension) != 3:
        print("Invalid dimension format")
        sys.exit(1)
    dimension = [int(x) for x in dimension]
else:
    dimension = [5, 5, 5]
print(f"Dimension: {dimension}")
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

def full_triangle(a, b, c):
    ab = rg.bresenham_line(a, b, endpoint=True)
    for x in set(ab):
        yield from rg.bresenham_line(c, x, endpoint=True)

coords_count = 0
for vector in mesh_data.vectors:
    a,b,c = vector
    a = get_grid_coords_from_mesh_coords(a[0], a[1], a[2])
    a = (a[0]*10, a[1]*10, a[2]*10)
    a = (my_round(a[0]), my_round(a[1]), my_round(a[2]))
    b = get_grid_coords_from_mesh_coords(b[0], b[1], b[2])
    b = (b[0]*10, b[1]*10, b[2]*10)
    b = (my_round(b[0]), my_round(b[1]), my_round(b[2]))
    c = get_grid_coords_from_mesh_coords(c[0], c[1], c[2])
    c = (c[0]*10, c[1]*10, c[2]*10)
    c = (my_round(c[0]), my_round(c[1]), my_round(c[2]))
    coords = set(full_triangle(a, b, c))
    coords_count += len(coords)
    for x, y, z in coords:
        x= float(x)/10 
        y= float(y)/10
        z= float(z)/10
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