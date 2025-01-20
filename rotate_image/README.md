# Rotate Image

## Description
Rotate an image with buttons

## Preperation
```
pip install -r requirements.txt 
```

## Use stl file
> Should have a frontend UI in the future(WIP)
**CURRENT:**<br>
Run:
```
python stl_generate_array.py
```
Then, input the stl file name and run
```
make
```
Then, flash the new hex to the Inspire3D-RV

## Control
### When not using stl file(i.e. random image)
ARROW_KEY: control rotation<br>
A: regenerate image<br>
B: reset image state
> Note: remeber to comment `#define USE_STL_FILE`
### When using stl file
ARROW_KEY: control rotation<br>
B: reset image state
> Note: remeber to uncomment `#define USE_STL_FILE`
