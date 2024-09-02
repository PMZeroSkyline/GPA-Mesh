# GPA-Mesh

## Append UV to mesh

This is a very tiny cli(Command-line interface). parse UV information from VBV(GPA Vertex buffer view) csv file to insert into the .obj model.

Use:

## 1. Append uv into a single model
```
gmesh -s TEXCOORD1 D:/gpa_exported_vertex_buffer_view.csv D:/gpa_exported_geometry.obj D:/result_mesh.obj
```
command :-s [texcoord column name] [gpa_exported_vertex_buffer_view path] [gpa_exported_geometry path] [result mesh path]
- texcoord column name (specific name for UV in VBV)
- gpa_exported_vertex_buffer_view.csv path (It should be the vertex buffer view(VBV) file '.csv' exported from GPA)
- gpa_exported_geometry path (It should be the mesh file (.obj) exported from GPA)
## 2. Append uv to multiple models
```
gmesh -m TEXCOORD1 D:/gpa_exported_files
```
command : -m [texcoord column name] [mesh and vbv files folder]
- gpa_exported_geometry.obj and gpa_exported_vertex_buffer_view.csv files folder (the geometry file and vertex_buffer_view file name must be the same, e.g. ground.obj & ground.csv)

## Limitation
- Currently, only models with a StartIndexLocation of 0 in the GPA DrawCall message Execution Arguments are supported, and support for StartIndexLocation will be added later.

- As the Obj file can only store one layer of UV, if need UV2 information (such as LightmapUV), can export two obj models with UV1 information and UV2 information respectively, and use the copy uv function in the free software 'blender' to merge uv1 and uv2 together

## Others
- If need to batch convert .dds image to .png can check this library
    https://github.com/PMZeroSkyline/dds_batch_to_png
