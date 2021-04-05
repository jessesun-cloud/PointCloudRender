# VertexBuffer
A class to render point cloud use Vertex Buffer Object, and fall back to client array if VBO is not supported.

ABOUT
================================================================================
Use OpenGL VBO

usage:

VertexBufferVboRender vbo;

vbo.AddPoints(npt, ...);//upload points to GPU

vbo.DrawPoints(0, -1);//draw point cloud
