# GPA-Mesh

## Extraction models append uv from Intel GPA

This is a very tiny cmd tool

The model extracted from GPA does not come with UV information, we can extract UV information from VBV(Vertex buffer vector) csv file to insert into the .obj model.

How to use:

## 1. Convert a single model

To insert UV data to a single model and export it, need to enter the model path + UV Buffer file path + the column name that represents UV in the UV Buffer + path to the exported model

command : -s [geometry.obj path] [vbv.csv path] [texcoord column name] [output path]

- geometry.obj path
    
    It should be the obj model exported from GPA

- vbv.csv path

    It should be the VBV file (.csv) exported from GPA, usually there will be more than one VBV in one DrawCall of GPA, please pay attention to which VBV file the UV information exists, we need to use the VBV with UV information.

- texcoord column name

    Usually there is no specific name for UV in VBV, please select a column of data in VBV as UV information, usually this column will be called TEXCOORD / TEXCOORD0 / TEXCOORD* / float2, the value of this column is characterized by only x and y values, and are between 0 and 1, cut are floating point numbers, need to screen the name of this UV column yourself What is then passed in, the name can be seen in the first line of the data displayed in GPA by clicking VBV, the general layout format is index columnName1 columnName2 ...

- output path

    Path to export model

Example (in cmd):
```
gmesh -s D:/my_mesh.obj D:/my_buffer.csv TEXCOORD D:/my_new_mesh.obj
```
## 2. Convert multiple models

To batch insert UV datas to models and export, need to enter the path to store all of the model and UVBuffer files folder path + the column name that represents UV in the UV Buffer, note: the model file and UV Buffer file name must be the same, e.g. head.obj & head.csv

command : -m [geometry and vbv files folder] [texcoord column name]

- geometry and vbv files folder
    
    store all of the model and UVBuffer files folder path. note: the model file and UV Buffer file name must be the same, e.g. head.obj & head.csv

- texcoord column name

    Usually there is no specific name for UV in VBV, please select a column of data in VBV as UV information, usually this column will be called TEXCOORD / TEXCOORD0 / TEXCOORD* / float2, the value of this column is characterized by only x and y values, and are between 0 and 1, cut are floating point numbers, need to screen the name of this UV column yourself What is then passed in, the name can be seen in the first line of the data displayed in GPA by clicking VBV, the general layout format is index columnName1 columnName2 ...

Example (in cmd):
```
gmesh -m D:/my_gpa_files TEXCOORD
```

## Limitation
- Currently, only models with a StartIndexLocation of 0 in the GPA DrawCall message Execution Arguments are supported, and support for StartIndexLocation will be added later.

- As the Obj file can only store one layer of UV, if need UV2 information (such as LightmapUV), can export two obj models with UV1 information and UV2 information respectively, and use the copy uv function in the free software 'blender' to merge uv1 and uv2 together

## Others
- If need to batch convert .dds image to .png can check this library
    https://github.com/PMZeroSkyline/dds_batch_to_png