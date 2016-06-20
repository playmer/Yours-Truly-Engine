
#include "Binding_pch.h"

using namespace gl; // ToDo: multiple APIs?


namespace glbinding
{


Function<void> Binding::TagSampleBufferSGIX("glTagSampleBufferSGIX");
Function<void, GLbyte, GLbyte, GLbyte> Binding::Tangent3bEXT("glTangent3bEXT");
Function<void, const GLbyte *> Binding::Tangent3bvEXT("glTangent3bvEXT");
Function<void, GLdouble, GLdouble, GLdouble> Binding::Tangent3dEXT("glTangent3dEXT");
Function<void, const GLdouble *> Binding::Tangent3dvEXT("glTangent3dvEXT");
Function<void, GLfloat, GLfloat, GLfloat> Binding::Tangent3fEXT("glTangent3fEXT");
Function<void, const GLfloat *> Binding::Tangent3fvEXT("glTangent3fvEXT");
Function<void, GLint, GLint, GLint> Binding::Tangent3iEXT("glTangent3iEXT");
Function<void, const GLint *> Binding::Tangent3ivEXT("glTangent3ivEXT");
Function<void, GLshort, GLshort, GLshort> Binding::Tangent3sEXT("glTangent3sEXT");
Function<void, const GLshort *> Binding::Tangent3svEXT("glTangent3svEXT");
Function<void, GLenum, GLsizei, const void *> Binding::TangentPointerEXT("glTangentPointerEXT");
Function<void, GLuint> Binding::TbufferMask3DFX("glTbufferMask3DFX");
Function<void, GLfloat> Binding::TessellationFactorAMD("glTessellationFactorAMD");
Function<void, GLenum> Binding::TessellationModeAMD("glTessellationModeAMD");
Function<GLboolean, GLuint> Binding::TestFenceAPPLE("glTestFenceAPPLE");
Function<GLboolean, GLuint> Binding::TestFenceNV("glTestFenceNV");
Function<GLboolean, GLenum, GLuint> Binding::TestObjectAPPLE("glTestObjectAPPLE");
Function<void, GLenum, GLenum, GLuint> Binding::TexBuffer("glTexBuffer");
Function<void, GLenum, GLenum, GLuint> Binding::TexBufferARB("glTexBufferARB");
Function<void, GLenum, GLenum, GLuint> Binding::TexBufferEXT("glTexBufferEXT");
Function<void, GLenum, GLenum, GLuint, GLintptr, GLsizeiptr> Binding::TexBufferRange("glTexBufferRange");
Function<void, GLenum, const GLfloat *> Binding::TexBumpParameterfvATI("glTexBumpParameterfvATI");
Function<void, GLenum, const GLint *> Binding::TexBumpParameterivATI("glTexBumpParameterivATI");
Function<void, GLbyte> Binding::TexCoord1bOES("glTexCoord1bOES");
Function<void, const GLbyte *> Binding::TexCoord1bvOES("glTexCoord1bvOES");
Function<void, GLdouble> Binding::TexCoord1d("glTexCoord1d");
Function<void, const GLdouble *> Binding::TexCoord1dv("glTexCoord1dv");
Function<void, GLfloat> Binding::TexCoord1f("glTexCoord1f");
Function<void, const GLfloat *> Binding::TexCoord1fv("glTexCoord1fv");
Function<void, GLhalfNV> Binding::TexCoord1hNV("glTexCoord1hNV");
Function<void, const GLhalfNV *> Binding::TexCoord1hvNV("glTexCoord1hvNV");
Function<void, GLint> Binding::TexCoord1i("glTexCoord1i");
Function<void, const GLint *> Binding::TexCoord1iv("glTexCoord1iv");
Function<void, GLshort> Binding::TexCoord1s("glTexCoord1s");
Function<void, const GLshort *> Binding::TexCoord1sv("glTexCoord1sv");
Function<void, GLfixed> Binding::TexCoord1xOES("glTexCoord1xOES");
Function<void, const GLfixed *> Binding::TexCoord1xvOES("glTexCoord1xvOES");
Function<void, GLbyte, GLbyte> Binding::TexCoord2bOES("glTexCoord2bOES");
Function<void, const GLbyte *> Binding::TexCoord2bvOES("glTexCoord2bvOES");
Function<void, GLdouble, GLdouble> Binding::TexCoord2d("glTexCoord2d");
Function<void, const GLdouble *> Binding::TexCoord2dv("glTexCoord2dv");
Function<void, GLfloat, GLfloat> Binding::TexCoord2f("glTexCoord2f");
Function<void, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat> Binding::TexCoord2fColor3fVertex3fSUN("glTexCoord2fColor3fVertex3fSUN");
Function<void, const GLfloat *, const GLfloat *, const GLfloat *> Binding::TexCoord2fColor3fVertex3fvSUN("glTexCoord2fColor3fVertex3fvSUN");
Function<void, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat> Binding::TexCoord2fColor4fNormal3fVertex3fSUN("glTexCoord2fColor4fNormal3fVertex3fSUN");
Function<void, const GLfloat *, const GLfloat *, const GLfloat *, const GLfloat *> Binding::TexCoord2fColor4fNormal3fVertex3fvSUN("glTexCoord2fColor4fNormal3fVertex3fvSUN");
Function<void, GLfloat, GLfloat, GLubyte, GLubyte, GLubyte, GLubyte, GLfloat, GLfloat, GLfloat> Binding::TexCoord2fColor4ubVertex3fSUN("glTexCoord2fColor4ubVertex3fSUN");
Function<void, const GLfloat *, const GLubyte *, const GLfloat *> Binding::TexCoord2fColor4ubVertex3fvSUN("glTexCoord2fColor4ubVertex3fvSUN");
Function<void, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat> Binding::TexCoord2fNormal3fVertex3fSUN("glTexCoord2fNormal3fVertex3fSUN");
Function<void, const GLfloat *, const GLfloat *, const GLfloat *> Binding::TexCoord2fNormal3fVertex3fvSUN("glTexCoord2fNormal3fVertex3fvSUN");
Function<void, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat> Binding::TexCoord2fVertex3fSUN("glTexCoord2fVertex3fSUN");
Function<void, const GLfloat *, const GLfloat *> Binding::TexCoord2fVertex3fvSUN("glTexCoord2fVertex3fvSUN");
Function<void, const GLfloat *> Binding::TexCoord2fv("glTexCoord2fv");
Function<void, GLhalfNV, GLhalfNV> Binding::TexCoord2hNV("glTexCoord2hNV");
Function<void, const GLhalfNV *> Binding::TexCoord2hvNV("glTexCoord2hvNV");
Function<void, GLint, GLint> Binding::TexCoord2i("glTexCoord2i");
Function<void, const GLint *> Binding::TexCoord2iv("glTexCoord2iv");
Function<void, GLshort, GLshort> Binding::TexCoord2s("glTexCoord2s");
Function<void, const GLshort *> Binding::TexCoord2sv("glTexCoord2sv");
Function<void, GLfixed, GLfixed> Binding::TexCoord2xOES("glTexCoord2xOES");
Function<void, const GLfixed *> Binding::TexCoord2xvOES("glTexCoord2xvOES");
Function<void, GLbyte, GLbyte, GLbyte> Binding::TexCoord3bOES("glTexCoord3bOES");
Function<void, const GLbyte *> Binding::TexCoord3bvOES("glTexCoord3bvOES");
Function<void, GLdouble, GLdouble, GLdouble> Binding::TexCoord3d("glTexCoord3d");
Function<void, const GLdouble *> Binding::TexCoord3dv("glTexCoord3dv");
Function<void, GLfloat, GLfloat, GLfloat> Binding::TexCoord3f("glTexCoord3f");
Function<void, const GLfloat *> Binding::TexCoord3fv("glTexCoord3fv");
Function<void, GLhalfNV, GLhalfNV, GLhalfNV> Binding::TexCoord3hNV("glTexCoord3hNV");
Function<void, const GLhalfNV *> Binding::TexCoord3hvNV("glTexCoord3hvNV");
Function<void, GLint, GLint, GLint> Binding::TexCoord3i("glTexCoord3i");
Function<void, const GLint *> Binding::TexCoord3iv("glTexCoord3iv");
Function<void, GLshort, GLshort, GLshort> Binding::TexCoord3s("glTexCoord3s");
Function<void, const GLshort *> Binding::TexCoord3sv("glTexCoord3sv");
Function<void, GLfixed, GLfixed, GLfixed> Binding::TexCoord3xOES("glTexCoord3xOES");
Function<void, const GLfixed *> Binding::TexCoord3xvOES("glTexCoord3xvOES");
Function<void, GLbyte, GLbyte, GLbyte, GLbyte> Binding::TexCoord4bOES("glTexCoord4bOES");
Function<void, const GLbyte *> Binding::TexCoord4bvOES("glTexCoord4bvOES");
Function<void, GLdouble, GLdouble, GLdouble, GLdouble> Binding::TexCoord4d("glTexCoord4d");
Function<void, const GLdouble *> Binding::TexCoord4dv("glTexCoord4dv");
Function<void, GLfloat, GLfloat, GLfloat, GLfloat> Binding::TexCoord4f("glTexCoord4f");
Function<void, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat> Binding::TexCoord4fColor4fNormal3fVertex4fSUN("glTexCoord4fColor4fNormal3fVertex4fSUN");
Function<void, const GLfloat *, const GLfloat *, const GLfloat *, const GLfloat *> Binding::TexCoord4fColor4fNormal3fVertex4fvSUN("glTexCoord4fColor4fNormal3fVertex4fvSUN");
Function<void, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat> Binding::TexCoord4fVertex4fSUN("glTexCoord4fVertex4fSUN");
Function<void, const GLfloat *, const GLfloat *> Binding::TexCoord4fVertex4fvSUN("glTexCoord4fVertex4fvSUN");
Function<void, const GLfloat *> Binding::TexCoord4fv("glTexCoord4fv");
Function<void, GLhalfNV, GLhalfNV, GLhalfNV, GLhalfNV> Binding::TexCoord4hNV("glTexCoord4hNV");
Function<void, const GLhalfNV *> Binding::TexCoord4hvNV("glTexCoord4hvNV");
Function<void, GLint, GLint, GLint, GLint> Binding::TexCoord4i("glTexCoord4i");
Function<void, const GLint *> Binding::TexCoord4iv("glTexCoord4iv");
Function<void, GLshort, GLshort, GLshort, GLshort> Binding::TexCoord4s("glTexCoord4s");
Function<void, const GLshort *> Binding::TexCoord4sv("glTexCoord4sv");
Function<void, GLfixed, GLfixed, GLfixed, GLfixed> Binding::TexCoord4xOES("glTexCoord4xOES");
Function<void, const GLfixed *> Binding::TexCoord4xvOES("glTexCoord4xvOES");
Function<void, GLint, GLenum, GLsizei> Binding::TexCoordFormatNV("glTexCoordFormatNV");
Function<void, GLenum, GLuint> Binding::TexCoordP1ui("glTexCoordP1ui");
Function<void, GLenum, const GLuint *> Binding::TexCoordP1uiv("glTexCoordP1uiv");
Function<void, GLenum, GLuint> Binding::TexCoordP2ui("glTexCoordP2ui");
Function<void, GLenum, const GLuint *> Binding::TexCoordP2uiv("glTexCoordP2uiv");
Function<void, GLenum, GLuint> Binding::TexCoordP3ui("glTexCoordP3ui");
Function<void, GLenum, const GLuint *> Binding::TexCoordP3uiv("glTexCoordP3uiv");
Function<void, GLenum, GLuint> Binding::TexCoordP4ui("glTexCoordP4ui");
Function<void, GLenum, const GLuint *> Binding::TexCoordP4uiv("glTexCoordP4uiv");
Function<void, GLint, GLenum, GLsizei, const void *> Binding::TexCoordPointer("glTexCoordPointer");
Function<void, GLint, GLenum, GLsizei, GLsizei, const void *> Binding::TexCoordPointerEXT("glTexCoordPointerEXT");
Function<void, GLint, GLenum, GLint, const void **, GLint> Binding::TexCoordPointerListIBM("glTexCoordPointerListIBM");
Function<void, GLint, GLenum, const void **> Binding::TexCoordPointervINTEL("glTexCoordPointervINTEL");
Function<void, GLenum, GLenum, GLfloat> Binding::TexEnvf("glTexEnvf");
Function<void, GLenum, GLenum, const GLfloat *> Binding::TexEnvfv("glTexEnvfv");
Function<void, GLenum, GLenum, GLint> Binding::TexEnvi("glTexEnvi");
Function<void, GLenum, GLenum, const GLint *> Binding::TexEnviv("glTexEnviv");
Function<void, GLenum, GLenum, GLfixed> Binding::TexEnvxOES("glTexEnvxOES");
Function<void, GLenum, GLenum, const GLfixed *> Binding::TexEnvxvOES("glTexEnvxvOES");
Function<void, GLenum, GLenum, GLsizei, const GLfloat *> Binding::TexFilterFuncSGIS("glTexFilterFuncSGIS");
Function<void, GLenum, GLenum, GLdouble> Binding::TexGend("glTexGend");
Function<void, GLenum, GLenum, const GLdouble *> Binding::TexGendv("glTexGendv");
Function<void, GLenum, GLenum, GLfloat> Binding::TexGenf("glTexGenf");
Function<void, GLenum, GLenum, const GLfloat *> Binding::TexGenfv("glTexGenfv");
Function<void, GLenum, GLenum, GLint> Binding::TexGeni("glTexGeni");
Function<void, GLenum, GLenum, const GLint *> Binding::TexGeniv("glTexGeniv");
Function<void, GLenum, GLenum, GLfixed> Binding::TexGenxOES("glTexGenxOES");
Function<void, GLenum, GLenum, const GLfixed *> Binding::TexGenxvOES("glTexGenxvOES");
Function<void, GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const void *> Binding::TexImage1D("glTexImage1D");
Function<void, GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *> Binding::TexImage2D("glTexImage2D");
Function<void, GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean> Binding::TexImage2DMultisample("glTexImage2DMultisample");
Function<void, GLenum, GLsizei, GLsizei, GLint, GLsizei, GLsizei, GLboolean> Binding::TexImage2DMultisampleCoverageNV("glTexImage2DMultisampleCoverageNV");
Function<void, GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *> Binding::TexImage3D("glTexImage3D");
Function<void, GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *> Binding::TexImage3DEXT("glTexImage3DEXT");
Function<void, GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean> Binding::TexImage3DMultisample("glTexImage3DMultisample");
Function<void, GLenum, GLsizei, GLsizei, GLint, GLsizei, GLsizei, GLsizei, GLboolean> Binding::TexImage3DMultisampleCoverageNV("glTexImage3DMultisampleCoverageNV");
Function<void, GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *> Binding::TexImage4DSGIS("glTexImage4DSGIS");
Function<void, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLboolean> Binding::TexPageCommitmentARB("glTexPageCommitmentARB");
Function<void, GLenum, GLenum, const GLint *> Binding::TexParameterIiv("glTexParameterIiv");
Function<void, GLenum, GLenum, const GLint *> Binding::TexParameterIivEXT("glTexParameterIivEXT");
Function<void, GLenum, GLenum, const GLuint *> Binding::TexParameterIuiv("glTexParameterIuiv");
Function<void, GLenum, GLenum, const GLuint *> Binding::TexParameterIuivEXT("glTexParameterIuivEXT");
Function<void, GLenum, GLenum, GLfloat> Binding::TexParameterf("glTexParameterf");
Function<void, GLenum, GLenum, const GLfloat *> Binding::TexParameterfv("glTexParameterfv");
Function<void, GLenum, GLenum, GLint> Binding::TexParameteri("glTexParameteri");
Function<void, GLenum, GLenum, const GLint *> Binding::TexParameteriv("glTexParameteriv");
Function<void, GLenum, GLenum, GLfixed> Binding::TexParameterxOES("glTexParameterxOES");
Function<void, GLenum, GLenum, const GLfixed *> Binding::TexParameterxvOES("glTexParameterxvOES");
Function<void, GLenum, GLuint> Binding::TexRenderbufferNV("glTexRenderbufferNV");
Function<void, GLenum, GLsizei, GLenum, GLsizei> Binding::TexStorage1D("glTexStorage1D");
Function<void, GLenum, GLsizei, GLenum, GLsizei, GLsizei> Binding::TexStorage2D("glTexStorage2D");
Function<void, GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean> Binding::TexStorage2DMultisample("glTexStorage2DMultisample");
Function<void, GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei> Binding::TexStorage3D("glTexStorage3D");
Function<void, GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean> Binding::TexStorage3DMultisample("glTexStorage3DMultisample");
Function<void, GLenum, GLenum, GLsizei, GLsizei, GLsizei, GLsizei, TextureStorageMaskAMD> Binding::TexStorageSparseAMD("glTexStorageSparseAMD");
Function<void, GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const void *> Binding::TexSubImage1D("glTexSubImage1D");
Function<void, GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const void *> Binding::TexSubImage1DEXT("glTexSubImage1DEXT");
Function<void, GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *> Binding::TexSubImage2D("glTexSubImage2D");
Function<void, GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *> Binding::TexSubImage2DEXT("glTexSubImage2DEXT");
Function<void, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *> Binding::TexSubImage3D("glTexSubImage3D");
Function<void, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *> Binding::TexSubImage3DEXT("glTexSubImage3DEXT");
Function<void, GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *> Binding::TexSubImage4DSGIS("glTexSubImage4DSGIS");
Function<void> Binding::TextureBarrier("glTextureBarrier");
Function<void> Binding::TextureBarrierNV("glTextureBarrierNV");
Function<void, GLuint, GLenum, GLuint> Binding::TextureBuffer("glTextureBuffer");
Function<void, GLuint, GLenum, GLenum, GLuint> Binding::TextureBufferEXT("glTextureBufferEXT");
Function<void, GLuint, GLenum, GLuint, GLintptr, GLsizeiptr> Binding::TextureBufferRange("glTextureBufferRange");
Function<void, GLuint, GLenum, GLenum, GLuint, GLintptr, GLsizeiptr> Binding::TextureBufferRangeEXT("glTextureBufferRangeEXT");
Function<void, GLboolean, GLboolean, GLboolean, GLboolean> Binding::TextureColorMaskSGIS("glTextureColorMaskSGIS");
Function<void, GLuint, GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const void *> Binding::TextureImage1DEXT("glTextureImage1DEXT");
Function<void, GLuint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *> Binding::TextureImage2DEXT("glTextureImage2DEXT");
Function<void, GLuint, GLenum, GLsizei, GLsizei, GLint, GLsizei, GLsizei, GLboolean> Binding::TextureImage2DMultisampleCoverageNV("glTextureImage2DMultisampleCoverageNV");
Function<void, GLuint, GLenum, GLsizei, GLint, GLsizei, GLsizei, GLboolean> Binding::TextureImage2DMultisampleNV("glTextureImage2DMultisampleNV");
Function<void, GLuint, GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *> Binding::TextureImage3DEXT("glTextureImage3DEXT");
Function<void, GLuint, GLenum, GLsizei, GLsizei, GLint, GLsizei, GLsizei, GLsizei, GLboolean> Binding::TextureImage3DMultisampleCoverageNV("glTextureImage3DMultisampleCoverageNV");
Function<void, GLuint, GLenum, GLsizei, GLint, GLsizei, GLsizei, GLsizei, GLboolean> Binding::TextureImage3DMultisampleNV("glTextureImage3DMultisampleNV");
Function<void, GLenum> Binding::TextureLightEXT("glTextureLightEXT");
Function<void, GLenum, GLenum> Binding::TextureMaterialEXT("glTextureMaterialEXT");
Function<void, GLenum> Binding::TextureNormalEXT("glTextureNormalEXT");
Function<void, GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLboolean> Binding::TexturePageCommitmentEXT("glTexturePageCommitmentEXT");
Function<void, GLuint, GLenum, const GLint *> Binding::TextureParameterIiv("glTextureParameterIiv");
Function<void, GLuint, GLenum, GLenum, const GLint *> Binding::TextureParameterIivEXT("glTextureParameterIivEXT");
Function<void, GLuint, GLenum, const GLuint *> Binding::TextureParameterIuiv("glTextureParameterIuiv");
Function<void, GLuint, GLenum, GLenum, const GLuint *> Binding::TextureParameterIuivEXT("glTextureParameterIuivEXT");
Function<void, GLuint, GLenum, GLfloat> Binding::TextureParameterf("glTextureParameterf");
Function<void, GLuint, GLenum, GLenum, GLfloat> Binding::TextureParameterfEXT("glTextureParameterfEXT");
Function<void, GLuint, GLenum, const GLfloat *> Binding::TextureParameterfv("glTextureParameterfv");
Function<void, GLuint, GLenum, GLenum, const GLfloat *> Binding::TextureParameterfvEXT("glTextureParameterfvEXT");
Function<void, GLuint, GLenum, GLint> Binding::TextureParameteri("glTextureParameteri");
Function<void, GLuint, GLenum, GLenum, GLint> Binding::TextureParameteriEXT("glTextureParameteriEXT");
Function<void, GLuint, GLenum, const GLint *> Binding::TextureParameteriv("glTextureParameteriv");
Function<void, GLuint, GLenum, GLenum, const GLint *> Binding::TextureParameterivEXT("glTextureParameterivEXT");
Function<void, GLenum, GLsizei, const void *> Binding::TextureRangeAPPLE("glTextureRangeAPPLE");
Function<void, GLuint, GLenum, GLuint> Binding::TextureRenderbufferEXT("glTextureRenderbufferEXT");
Function<void, GLuint, GLsizei, GLenum, GLsizei> Binding::TextureStorage1D("glTextureStorage1D");
Function<void, GLuint, GLenum, GLsizei, GLenum, GLsizei> Binding::TextureStorage1DEXT("glTextureStorage1DEXT");
Function<void, GLuint, GLsizei, GLenum, GLsizei, GLsizei> Binding::TextureStorage2D("glTextureStorage2D");
Function<void, GLuint, GLenum, GLsizei, GLenum, GLsizei, GLsizei> Binding::TextureStorage2DEXT("glTextureStorage2DEXT");
Function<void, GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLboolean> Binding::TextureStorage2DMultisample("glTextureStorage2DMultisample");
Function<void, GLuint, GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean> Binding::TextureStorage2DMultisampleEXT("glTextureStorage2DMultisampleEXT");
Function<void, GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei> Binding::TextureStorage3D("glTextureStorage3D");
Function<void, GLuint, GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei> Binding::TextureStorage3DEXT("glTextureStorage3DEXT");
Function<void, GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean> Binding::TextureStorage3DMultisample("glTextureStorage3DMultisample");
Function<void, GLuint, GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean> Binding::TextureStorage3DMultisampleEXT("glTextureStorage3DMultisampleEXT");
Function<void, GLuint, GLenum, GLenum, GLsizei, GLsizei, GLsizei, GLsizei, TextureStorageMaskAMD> Binding::TextureStorageSparseAMD("glTextureStorageSparseAMD");
Function<void, GLuint, GLint, GLint, GLsizei, GLenum, GLenum, const void *> Binding::TextureSubImage1D("glTextureSubImage1D");
Function<void, GLuint, GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const void *> Binding::TextureSubImage1DEXT("glTextureSubImage1DEXT");
Function<void, GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *> Binding::TextureSubImage2D("glTextureSubImage2D");
Function<void, GLuint, GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *> Binding::TextureSubImage2DEXT("glTextureSubImage2DEXT");
Function<void, GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *> Binding::TextureSubImage3D("glTextureSubImage3D");
Function<void, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *> Binding::TextureSubImage3DEXT("glTextureSubImage3DEXT");
Function<void, GLuint, GLenum, GLuint, GLenum, GLuint, GLuint, GLuint, GLuint> Binding::TextureView("glTextureView");
Function<void, GLenum, GLuint, GLenum, GLenum> Binding::TrackMatrixNV("glTrackMatrixNV");
Function<void, GLsizei, const GLint *, GLenum> Binding::TransformFeedbackAttribsNV("glTransformFeedbackAttribsNV");
Function<void, GLuint, GLuint, GLuint> Binding::TransformFeedbackBufferBase("glTransformFeedbackBufferBase");
Function<void, GLuint, GLuint, GLuint, GLintptr, GLsizeiptr> Binding::TransformFeedbackBufferRange("glTransformFeedbackBufferRange");
Function<void, GLsizei, const GLint *, GLsizei, const GLint *, GLenum> Binding::TransformFeedbackStreamAttribsNV("glTransformFeedbackStreamAttribsNV");
Function<void, GLuint, GLsizei, const GLchar *const*, GLenum> Binding::TransformFeedbackVaryings("glTransformFeedbackVaryings");
Function<void, GLuint, GLsizei, const GLchar *const*, GLenum> Binding::TransformFeedbackVaryingsEXT("glTransformFeedbackVaryingsEXT");
Function<void, GLuint, GLsizei, const GLint *, GLenum> Binding::TransformFeedbackVaryingsNV("glTransformFeedbackVaryingsNV");
Function<void, GLuint, GLuint, GLenum, const GLfloat *> Binding::TransformPathNV("glTransformPathNV");
Function<void, GLdouble, GLdouble, GLdouble> Binding::Translated("glTranslated");
Function<void, GLfloat, GLfloat, GLfloat> Binding::Translatef("glTranslatef");
Function<void, GLfixed, GLfixed, GLfixed> Binding::TranslatexOES("glTranslatexOES");


} // namespace glbinding
