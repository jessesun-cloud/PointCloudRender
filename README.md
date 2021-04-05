# VertexBuffer
A class to render point cloud use Vertex Buffer

ABOUT
================================================================================
Use OpenGL VBO


usage:
VertexBufferVboRender vbo;
vbo.AddPoints(npt, ...);//upload points to GPU

vbo.DrawPoints(0, -1,  false);//draw point cloud
