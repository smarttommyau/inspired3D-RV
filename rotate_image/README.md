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
python stl_generate_array.py <filename>
```
Use `--help` for more information<br>
Then, run below to generate new hex file
```
make
```
Then, flash the new hex to the Inspire3D-RV
### Note
Sample:
- basic_sphere.stl
- basic_triagle.stl
- basic_rectangle.stl
## Control
### When not using stl file(i.e. random image)
ARROW_KEY: control rotation<br>
A: regenerate image<br>
B: reset image state<br>
CD: change brightness
> Note: remeber to comment `#define USE_STL_FILE`
### When using stl file
ARROW_KEY: control rotation<br>
B: reset image state<br>
CD: change brightness
> Note: remeber to uncomment `#define USE_STL_FILE`

## Dimension other than 5x5x5
### When running stl_data generation
For dimension n(x) X m(y) X o(z), run
```
python stl_generate_array.py nxmxo
```

### TODO
- [ ] Fix: rotate image for pyramid
- [ ] Add: frontend UI for selecting stl file