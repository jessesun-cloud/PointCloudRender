#include "windows.h"
#include "VertexBuffer.hpp"
#include "glext.h"
#include <vector>

#ifdef _WIN32
PFNGLGENBUFFERSARBPROC            pglGenBuffersARB = 0;             // VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC            pglBindBufferARB = 0;             // VBO Bind Procedure
PFNGLBUFFERDATAARBPROC            pglBufferDataARB = 0;             // VBO Data Loading Procedure
PFNGLBUFFERSUBDATAARBPROC         pglBufferSubDataARB = 0;          // VBO Sub Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC         pglDeleteBuffersARB = 0;          // VBO Deletion Procedure
PFNGLGETBUFFERPARAMETERIVARBPROC  pglGetBufferParameterivARB = 0;   // return various parameters of VBO
PFNGLMAPBUFFERARBPROC             pglMapBufferARB = 0;              // map VBO procedure
PFNGLUNMAPBUFFERARBPROC           pglUnmapBufferARB = 0;            // unmap VBO procedure

#define glGenBuffersARB           pglGenBuffersARB
#define glBindBufferARB           pglBindBufferARB
#define glBufferDataARB           pglBufferDataARB
#define glBufferSubDataARB        pglBufferSubDataARB
#define glDeleteBuffersARB        pglDeleteBuffersARB
#define glGetBufferParameterivARB pglGetBufferParameterivARB
#define glMapBufferARB            pglMapBufferARB
#define glUnmapBufferARB          pglUnmapBufferARB
#endif

static bool gglVBOInitialized = false;
static bool gglVBOAvailable = false;

bool 
VertexBufferRenderBase::IsVboAvailable()
{
  return gglVBOAvailable;
}

void 
VertexBufferRenderBase::Init()
{
  if (!gglVBOInitialized)
  {
    const GLubyte *str = glGetString(GL_EXTENSIONS);
    if (strstr((char*) str, "GL_ARB_vertex_buffer_object"))
    {
      gglVBOInitialized = true;
      // get pointers to GL functions
      glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
      glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
      glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
      glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
      glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
      glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
      glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
      glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");

      // check once again VBO extension
      if(glGenBuffersARB && glBindBufferARB && glBufferDataARB && glBufferSubDataARB &&
        glMapBufferARB && glUnmapBufferARB && glDeleteBuffersARB && glGetBufferParameterivARB)
      {
        gglVBOAvailable = true;
        std::cout << "Video card supports GL_ARB_vertex_buffer_object." << std::endl;
      }
      else
      {
        gglVBOAvailable = false;
        std::cout << "Video card does NOT support GL_ARB_vertex_buffer_object." << std::endl;
      }
    }
  }
}

VertexBufferRenderBase::VertexBufferRenderBase()
{
  Init();
  mPointCount = 0;
  mbInit = false;
  mpWidthBuffer = NULL;
}

void 
VertexBufferRenderBase::DrawPoints(int start, int npt,
                            bool bVariableThickness, bool bVariableColor)
{
  if (npt == 0)
    npt = mPointCount;
  if (bVariableThickness)
  {
    int i;
    int lastIndex = start;
    float *pPointWidth =  mpWidthBuffer + start;
    float lastPointWidth = *pPointWidth;
    glPointSize(lastPointWidth);
    for (i = start; i < start + npt; i++, pPointWidth++)
    {
      if (bVariableThickness && *pPointWidth != lastPointWidth)
      { // check for change in point width
        DrawPoints(lastIndex, i - lastIndex, bVariableColor);
        lastPointWidth = *pPointWidth;
        lastIndex = i;
        glPointSize(lastPointWidth);
      }
    }
    if (i - lastIndex > 0)
    {
      DrawPoints(lastIndex, i - lastIndex, bVariableColor);
    }
  }
  else
  {
    DrawPoints(start, npt, bVariableColor);
  }
}

VertexBufferRenderBase* 
VertexBufferRenderBase::Create(bool bUseVbo)
{
  Init();
  if (bUseVbo && IsVboAvailable())
  {
    return new VertexBufferVboRender();
  }
  else
  {
    return new VertexBufferRender();
  }
}

/////////////////////////////////////////////////
void 
VertexBufferRender::AddPoints(int npt, float* pPointWidth,
                                      unsigned char *pRgbaColor, float *pPoints, float *pNormals)
{
  mbInit = true;

  mPointCount = npt;
  mpWidthBuffer = pPointWidth;
  mpPointBuffer = pPoints;
  mpColorBuffer = pRgbaColor;
  mpNormalBuffer = pNormals;
}

class GlAutoSaveClientState
{
public:
  bool mbEnableVertexState;
  bool mbEnableColorState;
  bool mbEnableNormalState;
  GlAutoSaveClientState()
  {
    mbEnableVertexState = (bool)glIsEnabled(GL_VERTEX_ARRAY);
    mbEnableColorState = (bool)glIsEnabled(GL_COLOR_ARRAY);
    mbEnableNormalState = (bool)glIsEnabled(GL_NORMAL_ARRAY);
  }
  ~GlAutoSaveClientState()
  {
    if (mbEnableNormalState == false)
      glDisableClientState(GL_NORMAL_ARRAY);
    if (mbEnableColorState == false)
      glDisableClientState(GL_COLOR_ARRAY);
    if (mbEnableVertexState == false)
      glDisableClientState(GL_VERTEX_ARRAY);
  }
};

