# GPA-Mesh

## Attaching UV to models extracted from GPA

This is a very tiny cmd tool. extract UV information from VBV(Vertex buffer vector) csv file to insert into the .obj model.

Use:

## 1. Extract a single model
```
gmesh -s TEXCOORD1 D:/my_buffer.csv D:/my_mesh.obj D:/out_mesh.obj
```
command :-s [texcoord column name] [vbv.csv path] [mesh.obj path] [output path]
- mesh.obj path (It should be the mesh file (.obj) exported from GPA)
- vbv.csv path (It should be the vertex buffer vector(VBV) file '.csv' exported from GPA)
- texcoord column name (specific name for UV in VBV)
## 2. Extract multiple models
```
gmesh -m TEXCOORD1 D:/mesh_and_vbv
```
command : -m [texcoord column name] [mesh and vbv files folder]
- mesh.obj and vbv.csv files folder (the mesh file and vbv file name must be the same, e.g. ground.obj & ground.csv)

## Limitation
- Currently, only models with a StartIndexLocation of 0 in the GPA DrawCall message Execution Arguments are supported, and support for StartIndexLocation will be added later.

- As the Obj file can only store one layer of UV, if need UV2 information (such as LightmapUV), can export two obj models with UV1 information and UV2 information respectively, and use the copy uv function in the free software 'blender' to merge uv1 and uv2 together

## Others
- If need to batch convert .dds image to .png can check this library
    https://github.com/PMZeroSkyline/dds_batch_to_png