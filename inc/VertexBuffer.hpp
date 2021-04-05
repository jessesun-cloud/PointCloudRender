#pragma once

#include "gl/gl.h"
#include "gl/glu.h"
#include "iostream"
#define VERTEX_API 

class VERTEX_API ImpPointRenderInterface
{
public:
    virtual void DrawPoints(int start, int npt, bool bVariableColor) = 0;
    virtual void AddPoints(int npt, float* pPointWidth, 
      unsigned char *pRgbaColor, float *pPoints, float *pNormals) = 0;
};

class VERTEX_API VertexBufferRenderBase : public ImpPointRenderInterface
{
protected:
  float* mpWidthBuffer;
  int mPointCount;
  bool mbInit;

public:
    static void Init();
    static bool IsVboAvailable();
    static VertexBufferRenderBase* Create(bool bUseVbo = true);

    virtual ~VertexBufferRenderBase() { }
    VertexBufferRenderBase();
    
    int GetPointCount() { return mPointCount; }
    virtual void AddPoints(int npt, float* pPointWidth,
      unsigned char *pRgbaColor, float *pPoints, float *pNormals) = 0;

    void DrawPoints(int start, int npt,
                 bool bVariableThickness, bool bVariableColor);

    virtual void DrawPoints(int start, int npt, bool bVariableColor = true) = 0;
    virtual void AddPoints(int npt, float* pPointWidth,
      unsigned char *pRgbaColor, double *pPoints, float *pNormals) {};
};

class VERTEX_API VertexBufferRender : public  VertexBufferRenderBase
{
  float* mpPointBuffer;
  unsigned char* mpColorBuffer;
  float *mpNormalBuffer;

public:
  VertexBufferRender()
  {
    mpPointBuffer = NULL;
    mpColorBuffer = NULL;
    mpNormalBuffer = NULL;
  }
  virtual void DrawPoints(int start, int npt, bool bVariableColor = true);
  virtual void AddPoints(int npt, float* pPointWidth,
               unsigned char *pRgbaColor, float *pPoints, float *pNormals);
};

class VERTEX_API VertexBufferVboRender : public  VertexBufferRenderBase
{
  GLuint mVboId[3];
  bool mbHasColor;
  bool mbHasNormal;
  bool mbDouble;
public:
  VertexBufferVboRender();
  virtual void AddPoints(int npt, float* pPointWidth,
               unsigned char *pRgbaColor, float *pPoints, float *pNormals);
  virtual void DrawPoints(int start, int npt, bool bVariableColor = true);
  virtual ~VertexBufferVboRender();
  virtual void AddPoints(int npt, float* pPointWidth,
    unsigned char *pRgbaColor, double *pPoints, float *pNormals);
  virtual void AddRgbPoints(int npt, float* pPointWidth,
    unsigned char *pRgbaColor, float *pPoints, float *pNormals);
};