void 
VertexBufferRender::DrawPoints(int start, int npt, bool bVariableColor)
{
  try
  {
    if (npt > 0 && start + npt <= mPointCount)
    {
      GlAutoSaveClientState autoSaveState;
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, mpPointBuffer);

      if (mpColorBuffer != NULL && bVariableColor)
      {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, mpColorBuffer);
      }
      if (mpNormalBuffer != NULL) 
      {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, mpNormalBuffer);
      }
      glDrawArrays(GL_POINTS, start, npt);
    }
  }
  catch(...)
  {
    printf("...Error...");
  }
}

VertexBufferVboRender::VertexBufferVboRender()
{
  mbDouble = 0;
  mbHasColor = false;
  mbHasNormal = false;
  mVboId[0] = 0;
  mVboId[1] = mVboId[2] = 0;
  glGenBuffersARB(3, mVboId);
}

VertexBufferVboRender::~VertexBufferVboRender()
{
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  glDeleteBuffersARB(3, mVboId);
}

void 
VertexBufferVboRender::DrawPoints(int start, int npt, bool bVariableColor)
{
  if (npt == -1)
      npt = mPointCount;
  if (npt > 0 && start + npt <= mPointCount)
  {
    GlAutoSaveClientState autoSaveState;
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, mVboId[0]);
    if (mbDouble)
      glVertexPointer(3, GL_DOUBLE, 0, NULL);
    else
      glVertexPointer(3, GL_FLOAT, 0, NULL);     

    if (bVariableColor && mbHasColor)
    {
      glEnableClientState(GL_COLOR_ARRAY);
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, mVboId[1]);
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, NULL);
    }
    if (mbHasNormal) 
    {
      glEnableClientState(GL_NORMAL_ARRAY);
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, mVboId[2]);
      glNormalPointer(GL_FLOAT, 0, NULL);
    }
    glDrawArrays(GL_POINTS, start, npt);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }
}

void 
VertexBufferVboRender::AddPoints(int npt, float* pPointWidth,
     unsigned char *pRgbaColor, float *pPoints, float *pNormals)
{
  mbInit = true;
  mbDouble = false;
  mPointCount = npt;
  mpWidthBuffer = pPointWidth;

  if (pPoints != NULL)
  {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, mVboId[0]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float) * 3 * mPointCount,
      pPoints, GL_STATIC_DRAW_ARB);
  }

  mbHasColor = false;
  if (pRgbaColor != NULL)
  {
    mbHasColor = true;
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, mVboId[1]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, 4 * mPointCount,
      pRgbaColor, GL_STATIC_DRAW_ARB);
  }

  mbHasNormal = false;
  if (pNormals != NULL)
  {
    mbHasNormal = true;
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, mVboId[2]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float) * 3 * mPointCount, 
      pNormals, GL_STATIC_DRAW_ARB);
  }
}


void
VertexBufferVboRender::AddPoints(int npt, float* pPointWidth,
unsigned char *pRgbaColor, double *pPoints, float *pNormals)
{
  mbInit = true;
  mbDouble = true;
  mPointCount = npt;
  mpWidthBuffer = pPointWidth;

  if (pPoints != NULL)
  {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, mVboId[0]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(double) * 3 * mPointCount,
      pPoints, GL_STATIC_DRAW_ARB);
  }

  mbHasColor = false;
  if (pRgbaColor != NULL)
  {
    mbHasColor = true;
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, mVboId[1]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, 4 * mPointCount,
      pRgbaColor, GL_STATIC_DRAW_ARB);
  }

  mbHasNormal = false;
  if (pNormals != NULL)
  {
    mbHasNormal = true;
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, mVboId[2]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float) * 3 * mPointCount,
      pNormals, GL_STATIC_DRAW_ARB);
  }
}

void VertexBufferVboRender::AddRgbPoints(int npt, float* pPointWidth,
  unsigned char *pRgbColor, float *pPoints, float *pNormals)
{
   std::vector<unsigned char> colors;
   if (pRgbColor)
   {
     colors.resize(npt * 4, 0);
     for (int i = 0; i <npt; i++)
     {
       colors[i * 4] = pRgbColor[i * 3];
       colors[i * 4 + 1] = pRgbColor[i * 3 + 1];
       colors[i * 4 + 2] = pRgbColor[i * 3 + 2];
     }
   }
   std::vector <double> points;
   points.resize(npt * 3, 0);
   for (int i = 0; i < npt; i++)
   {
     points[i * 3] = pPoints[i * 3];
     points[i * 3 + 1] = pPoints[i * 3 + 1];
     points[i * 3 + 2] = pPoints[i * 3 + 2];
   }
   return AddPoints(npt, pPointWidth, colors.data(), points.data(), pNormals);
}
