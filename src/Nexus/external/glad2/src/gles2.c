/**
 * SPDX-License-Identifier: (WTFPL OR CC0-1.0) AND Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/gles2.h>

#ifndef GLAD_IMPL_UTIL_C_
#define GLAD_IMPL_UTIL_C_

#ifdef _MSC_VER
#define GLAD_IMPL_UTIL_SSCANF sscanf_s
#else
#define GLAD_IMPL_UTIL_SSCANF sscanf
#endif

#endif /* GLAD_IMPL_UTIL_C_ */

#ifdef __cplusplus
extern "C" {
#endif








static void glad_gl_load_GL_ES_VERSION_2_0(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->ES_VERSION_2_0) return;
    context->ActiveTexture = (PFNGLACTIVETEXTUREPROC) load(userptr, "glActiveTexture");
    context->AttachShader = (PFNGLATTACHSHADERPROC) load(userptr, "glAttachShader");
    context->BindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) load(userptr, "glBindAttribLocation");
    context->BindBuffer = (PFNGLBINDBUFFERPROC) load(userptr, "glBindBuffer");
    context->BindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) load(userptr, "glBindFramebuffer");
    context->BindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) load(userptr, "glBindRenderbuffer");
    context->BindTexture = (PFNGLBINDTEXTUREPROC) load(userptr, "glBindTexture");
    context->BlendColor = (PFNGLBLENDCOLORPROC) load(userptr, "glBlendColor");
    context->BlendEquation = (PFNGLBLENDEQUATIONPROC) load(userptr, "glBlendEquation");
    context->BlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC) load(userptr, "glBlendEquationSeparate");
    context->BlendFunc = (PFNGLBLENDFUNCPROC) load(userptr, "glBlendFunc");
    context->BlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) load(userptr, "glBlendFuncSeparate");
    context->BufferData = (PFNGLBUFFERDATAPROC) load(userptr, "glBufferData");
    context->BufferSubData = (PFNGLBUFFERSUBDATAPROC) load(userptr, "glBufferSubData");
    context->CheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) load(userptr, "glCheckFramebufferStatus");
    context->Clear = (PFNGLCLEARPROC) load(userptr, "glClear");
    context->ClearColor = (PFNGLCLEARCOLORPROC) load(userptr, "glClearColor");
    context->ClearDepthf = (PFNGLCLEARDEPTHFPROC) load(userptr, "glClearDepthf");
    context->ClearStencil = (PFNGLCLEARSTENCILPROC) load(userptr, "glClearStencil");
    context->ColorMask = (PFNGLCOLORMASKPROC) load(userptr, "glColorMask");
    context->CompileShader = (PFNGLCOMPILESHADERPROC) load(userptr, "glCompileShader");
    context->CompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) load(userptr, "glCompressedTexImage2D");
    context->CompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) load(userptr, "glCompressedTexSubImage2D");
    context->CopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC) load(userptr, "glCopyTexImage2D");
    context->CopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC) load(userptr, "glCopyTexSubImage2D");
    context->CreateProgram = (PFNGLCREATEPROGRAMPROC) load(userptr, "glCreateProgram");
    context->CreateShader = (PFNGLCREATESHADERPROC) load(userptr, "glCreateShader");
    context->CullFace = (PFNGLCULLFACEPROC) load(userptr, "glCullFace");
    context->DeleteBuffers = (PFNGLDELETEBUFFERSPROC) load(userptr, "glDeleteBuffers");
    context->DeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) load(userptr, "glDeleteFramebuffers");
    context->DeleteProgram = (PFNGLDELETEPROGRAMPROC) load(userptr, "glDeleteProgram");
    context->DeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) load(userptr, "glDeleteRenderbuffers");
    context->DeleteShader = (PFNGLDELETESHADERPROC) load(userptr, "glDeleteShader");
    context->DeleteTextures = (PFNGLDELETETEXTURESPROC) load(userptr, "glDeleteTextures");
    context->DepthFunc = (PFNGLDEPTHFUNCPROC) load(userptr, "glDepthFunc");
    context->DepthMask = (PFNGLDEPTHMASKPROC) load(userptr, "glDepthMask");
    context->DepthRangef = (PFNGLDEPTHRANGEFPROC) load(userptr, "glDepthRangef");
    context->DetachShader = (PFNGLDETACHSHADERPROC) load(userptr, "glDetachShader");
    context->Disable = (PFNGLDISABLEPROC) load(userptr, "glDisable");
    context->DisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) load(userptr, "glDisableVertexAttribArray");
    context->DrawArrays = (PFNGLDRAWARRAYSPROC) load(userptr, "glDrawArrays");
    context->DrawElements = (PFNGLDRAWELEMENTSPROC) load(userptr, "glDrawElements");
    context->Enable = (PFNGLENABLEPROC) load(userptr, "glEnable");
    context->EnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) load(userptr, "glEnableVertexAttribArray");
    context->Finish = (PFNGLFINISHPROC) load(userptr, "glFinish");
    context->Flush = (PFNGLFLUSHPROC) load(userptr, "glFlush");
    context->FramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) load(userptr, "glFramebufferRenderbuffer");
    context->FramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) load(userptr, "glFramebufferTexture2D");
    context->FrontFace = (PFNGLFRONTFACEPROC) load(userptr, "glFrontFace");
    context->GenBuffers = (PFNGLGENBUFFERSPROC) load(userptr, "glGenBuffers");
    context->GenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) load(userptr, "glGenFramebuffers");
    context->GenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) load(userptr, "glGenRenderbuffers");
    context->GenTextures = (PFNGLGENTEXTURESPROC) load(userptr, "glGenTextures");
    context->GenerateMipmap = (PFNGLGENERATEMIPMAPPROC) load(userptr, "glGenerateMipmap");
    context->GetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) load(userptr, "glGetActiveAttrib");
    context->GetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC) load(userptr, "glGetActiveUniform");
    context->GetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC) load(userptr, "glGetAttachedShaders");
    context->GetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) load(userptr, "glGetAttribLocation");
    context->GetBooleanv = (PFNGLGETBOOLEANVPROC) load(userptr, "glGetBooleanv");
    context->GetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) load(userptr, "glGetBufferParameteriv");
    context->GetError = (PFNGLGETERRORPROC) load(userptr, "glGetError");
    context->GetFloatv = (PFNGLGETFLOATVPROC) load(userptr, "glGetFloatv");
    context->GetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) load(userptr, "glGetFramebufferAttachmentParameteriv");
    context->GetIntegerv = (PFNGLGETINTEGERVPROC) load(userptr, "glGetIntegerv");
    context->GetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) load(userptr, "glGetProgramInfoLog");
    context->GetProgramiv = (PFNGLGETPROGRAMIVPROC) load(userptr, "glGetProgramiv");
    context->GetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) load(userptr, "glGetRenderbufferParameteriv");
    context->GetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) load(userptr, "glGetShaderInfoLog");
    context->GetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC) load(userptr, "glGetShaderPrecisionFormat");
    context->GetShaderSource = (PFNGLGETSHADERSOURCEPROC) load(userptr, "glGetShaderSource");
    context->GetShaderiv = (PFNGLGETSHADERIVPROC) load(userptr, "glGetShaderiv");
    context->GetString = (PFNGLGETSTRINGPROC) load(userptr, "glGetString");
    context->GetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC) load(userptr, "glGetTexParameterfv");
    context->GetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC) load(userptr, "glGetTexParameteriv");
    context->GetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) load(userptr, "glGetUniformLocation");
    context->GetUniformfv = (PFNGLGETUNIFORMFVPROC) load(userptr, "glGetUniformfv");
    context->GetUniformiv = (PFNGLGETUNIFORMIVPROC) load(userptr, "glGetUniformiv");
    context->GetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC) load(userptr, "glGetVertexAttribPointerv");
    context->GetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC) load(userptr, "glGetVertexAttribfv");
    context->GetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC) load(userptr, "glGetVertexAttribiv");
    context->Hint = (PFNGLHINTPROC) load(userptr, "glHint");
    context->IsBuffer = (PFNGLISBUFFERPROC) load(userptr, "glIsBuffer");
    context->IsEnabled = (PFNGLISENABLEDPROC) load(userptr, "glIsEnabled");
    context->IsFramebuffer = (PFNGLISFRAMEBUFFERPROC) load(userptr, "glIsFramebuffer");
    context->IsProgram = (PFNGLISPROGRAMPROC) load(userptr, "glIsProgram");
    context->IsRenderbuffer = (PFNGLISRENDERBUFFERPROC) load(userptr, "glIsRenderbuffer");
    context->IsShader = (PFNGLISSHADERPROC) load(userptr, "glIsShader");
    context->IsTexture = (PFNGLISTEXTUREPROC) load(userptr, "glIsTexture");
    context->LineWidth = (PFNGLLINEWIDTHPROC) load(userptr, "glLineWidth");
    context->LinkProgram = (PFNGLLINKPROGRAMPROC) load(userptr, "glLinkProgram");
    context->PixelStorei = (PFNGLPIXELSTOREIPROC) load(userptr, "glPixelStorei");
    context->PolygonOffset = (PFNGLPOLYGONOFFSETPROC) load(userptr, "glPolygonOffset");
    context->ReadPixels = (PFNGLREADPIXELSPROC) load(userptr, "glReadPixels");
    context->ReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC) load(userptr, "glReleaseShaderCompiler");
    context->RenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) load(userptr, "glRenderbufferStorage");
    context->SampleCoverage = (PFNGLSAMPLECOVERAGEPROC) load(userptr, "glSampleCoverage");
    context->Scissor = (PFNGLSCISSORPROC) load(userptr, "glScissor");
    context->ShaderBinary = (PFNGLSHADERBINARYPROC) load(userptr, "glShaderBinary");
    context->ShaderSource = (PFNGLSHADERSOURCEPROC) load(userptr, "glShaderSource");
    context->StencilFunc = (PFNGLSTENCILFUNCPROC) load(userptr, "glStencilFunc");
    context->StencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC) load(userptr, "glStencilFuncSeparate");
    context->StencilMask = (PFNGLSTENCILMASKPROC) load(userptr, "glStencilMask");
    context->StencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC) load(userptr, "glStencilMaskSeparate");
    context->StencilOp = (PFNGLSTENCILOPPROC) load(userptr, "glStencilOp");
    context->StencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC) load(userptr, "glStencilOpSeparate");
    context->TexImage2D = (PFNGLTEXIMAGE2DPROC) load(userptr, "glTexImage2D");
    context->TexParameterf = (PFNGLTEXPARAMETERFPROC) load(userptr, "glTexParameterf");
    context->TexParameterfv = (PFNGLTEXPARAMETERFVPROC) load(userptr, "glTexParameterfv");
    context->TexParameteri = (PFNGLTEXPARAMETERIPROC) load(userptr, "glTexParameteri");
    context->TexParameteriv = (PFNGLTEXPARAMETERIVPROC) load(userptr, "glTexParameteriv");
    context->TexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC) load(userptr, "glTexSubImage2D");
    context->Uniform1f = (PFNGLUNIFORM1FPROC) load(userptr, "glUniform1f");
    context->Uniform1fv = (PFNGLUNIFORM1FVPROC) load(userptr, "glUniform1fv");
    context->Uniform1i = (PFNGLUNIFORM1IPROC) load(userptr, "glUniform1i");
    context->Uniform1iv = (PFNGLUNIFORM1IVPROC) load(userptr, "glUniform1iv");
    context->Uniform2f = (PFNGLUNIFORM2FPROC) load(userptr, "glUniform2f");
    context->Uniform2fv = (PFNGLUNIFORM2FVPROC) load(userptr, "glUniform2fv");
    context->Uniform2i = (PFNGLUNIFORM2IPROC) load(userptr, "glUniform2i");
    context->Uniform2iv = (PFNGLUNIFORM2IVPROC) load(userptr, "glUniform2iv");
    context->Uniform3f = (PFNGLUNIFORM3FPROC) load(userptr, "glUniform3f");
    context->Uniform3fv = (PFNGLUNIFORM3FVPROC) load(userptr, "glUniform3fv");
    context->Uniform3i = (PFNGLUNIFORM3IPROC) load(userptr, "glUniform3i");
    context->Uniform3iv = (PFNGLUNIFORM3IVPROC) load(userptr, "glUniform3iv");
    context->Uniform4f = (PFNGLUNIFORM4FPROC) load(userptr, "glUniform4f");
    context->Uniform4fv = (PFNGLUNIFORM4FVPROC) load(userptr, "glUniform4fv");
    context->Uniform4i = (PFNGLUNIFORM4IPROC) load(userptr, "glUniform4i");
    context->Uniform4iv = (PFNGLUNIFORM4IVPROC) load(userptr, "glUniform4iv");
    context->UniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC) load(userptr, "glUniformMatrix2fv");
    context->UniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC) load(userptr, "glUniformMatrix3fv");
    context->UniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) load(userptr, "glUniformMatrix4fv");
    context->UseProgram = (PFNGLUSEPROGRAMPROC) load(userptr, "glUseProgram");
    context->ValidateProgram = (PFNGLVALIDATEPROGRAMPROC) load(userptr, "glValidateProgram");
    context->VertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC) load(userptr, "glVertexAttrib1f");
    context->VertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC) load(userptr, "glVertexAttrib1fv");
    context->VertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC) load(userptr, "glVertexAttrib2f");
    context->VertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC) load(userptr, "glVertexAttrib2fv");
    context->VertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC) load(userptr, "glVertexAttrib3f");
    context->VertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC) load(userptr, "glVertexAttrib3fv");
    context->VertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC) load(userptr, "glVertexAttrib4f");
    context->VertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC) load(userptr, "glVertexAttrib4fv");
    context->VertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) load(userptr, "glVertexAttribPointer");
    context->Viewport = (PFNGLVIEWPORTPROC) load(userptr, "glViewport");
}
static void glad_gl_load_GL_ES_VERSION_3_0(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->ES_VERSION_3_0) return;
    context->BeginQuery = (PFNGLBEGINQUERYPROC) load(userptr, "glBeginQuery");
    context->BeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC) load(userptr, "glBeginTransformFeedback");
    context->BindBufferBase = (PFNGLBINDBUFFERBASEPROC) load(userptr, "glBindBufferBase");
    context->BindBufferRange = (PFNGLBINDBUFFERRANGEPROC) load(userptr, "glBindBufferRange");
    context->BindSampler = (PFNGLBINDSAMPLERPROC) load(userptr, "glBindSampler");
    context->BindTransformFeedback = (PFNGLBINDTRANSFORMFEEDBACKPROC) load(userptr, "glBindTransformFeedback");
    context->BindVertexArray = (PFNGLBINDVERTEXARRAYPROC) load(userptr, "glBindVertexArray");
    context->BlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC) load(userptr, "glBlitFramebuffer");
    context->ClearBufferfi = (PFNGLCLEARBUFFERFIPROC) load(userptr, "glClearBufferfi");
    context->ClearBufferfv = (PFNGLCLEARBUFFERFVPROC) load(userptr, "glClearBufferfv");
    context->ClearBufferiv = (PFNGLCLEARBUFFERIVPROC) load(userptr, "glClearBufferiv");
    context->ClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC) load(userptr, "glClearBufferuiv");
    context->ClientWaitSync = (PFNGLCLIENTWAITSYNCPROC) load(userptr, "glClientWaitSync");
    context->CompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) load(userptr, "glCompressedTexImage3D");
    context->CompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) load(userptr, "glCompressedTexSubImage3D");
    context->CopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC) load(userptr, "glCopyBufferSubData");
    context->CopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC) load(userptr, "glCopyTexSubImage3D");
    context->DeleteQueries = (PFNGLDELETEQUERIESPROC) load(userptr, "glDeleteQueries");
    context->DeleteSamplers = (PFNGLDELETESAMPLERSPROC) load(userptr, "glDeleteSamplers");
    context->DeleteSync = (PFNGLDELETESYNCPROC) load(userptr, "glDeleteSync");
    context->DeleteTransformFeedbacks = (PFNGLDELETETRANSFORMFEEDBACKSPROC) load(userptr, "glDeleteTransformFeedbacks");
    context->DeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) load(userptr, "glDeleteVertexArrays");
    context->DrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC) load(userptr, "glDrawArraysInstanced");
    context->DrawBuffers = (PFNGLDRAWBUFFERSPROC) load(userptr, "glDrawBuffers");
    context->DrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC) load(userptr, "glDrawElementsInstanced");
    context->DrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC) load(userptr, "glDrawRangeElements");
    context->EndQuery = (PFNGLENDQUERYPROC) load(userptr, "glEndQuery");
    context->EndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC) load(userptr, "glEndTransformFeedback");
    context->FenceSync = (PFNGLFENCESYNCPROC) load(userptr, "glFenceSync");
    context->FlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC) load(userptr, "glFlushMappedBufferRange");
    context->FramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC) load(userptr, "glFramebufferTextureLayer");
    context->GenQueries = (PFNGLGENQUERIESPROC) load(userptr, "glGenQueries");
    context->GenSamplers = (PFNGLGENSAMPLERSPROC) load(userptr, "glGenSamplers");
    context->GenTransformFeedbacks = (PFNGLGENTRANSFORMFEEDBACKSPROC) load(userptr, "glGenTransformFeedbacks");
    context->GenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) load(userptr, "glGenVertexArrays");
    context->GetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC) load(userptr, "glGetActiveUniformBlockName");
    context->GetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC) load(userptr, "glGetActiveUniformBlockiv");
    context->GetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC) load(userptr, "glGetActiveUniformsiv");
    context->GetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC) load(userptr, "glGetBufferParameteri64v");
    context->GetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC) load(userptr, "glGetBufferPointerv");
    context->GetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC) load(userptr, "glGetFragDataLocation");
    context->GetInteger64i_v = (PFNGLGETINTEGER64I_VPROC) load(userptr, "glGetInteger64i_v");
    context->GetInteger64v = (PFNGLGETINTEGER64VPROC) load(userptr, "glGetInteger64v");
    context->GetIntegeri_v = (PFNGLGETINTEGERI_VPROC) load(userptr, "glGetIntegeri_v");
    context->GetInternalformativ = (PFNGLGETINTERNALFORMATIVPROC) load(userptr, "glGetInternalformativ");
    context->GetProgramBinary = (PFNGLGETPROGRAMBINARYPROC) load(userptr, "glGetProgramBinary");
    context->GetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC) load(userptr, "glGetQueryObjectuiv");
    context->GetQueryiv = (PFNGLGETQUERYIVPROC) load(userptr, "glGetQueryiv");
    context->GetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC) load(userptr, "glGetSamplerParameterfv");
    context->GetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC) load(userptr, "glGetSamplerParameteriv");
    context->GetStringi = (PFNGLGETSTRINGIPROC) load(userptr, "glGetStringi");
    context->GetSynciv = (PFNGLGETSYNCIVPROC) load(userptr, "glGetSynciv");
    context->GetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) load(userptr, "glGetTransformFeedbackVarying");
    context->GetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC) load(userptr, "glGetUniformBlockIndex");
    context->GetUniformIndices = (PFNGLGETUNIFORMINDICESPROC) load(userptr, "glGetUniformIndices");
    context->GetUniformuiv = (PFNGLGETUNIFORMUIVPROC) load(userptr, "glGetUniformuiv");
    context->GetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC) load(userptr, "glGetVertexAttribIiv");
    context->GetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC) load(userptr, "glGetVertexAttribIuiv");
    context->InvalidateFramebuffer = (PFNGLINVALIDATEFRAMEBUFFERPROC) load(userptr, "glInvalidateFramebuffer");
    context->InvalidateSubFramebuffer = (PFNGLINVALIDATESUBFRAMEBUFFERPROC) load(userptr, "glInvalidateSubFramebuffer");
    context->IsQuery = (PFNGLISQUERYPROC) load(userptr, "glIsQuery");
    context->IsSampler = (PFNGLISSAMPLERPROC) load(userptr, "glIsSampler");
    context->IsSync = (PFNGLISSYNCPROC) load(userptr, "glIsSync");
    context->IsTransformFeedback = (PFNGLISTRANSFORMFEEDBACKPROC) load(userptr, "glIsTransformFeedback");
    context->IsVertexArray = (PFNGLISVERTEXARRAYPROC) load(userptr, "glIsVertexArray");
    context->MapBufferRange = (PFNGLMAPBUFFERRANGEPROC) load(userptr, "glMapBufferRange");
    context->PauseTransformFeedback = (PFNGLPAUSETRANSFORMFEEDBACKPROC) load(userptr, "glPauseTransformFeedback");
    context->ProgramBinary = (PFNGLPROGRAMBINARYPROC) load(userptr, "glProgramBinary");
    context->ProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC) load(userptr, "glProgramParameteri");
    context->ReadBuffer = (PFNGLREADBUFFERPROC) load(userptr, "glReadBuffer");
    context->RenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) load(userptr, "glRenderbufferStorageMultisample");
    context->ResumeTransformFeedback = (PFNGLRESUMETRANSFORMFEEDBACKPROC) load(userptr, "glResumeTransformFeedback");
    context->SamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC) load(userptr, "glSamplerParameterf");
    context->SamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC) load(userptr, "glSamplerParameterfv");
    context->SamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC) load(userptr, "glSamplerParameteri");
    context->SamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC) load(userptr, "glSamplerParameteriv");
    context->TexImage3D = (PFNGLTEXIMAGE3DPROC) load(userptr, "glTexImage3D");
    context->TexStorage2D = (PFNGLTEXSTORAGE2DPROC) load(userptr, "glTexStorage2D");
    context->TexStorage3D = (PFNGLTEXSTORAGE3DPROC) load(userptr, "glTexStorage3D");
    context->TexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC) load(userptr, "glTexSubImage3D");
    context->TransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC) load(userptr, "glTransformFeedbackVaryings");
    context->Uniform1ui = (PFNGLUNIFORM1UIPROC) load(userptr, "glUniform1ui");
    context->Uniform1uiv = (PFNGLUNIFORM1UIVPROC) load(userptr, "glUniform1uiv");
    context->Uniform2ui = (PFNGLUNIFORM2UIPROC) load(userptr, "glUniform2ui");
    context->Uniform2uiv = (PFNGLUNIFORM2UIVPROC) load(userptr, "glUniform2uiv");
    context->Uniform3ui = (PFNGLUNIFORM3UIPROC) load(userptr, "glUniform3ui");
    context->Uniform3uiv = (PFNGLUNIFORM3UIVPROC) load(userptr, "glUniform3uiv");
    context->Uniform4ui = (PFNGLUNIFORM4UIPROC) load(userptr, "glUniform4ui");
    context->Uniform4uiv = (PFNGLUNIFORM4UIVPROC) load(userptr, "glUniform4uiv");
    context->UniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC) load(userptr, "glUniformBlockBinding");
    context->UniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC) load(userptr, "glUniformMatrix2x3fv");
    context->UniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC) load(userptr, "glUniformMatrix2x4fv");
    context->UniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC) load(userptr, "glUniformMatrix3x2fv");
    context->UniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC) load(userptr, "glUniformMatrix3x4fv");
    context->UniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC) load(userptr, "glUniformMatrix4x2fv");
    context->UniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC) load(userptr, "glUniformMatrix4x3fv");
    context->UnmapBuffer = (PFNGLUNMAPBUFFERPROC) load(userptr, "glUnmapBuffer");
    context->VertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC) load(userptr, "glVertexAttribDivisor");
    context->VertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC) load(userptr, "glVertexAttribI4i");
    context->VertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC) load(userptr, "glVertexAttribI4iv");
    context->VertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC) load(userptr, "glVertexAttribI4ui");
    context->VertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC) load(userptr, "glVertexAttribI4uiv");
    context->VertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC) load(userptr, "glVertexAttribIPointer");
    context->WaitSync = (PFNGLWAITSYNCPROC) load(userptr, "glWaitSync");
}
static void glad_gl_load_GL_ES_VERSION_3_1(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->ES_VERSION_3_1) return;
    context->ActiveShaderProgram = (PFNGLACTIVESHADERPROGRAMPROC) load(userptr, "glActiveShaderProgram");
    context->BindImageTexture = (PFNGLBINDIMAGETEXTUREPROC) load(userptr, "glBindImageTexture");
    context->BindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC) load(userptr, "glBindProgramPipeline");
    context->BindVertexBuffer = (PFNGLBINDVERTEXBUFFERPROC) load(userptr, "glBindVertexBuffer");
    context->CreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC) load(userptr, "glCreateShaderProgramv");
    context->DeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC) load(userptr, "glDeleteProgramPipelines");
    context->DispatchCompute = (PFNGLDISPATCHCOMPUTEPROC) load(userptr, "glDispatchCompute");
    context->DispatchComputeIndirect = (PFNGLDISPATCHCOMPUTEINDIRECTPROC) load(userptr, "glDispatchComputeIndirect");
    context->DrawArraysIndirect = (PFNGLDRAWARRAYSINDIRECTPROC) load(userptr, "glDrawArraysIndirect");
    context->DrawElementsIndirect = (PFNGLDRAWELEMENTSINDIRECTPROC) load(userptr, "glDrawElementsIndirect");
    context->FramebufferParameteri = (PFNGLFRAMEBUFFERPARAMETERIPROC) load(userptr, "glFramebufferParameteri");
    context->GenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC) load(userptr, "glGenProgramPipelines");
    context->GetBooleani_v = (PFNGLGETBOOLEANI_VPROC) load(userptr, "glGetBooleani_v");
    context->GetFramebufferParameteriv = (PFNGLGETFRAMEBUFFERPARAMETERIVPROC) load(userptr, "glGetFramebufferParameteriv");
    context->GetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC) load(userptr, "glGetMultisamplefv");
    context->GetProgramInterfaceiv = (PFNGLGETPROGRAMINTERFACEIVPROC) load(userptr, "glGetProgramInterfaceiv");
    context->GetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC) load(userptr, "glGetProgramPipelineInfoLog");
    context->GetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC) load(userptr, "glGetProgramPipelineiv");
    context->GetProgramResourceIndex = (PFNGLGETPROGRAMRESOURCEINDEXPROC) load(userptr, "glGetProgramResourceIndex");
    context->GetProgramResourceLocation = (PFNGLGETPROGRAMRESOURCELOCATIONPROC) load(userptr, "glGetProgramResourceLocation");
    context->GetProgramResourceName = (PFNGLGETPROGRAMRESOURCENAMEPROC) load(userptr, "glGetProgramResourceName");
    context->GetProgramResourceiv = (PFNGLGETPROGRAMRESOURCEIVPROC) load(userptr, "glGetProgramResourceiv");
    context->GetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC) load(userptr, "glGetTexLevelParameterfv");
    context->GetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC) load(userptr, "glGetTexLevelParameteriv");
    context->IsProgramPipeline = (PFNGLISPROGRAMPIPELINEPROC) load(userptr, "glIsProgramPipeline");
    context->MemoryBarrier = (PFNGLMEMORYBARRIERPROC) load(userptr, "glMemoryBarrier");
    context->MemoryBarrierByRegion = (PFNGLMEMORYBARRIERBYREGIONPROC) load(userptr, "glMemoryBarrierByRegion");
    context->ProgramUniform1f = (PFNGLPROGRAMUNIFORM1FPROC) load(userptr, "glProgramUniform1f");
    context->ProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC) load(userptr, "glProgramUniform1fv");
    context->ProgramUniform1i = (PFNGLPROGRAMUNIFORM1IPROC) load(userptr, "glProgramUniform1i");
    context->ProgramUniform1iv = (PFNGLPROGRAMUNIFORM1IVPROC) load(userptr, "glProgramUniform1iv");
    context->ProgramUniform1ui = (PFNGLPROGRAMUNIFORM1UIPROC) load(userptr, "glProgramUniform1ui");
    context->ProgramUniform1uiv = (PFNGLPROGRAMUNIFORM1UIVPROC) load(userptr, "glProgramUniform1uiv");
    context->ProgramUniform2f = (PFNGLPROGRAMUNIFORM2FPROC) load(userptr, "glProgramUniform2f");
    context->ProgramUniform2fv = (PFNGLPROGRAMUNIFORM2FVPROC) load(userptr, "glProgramUniform2fv");
    context->ProgramUniform2i = (PFNGLPROGRAMUNIFORM2IPROC) load(userptr, "glProgramUniform2i");
    context->ProgramUniform2iv = (PFNGLPROGRAMUNIFORM2IVPROC) load(userptr, "glProgramUniform2iv");
    context->ProgramUniform2ui = (PFNGLPROGRAMUNIFORM2UIPROC) load(userptr, "glProgramUniform2ui");
    context->ProgramUniform2uiv = (PFNGLPROGRAMUNIFORM2UIVPROC) load(userptr, "glProgramUniform2uiv");
    context->ProgramUniform3f = (PFNGLPROGRAMUNIFORM3FPROC) load(userptr, "glProgramUniform3f");
    context->ProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC) load(userptr, "glProgramUniform3fv");
    context->ProgramUniform3i = (PFNGLPROGRAMUNIFORM3IPROC) load(userptr, "glProgramUniform3i");
    context->ProgramUniform3iv = (PFNGLPROGRAMUNIFORM3IVPROC) load(userptr, "glProgramUniform3iv");
    context->ProgramUniform3ui = (PFNGLPROGRAMUNIFORM3UIPROC) load(userptr, "glProgramUniform3ui");
    context->ProgramUniform3uiv = (PFNGLPROGRAMUNIFORM3UIVPROC) load(userptr, "glProgramUniform3uiv");
    context->ProgramUniform4f = (PFNGLPROGRAMUNIFORM4FPROC) load(userptr, "glProgramUniform4f");
    context->ProgramUniform4fv = (PFNGLPROGRAMUNIFORM4FVPROC) load(userptr, "glProgramUniform4fv");
    context->ProgramUniform4i = (PFNGLPROGRAMUNIFORM4IPROC) load(userptr, "glProgramUniform4i");
    context->ProgramUniform4iv = (PFNGLPROGRAMUNIFORM4IVPROC) load(userptr, "glProgramUniform4iv");
    context->ProgramUniform4ui = (PFNGLPROGRAMUNIFORM4UIPROC) load(userptr, "glProgramUniform4ui");
    context->ProgramUniform4uiv = (PFNGLPROGRAMUNIFORM4UIVPROC) load(userptr, "glProgramUniform4uiv");
    context->ProgramUniformMatrix2fv = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC) load(userptr, "glProgramUniformMatrix2fv");
    context->ProgramUniformMatrix2x3fv = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC) load(userptr, "glProgramUniformMatrix2x3fv");
    context->ProgramUniformMatrix2x4fv = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC) load(userptr, "glProgramUniformMatrix2x4fv");
    context->ProgramUniformMatrix3fv = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC) load(userptr, "glProgramUniformMatrix3fv");
    context->ProgramUniformMatrix3x2fv = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC) load(userptr, "glProgramUniformMatrix3x2fv");
    context->ProgramUniformMatrix3x4fv = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC) load(userptr, "glProgramUniformMatrix3x4fv");
    context->ProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC) load(userptr, "glProgramUniformMatrix4fv");
    context->ProgramUniformMatrix4x2fv = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC) load(userptr, "glProgramUniformMatrix4x2fv");
    context->ProgramUniformMatrix4x3fv = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC) load(userptr, "glProgramUniformMatrix4x3fv");
    context->SampleMaski = (PFNGLSAMPLEMASKIPROC) load(userptr, "glSampleMaski");
    context->TexStorage2DMultisample = (PFNGLTEXSTORAGE2DMULTISAMPLEPROC) load(userptr, "glTexStorage2DMultisample");
    context->UseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC) load(userptr, "glUseProgramStages");
    context->ValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC) load(userptr, "glValidateProgramPipeline");
    context->VertexAttribBinding = (PFNGLVERTEXATTRIBBINDINGPROC) load(userptr, "glVertexAttribBinding");
    context->VertexAttribFormat = (PFNGLVERTEXATTRIBFORMATPROC) load(userptr, "glVertexAttribFormat");
    context->VertexAttribIFormat = (PFNGLVERTEXATTRIBIFORMATPROC) load(userptr, "glVertexAttribIFormat");
    context->VertexBindingDivisor = (PFNGLVERTEXBINDINGDIVISORPROC) load(userptr, "glVertexBindingDivisor");
}
static void glad_gl_load_GL_ES_VERSION_3_2(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->ES_VERSION_3_2) return;
    context->BlendBarrier = (PFNGLBLENDBARRIERPROC) load(userptr, "glBlendBarrier");
    context->BlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC) load(userptr, "glBlendEquationSeparatei");
    context->BlendEquationi = (PFNGLBLENDEQUATIONIPROC) load(userptr, "glBlendEquationi");
    context->BlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC) load(userptr, "glBlendFuncSeparatei");
    context->BlendFunci = (PFNGLBLENDFUNCIPROC) load(userptr, "glBlendFunci");
    context->ColorMaski = (PFNGLCOLORMASKIPROC) load(userptr, "glColorMaski");
    context->CopyImageSubData = (PFNGLCOPYIMAGESUBDATAPROC) load(userptr, "glCopyImageSubData");
    context->DebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC) load(userptr, "glDebugMessageCallback");
    context->DebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC) load(userptr, "glDebugMessageControl");
    context->DebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC) load(userptr, "glDebugMessageInsert");
    context->Disablei = (PFNGLDISABLEIPROC) load(userptr, "glDisablei");
    context->DrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC) load(userptr, "glDrawElementsBaseVertex");
    context->DrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC) load(userptr, "glDrawElementsInstancedBaseVertex");
    context->DrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC) load(userptr, "glDrawRangeElementsBaseVertex");
    context->Enablei = (PFNGLENABLEIPROC) load(userptr, "glEnablei");
    context->FramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC) load(userptr, "glFramebufferTexture");
    context->GetDebugMessageLog = (PFNGLGETDEBUGMESSAGELOGPROC) load(userptr, "glGetDebugMessageLog");
    context->GetGraphicsResetStatus = (PFNGLGETGRAPHICSRESETSTATUSPROC) load(userptr, "glGetGraphicsResetStatus");
    context->GetObjectLabel = (PFNGLGETOBJECTLABELPROC) load(userptr, "glGetObjectLabel");
    context->GetObjectPtrLabel = (PFNGLGETOBJECTPTRLABELPROC) load(userptr, "glGetObjectPtrLabel");
    context->GetPointerv = (PFNGLGETPOINTERVPROC) load(userptr, "glGetPointerv");
    context->GetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC) load(userptr, "glGetSamplerParameterIiv");
    context->GetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC) load(userptr, "glGetSamplerParameterIuiv");
    context->GetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC) load(userptr, "glGetTexParameterIiv");
    context->GetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC) load(userptr, "glGetTexParameterIuiv");
    context->GetnUniformfv = (PFNGLGETNUNIFORMFVPROC) load(userptr, "glGetnUniformfv");
    context->GetnUniformiv = (PFNGLGETNUNIFORMIVPROC) load(userptr, "glGetnUniformiv");
    context->GetnUniformuiv = (PFNGLGETNUNIFORMUIVPROC) load(userptr, "glGetnUniformuiv");
    context->IsEnabledi = (PFNGLISENABLEDIPROC) load(userptr, "glIsEnabledi");
    context->MinSampleShading = (PFNGLMINSAMPLESHADINGPROC) load(userptr, "glMinSampleShading");
    context->ObjectLabel = (PFNGLOBJECTLABELPROC) load(userptr, "glObjectLabel");
    context->ObjectPtrLabel = (PFNGLOBJECTPTRLABELPROC) load(userptr, "glObjectPtrLabel");
    context->PatchParameteri = (PFNGLPATCHPARAMETERIPROC) load(userptr, "glPatchParameteri");
    context->PopDebugGroup = (PFNGLPOPDEBUGGROUPPROC) load(userptr, "glPopDebugGroup");
    context->PrimitiveBoundingBox = (PFNGLPRIMITIVEBOUNDINGBOXPROC) load(userptr, "glPrimitiveBoundingBox");
    context->PushDebugGroup = (PFNGLPUSHDEBUGGROUPPROC) load(userptr, "glPushDebugGroup");
    context->ReadnPixels = (PFNGLREADNPIXELSPROC) load(userptr, "glReadnPixels");
    context->SamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC) load(userptr, "glSamplerParameterIiv");
    context->SamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC) load(userptr, "glSamplerParameterIuiv");
    context->TexBuffer = (PFNGLTEXBUFFERPROC) load(userptr, "glTexBuffer");
    context->TexBufferRange = (PFNGLTEXBUFFERRANGEPROC) load(userptr, "glTexBufferRange");
    context->TexParameterIiv = (PFNGLTEXPARAMETERIIVPROC) load(userptr, "glTexParameterIiv");
    context->TexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC) load(userptr, "glTexParameterIuiv");
    context->TexStorage3DMultisample = (PFNGLTEXSTORAGE3DMULTISAMPLEPROC) load(userptr, "glTexStorage3DMultisample");
}
static void glad_gl_load_GL_AMD_framebuffer_multisample_advanced(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->AMD_framebuffer_multisample_advanced) return;
    context->NamedRenderbufferStorageMultisampleAdvancedAMD = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEADVANCEDAMDPROC) load(userptr, "glNamedRenderbufferStorageMultisampleAdvancedAMD");
    context->RenderbufferStorageMultisampleAdvancedAMD = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEADVANCEDAMDPROC) load(userptr, "glRenderbufferStorageMultisampleAdvancedAMD");
}
static void glad_gl_load_GL_AMD_performance_monitor(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->AMD_performance_monitor) return;
    context->BeginPerfMonitorAMD = (PFNGLBEGINPERFMONITORAMDPROC) load(userptr, "glBeginPerfMonitorAMD");
    context->DeletePerfMonitorsAMD = (PFNGLDELETEPERFMONITORSAMDPROC) load(userptr, "glDeletePerfMonitorsAMD");
    context->EndPerfMonitorAMD = (PFNGLENDPERFMONITORAMDPROC) load(userptr, "glEndPerfMonitorAMD");
    context->GenPerfMonitorsAMD = (PFNGLGENPERFMONITORSAMDPROC) load(userptr, "glGenPerfMonitorsAMD");
    context->GetPerfMonitorCounterDataAMD = (PFNGLGETPERFMONITORCOUNTERDATAAMDPROC) load(userptr, "glGetPerfMonitorCounterDataAMD");
    context->GetPerfMonitorCounterInfoAMD = (PFNGLGETPERFMONITORCOUNTERINFOAMDPROC) load(userptr, "glGetPerfMonitorCounterInfoAMD");
    context->GetPerfMonitorCounterStringAMD = (PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC) load(userptr, "glGetPerfMonitorCounterStringAMD");
    context->GetPerfMonitorCountersAMD = (PFNGLGETPERFMONITORCOUNTERSAMDPROC) load(userptr, "glGetPerfMonitorCountersAMD");
    context->GetPerfMonitorGroupStringAMD = (PFNGLGETPERFMONITORGROUPSTRINGAMDPROC) load(userptr, "glGetPerfMonitorGroupStringAMD");
    context->GetPerfMonitorGroupsAMD = (PFNGLGETPERFMONITORGROUPSAMDPROC) load(userptr, "glGetPerfMonitorGroupsAMD");
    context->SelectPerfMonitorCountersAMD = (PFNGLSELECTPERFMONITORCOUNTERSAMDPROC) load(userptr, "glSelectPerfMonitorCountersAMD");
}
static void glad_gl_load_GL_ANGLE_framebuffer_blit(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->ANGLE_framebuffer_blit) return;
    context->BlitFramebufferANGLE = (PFNGLBLITFRAMEBUFFERANGLEPROC) load(userptr, "glBlitFramebufferANGLE");
}
static void glad_gl_load_GL_ANGLE_framebuffer_multisample(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->ANGLE_framebuffer_multisample) return;
    context->RenderbufferStorageMultisampleANGLE = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEANGLEPROC) load(userptr, "glRenderbufferStorageMultisampleANGLE");
}
static void glad_gl_load_GL_ANGLE_instanced_arrays(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->ANGLE_instanced_arrays) return;
    context->DrawArraysInstancedANGLE = (PFNGLDRAWARRAYSINSTANCEDANGLEPROC) load(userptr, "glDrawArraysInstancedANGLE");
    context->DrawElementsInstancedANGLE = (PFNGLDRAWELEMENTSINSTANCEDANGLEPROC) load(userptr, "glDrawElementsInstancedANGLE");
    context->VertexAttribDivisorANGLE = (PFNGLVERTEXATTRIBDIVISORANGLEPROC) load(userptr, "glVertexAttribDivisorANGLE");
}
static void glad_gl_load_GL_ANGLE_translated_shader_source(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->ANGLE_translated_shader_source) return;
    context->GetTranslatedShaderSourceANGLE = (PFNGLGETTRANSLATEDSHADERSOURCEANGLEPROC) load(userptr, "glGetTranslatedShaderSourceANGLE");
}
static void glad_gl_load_GL_APPLE_copy_texture_levels(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->APPLE_copy_texture_levels) return;
    context->CopyTextureLevelsAPPLE = (PFNGLCOPYTEXTURELEVELSAPPLEPROC) load(userptr, "glCopyTextureLevelsAPPLE");
}
static void glad_gl_load_GL_APPLE_framebuffer_multisample(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->APPLE_framebuffer_multisample) return;
    context->RenderbufferStorageMultisampleAPPLE = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC) load(userptr, "glRenderbufferStorageMultisampleAPPLE");
    context->ResolveMultisampleFramebufferAPPLE = (PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC) load(userptr, "glResolveMultisampleFramebufferAPPLE");
}
static void glad_gl_load_GL_APPLE_sync(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->APPLE_sync) return;
    context->ClientWaitSyncAPPLE = (PFNGLCLIENTWAITSYNCAPPLEPROC) load(userptr, "glClientWaitSyncAPPLE");
    context->DeleteSyncAPPLE = (PFNGLDELETESYNCAPPLEPROC) load(userptr, "glDeleteSyncAPPLE");
    context->FenceSyncAPPLE = (PFNGLFENCESYNCAPPLEPROC) load(userptr, "glFenceSyncAPPLE");
    context->GetInteger64vAPPLE = (PFNGLGETINTEGER64VAPPLEPROC) load(userptr, "glGetInteger64vAPPLE");
    context->GetSyncivAPPLE = (PFNGLGETSYNCIVAPPLEPROC) load(userptr, "glGetSyncivAPPLE");
    context->IsSyncAPPLE = (PFNGLISSYNCAPPLEPROC) load(userptr, "glIsSyncAPPLE");
    context->WaitSyncAPPLE = (PFNGLWAITSYNCAPPLEPROC) load(userptr, "glWaitSyncAPPLE");
}
static void glad_gl_load_GL_ARM_shader_core_properties(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->ARM_shader_core_properties) return;
    context->MaxActiveShaderCoresARM = (PFNGLMAXACTIVESHADERCORESARMPROC) load(userptr, "glMaxActiveShaderCoresARM");
}
static void glad_gl_load_GL_EXT_EGL_image_storage(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_EGL_image_storage) return;
    context->EGLImageTargetTexStorageEXT = (PFNGLEGLIMAGETARGETTEXSTORAGEEXTPROC) load(userptr, "glEGLImageTargetTexStorageEXT");
    context->EGLImageTargetTextureStorageEXT = (PFNGLEGLIMAGETARGETTEXTURESTORAGEEXTPROC) load(userptr, "glEGLImageTargetTextureStorageEXT");
}
static void glad_gl_load_GL_EXT_base_instance(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_base_instance) return;
    context->DrawArraysInstancedBaseInstanceEXT = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEEXTPROC) load(userptr, "glDrawArraysInstancedBaseInstanceEXT");
    context->DrawElementsInstancedBaseInstanceEXT = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEEXTPROC) load(userptr, "glDrawElementsInstancedBaseInstanceEXT");
    context->DrawElementsInstancedBaseVertexBaseInstanceEXT = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEEXTPROC) load(userptr, "glDrawElementsInstancedBaseVertexBaseInstanceEXT");
}
static void glad_gl_load_GL_EXT_blend_func_extended(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_blend_func_extended) return;
    context->BindFragDataLocationEXT = (PFNGLBINDFRAGDATALOCATIONEXTPROC) load(userptr, "glBindFragDataLocationEXT");
    context->BindFragDataLocationIndexedEXT = (PFNGLBINDFRAGDATALOCATIONINDEXEDEXTPROC) load(userptr, "glBindFragDataLocationIndexedEXT");
    context->GetFragDataIndexEXT = (PFNGLGETFRAGDATAINDEXEXTPROC) load(userptr, "glGetFragDataIndexEXT");
    context->GetProgramResourceLocationIndexEXT = (PFNGLGETPROGRAMRESOURCELOCATIONINDEXEXTPROC) load(userptr, "glGetProgramResourceLocationIndexEXT");
}
static void glad_gl_load_GL_EXT_buffer_storage(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_buffer_storage) return;
    context->BufferStorageEXT = (PFNGLBUFFERSTORAGEEXTPROC) load(userptr, "glBufferStorageEXT");
}
static void glad_gl_load_GL_EXT_clear_texture(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_clear_texture) return;
    context->ClearTexImageEXT = (PFNGLCLEARTEXIMAGEEXTPROC) load(userptr, "glClearTexImageEXT");
    context->ClearTexSubImageEXT = (PFNGLCLEARTEXSUBIMAGEEXTPROC) load(userptr, "glClearTexSubImageEXT");
}
static void glad_gl_load_GL_EXT_clip_control(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_clip_control) return;
    context->ClipControlEXT = (PFNGLCLIPCONTROLEXTPROC) load(userptr, "glClipControlEXT");
}
static void glad_gl_load_GL_EXT_copy_image(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_copy_image) return;
    context->CopyImageSubDataEXT = (PFNGLCOPYIMAGESUBDATAEXTPROC) load(userptr, "glCopyImageSubDataEXT");
}
static void glad_gl_load_GL_EXT_debug_label(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_debug_label) return;
    context->GetObjectLabelEXT = (PFNGLGETOBJECTLABELEXTPROC) load(userptr, "glGetObjectLabelEXT");
    context->LabelObjectEXT = (PFNGLLABELOBJECTEXTPROC) load(userptr, "glLabelObjectEXT");
}
static void glad_gl_load_GL_EXT_debug_marker(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_debug_marker) return;
    context->InsertEventMarkerEXT = (PFNGLINSERTEVENTMARKEREXTPROC) load(userptr, "glInsertEventMarkerEXT");
    context->PopGroupMarkerEXT = (PFNGLPOPGROUPMARKEREXTPROC) load(userptr, "glPopGroupMarkerEXT");
    context->PushGroupMarkerEXT = (PFNGLPUSHGROUPMARKEREXTPROC) load(userptr, "glPushGroupMarkerEXT");
}
static void glad_gl_load_GL_EXT_discard_framebuffer(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_discard_framebuffer) return;
    context->DiscardFramebufferEXT = (PFNGLDISCARDFRAMEBUFFEREXTPROC) load(userptr, "glDiscardFramebufferEXT");
}
static void glad_gl_load_GL_EXT_disjoint_timer_query(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_disjoint_timer_query) return;
    context->BeginQueryEXT = (PFNGLBEGINQUERYEXTPROC) load(userptr, "glBeginQueryEXT");
    context->DeleteQueriesEXT = (PFNGLDELETEQUERIESEXTPROC) load(userptr, "glDeleteQueriesEXT");
    context->EndQueryEXT = (PFNGLENDQUERYEXTPROC) load(userptr, "glEndQueryEXT");
    context->GenQueriesEXT = (PFNGLGENQUERIESEXTPROC) load(userptr, "glGenQueriesEXT");
    context->GetInteger64vEXT = (PFNGLGETINTEGER64VEXTPROC) load(userptr, "glGetInteger64vEXT");
    context->GetQueryObjecti64vEXT = (PFNGLGETQUERYOBJECTI64VEXTPROC) load(userptr, "glGetQueryObjecti64vEXT");
    context->GetQueryObjectivEXT = (PFNGLGETQUERYOBJECTIVEXTPROC) load(userptr, "glGetQueryObjectivEXT");
    context->GetQueryObjectui64vEXT = (PFNGLGETQUERYOBJECTUI64VEXTPROC) load(userptr, "glGetQueryObjectui64vEXT");
    context->GetQueryObjectuivEXT = (PFNGLGETQUERYOBJECTUIVEXTPROC) load(userptr, "glGetQueryObjectuivEXT");
    context->GetQueryivEXT = (PFNGLGETQUERYIVEXTPROC) load(userptr, "glGetQueryivEXT");
    context->IsQueryEXT = (PFNGLISQUERYEXTPROC) load(userptr, "glIsQueryEXT");
    context->QueryCounterEXT = (PFNGLQUERYCOUNTEREXTPROC) load(userptr, "glQueryCounterEXT");
}
static void glad_gl_load_GL_EXT_draw_buffers(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_draw_buffers) return;
    context->DrawBuffersEXT = (PFNGLDRAWBUFFERSEXTPROC) load(userptr, "glDrawBuffersEXT");
}
static void glad_gl_load_GL_EXT_draw_buffers_indexed(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_draw_buffers_indexed) return;
    context->BlendEquationSeparateiEXT = (PFNGLBLENDEQUATIONSEPARATEIEXTPROC) load(userptr, "glBlendEquationSeparateiEXT");
    context->BlendEquationiEXT = (PFNGLBLENDEQUATIONIEXTPROC) load(userptr, "glBlendEquationiEXT");
    context->BlendFuncSeparateiEXT = (PFNGLBLENDFUNCSEPARATEIEXTPROC) load(userptr, "glBlendFuncSeparateiEXT");
    context->BlendFunciEXT = (PFNGLBLENDFUNCIEXTPROC) load(userptr, "glBlendFunciEXT");
    context->ColorMaskiEXT = (PFNGLCOLORMASKIEXTPROC) load(userptr, "glColorMaskiEXT");
    context->DisableiEXT = (PFNGLDISABLEIEXTPROC) load(userptr, "glDisableiEXT");
    context->EnableiEXT = (PFNGLENABLEIEXTPROC) load(userptr, "glEnableiEXT");
    context->IsEnablediEXT = (PFNGLISENABLEDIEXTPROC) load(userptr, "glIsEnablediEXT");
}
static void glad_gl_load_GL_EXT_draw_elements_base_vertex(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_draw_elements_base_vertex) return;
    context->DrawElementsBaseVertexEXT = (PFNGLDRAWELEMENTSBASEVERTEXEXTPROC) load(userptr, "glDrawElementsBaseVertexEXT");
    context->DrawElementsInstancedBaseVertexEXT = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXEXTPROC) load(userptr, "glDrawElementsInstancedBaseVertexEXT");
    context->DrawRangeElementsBaseVertexEXT = (PFNGLDRAWRANGEELEMENTSBASEVERTEXEXTPROC) load(userptr, "glDrawRangeElementsBaseVertexEXT");
    context->MultiDrawElementsBaseVertexEXT = (PFNGLMULTIDRAWELEMENTSBASEVERTEXEXTPROC) load(userptr, "glMultiDrawElementsBaseVertexEXT");
}
static void glad_gl_load_GL_EXT_draw_instanced(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_draw_instanced) return;
    context->DrawArraysInstancedEXT = (PFNGLDRAWARRAYSINSTANCEDEXTPROC) load(userptr, "glDrawArraysInstancedEXT");
    context->DrawElementsInstancedEXT = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC) load(userptr, "glDrawElementsInstancedEXT");
}
static void glad_gl_load_GL_EXT_draw_transform_feedback(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_draw_transform_feedback) return;
    context->DrawTransformFeedbackEXT = (PFNGLDRAWTRANSFORMFEEDBACKEXTPROC) load(userptr, "glDrawTransformFeedbackEXT");
    context->DrawTransformFeedbackInstancedEXT = (PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDEXTPROC) load(userptr, "glDrawTransformFeedbackInstancedEXT");
}
static void glad_gl_load_GL_EXT_external_buffer(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_external_buffer) return;
    context->BufferStorageExternalEXT = (PFNGLBUFFERSTORAGEEXTERNALEXTPROC) load(userptr, "glBufferStorageExternalEXT");
    context->NamedBufferStorageExternalEXT = (PFNGLNAMEDBUFFERSTORAGEEXTERNALEXTPROC) load(userptr, "glNamedBufferStorageExternalEXT");
}
static void glad_gl_load_GL_EXT_fragment_shading_rate(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_fragment_shading_rate) return;
    context->FramebufferShadingRateEXT = (PFNGLFRAMEBUFFERSHADINGRATEEXTPROC) load(userptr, "glFramebufferShadingRateEXT");
    context->GetFragmentShadingRatesEXT = (PFNGLGETFRAGMENTSHADINGRATESEXTPROC) load(userptr, "glGetFragmentShadingRatesEXT");
    context->ShadingRateCombinerOpsEXT = (PFNGLSHADINGRATECOMBINEROPSEXTPROC) load(userptr, "glShadingRateCombinerOpsEXT");
    context->ShadingRateEXT = (PFNGLSHADINGRATEEXTPROC) load(userptr, "glShadingRateEXT");
}
static void glad_gl_load_GL_EXT_framebuffer_blit_layers(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_framebuffer_blit_layers) return;
    context->BlitFramebufferLayerEXT = (PFNGLBLITFRAMEBUFFERLAYEREXTPROC) load(userptr, "glBlitFramebufferLayerEXT");
    context->BlitFramebufferLayersEXT = (PFNGLBLITFRAMEBUFFERLAYERSEXTPROC) load(userptr, "glBlitFramebufferLayersEXT");
}
static void glad_gl_load_GL_EXT_geometry_shader(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_geometry_shader) return;
    context->FramebufferTextureEXT = (PFNGLFRAMEBUFFERTEXTUREEXTPROC) load(userptr, "glFramebufferTextureEXT");
}
static void glad_gl_load_GL_EXT_instanced_arrays(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_instanced_arrays) return;
    context->DrawArraysInstancedEXT = (PFNGLDRAWARRAYSINSTANCEDEXTPROC) load(userptr, "glDrawArraysInstancedEXT");
    context->DrawElementsInstancedEXT = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC) load(userptr, "glDrawElementsInstancedEXT");
    context->VertexAttribDivisorEXT = (PFNGLVERTEXATTRIBDIVISOREXTPROC) load(userptr, "glVertexAttribDivisorEXT");
}
static void glad_gl_load_GL_EXT_map_buffer_range(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_map_buffer_range) return;
    context->FlushMappedBufferRangeEXT = (PFNGLFLUSHMAPPEDBUFFERRANGEEXTPROC) load(userptr, "glFlushMappedBufferRangeEXT");
    context->MapBufferRangeEXT = (PFNGLMAPBUFFERRANGEEXTPROC) load(userptr, "glMapBufferRangeEXT");
}
static void glad_gl_load_GL_EXT_memory_object(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_memory_object) return;
    context->BufferStorageMemEXT = (PFNGLBUFFERSTORAGEMEMEXTPROC) load(userptr, "glBufferStorageMemEXT");
    context->CreateMemoryObjectsEXT = (PFNGLCREATEMEMORYOBJECTSEXTPROC) load(userptr, "glCreateMemoryObjectsEXT");
    context->DeleteMemoryObjectsEXT = (PFNGLDELETEMEMORYOBJECTSEXTPROC) load(userptr, "glDeleteMemoryObjectsEXT");
    context->GetMemoryObjectParameterivEXT = (PFNGLGETMEMORYOBJECTPARAMETERIVEXTPROC) load(userptr, "glGetMemoryObjectParameterivEXT");
    context->GetUnsignedBytei_vEXT = (PFNGLGETUNSIGNEDBYTEI_VEXTPROC) load(userptr, "glGetUnsignedBytei_vEXT");
    context->GetUnsignedBytevEXT = (PFNGLGETUNSIGNEDBYTEVEXTPROC) load(userptr, "glGetUnsignedBytevEXT");
    context->IsMemoryObjectEXT = (PFNGLISMEMORYOBJECTEXTPROC) load(userptr, "glIsMemoryObjectEXT");
    context->MemoryObjectParameterivEXT = (PFNGLMEMORYOBJECTPARAMETERIVEXTPROC) load(userptr, "glMemoryObjectParameterivEXT");
    context->NamedBufferStorageMemEXT = (PFNGLNAMEDBUFFERSTORAGEMEMEXTPROC) load(userptr, "glNamedBufferStorageMemEXT");
    context->TexStorageMem2DEXT = (PFNGLTEXSTORAGEMEM2DEXTPROC) load(userptr, "glTexStorageMem2DEXT");
    context->TexStorageMem2DMultisampleEXT = (PFNGLTEXSTORAGEMEM2DMULTISAMPLEEXTPROC) load(userptr, "glTexStorageMem2DMultisampleEXT");
    context->TexStorageMem3DEXT = (PFNGLTEXSTORAGEMEM3DEXTPROC) load(userptr, "glTexStorageMem3DEXT");
    context->TexStorageMem3DMultisampleEXT = (PFNGLTEXSTORAGEMEM3DMULTISAMPLEEXTPROC) load(userptr, "glTexStorageMem3DMultisampleEXT");
    context->TextureStorageMem2DEXT = (PFNGLTEXTURESTORAGEMEM2DEXTPROC) load(userptr, "glTextureStorageMem2DEXT");
    context->TextureStorageMem2DMultisampleEXT = (PFNGLTEXTURESTORAGEMEM2DMULTISAMPLEEXTPROC) load(userptr, "glTextureStorageMem2DMultisampleEXT");
    context->TextureStorageMem3DEXT = (PFNGLTEXTURESTORAGEMEM3DEXTPROC) load(userptr, "glTextureStorageMem3DEXT");
    context->TextureStorageMem3DMultisampleEXT = (PFNGLTEXTURESTORAGEMEM3DMULTISAMPLEEXTPROC) load(userptr, "glTextureStorageMem3DMultisampleEXT");
}
static void glad_gl_load_GL_EXT_memory_object_fd(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_memory_object_fd) return;
    context->ImportMemoryFdEXT = (PFNGLIMPORTMEMORYFDEXTPROC) load(userptr, "glImportMemoryFdEXT");
}
static void glad_gl_load_GL_EXT_memory_object_win32(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_memory_object_win32) return;
    context->ImportMemoryWin32HandleEXT = (PFNGLIMPORTMEMORYWIN32HANDLEEXTPROC) load(userptr, "glImportMemoryWin32HandleEXT");
    context->ImportMemoryWin32NameEXT = (PFNGLIMPORTMEMORYWIN32NAMEEXTPROC) load(userptr, "glImportMemoryWin32NameEXT");
}
static void glad_gl_load_GL_EXT_multi_draw_arrays(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_multi_draw_arrays) return;
    context->MultiDrawArraysEXT = (PFNGLMULTIDRAWARRAYSEXTPROC) load(userptr, "glMultiDrawArraysEXT");
    context->MultiDrawElementsEXT = (PFNGLMULTIDRAWELEMENTSEXTPROC) load(userptr, "glMultiDrawElementsEXT");
}
static void glad_gl_load_GL_EXT_multi_draw_indirect(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_multi_draw_indirect) return;
    context->MultiDrawArraysIndirectEXT = (PFNGLMULTIDRAWARRAYSINDIRECTEXTPROC) load(userptr, "glMultiDrawArraysIndirectEXT");
    context->MultiDrawElementsIndirectEXT = (PFNGLMULTIDRAWELEMENTSINDIRECTEXTPROC) load(userptr, "glMultiDrawElementsIndirectEXT");
}
static void glad_gl_load_GL_EXT_multisampled_render_to_texture(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_multisampled_render_to_texture) return;
    context->FramebufferTexture2DMultisampleEXT = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC) load(userptr, "glFramebufferTexture2DMultisampleEXT");
    context->RenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) load(userptr, "glRenderbufferStorageMultisampleEXT");
}
static void glad_gl_load_GL_EXT_multiview_draw_buffers(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_multiview_draw_buffers) return;
    context->DrawBuffersIndexedEXT = (PFNGLDRAWBUFFERSINDEXEDEXTPROC) load(userptr, "glDrawBuffersIndexedEXT");
    context->GetIntegeri_vEXT = (PFNGLGETINTEGERI_VEXTPROC) load(userptr, "glGetIntegeri_vEXT");
    context->ReadBufferIndexedEXT = (PFNGLREADBUFFERINDEXEDEXTPROC) load(userptr, "glReadBufferIndexedEXT");
}
static void glad_gl_load_GL_EXT_occlusion_query_boolean(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_occlusion_query_boolean) return;
    context->BeginQueryEXT = (PFNGLBEGINQUERYEXTPROC) load(userptr, "glBeginQueryEXT");
    context->DeleteQueriesEXT = (PFNGLDELETEQUERIESEXTPROC) load(userptr, "glDeleteQueriesEXT");
    context->EndQueryEXT = (PFNGLENDQUERYEXTPROC) load(userptr, "glEndQueryEXT");
    context->GenQueriesEXT = (PFNGLGENQUERIESEXTPROC) load(userptr, "glGenQueriesEXT");
    context->GetQueryObjectuivEXT = (PFNGLGETQUERYOBJECTUIVEXTPROC) load(userptr, "glGetQueryObjectuivEXT");
    context->GetQueryivEXT = (PFNGLGETQUERYIVEXTPROC) load(userptr, "glGetQueryivEXT");
    context->IsQueryEXT = (PFNGLISQUERYEXTPROC) load(userptr, "glIsQueryEXT");
}
static void glad_gl_load_GL_EXT_polygon_offset_clamp(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_polygon_offset_clamp) return;
    context->PolygonOffsetClampEXT = (PFNGLPOLYGONOFFSETCLAMPEXTPROC) load(userptr, "glPolygonOffsetClampEXT");
}
static void glad_gl_load_GL_EXT_primitive_bounding_box(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_primitive_bounding_box) return;
    context->PrimitiveBoundingBoxEXT = (PFNGLPRIMITIVEBOUNDINGBOXEXTPROC) load(userptr, "glPrimitiveBoundingBoxEXT");
}
static void glad_gl_load_GL_EXT_raster_multisample(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_raster_multisample) return;
    context->RasterSamplesEXT = (PFNGLRASTERSAMPLESEXTPROC) load(userptr, "glRasterSamplesEXT");
}
static void glad_gl_load_GL_EXT_robustness(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_robustness) return;
    context->GetGraphicsResetStatusEXT = (PFNGLGETGRAPHICSRESETSTATUSEXTPROC) load(userptr, "glGetGraphicsResetStatusEXT");
    context->GetnUniformfvEXT = (PFNGLGETNUNIFORMFVEXTPROC) load(userptr, "glGetnUniformfvEXT");
    context->GetnUniformivEXT = (PFNGLGETNUNIFORMIVEXTPROC) load(userptr, "glGetnUniformivEXT");
    context->ReadnPixelsEXT = (PFNGLREADNPIXELSEXTPROC) load(userptr, "glReadnPixelsEXT");
}
static void glad_gl_load_GL_EXT_semaphore(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_semaphore) return;
    context->DeleteSemaphoresEXT = (PFNGLDELETESEMAPHORESEXTPROC) load(userptr, "glDeleteSemaphoresEXT");
    context->GenSemaphoresEXT = (PFNGLGENSEMAPHORESEXTPROC) load(userptr, "glGenSemaphoresEXT");
    context->GetSemaphoreParameterui64vEXT = (PFNGLGETSEMAPHOREPARAMETERUI64VEXTPROC) load(userptr, "glGetSemaphoreParameterui64vEXT");
    context->GetUnsignedBytei_vEXT = (PFNGLGETUNSIGNEDBYTEI_VEXTPROC) load(userptr, "glGetUnsignedBytei_vEXT");
    context->GetUnsignedBytevEXT = (PFNGLGETUNSIGNEDBYTEVEXTPROC) load(userptr, "glGetUnsignedBytevEXT");
    context->IsSemaphoreEXT = (PFNGLISSEMAPHOREEXTPROC) load(userptr, "glIsSemaphoreEXT");
    context->SemaphoreParameterui64vEXT = (PFNGLSEMAPHOREPARAMETERUI64VEXTPROC) load(userptr, "glSemaphoreParameterui64vEXT");
    context->SignalSemaphoreEXT = (PFNGLSIGNALSEMAPHOREEXTPROC) load(userptr, "glSignalSemaphoreEXT");
    context->WaitSemaphoreEXT = (PFNGLWAITSEMAPHOREEXTPROC) load(userptr, "glWaitSemaphoreEXT");
}
static void glad_gl_load_GL_EXT_semaphore_fd(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_semaphore_fd) return;
    context->ImportSemaphoreFdEXT = (PFNGLIMPORTSEMAPHOREFDEXTPROC) load(userptr, "glImportSemaphoreFdEXT");
}
static void glad_gl_load_GL_EXT_semaphore_win32(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_semaphore_win32) return;
    context->ImportSemaphoreWin32HandleEXT = (PFNGLIMPORTSEMAPHOREWIN32HANDLEEXTPROC) load(userptr, "glImportSemaphoreWin32HandleEXT");
    context->ImportSemaphoreWin32NameEXT = (PFNGLIMPORTSEMAPHOREWIN32NAMEEXTPROC) load(userptr, "glImportSemaphoreWin32NameEXT");
}
static void glad_gl_load_GL_EXT_separate_shader_objects(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_separate_shader_objects) return;
    context->ActiveShaderProgramEXT = (PFNGLACTIVESHADERPROGRAMEXTPROC) load(userptr, "glActiveShaderProgramEXT");
    context->BindProgramPipelineEXT = (PFNGLBINDPROGRAMPIPELINEEXTPROC) load(userptr, "glBindProgramPipelineEXT");
    context->CreateShaderProgramvEXT = (PFNGLCREATESHADERPROGRAMVEXTPROC) load(userptr, "glCreateShaderProgramvEXT");
    context->DeleteProgramPipelinesEXT = (PFNGLDELETEPROGRAMPIPELINESEXTPROC) load(userptr, "glDeleteProgramPipelinesEXT");
    context->GenProgramPipelinesEXT = (PFNGLGENPROGRAMPIPELINESEXTPROC) load(userptr, "glGenProgramPipelinesEXT");
    context->GetProgramPipelineInfoLogEXT = (PFNGLGETPROGRAMPIPELINEINFOLOGEXTPROC) load(userptr, "glGetProgramPipelineInfoLogEXT");
    context->GetProgramPipelineivEXT = (PFNGLGETPROGRAMPIPELINEIVEXTPROC) load(userptr, "glGetProgramPipelineivEXT");
    context->IsProgramPipelineEXT = (PFNGLISPROGRAMPIPELINEEXTPROC) load(userptr, "glIsProgramPipelineEXT");
    context->ProgramParameteriEXT = (PFNGLPROGRAMPARAMETERIEXTPROC) load(userptr, "glProgramParameteriEXT");
    context->ProgramUniform1fEXT = (PFNGLPROGRAMUNIFORM1FEXTPROC) load(userptr, "glProgramUniform1fEXT");
    context->ProgramUniform1fvEXT = (PFNGLPROGRAMUNIFORM1FVEXTPROC) load(userptr, "glProgramUniform1fvEXT");
    context->ProgramUniform1iEXT = (PFNGLPROGRAMUNIFORM1IEXTPROC) load(userptr, "glProgramUniform1iEXT");
    context->ProgramUniform1ivEXT = (PFNGLPROGRAMUNIFORM1IVEXTPROC) load(userptr, "glProgramUniform1ivEXT");
    context->ProgramUniform1uiEXT = (PFNGLPROGRAMUNIFORM1UIEXTPROC) load(userptr, "glProgramUniform1uiEXT");
    context->ProgramUniform1uivEXT = (PFNGLPROGRAMUNIFORM1UIVEXTPROC) load(userptr, "glProgramUniform1uivEXT");
    context->ProgramUniform2fEXT = (PFNGLPROGRAMUNIFORM2FEXTPROC) load(userptr, "glProgramUniform2fEXT");
    context->ProgramUniform2fvEXT = (PFNGLPROGRAMUNIFORM2FVEXTPROC) load(userptr, "glProgramUniform2fvEXT");
    context->ProgramUniform2iEXT = (PFNGLPROGRAMUNIFORM2IEXTPROC) load(userptr, "glProgramUniform2iEXT");
    context->ProgramUniform2ivEXT = (PFNGLPROGRAMUNIFORM2IVEXTPROC) load(userptr, "glProgramUniform2ivEXT");
    context->ProgramUniform2uiEXT = (PFNGLPROGRAMUNIFORM2UIEXTPROC) load(userptr, "glProgramUniform2uiEXT");
    context->ProgramUniform2uivEXT = (PFNGLPROGRAMUNIFORM2UIVEXTPROC) load(userptr, "glProgramUniform2uivEXT");
    context->ProgramUniform3fEXT = (PFNGLPROGRAMUNIFORM3FEXTPROC) load(userptr, "glProgramUniform3fEXT");
    context->ProgramUniform3fvEXT = (PFNGLPROGRAMUNIFORM3FVEXTPROC) load(userptr, "glProgramUniform3fvEXT");
    context->ProgramUniform3iEXT = (PFNGLPROGRAMUNIFORM3IEXTPROC) load(userptr, "glProgramUniform3iEXT");
    context->ProgramUniform3ivEXT = (PFNGLPROGRAMUNIFORM3IVEXTPROC) load(userptr, "glProgramUniform3ivEXT");
    context->ProgramUniform3uiEXT = (PFNGLPROGRAMUNIFORM3UIEXTPROC) load(userptr, "glProgramUniform3uiEXT");
    context->ProgramUniform3uivEXT = (PFNGLPROGRAMUNIFORM3UIVEXTPROC) load(userptr, "glProgramUniform3uivEXT");
    context->ProgramUniform4fEXT = (PFNGLPROGRAMUNIFORM4FEXTPROC) load(userptr, "glProgramUniform4fEXT");
    context->ProgramUniform4fvEXT = (PFNGLPROGRAMUNIFORM4FVEXTPROC) load(userptr, "glProgramUniform4fvEXT");
    context->ProgramUniform4iEXT = (PFNGLPROGRAMUNIFORM4IEXTPROC) load(userptr, "glProgramUniform4iEXT");
    context->ProgramUniform4ivEXT = (PFNGLPROGRAMUNIFORM4IVEXTPROC) load(userptr, "glProgramUniform4ivEXT");
    context->ProgramUniform4uiEXT = (PFNGLPROGRAMUNIFORM4UIEXTPROC) load(userptr, "glProgramUniform4uiEXT");
    context->ProgramUniform4uivEXT = (PFNGLPROGRAMUNIFORM4UIVEXTPROC) load(userptr, "glProgramUniform4uivEXT");
    context->ProgramUniformMatrix2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC) load(userptr, "glProgramUniformMatrix2fvEXT");
    context->ProgramUniformMatrix2x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC) load(userptr, "glProgramUniformMatrix2x3fvEXT");
    context->ProgramUniformMatrix2x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC) load(userptr, "glProgramUniformMatrix2x4fvEXT");
    context->ProgramUniformMatrix3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC) load(userptr, "glProgramUniformMatrix3fvEXT");
    context->ProgramUniformMatrix3x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC) load(userptr, "glProgramUniformMatrix3x2fvEXT");
    context->ProgramUniformMatrix3x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC) load(userptr, "glProgramUniformMatrix3x4fvEXT");
    context->ProgramUniformMatrix4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC) load(userptr, "glProgramUniformMatrix4fvEXT");
    context->ProgramUniformMatrix4x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC) load(userptr, "glProgramUniformMatrix4x2fvEXT");
    context->ProgramUniformMatrix4x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC) load(userptr, "glProgramUniformMatrix4x3fvEXT");
    context->UseProgramStagesEXT = (PFNGLUSEPROGRAMSTAGESEXTPROC) load(userptr, "glUseProgramStagesEXT");
    context->ValidateProgramPipelineEXT = (PFNGLVALIDATEPROGRAMPIPELINEEXTPROC) load(userptr, "glValidateProgramPipelineEXT");
}
static void glad_gl_load_GL_EXT_shader_framebuffer_fetch_non_coherent(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_shader_framebuffer_fetch_non_coherent) return;
    context->FramebufferFetchBarrierEXT = (PFNGLFRAMEBUFFERFETCHBARRIEREXTPROC) load(userptr, "glFramebufferFetchBarrierEXT");
}
static void glad_gl_load_GL_EXT_shader_pixel_local_storage2(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_shader_pixel_local_storage2) return;
    context->ClearPixelLocalStorageuiEXT = (PFNGLCLEARPIXELLOCALSTORAGEUIEXTPROC) load(userptr, "glClearPixelLocalStorageuiEXT");
    context->FramebufferPixelLocalStorageSizeEXT = (PFNGLFRAMEBUFFERPIXELLOCALSTORAGESIZEEXTPROC) load(userptr, "glFramebufferPixelLocalStorageSizeEXT");
    context->GetFramebufferPixelLocalStorageSizeEXT = (PFNGLGETFRAMEBUFFERPIXELLOCALSTORAGESIZEEXTPROC) load(userptr, "glGetFramebufferPixelLocalStorageSizeEXT");
}
static void glad_gl_load_GL_EXT_sparse_texture(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_sparse_texture) return;
    context->TexPageCommitmentEXT = (PFNGLTEXPAGECOMMITMENTEXTPROC) load(userptr, "glTexPageCommitmentEXT");
}
static void glad_gl_load_GL_EXT_tessellation_shader(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_tessellation_shader) return;
    context->PatchParameteriEXT = (PFNGLPATCHPARAMETERIEXTPROC) load(userptr, "glPatchParameteriEXT");
}
static void glad_gl_load_GL_EXT_texture_border_clamp(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_texture_border_clamp) return;
    context->GetSamplerParameterIivEXT = (PFNGLGETSAMPLERPARAMETERIIVEXTPROC) load(userptr, "glGetSamplerParameterIivEXT");
    context->GetSamplerParameterIuivEXT = (PFNGLGETSAMPLERPARAMETERIUIVEXTPROC) load(userptr, "glGetSamplerParameterIuivEXT");
    context->GetTexParameterIivEXT = (PFNGLGETTEXPARAMETERIIVEXTPROC) load(userptr, "glGetTexParameterIivEXT");
    context->GetTexParameterIuivEXT = (PFNGLGETTEXPARAMETERIUIVEXTPROC) load(userptr, "glGetTexParameterIuivEXT");
    context->SamplerParameterIivEXT = (PFNGLSAMPLERPARAMETERIIVEXTPROC) load(userptr, "glSamplerParameterIivEXT");
    context->SamplerParameterIuivEXT = (PFNGLSAMPLERPARAMETERIUIVEXTPROC) load(userptr, "glSamplerParameterIuivEXT");
    context->TexParameterIivEXT = (PFNGLTEXPARAMETERIIVEXTPROC) load(userptr, "glTexParameterIivEXT");
    context->TexParameterIuivEXT = (PFNGLTEXPARAMETERIUIVEXTPROC) load(userptr, "glTexParameterIuivEXT");
}
static void glad_gl_load_GL_EXT_texture_buffer(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_texture_buffer) return;
    context->TexBufferEXT = (PFNGLTEXBUFFEREXTPROC) load(userptr, "glTexBufferEXT");
    context->TexBufferRangeEXT = (PFNGLTEXBUFFERRANGEEXTPROC) load(userptr, "glTexBufferRangeEXT");
}
static void glad_gl_load_GL_EXT_texture_storage(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_texture_storage) return;
    context->TexStorage1DEXT = (PFNGLTEXSTORAGE1DEXTPROC) load(userptr, "glTexStorage1DEXT");
    context->TexStorage2DEXT = (PFNGLTEXSTORAGE2DEXTPROC) load(userptr, "glTexStorage2DEXT");
    context->TexStorage3DEXT = (PFNGLTEXSTORAGE3DEXTPROC) load(userptr, "glTexStorage3DEXT");
    context->TextureStorage1DEXT = (PFNGLTEXTURESTORAGE1DEXTPROC) load(userptr, "glTextureStorage1DEXT");
    context->TextureStorage2DEXT = (PFNGLTEXTURESTORAGE2DEXTPROC) load(userptr, "glTextureStorage2DEXT");
    context->TextureStorage3DEXT = (PFNGLTEXTURESTORAGE3DEXTPROC) load(userptr, "glTextureStorage3DEXT");
}
static void glad_gl_load_GL_EXT_texture_storage_compression(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_texture_storage_compression) return;
    context->TexStorageAttribs2DEXT = (PFNGLTEXSTORAGEATTRIBS2DEXTPROC) load(userptr, "glTexStorageAttribs2DEXT");
    context->TexStorageAttribs3DEXT = (PFNGLTEXSTORAGEATTRIBS3DEXTPROC) load(userptr, "glTexStorageAttribs3DEXT");
}
static void glad_gl_load_GL_EXT_texture_view(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_texture_view) return;
    context->TextureViewEXT = (PFNGLTEXTUREVIEWEXTPROC) load(userptr, "glTextureViewEXT");
}
static void glad_gl_load_GL_EXT_win32_keyed_mutex(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_win32_keyed_mutex) return;
    context->AcquireKeyedMutexWin32EXT = (PFNGLACQUIREKEYEDMUTEXWIN32EXTPROC) load(userptr, "glAcquireKeyedMutexWin32EXT");
    context->ReleaseKeyedMutexWin32EXT = (PFNGLRELEASEKEYEDMUTEXWIN32EXTPROC) load(userptr, "glReleaseKeyedMutexWin32EXT");
}
static void glad_gl_load_GL_EXT_window_rectangles(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->EXT_window_rectangles) return;
    context->WindowRectanglesEXT = (PFNGLWINDOWRECTANGLESEXTPROC) load(userptr, "glWindowRectanglesEXT");
}
static void glad_gl_load_GL_IMG_bindless_texture(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->IMG_bindless_texture) return;
    context->GetTextureHandleIMG = (PFNGLGETTEXTUREHANDLEIMGPROC) load(userptr, "glGetTextureHandleIMG");
    context->GetTextureSamplerHandleIMG = (PFNGLGETTEXTURESAMPLERHANDLEIMGPROC) load(userptr, "glGetTextureSamplerHandleIMG");
    context->ProgramUniformHandleui64IMG = (PFNGLPROGRAMUNIFORMHANDLEUI64IMGPROC) load(userptr, "glProgramUniformHandleui64IMG");
    context->ProgramUniformHandleui64vIMG = (PFNGLPROGRAMUNIFORMHANDLEUI64VIMGPROC) load(userptr, "glProgramUniformHandleui64vIMG");
    context->UniformHandleui64IMG = (PFNGLUNIFORMHANDLEUI64IMGPROC) load(userptr, "glUniformHandleui64IMG");
    context->UniformHandleui64vIMG = (PFNGLUNIFORMHANDLEUI64VIMGPROC) load(userptr, "glUniformHandleui64vIMG");
}
static void glad_gl_load_GL_IMG_framebuffer_downsample(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->IMG_framebuffer_downsample) return;
    context->FramebufferTexture2DDownsampleIMG = (PFNGLFRAMEBUFFERTEXTURE2DDOWNSAMPLEIMGPROC) load(userptr, "glFramebufferTexture2DDownsampleIMG");
    context->FramebufferTextureLayerDownsampleIMG = (PFNGLFRAMEBUFFERTEXTURELAYERDOWNSAMPLEIMGPROC) load(userptr, "glFramebufferTextureLayerDownsampleIMG");
}
static void glad_gl_load_GL_IMG_multisampled_render_to_texture(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->IMG_multisampled_render_to_texture) return;
    context->FramebufferTexture2DMultisampleIMG = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC) load(userptr, "glFramebufferTexture2DMultisampleIMG");
    context->RenderbufferStorageMultisampleIMG = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC) load(userptr, "glRenderbufferStorageMultisampleIMG");
}
static void glad_gl_load_GL_INTEL_framebuffer_CMAA(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->INTEL_framebuffer_CMAA) return;
    context->ApplyFramebufferAttachmentCMAAINTEL = (PFNGLAPPLYFRAMEBUFFERATTACHMENTCMAAINTELPROC) load(userptr, "glApplyFramebufferAttachmentCMAAINTEL");
}
static void glad_gl_load_GL_INTEL_performance_query(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->INTEL_performance_query) return;
    context->BeginPerfQueryINTEL = (PFNGLBEGINPERFQUERYINTELPROC) load(userptr, "glBeginPerfQueryINTEL");
    context->CreatePerfQueryINTEL = (PFNGLCREATEPERFQUERYINTELPROC) load(userptr, "glCreatePerfQueryINTEL");
    context->DeletePerfQueryINTEL = (PFNGLDELETEPERFQUERYINTELPROC) load(userptr, "glDeletePerfQueryINTEL");
    context->EndPerfQueryINTEL = (PFNGLENDPERFQUERYINTELPROC) load(userptr, "glEndPerfQueryINTEL");
    context->GetFirstPerfQueryIdINTEL = (PFNGLGETFIRSTPERFQUERYIDINTELPROC) load(userptr, "glGetFirstPerfQueryIdINTEL");
    context->GetNextPerfQueryIdINTEL = (PFNGLGETNEXTPERFQUERYIDINTELPROC) load(userptr, "glGetNextPerfQueryIdINTEL");
    context->GetPerfCounterInfoINTEL = (PFNGLGETPERFCOUNTERINFOINTELPROC) load(userptr, "glGetPerfCounterInfoINTEL");
    context->GetPerfQueryDataINTEL = (PFNGLGETPERFQUERYDATAINTELPROC) load(userptr, "glGetPerfQueryDataINTEL");
    context->GetPerfQueryIdByNameINTEL = (PFNGLGETPERFQUERYIDBYNAMEINTELPROC) load(userptr, "glGetPerfQueryIdByNameINTEL");
    context->GetPerfQueryInfoINTEL = (PFNGLGETPERFQUERYINFOINTELPROC) load(userptr, "glGetPerfQueryInfoINTEL");
}
static void glad_gl_load_GL_KHR_blend_equation_advanced(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_blend_equation_advanced) return;
    context->BlendBarrierKHR = (PFNGLBLENDBARRIERKHRPROC) load(userptr, "glBlendBarrierKHR");
}
static void glad_gl_load_GL_KHR_debug(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_debug) return;
    context->DebugMessageCallbackKHR = (PFNGLDEBUGMESSAGECALLBACKKHRPROC) load(userptr, "glDebugMessageCallbackKHR");
    context->DebugMessageControlKHR = (PFNGLDEBUGMESSAGECONTROLKHRPROC) load(userptr, "glDebugMessageControlKHR");
    context->DebugMessageInsertKHR = (PFNGLDEBUGMESSAGEINSERTKHRPROC) load(userptr, "glDebugMessageInsertKHR");
    context->GetDebugMessageLogKHR = (PFNGLGETDEBUGMESSAGELOGKHRPROC) load(userptr, "glGetDebugMessageLogKHR");
    context->GetObjectLabelKHR = (PFNGLGETOBJECTLABELKHRPROC) load(userptr, "glGetObjectLabelKHR");
    context->GetObjectPtrLabelKHR = (PFNGLGETOBJECTPTRLABELKHRPROC) load(userptr, "glGetObjectPtrLabelKHR");
    context->GetPointervKHR = (PFNGLGETPOINTERVKHRPROC) load(userptr, "glGetPointervKHR");
    context->ObjectLabelKHR = (PFNGLOBJECTLABELKHRPROC) load(userptr, "glObjectLabelKHR");
    context->ObjectPtrLabelKHR = (PFNGLOBJECTPTRLABELKHRPROC) load(userptr, "glObjectPtrLabelKHR");
    context->PopDebugGroupKHR = (PFNGLPOPDEBUGGROUPKHRPROC) load(userptr, "glPopDebugGroupKHR");
    context->PushDebugGroupKHR = (PFNGLPUSHDEBUGGROUPKHRPROC) load(userptr, "glPushDebugGroupKHR");
}
static void glad_gl_load_GL_KHR_parallel_shader_compile(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_parallel_shader_compile) return;
    context->MaxShaderCompilerThreadsKHR = (PFNGLMAXSHADERCOMPILERTHREADSKHRPROC) load(userptr, "glMaxShaderCompilerThreadsKHR");
}
static void glad_gl_load_GL_KHR_robustness(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->KHR_robustness) return;
    context->GetGraphicsResetStatusKHR = (PFNGLGETGRAPHICSRESETSTATUSKHRPROC) load(userptr, "glGetGraphicsResetStatusKHR");
    context->GetnUniformfvKHR = (PFNGLGETNUNIFORMFVKHRPROC) load(userptr, "glGetnUniformfvKHR");
    context->GetnUniformivKHR = (PFNGLGETNUNIFORMIVKHRPROC) load(userptr, "glGetnUniformivKHR");
    context->GetnUniformuivKHR = (PFNGLGETNUNIFORMUIVKHRPROC) load(userptr, "glGetnUniformuivKHR");
    context->ReadnPixelsKHR = (PFNGLREADNPIXELSKHRPROC) load(userptr, "glReadnPixelsKHR");
}
static void glad_gl_load_GL_MESA_framebuffer_flip_y(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->MESA_framebuffer_flip_y) return;
    context->FramebufferParameteriMESA = (PFNGLFRAMEBUFFERPARAMETERIMESAPROC) load(userptr, "glFramebufferParameteriMESA");
    context->GetFramebufferParameterivMESA = (PFNGLGETFRAMEBUFFERPARAMETERIVMESAPROC) load(userptr, "glGetFramebufferParameterivMESA");
}
static void glad_gl_load_GL_MESA_sampler_objects(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->MESA_sampler_objects) return;
    context->BindSampler = (PFNGLBINDSAMPLERPROC) load(userptr, "glBindSampler");
    context->DeleteSamplers = (PFNGLDELETESAMPLERSPROC) load(userptr, "glDeleteSamplers");
    context->GenSamplers = (PFNGLGENSAMPLERSPROC) load(userptr, "glGenSamplers");
    context->GetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC) load(userptr, "glGetSamplerParameterfv");
    context->GetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC) load(userptr, "glGetSamplerParameteriv");
    context->IsSampler = (PFNGLISSAMPLERPROC) load(userptr, "glIsSampler");
    context->SamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC) load(userptr, "glSamplerParameterf");
    context->SamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC) load(userptr, "glSamplerParameterfv");
    context->SamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC) load(userptr, "glSamplerParameteri");
    context->SamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC) load(userptr, "glSamplerParameteriv");
}
static void glad_gl_load_GL_NV_bindless_texture(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_bindless_texture) return;
    context->GetImageHandleNV = (PFNGLGETIMAGEHANDLENVPROC) load(userptr, "glGetImageHandleNV");
    context->GetTextureHandleNV = (PFNGLGETTEXTUREHANDLENVPROC) load(userptr, "glGetTextureHandleNV");
    context->GetTextureSamplerHandleNV = (PFNGLGETTEXTURESAMPLERHANDLENVPROC) load(userptr, "glGetTextureSamplerHandleNV");
    context->IsImageHandleResidentNV = (PFNGLISIMAGEHANDLERESIDENTNVPROC) load(userptr, "glIsImageHandleResidentNV");
    context->IsTextureHandleResidentNV = (PFNGLISTEXTUREHANDLERESIDENTNVPROC) load(userptr, "glIsTextureHandleResidentNV");
    context->MakeImageHandleNonResidentNV = (PFNGLMAKEIMAGEHANDLENONRESIDENTNVPROC) load(userptr, "glMakeImageHandleNonResidentNV");
    context->MakeImageHandleResidentNV = (PFNGLMAKEIMAGEHANDLERESIDENTNVPROC) load(userptr, "glMakeImageHandleResidentNV");
    context->MakeTextureHandleNonResidentNV = (PFNGLMAKETEXTUREHANDLENONRESIDENTNVPROC) load(userptr, "glMakeTextureHandleNonResidentNV");
    context->MakeTextureHandleResidentNV = (PFNGLMAKETEXTUREHANDLERESIDENTNVPROC) load(userptr, "glMakeTextureHandleResidentNV");
    context->ProgramUniformHandleui64NV = (PFNGLPROGRAMUNIFORMHANDLEUI64NVPROC) load(userptr, "glProgramUniformHandleui64NV");
    context->ProgramUniformHandleui64vNV = (PFNGLPROGRAMUNIFORMHANDLEUI64VNVPROC) load(userptr, "glProgramUniformHandleui64vNV");
    context->UniformHandleui64NV = (PFNGLUNIFORMHANDLEUI64NVPROC) load(userptr, "glUniformHandleui64NV");
    context->UniformHandleui64vNV = (PFNGLUNIFORMHANDLEUI64VNVPROC) load(userptr, "glUniformHandleui64vNV");
}
static void glad_gl_load_GL_NV_blend_equation_advanced(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_blend_equation_advanced) return;
    context->BlendBarrierNV = (PFNGLBLENDBARRIERNVPROC) load(userptr, "glBlendBarrierNV");
    context->BlendParameteriNV = (PFNGLBLENDPARAMETERINVPROC) load(userptr, "glBlendParameteriNV");
}
static void glad_gl_load_GL_NV_clip_space_w_scaling(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_clip_space_w_scaling) return;
    context->ViewportPositionWScaleNV = (PFNGLVIEWPORTPOSITIONWSCALENVPROC) load(userptr, "glViewportPositionWScaleNV");
}
static void glad_gl_load_GL_NV_conditional_render(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_conditional_render) return;
    context->BeginConditionalRenderNV = (PFNGLBEGINCONDITIONALRENDERNVPROC) load(userptr, "glBeginConditionalRenderNV");
    context->EndConditionalRenderNV = (PFNGLENDCONDITIONALRENDERNVPROC) load(userptr, "glEndConditionalRenderNV");
}
static void glad_gl_load_GL_NV_conservative_raster(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_conservative_raster) return;
    context->SubpixelPrecisionBiasNV = (PFNGLSUBPIXELPRECISIONBIASNVPROC) load(userptr, "glSubpixelPrecisionBiasNV");
}
static void glad_gl_load_GL_NV_conservative_raster_pre_snap_triangles(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_conservative_raster_pre_snap_triangles) return;
    context->ConservativeRasterParameteriNV = (PFNGLCONSERVATIVERASTERPARAMETERINVPROC) load(userptr, "glConservativeRasterParameteriNV");
}
static void glad_gl_load_GL_NV_copy_buffer(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_copy_buffer) return;
    context->CopyBufferSubDataNV = (PFNGLCOPYBUFFERSUBDATANVPROC) load(userptr, "glCopyBufferSubDataNV");
}
static void glad_gl_load_GL_NV_coverage_sample(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_coverage_sample) return;
    context->CoverageMaskNV = (PFNGLCOVERAGEMASKNVPROC) load(userptr, "glCoverageMaskNV");
    context->CoverageOperationNV = (PFNGLCOVERAGEOPERATIONNVPROC) load(userptr, "glCoverageOperationNV");
}
static void glad_gl_load_GL_NV_draw_buffers(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_draw_buffers) return;
    context->DrawBuffersNV = (PFNGLDRAWBUFFERSNVPROC) load(userptr, "glDrawBuffersNV");
}
static void glad_gl_load_GL_NV_draw_instanced(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_draw_instanced) return;
    context->DrawArraysInstancedNV = (PFNGLDRAWARRAYSINSTANCEDNVPROC) load(userptr, "glDrawArraysInstancedNV");
    context->DrawElementsInstancedNV = (PFNGLDRAWELEMENTSINSTANCEDNVPROC) load(userptr, "glDrawElementsInstancedNV");
}
static void glad_gl_load_GL_NV_draw_vulkan_image(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_draw_vulkan_image) return;
    context->DrawVkImageNV = (PFNGLDRAWVKIMAGENVPROC) load(userptr, "glDrawVkImageNV");
    context->GetVkProcAddrNV = (PFNGLGETVKPROCADDRNVPROC) load(userptr, "glGetVkProcAddrNV");
    context->SignalVkFenceNV = (PFNGLSIGNALVKFENCENVPROC) load(userptr, "glSignalVkFenceNV");
    context->SignalVkSemaphoreNV = (PFNGLSIGNALVKSEMAPHORENVPROC) load(userptr, "glSignalVkSemaphoreNV");
    context->WaitVkSemaphoreNV = (PFNGLWAITVKSEMAPHORENVPROC) load(userptr, "glWaitVkSemaphoreNV");
}
static void glad_gl_load_GL_NV_fence(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_fence) return;
    context->DeleteFencesNV = (PFNGLDELETEFENCESNVPROC) load(userptr, "glDeleteFencesNV");
    context->FinishFenceNV = (PFNGLFINISHFENCENVPROC) load(userptr, "glFinishFenceNV");
    context->GenFencesNV = (PFNGLGENFENCESNVPROC) load(userptr, "glGenFencesNV");
    context->GetFenceivNV = (PFNGLGETFENCEIVNVPROC) load(userptr, "glGetFenceivNV");
    context->IsFenceNV = (PFNGLISFENCENVPROC) load(userptr, "glIsFenceNV");
    context->SetFenceNV = (PFNGLSETFENCENVPROC) load(userptr, "glSetFenceNV");
    context->TestFenceNV = (PFNGLTESTFENCENVPROC) load(userptr, "glTestFenceNV");
}
static void glad_gl_load_GL_NV_fragment_coverage_to_color(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_fragment_coverage_to_color) return;
    context->FragmentCoverageColorNV = (PFNGLFRAGMENTCOVERAGECOLORNVPROC) load(userptr, "glFragmentCoverageColorNV");
}
static void glad_gl_load_GL_NV_framebuffer_blit(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_framebuffer_blit) return;
    context->BlitFramebufferNV = (PFNGLBLITFRAMEBUFFERNVPROC) load(userptr, "glBlitFramebufferNV");
}
static void glad_gl_load_GL_NV_framebuffer_mixed_samples(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_framebuffer_mixed_samples) return;
    context->CoverageModulationNV = (PFNGLCOVERAGEMODULATIONNVPROC) load(userptr, "glCoverageModulationNV");
    context->CoverageModulationTableNV = (PFNGLCOVERAGEMODULATIONTABLENVPROC) load(userptr, "glCoverageModulationTableNV");
    context->GetCoverageModulationTableNV = (PFNGLGETCOVERAGEMODULATIONTABLENVPROC) load(userptr, "glGetCoverageModulationTableNV");
    context->RasterSamplesEXT = (PFNGLRASTERSAMPLESEXTPROC) load(userptr, "glRasterSamplesEXT");
}
static void glad_gl_load_GL_NV_framebuffer_multisample(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_framebuffer_multisample) return;
    context->RenderbufferStorageMultisampleNV = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLENVPROC) load(userptr, "glRenderbufferStorageMultisampleNV");
}
static void glad_gl_load_GL_NV_gpu_shader5(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_gpu_shader5) return;
    context->GetUniformi64vNV = (PFNGLGETUNIFORMI64VNVPROC) load(userptr, "glGetUniformi64vNV");
    context->ProgramUniform1i64NV = (PFNGLPROGRAMUNIFORM1I64NVPROC) load(userptr, "glProgramUniform1i64NV");
    context->ProgramUniform1i64vNV = (PFNGLPROGRAMUNIFORM1I64VNVPROC) load(userptr, "glProgramUniform1i64vNV");
    context->ProgramUniform1ui64NV = (PFNGLPROGRAMUNIFORM1UI64NVPROC) load(userptr, "glProgramUniform1ui64NV");
    context->ProgramUniform1ui64vNV = (PFNGLPROGRAMUNIFORM1UI64VNVPROC) load(userptr, "glProgramUniform1ui64vNV");
    context->ProgramUniform2i64NV = (PFNGLPROGRAMUNIFORM2I64NVPROC) load(userptr, "glProgramUniform2i64NV");
    context->ProgramUniform2i64vNV = (PFNGLPROGRAMUNIFORM2I64VNVPROC) load(userptr, "glProgramUniform2i64vNV");
    context->ProgramUniform2ui64NV = (PFNGLPROGRAMUNIFORM2UI64NVPROC) load(userptr, "glProgramUniform2ui64NV");
    context->ProgramUniform2ui64vNV = (PFNGLPROGRAMUNIFORM2UI64VNVPROC) load(userptr, "glProgramUniform2ui64vNV");
    context->ProgramUniform3i64NV = (PFNGLPROGRAMUNIFORM3I64NVPROC) load(userptr, "glProgramUniform3i64NV");
    context->ProgramUniform3i64vNV = (PFNGLPROGRAMUNIFORM3I64VNVPROC) load(userptr, "glProgramUniform3i64vNV");
    context->ProgramUniform3ui64NV = (PFNGLPROGRAMUNIFORM3UI64NVPROC) load(userptr, "glProgramUniform3ui64NV");
    context->ProgramUniform3ui64vNV = (PFNGLPROGRAMUNIFORM3UI64VNVPROC) load(userptr, "glProgramUniform3ui64vNV");
    context->ProgramUniform4i64NV = (PFNGLPROGRAMUNIFORM4I64NVPROC) load(userptr, "glProgramUniform4i64NV");
    context->ProgramUniform4i64vNV = (PFNGLPROGRAMUNIFORM4I64VNVPROC) load(userptr, "glProgramUniform4i64vNV");
    context->ProgramUniform4ui64NV = (PFNGLPROGRAMUNIFORM4UI64NVPROC) load(userptr, "glProgramUniform4ui64NV");
    context->ProgramUniform4ui64vNV = (PFNGLPROGRAMUNIFORM4UI64VNVPROC) load(userptr, "glProgramUniform4ui64vNV");
    context->Uniform1i64NV = (PFNGLUNIFORM1I64NVPROC) load(userptr, "glUniform1i64NV");
    context->Uniform1i64vNV = (PFNGLUNIFORM1I64VNVPROC) load(userptr, "glUniform1i64vNV");
    context->Uniform1ui64NV = (PFNGLUNIFORM1UI64NVPROC) load(userptr, "glUniform1ui64NV");
    context->Uniform1ui64vNV = (PFNGLUNIFORM1UI64VNVPROC) load(userptr, "glUniform1ui64vNV");
    context->Uniform2i64NV = (PFNGLUNIFORM2I64NVPROC) load(userptr, "glUniform2i64NV");
    context->Uniform2i64vNV = (PFNGLUNIFORM2I64VNVPROC) load(userptr, "glUniform2i64vNV");
    context->Uniform2ui64NV = (PFNGLUNIFORM2UI64NVPROC) load(userptr, "glUniform2ui64NV");
    context->Uniform2ui64vNV = (PFNGLUNIFORM2UI64VNVPROC) load(userptr, "glUniform2ui64vNV");
    context->Uniform3i64NV = (PFNGLUNIFORM3I64NVPROC) load(userptr, "glUniform3i64NV");
    context->Uniform3i64vNV = (PFNGLUNIFORM3I64VNVPROC) load(userptr, "glUniform3i64vNV");
    context->Uniform3ui64NV = (PFNGLUNIFORM3UI64NVPROC) load(userptr, "glUniform3ui64NV");
    context->Uniform3ui64vNV = (PFNGLUNIFORM3UI64VNVPROC) load(userptr, "glUniform3ui64vNV");
    context->Uniform4i64NV = (PFNGLUNIFORM4I64NVPROC) load(userptr, "glUniform4i64NV");
    context->Uniform4i64vNV = (PFNGLUNIFORM4I64VNVPROC) load(userptr, "glUniform4i64vNV");
    context->Uniform4ui64NV = (PFNGLUNIFORM4UI64NVPROC) load(userptr, "glUniform4ui64NV");
    context->Uniform4ui64vNV = (PFNGLUNIFORM4UI64VNVPROC) load(userptr, "glUniform4ui64vNV");
}
static void glad_gl_load_GL_NV_instanced_arrays(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_instanced_arrays) return;
    context->VertexAttribDivisorNV = (PFNGLVERTEXATTRIBDIVISORNVPROC) load(userptr, "glVertexAttribDivisorNV");
}
static void glad_gl_load_GL_NV_internalformat_sample_query(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_internalformat_sample_query) return;
    context->GetInternalformatSampleivNV = (PFNGLGETINTERNALFORMATSAMPLEIVNVPROC) load(userptr, "glGetInternalformatSampleivNV");
}
static void glad_gl_load_GL_NV_memory_attachment(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_memory_attachment) return;
    context->BufferAttachMemoryNV = (PFNGLBUFFERATTACHMEMORYNVPROC) load(userptr, "glBufferAttachMemoryNV");
    context->GetMemoryObjectDetachedResourcesuivNV = (PFNGLGETMEMORYOBJECTDETACHEDRESOURCESUIVNVPROC) load(userptr, "glGetMemoryObjectDetachedResourcesuivNV");
    context->NamedBufferAttachMemoryNV = (PFNGLNAMEDBUFFERATTACHMEMORYNVPROC) load(userptr, "glNamedBufferAttachMemoryNV");
    context->ResetMemoryObjectParameterNV = (PFNGLRESETMEMORYOBJECTPARAMETERNVPROC) load(userptr, "glResetMemoryObjectParameterNV");
    context->TexAttachMemoryNV = (PFNGLTEXATTACHMEMORYNVPROC) load(userptr, "glTexAttachMemoryNV");
    context->TextureAttachMemoryNV = (PFNGLTEXTUREATTACHMEMORYNVPROC) load(userptr, "glTextureAttachMemoryNV");
}
static void glad_gl_load_GL_NV_memory_object_sparse(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_memory_object_sparse) return;
    context->BufferPageCommitmentMemNV = (PFNGLBUFFERPAGECOMMITMENTMEMNVPROC) load(userptr, "glBufferPageCommitmentMemNV");
    context->NamedBufferPageCommitmentMemNV = (PFNGLNAMEDBUFFERPAGECOMMITMENTMEMNVPROC) load(userptr, "glNamedBufferPageCommitmentMemNV");
    context->TexPageCommitmentMemNV = (PFNGLTEXPAGECOMMITMENTMEMNVPROC) load(userptr, "glTexPageCommitmentMemNV");
    context->TexturePageCommitmentMemNV = (PFNGLTEXTUREPAGECOMMITMENTMEMNVPROC) load(userptr, "glTexturePageCommitmentMemNV");
}
static void glad_gl_load_GL_NV_mesh_shader(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_mesh_shader) return;
    context->DrawMeshTasksIndirectNV = (PFNGLDRAWMESHTASKSINDIRECTNVPROC) load(userptr, "glDrawMeshTasksIndirectNV");
    context->DrawMeshTasksNV = (PFNGLDRAWMESHTASKSNVPROC) load(userptr, "glDrawMeshTasksNV");
    context->MultiDrawMeshTasksIndirectCountNV = (PFNGLMULTIDRAWMESHTASKSINDIRECTCOUNTNVPROC) load(userptr, "glMultiDrawMeshTasksIndirectCountNV");
    context->MultiDrawMeshTasksIndirectNV = (PFNGLMULTIDRAWMESHTASKSINDIRECTNVPROC) load(userptr, "glMultiDrawMeshTasksIndirectNV");
}
static void glad_gl_load_GL_NV_non_square_matrices(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_non_square_matrices) return;
    context->UniformMatrix2x3fvNV = (PFNGLUNIFORMMATRIX2X3FVNVPROC) load(userptr, "glUniformMatrix2x3fvNV");
    context->UniformMatrix2x4fvNV = (PFNGLUNIFORMMATRIX2X4FVNVPROC) load(userptr, "glUniformMatrix2x4fvNV");
    context->UniformMatrix3x2fvNV = (PFNGLUNIFORMMATRIX3X2FVNVPROC) load(userptr, "glUniformMatrix3x2fvNV");
    context->UniformMatrix3x4fvNV = (PFNGLUNIFORMMATRIX3X4FVNVPROC) load(userptr, "glUniformMatrix3x4fvNV");
    context->UniformMatrix4x2fvNV = (PFNGLUNIFORMMATRIX4X2FVNVPROC) load(userptr, "glUniformMatrix4x2fvNV");
    context->UniformMatrix4x3fvNV = (PFNGLUNIFORMMATRIX4X3FVNVPROC) load(userptr, "glUniformMatrix4x3fvNV");
}
static void glad_gl_load_GL_NV_path_rendering(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_path_rendering) return;
    context->CopyPathNV = (PFNGLCOPYPATHNVPROC) load(userptr, "glCopyPathNV");
    context->CoverFillPathInstancedNV = (PFNGLCOVERFILLPATHINSTANCEDNVPROC) load(userptr, "glCoverFillPathInstancedNV");
    context->CoverFillPathNV = (PFNGLCOVERFILLPATHNVPROC) load(userptr, "glCoverFillPathNV");
    context->CoverStrokePathInstancedNV = (PFNGLCOVERSTROKEPATHINSTANCEDNVPROC) load(userptr, "glCoverStrokePathInstancedNV");
    context->CoverStrokePathNV = (PFNGLCOVERSTROKEPATHNVPROC) load(userptr, "glCoverStrokePathNV");
    context->DeletePathsNV = (PFNGLDELETEPATHSNVPROC) load(userptr, "glDeletePathsNV");
    context->GenPathsNV = (PFNGLGENPATHSNVPROC) load(userptr, "glGenPathsNV");
    context->GetPathCommandsNV = (PFNGLGETPATHCOMMANDSNVPROC) load(userptr, "glGetPathCommandsNV");
    context->GetPathCoordsNV = (PFNGLGETPATHCOORDSNVPROC) load(userptr, "glGetPathCoordsNV");
    context->GetPathDashArrayNV = (PFNGLGETPATHDASHARRAYNVPROC) load(userptr, "glGetPathDashArrayNV");
    context->GetPathLengthNV = (PFNGLGETPATHLENGTHNVPROC) load(userptr, "glGetPathLengthNV");
    context->GetPathMetricRangeNV = (PFNGLGETPATHMETRICRANGENVPROC) load(userptr, "glGetPathMetricRangeNV");
    context->GetPathMetricsNV = (PFNGLGETPATHMETRICSNVPROC) load(userptr, "glGetPathMetricsNV");
    context->GetPathParameterfvNV = (PFNGLGETPATHPARAMETERFVNVPROC) load(userptr, "glGetPathParameterfvNV");
    context->GetPathParameterivNV = (PFNGLGETPATHPARAMETERIVNVPROC) load(userptr, "glGetPathParameterivNV");
    context->GetPathSpacingNV = (PFNGLGETPATHSPACINGNVPROC) load(userptr, "glGetPathSpacingNV");
    context->GetProgramResourcefvNV = (PFNGLGETPROGRAMRESOURCEFVNVPROC) load(userptr, "glGetProgramResourcefvNV");
    context->InterpolatePathsNV = (PFNGLINTERPOLATEPATHSNVPROC) load(userptr, "glInterpolatePathsNV");
    context->IsPathNV = (PFNGLISPATHNVPROC) load(userptr, "glIsPathNV");
    context->IsPointInFillPathNV = (PFNGLISPOINTINFILLPATHNVPROC) load(userptr, "glIsPointInFillPathNV");
    context->IsPointInStrokePathNV = (PFNGLISPOINTINSTROKEPATHNVPROC) load(userptr, "glIsPointInStrokePathNV");
    context->MatrixFrustumEXT = (PFNGLMATRIXFRUSTUMEXTPROC) load(userptr, "glMatrixFrustumEXT");
    context->MatrixLoad3x2fNV = (PFNGLMATRIXLOAD3X2FNVPROC) load(userptr, "glMatrixLoad3x2fNV");
    context->MatrixLoad3x3fNV = (PFNGLMATRIXLOAD3X3FNVPROC) load(userptr, "glMatrixLoad3x3fNV");
    context->MatrixLoadIdentityEXT = (PFNGLMATRIXLOADIDENTITYEXTPROC) load(userptr, "glMatrixLoadIdentityEXT");
    context->MatrixLoadTranspose3x3fNV = (PFNGLMATRIXLOADTRANSPOSE3X3FNVPROC) load(userptr, "glMatrixLoadTranspose3x3fNV");
    context->MatrixLoadTransposedEXT = (PFNGLMATRIXLOADTRANSPOSEDEXTPROC) load(userptr, "glMatrixLoadTransposedEXT");
    context->MatrixLoadTransposefEXT = (PFNGLMATRIXLOADTRANSPOSEFEXTPROC) load(userptr, "glMatrixLoadTransposefEXT");
    context->MatrixLoaddEXT = (PFNGLMATRIXLOADDEXTPROC) load(userptr, "glMatrixLoaddEXT");
    context->MatrixLoadfEXT = (PFNGLMATRIXLOADFEXTPROC) load(userptr, "glMatrixLoadfEXT");
    context->MatrixMult3x2fNV = (PFNGLMATRIXMULT3X2FNVPROC) load(userptr, "glMatrixMult3x2fNV");
    context->MatrixMult3x3fNV = (PFNGLMATRIXMULT3X3FNVPROC) load(userptr, "glMatrixMult3x3fNV");
    context->MatrixMultTranspose3x3fNV = (PFNGLMATRIXMULTTRANSPOSE3X3FNVPROC) load(userptr, "glMatrixMultTranspose3x3fNV");
    context->MatrixMultTransposedEXT = (PFNGLMATRIXMULTTRANSPOSEDEXTPROC) load(userptr, "glMatrixMultTransposedEXT");
    context->MatrixMultTransposefEXT = (PFNGLMATRIXMULTTRANSPOSEFEXTPROC) load(userptr, "glMatrixMultTransposefEXT");
    context->MatrixMultdEXT = (PFNGLMATRIXMULTDEXTPROC) load(userptr, "glMatrixMultdEXT");
    context->MatrixMultfEXT = (PFNGLMATRIXMULTFEXTPROC) load(userptr, "glMatrixMultfEXT");
    context->MatrixOrthoEXT = (PFNGLMATRIXORTHOEXTPROC) load(userptr, "glMatrixOrthoEXT");
    context->MatrixPopEXT = (PFNGLMATRIXPOPEXTPROC) load(userptr, "glMatrixPopEXT");
    context->MatrixPushEXT = (PFNGLMATRIXPUSHEXTPROC) load(userptr, "glMatrixPushEXT");
    context->MatrixRotatedEXT = (PFNGLMATRIXROTATEDEXTPROC) load(userptr, "glMatrixRotatedEXT");
    context->MatrixRotatefEXT = (PFNGLMATRIXROTATEFEXTPROC) load(userptr, "glMatrixRotatefEXT");
    context->MatrixScaledEXT = (PFNGLMATRIXSCALEDEXTPROC) load(userptr, "glMatrixScaledEXT");
    context->MatrixScalefEXT = (PFNGLMATRIXSCALEFEXTPROC) load(userptr, "glMatrixScalefEXT");
    context->MatrixTranslatedEXT = (PFNGLMATRIXTRANSLATEDEXTPROC) load(userptr, "glMatrixTranslatedEXT");
    context->MatrixTranslatefEXT = (PFNGLMATRIXTRANSLATEFEXTPROC) load(userptr, "glMatrixTranslatefEXT");
    context->PathCommandsNV = (PFNGLPATHCOMMANDSNVPROC) load(userptr, "glPathCommandsNV");
    context->PathCoordsNV = (PFNGLPATHCOORDSNVPROC) load(userptr, "glPathCoordsNV");
    context->PathCoverDepthFuncNV = (PFNGLPATHCOVERDEPTHFUNCNVPROC) load(userptr, "glPathCoverDepthFuncNV");
    context->PathDashArrayNV = (PFNGLPATHDASHARRAYNVPROC) load(userptr, "glPathDashArrayNV");
    context->PathGlyphIndexArrayNV = (PFNGLPATHGLYPHINDEXARRAYNVPROC) load(userptr, "glPathGlyphIndexArrayNV");
    context->PathGlyphIndexRangeNV = (PFNGLPATHGLYPHINDEXRANGENVPROC) load(userptr, "glPathGlyphIndexRangeNV");
    context->PathGlyphRangeNV = (PFNGLPATHGLYPHRANGENVPROC) load(userptr, "glPathGlyphRangeNV");
    context->PathGlyphsNV = (PFNGLPATHGLYPHSNVPROC) load(userptr, "glPathGlyphsNV");
    context->PathMemoryGlyphIndexArrayNV = (PFNGLPATHMEMORYGLYPHINDEXARRAYNVPROC) load(userptr, "glPathMemoryGlyphIndexArrayNV");
    context->PathParameterfNV = (PFNGLPATHPARAMETERFNVPROC) load(userptr, "glPathParameterfNV");
    context->PathParameterfvNV = (PFNGLPATHPARAMETERFVNVPROC) load(userptr, "glPathParameterfvNV");
    context->PathParameteriNV = (PFNGLPATHPARAMETERINVPROC) load(userptr, "glPathParameteriNV");
    context->PathParameterivNV = (PFNGLPATHPARAMETERIVNVPROC) load(userptr, "glPathParameterivNV");
    context->PathStencilDepthOffsetNV = (PFNGLPATHSTENCILDEPTHOFFSETNVPROC) load(userptr, "glPathStencilDepthOffsetNV");
    context->PathStencilFuncNV = (PFNGLPATHSTENCILFUNCNVPROC) load(userptr, "glPathStencilFuncNV");
    context->PathStringNV = (PFNGLPATHSTRINGNVPROC) load(userptr, "glPathStringNV");
    context->PathSubCommandsNV = (PFNGLPATHSUBCOMMANDSNVPROC) load(userptr, "glPathSubCommandsNV");
    context->PathSubCoordsNV = (PFNGLPATHSUBCOORDSNVPROC) load(userptr, "glPathSubCoordsNV");
    context->PointAlongPathNV = (PFNGLPOINTALONGPATHNVPROC) load(userptr, "glPointAlongPathNV");
    context->ProgramPathFragmentInputGenNV = (PFNGLPROGRAMPATHFRAGMENTINPUTGENNVPROC) load(userptr, "glProgramPathFragmentInputGenNV");
    context->StencilFillPathInstancedNV = (PFNGLSTENCILFILLPATHINSTANCEDNVPROC) load(userptr, "glStencilFillPathInstancedNV");
    context->StencilFillPathNV = (PFNGLSTENCILFILLPATHNVPROC) load(userptr, "glStencilFillPathNV");
    context->StencilStrokePathInstancedNV = (PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC) load(userptr, "glStencilStrokePathInstancedNV");
    context->StencilStrokePathNV = (PFNGLSTENCILSTROKEPATHNVPROC) load(userptr, "glStencilStrokePathNV");
    context->StencilThenCoverFillPathInstancedNV = (PFNGLSTENCILTHENCOVERFILLPATHINSTANCEDNVPROC) load(userptr, "glStencilThenCoverFillPathInstancedNV");
    context->StencilThenCoverFillPathNV = (PFNGLSTENCILTHENCOVERFILLPATHNVPROC) load(userptr, "glStencilThenCoverFillPathNV");
    context->StencilThenCoverStrokePathInstancedNV = (PFNGLSTENCILTHENCOVERSTROKEPATHINSTANCEDNVPROC) load(userptr, "glStencilThenCoverStrokePathInstancedNV");
    context->StencilThenCoverStrokePathNV = (PFNGLSTENCILTHENCOVERSTROKEPATHNVPROC) load(userptr, "glStencilThenCoverStrokePathNV");
    context->TransformPathNV = (PFNGLTRANSFORMPATHNVPROC) load(userptr, "glTransformPathNV");
    context->WeightPathsNV = (PFNGLWEIGHTPATHSNVPROC) load(userptr, "glWeightPathsNV");
}
static void glad_gl_load_GL_NV_polygon_mode(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_polygon_mode) return;
    context->PolygonModeNV = (PFNGLPOLYGONMODENVPROC) load(userptr, "glPolygonModeNV");
}
static void glad_gl_load_GL_NV_read_buffer(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_read_buffer) return;
    context->ReadBufferNV = (PFNGLREADBUFFERNVPROC) load(userptr, "glReadBufferNV");
}
static void glad_gl_load_GL_NV_sample_locations(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_sample_locations) return;
    context->FramebufferSampleLocationsfvNV = (PFNGLFRAMEBUFFERSAMPLELOCATIONSFVNVPROC) load(userptr, "glFramebufferSampleLocationsfvNV");
    context->NamedFramebufferSampleLocationsfvNV = (PFNGLNAMEDFRAMEBUFFERSAMPLELOCATIONSFVNVPROC) load(userptr, "glNamedFramebufferSampleLocationsfvNV");
    context->ResolveDepthValuesNV = (PFNGLRESOLVEDEPTHVALUESNVPROC) load(userptr, "glResolveDepthValuesNV");
}
static void glad_gl_load_GL_NV_scissor_exclusive(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_scissor_exclusive) return;
    context->ScissorExclusiveArrayvNV = (PFNGLSCISSOREXCLUSIVEARRAYVNVPROC) load(userptr, "glScissorExclusiveArrayvNV");
    context->ScissorExclusiveNV = (PFNGLSCISSOREXCLUSIVENVPROC) load(userptr, "glScissorExclusiveNV");
}
static void glad_gl_load_GL_NV_shading_rate_image(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_shading_rate_image) return;
    context->BindShadingRateImageNV = (PFNGLBINDSHADINGRATEIMAGENVPROC) load(userptr, "glBindShadingRateImageNV");
    context->GetShadingRateImagePaletteNV = (PFNGLGETSHADINGRATEIMAGEPALETTENVPROC) load(userptr, "glGetShadingRateImagePaletteNV");
    context->GetShadingRateSampleLocationivNV = (PFNGLGETSHADINGRATESAMPLELOCATIONIVNVPROC) load(userptr, "glGetShadingRateSampleLocationivNV");
    context->ShadingRateImageBarrierNV = (PFNGLSHADINGRATEIMAGEBARRIERNVPROC) load(userptr, "glShadingRateImageBarrierNV");
    context->ShadingRateImagePaletteNV = (PFNGLSHADINGRATEIMAGEPALETTENVPROC) load(userptr, "glShadingRateImagePaletteNV");
    context->ShadingRateSampleOrderCustomNV = (PFNGLSHADINGRATESAMPLEORDERCUSTOMNVPROC) load(userptr, "glShadingRateSampleOrderCustomNV");
    context->ShadingRateSampleOrderNV = (PFNGLSHADINGRATESAMPLEORDERNVPROC) load(userptr, "glShadingRateSampleOrderNV");
}
static void glad_gl_load_GL_NV_texture_barrier(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_texture_barrier) return;
    context->TextureBarrierNV = (PFNGLTEXTUREBARRIERNVPROC) load(userptr, "glTextureBarrierNV");
}
static void glad_gl_load_GL_NV_timeline_semaphore(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_timeline_semaphore) return;
    context->CreateSemaphoresNV = (PFNGLCREATESEMAPHORESNVPROC) load(userptr, "glCreateSemaphoresNV");
    context->GetSemaphoreParameterivNV = (PFNGLGETSEMAPHOREPARAMETERIVNVPROC) load(userptr, "glGetSemaphoreParameterivNV");
    context->SemaphoreParameterivNV = (PFNGLSEMAPHOREPARAMETERIVNVPROC) load(userptr, "glSemaphoreParameterivNV");
}
static void glad_gl_load_GL_NV_viewport_array(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_viewport_array) return;
    context->DepthRangeArrayfvNV = (PFNGLDEPTHRANGEARRAYFVNVPROC) load(userptr, "glDepthRangeArrayfvNV");
    context->DepthRangeIndexedfNV = (PFNGLDEPTHRANGEINDEXEDFNVPROC) load(userptr, "glDepthRangeIndexedfNV");
    context->DisableiNV = (PFNGLDISABLEINVPROC) load(userptr, "glDisableiNV");
    context->EnableiNV = (PFNGLENABLEINVPROC) load(userptr, "glEnableiNV");
    context->GetFloati_vNV = (PFNGLGETFLOATI_VNVPROC) load(userptr, "glGetFloati_vNV");
    context->IsEnablediNV = (PFNGLISENABLEDINVPROC) load(userptr, "glIsEnablediNV");
    context->ScissorArrayvNV = (PFNGLSCISSORARRAYVNVPROC) load(userptr, "glScissorArrayvNV");
    context->ScissorIndexedNV = (PFNGLSCISSORINDEXEDNVPROC) load(userptr, "glScissorIndexedNV");
    context->ScissorIndexedvNV = (PFNGLSCISSORINDEXEDVNVPROC) load(userptr, "glScissorIndexedvNV");
    context->ViewportArrayvNV = (PFNGLVIEWPORTARRAYVNVPROC) load(userptr, "glViewportArrayvNV");
    context->ViewportIndexedfNV = (PFNGLVIEWPORTINDEXEDFNVPROC) load(userptr, "glViewportIndexedfNV");
    context->ViewportIndexedfvNV = (PFNGLVIEWPORTINDEXEDFVNVPROC) load(userptr, "glViewportIndexedfvNV");
}
static void glad_gl_load_GL_NV_viewport_swizzle(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->NV_viewport_swizzle) return;
    context->ViewportSwizzleNV = (PFNGLVIEWPORTSWIZZLENVPROC) load(userptr, "glViewportSwizzleNV");
}
static void glad_gl_load_GL_OES_EGL_image(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_EGL_image) return;
    context->EGLImageTargetRenderbufferStorageOES = (PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC) load(userptr, "glEGLImageTargetRenderbufferStorageOES");
    context->EGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) load(userptr, "glEGLImageTargetTexture2DOES");
}
static void glad_gl_load_GL_OES_EGL_image_external(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_EGL_image_external) return;
    context->EGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) load(userptr, "glEGLImageTargetTexture2DOES");
}
static void glad_gl_load_GL_OES_copy_image(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_copy_image) return;
    context->CopyImageSubDataOES = (PFNGLCOPYIMAGESUBDATAOESPROC) load(userptr, "glCopyImageSubDataOES");
}
static void glad_gl_load_GL_OES_draw_buffers_indexed(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_draw_buffers_indexed) return;
    context->BlendEquationSeparateiOES = (PFNGLBLENDEQUATIONSEPARATEIOESPROC) load(userptr, "glBlendEquationSeparateiOES");
    context->BlendEquationiOES = (PFNGLBLENDEQUATIONIOESPROC) load(userptr, "glBlendEquationiOES");
    context->BlendFuncSeparateiOES = (PFNGLBLENDFUNCSEPARATEIOESPROC) load(userptr, "glBlendFuncSeparateiOES");
    context->BlendFunciOES = (PFNGLBLENDFUNCIOESPROC) load(userptr, "glBlendFunciOES");
    context->ColorMaskiOES = (PFNGLCOLORMASKIOESPROC) load(userptr, "glColorMaskiOES");
    context->DisableiOES = (PFNGLDISABLEIOESPROC) load(userptr, "glDisableiOES");
    context->EnableiOES = (PFNGLENABLEIOESPROC) load(userptr, "glEnableiOES");
    context->IsEnablediOES = (PFNGLISENABLEDIOESPROC) load(userptr, "glIsEnablediOES");
}
static void glad_gl_load_GL_OES_draw_elements_base_vertex(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_draw_elements_base_vertex) return;
    context->DrawElementsBaseVertexOES = (PFNGLDRAWELEMENTSBASEVERTEXOESPROC) load(userptr, "glDrawElementsBaseVertexOES");
    context->DrawElementsInstancedBaseVertexOES = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXOESPROC) load(userptr, "glDrawElementsInstancedBaseVertexOES");
    context->DrawRangeElementsBaseVertexOES = (PFNGLDRAWRANGEELEMENTSBASEVERTEXOESPROC) load(userptr, "glDrawRangeElementsBaseVertexOES");
    context->MultiDrawElementsBaseVertexEXT = (PFNGLMULTIDRAWELEMENTSBASEVERTEXEXTPROC) load(userptr, "glMultiDrawElementsBaseVertexEXT");
}
static void glad_gl_load_GL_OES_geometry_shader(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_geometry_shader) return;
    context->FramebufferTextureOES = (PFNGLFRAMEBUFFERTEXTUREOESPROC) load(userptr, "glFramebufferTextureOES");
}
static void glad_gl_load_GL_OES_get_program_binary(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_get_program_binary) return;
    context->GetProgramBinaryOES = (PFNGLGETPROGRAMBINARYOESPROC) load(userptr, "glGetProgramBinaryOES");
    context->ProgramBinaryOES = (PFNGLPROGRAMBINARYOESPROC) load(userptr, "glProgramBinaryOES");
}
static void glad_gl_load_GL_OES_mapbuffer(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_mapbuffer) return;
    context->GetBufferPointervOES = (PFNGLGETBUFFERPOINTERVOESPROC) load(userptr, "glGetBufferPointervOES");
    context->MapBufferOES = (PFNGLMAPBUFFEROESPROC) load(userptr, "glMapBufferOES");
    context->UnmapBufferOES = (PFNGLUNMAPBUFFEROESPROC) load(userptr, "glUnmapBufferOES");
}
static void glad_gl_load_GL_OES_primitive_bounding_box(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_primitive_bounding_box) return;
    context->PrimitiveBoundingBoxOES = (PFNGLPRIMITIVEBOUNDINGBOXOESPROC) load(userptr, "glPrimitiveBoundingBoxOES");
}
static void glad_gl_load_GL_OES_sample_shading(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_sample_shading) return;
    context->MinSampleShadingOES = (PFNGLMINSAMPLESHADINGOESPROC) load(userptr, "glMinSampleShadingOES");
}
static void glad_gl_load_GL_OES_tessellation_shader(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_tessellation_shader) return;
    context->PatchParameteriOES = (PFNGLPATCHPARAMETERIOESPROC) load(userptr, "glPatchParameteriOES");
}
static void glad_gl_load_GL_OES_texture_3D(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_texture_3D) return;
    context->CompressedTexImage3DOES = (PFNGLCOMPRESSEDTEXIMAGE3DOESPROC) load(userptr, "glCompressedTexImage3DOES");
    context->CompressedTexSubImage3DOES = (PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC) load(userptr, "glCompressedTexSubImage3DOES");
    context->CopyTexSubImage3DOES = (PFNGLCOPYTEXSUBIMAGE3DOESPROC) load(userptr, "glCopyTexSubImage3DOES");
    context->FramebufferTexture3DOES = (PFNGLFRAMEBUFFERTEXTURE3DOESPROC) load(userptr, "glFramebufferTexture3DOES");
    context->TexImage3DOES = (PFNGLTEXIMAGE3DOESPROC) load(userptr, "glTexImage3DOES");
    context->TexSubImage3DOES = (PFNGLTEXSUBIMAGE3DOESPROC) load(userptr, "glTexSubImage3DOES");
}
static void glad_gl_load_GL_OES_texture_border_clamp(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_texture_border_clamp) return;
    context->GetSamplerParameterIivOES = (PFNGLGETSAMPLERPARAMETERIIVOESPROC) load(userptr, "glGetSamplerParameterIivOES");
    context->GetSamplerParameterIuivOES = (PFNGLGETSAMPLERPARAMETERIUIVOESPROC) load(userptr, "glGetSamplerParameterIuivOES");
    context->GetTexParameterIivOES = (PFNGLGETTEXPARAMETERIIVOESPROC) load(userptr, "glGetTexParameterIivOES");
    context->GetTexParameterIuivOES = (PFNGLGETTEXPARAMETERIUIVOESPROC) load(userptr, "glGetTexParameterIuivOES");
    context->SamplerParameterIivOES = (PFNGLSAMPLERPARAMETERIIVOESPROC) load(userptr, "glSamplerParameterIivOES");
    context->SamplerParameterIuivOES = (PFNGLSAMPLERPARAMETERIUIVOESPROC) load(userptr, "glSamplerParameterIuivOES");
    context->TexParameterIivOES = (PFNGLTEXPARAMETERIIVOESPROC) load(userptr, "glTexParameterIivOES");
    context->TexParameterIuivOES = (PFNGLTEXPARAMETERIUIVOESPROC) load(userptr, "glTexParameterIuivOES");
}
static void glad_gl_load_GL_OES_texture_buffer(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_texture_buffer) return;
    context->TexBufferOES = (PFNGLTEXBUFFEROESPROC) load(userptr, "glTexBufferOES");
    context->TexBufferRangeOES = (PFNGLTEXBUFFERRANGEOESPROC) load(userptr, "glTexBufferRangeOES");
}
static void glad_gl_load_GL_OES_texture_storage_multisample_2d_array(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_texture_storage_multisample_2d_array) return;
    context->TexStorage3DMultisampleOES = (PFNGLTEXSTORAGE3DMULTISAMPLEOESPROC) load(userptr, "glTexStorage3DMultisampleOES");
}
static void glad_gl_load_GL_OES_texture_view(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_texture_view) return;
    context->TextureViewOES = (PFNGLTEXTUREVIEWOESPROC) load(userptr, "glTextureViewOES");
}
static void glad_gl_load_GL_OES_vertex_array_object(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_vertex_array_object) return;
    context->BindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC) load(userptr, "glBindVertexArrayOES");
    context->DeleteVertexArraysOES = (PFNGLDELETEVERTEXARRAYSOESPROC) load(userptr, "glDeleteVertexArraysOES");
    context->GenVertexArraysOES = (PFNGLGENVERTEXARRAYSOESPROC) load(userptr, "glGenVertexArraysOES");
    context->IsVertexArrayOES = (PFNGLISVERTEXARRAYOESPROC) load(userptr, "glIsVertexArrayOES");
}
static void glad_gl_load_GL_OES_viewport_array(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OES_viewport_array) return;
    context->DepthRangeArrayfvOES = (PFNGLDEPTHRANGEARRAYFVOESPROC) load(userptr, "glDepthRangeArrayfvOES");
    context->DepthRangeIndexedfOES = (PFNGLDEPTHRANGEINDEXEDFOESPROC) load(userptr, "glDepthRangeIndexedfOES");
    context->DisableiOES = (PFNGLDISABLEIOESPROC) load(userptr, "glDisableiOES");
    context->EnableiOES = (PFNGLENABLEIOESPROC) load(userptr, "glEnableiOES");
    context->GetFloati_vOES = (PFNGLGETFLOATI_VOESPROC) load(userptr, "glGetFloati_vOES");
    context->IsEnablediOES = (PFNGLISENABLEDIOESPROC) load(userptr, "glIsEnablediOES");
    context->ScissorArrayvOES = (PFNGLSCISSORARRAYVOESPROC) load(userptr, "glScissorArrayvOES");
    context->ScissorIndexedOES = (PFNGLSCISSORINDEXEDOESPROC) load(userptr, "glScissorIndexedOES");
    context->ScissorIndexedvOES = (PFNGLSCISSORINDEXEDVOESPROC) load(userptr, "glScissorIndexedvOES");
    context->ViewportArrayvOES = (PFNGLVIEWPORTARRAYVOESPROC) load(userptr, "glViewportArrayvOES");
    context->ViewportIndexedfOES = (PFNGLVIEWPORTINDEXEDFOESPROC) load(userptr, "glViewportIndexedfOES");
    context->ViewportIndexedfvOES = (PFNGLVIEWPORTINDEXEDFVOESPROC) load(userptr, "glViewportIndexedfvOES");
}
static void glad_gl_load_GL_OVR_multiview(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OVR_multiview) return;
    context->FramebufferTextureMultiviewOVR = (PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC) load(userptr, "glFramebufferTextureMultiviewOVR");
    context->NamedFramebufferTextureMultiviewOVR = (PFNGLNAMEDFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC) load(userptr, "glNamedFramebufferTextureMultiviewOVR");
}
static void glad_gl_load_GL_OVR_multiview_multisampled_render_to_texture(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->OVR_multiview_multisampled_render_to_texture) return;
    context->FramebufferTextureMultisampleMultiviewOVR = (PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVRPROC) load(userptr, "glFramebufferTextureMultisampleMultiviewOVR");
}
static void glad_gl_load_GL_QCOM_alpha_test(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QCOM_alpha_test) return;
    context->AlphaFuncQCOM = (PFNGLALPHAFUNCQCOMPROC) load(userptr, "glAlphaFuncQCOM");
}
static void glad_gl_load_GL_QCOM_driver_control(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QCOM_driver_control) return;
    context->DisableDriverControlQCOM = (PFNGLDISABLEDRIVERCONTROLQCOMPROC) load(userptr, "glDisableDriverControlQCOM");
    context->EnableDriverControlQCOM = (PFNGLENABLEDRIVERCONTROLQCOMPROC) load(userptr, "glEnableDriverControlQCOM");
    context->GetDriverControlStringQCOM = (PFNGLGETDRIVERCONTROLSTRINGQCOMPROC) load(userptr, "glGetDriverControlStringQCOM");
    context->GetDriverControlsQCOM = (PFNGLGETDRIVERCONTROLSQCOMPROC) load(userptr, "glGetDriverControlsQCOM");
}
static void glad_gl_load_GL_QCOM_extended_get(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QCOM_extended_get) return;
    context->ExtGetBufferPointervQCOM = (PFNGLEXTGETBUFFERPOINTERVQCOMPROC) load(userptr, "glExtGetBufferPointervQCOM");
    context->ExtGetBuffersQCOM = (PFNGLEXTGETBUFFERSQCOMPROC) load(userptr, "glExtGetBuffersQCOM");
    context->ExtGetFramebuffersQCOM = (PFNGLEXTGETFRAMEBUFFERSQCOMPROC) load(userptr, "glExtGetFramebuffersQCOM");
    context->ExtGetRenderbuffersQCOM = (PFNGLEXTGETRENDERBUFFERSQCOMPROC) load(userptr, "glExtGetRenderbuffersQCOM");
    context->ExtGetTexLevelParameterivQCOM = (PFNGLEXTGETTEXLEVELPARAMETERIVQCOMPROC) load(userptr, "glExtGetTexLevelParameterivQCOM");
    context->ExtGetTexSubImageQCOM = (PFNGLEXTGETTEXSUBIMAGEQCOMPROC) load(userptr, "glExtGetTexSubImageQCOM");
    context->ExtGetTexturesQCOM = (PFNGLEXTGETTEXTURESQCOMPROC) load(userptr, "glExtGetTexturesQCOM");
    context->ExtTexObjectStateOverrideiQCOM = (PFNGLEXTTEXOBJECTSTATEOVERRIDEIQCOMPROC) load(userptr, "glExtTexObjectStateOverrideiQCOM");
}
static void glad_gl_load_GL_QCOM_extended_get2(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QCOM_extended_get2) return;
    context->ExtGetProgramBinarySourceQCOM = (PFNGLEXTGETPROGRAMBINARYSOURCEQCOMPROC) load(userptr, "glExtGetProgramBinarySourceQCOM");
    context->ExtGetProgramsQCOM = (PFNGLEXTGETPROGRAMSQCOMPROC) load(userptr, "glExtGetProgramsQCOM");
    context->ExtGetShadersQCOM = (PFNGLEXTGETSHADERSQCOMPROC) load(userptr, "glExtGetShadersQCOM");
    context->ExtIsProgramBinaryQCOM = (PFNGLEXTISPROGRAMBINARYQCOMPROC) load(userptr, "glExtIsProgramBinaryQCOM");
}
static void glad_gl_load_GL_QCOM_frame_extrapolation(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QCOM_frame_extrapolation) return;
    context->ExtrapolateTex2DQCOM = (PFNGLEXTRAPOLATETEX2DQCOMPROC) load(userptr, "glExtrapolateTex2DQCOM");
}
static void glad_gl_load_GL_QCOM_framebuffer_foveated(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QCOM_framebuffer_foveated) return;
    context->FramebufferFoveationConfigQCOM = (PFNGLFRAMEBUFFERFOVEATIONCONFIGQCOMPROC) load(userptr, "glFramebufferFoveationConfigQCOM");
    context->FramebufferFoveationParametersQCOM = (PFNGLFRAMEBUFFERFOVEATIONPARAMETERSQCOMPROC) load(userptr, "glFramebufferFoveationParametersQCOM");
}
static void glad_gl_load_GL_QCOM_motion_estimation(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QCOM_motion_estimation) return;
    context->TexEstimateMotionQCOM = (PFNGLTEXESTIMATEMOTIONQCOMPROC) load(userptr, "glTexEstimateMotionQCOM");
    context->TexEstimateMotionRegionsQCOM = (PFNGLTEXESTIMATEMOTIONREGIONSQCOMPROC) load(userptr, "glTexEstimateMotionRegionsQCOM");
}
static void glad_gl_load_GL_QCOM_shader_framebuffer_fetch_noncoherent(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QCOM_shader_framebuffer_fetch_noncoherent) return;
    context->FramebufferFetchBarrierQCOM = (PFNGLFRAMEBUFFERFETCHBARRIERQCOMPROC) load(userptr, "glFramebufferFetchBarrierQCOM");
}
static void glad_gl_load_GL_QCOM_shading_rate(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QCOM_shading_rate) return;
    context->ShadingRateQCOM = (PFNGLSHADINGRATEQCOMPROC) load(userptr, "glShadingRateQCOM");
}
static void glad_gl_load_GL_QCOM_texture_foveated(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QCOM_texture_foveated) return;
    context->TextureFoveationParametersQCOM = (PFNGLTEXTUREFOVEATIONPARAMETERSQCOMPROC) load(userptr, "glTextureFoveationParametersQCOM");
}
static void glad_gl_load_GL_QCOM_tiled_rendering(GladGLES2Context *context, GLADuserptrloadfunc load, void* userptr) {
    if(!context->QCOM_tiled_rendering) return;
    context->EndTilingQCOM = (PFNGLENDTILINGQCOMPROC) load(userptr, "glEndTilingQCOM");
    context->StartTilingQCOM = (PFNGLSTARTTILINGQCOMPROC) load(userptr, "glStartTilingQCOM");
}



static void glad_gl_free_extensions(char **exts_i) {
    if (exts_i != NULL) {
        unsigned int index;
        for(index = 0; exts_i[index]; index++) {
            free((void *) (exts_i[index]));
        }
        free((void *)exts_i);
        exts_i = NULL;
    }
}
static int glad_gl_get_extensions(GladGLES2Context *context, const char **out_exts, char ***out_exts_i) {
#if defined(GL_ES_VERSION_3_0) || defined(GL_VERSION_3_0)
    if (context->GetStringi != NULL && context->GetIntegerv != NULL) {
        unsigned int index = 0;
        unsigned int num_exts_i = 0;
        char **exts_i = NULL;
        context->GetIntegerv(GL_NUM_EXTENSIONS, (int*) &num_exts_i);
        exts_i = (char **) malloc((num_exts_i + 1) * (sizeof *exts_i));
        if (exts_i == NULL) {
            return 0;
        }
        for(index = 0; index < num_exts_i; index++) {
            const char *gl_str_tmp = (const char*) context->GetStringi(GL_EXTENSIONS, index);
            size_t len = strlen(gl_str_tmp) + 1;

            char *local_str = (char*) malloc(len * sizeof(char));
            if(local_str == NULL) {
                exts_i[index] = NULL;
                glad_gl_free_extensions(exts_i);
                return 0;
            }

            memcpy(local_str, gl_str_tmp, len * sizeof(char));
            exts_i[index] = local_str;
        }
        exts_i[index] = NULL;

        *out_exts_i = exts_i;

        return 1;
    }
#else
    GLAD_UNUSED(out_exts_i);
#endif
    if (context->GetString == NULL) {
        return 0;
    }
    *out_exts = (const char *)context->GetString(GL_EXTENSIONS);
    return 1;
}
static int glad_gl_has_extension(const char *exts, char **exts_i, const char *ext) {
    if(exts_i) {
        unsigned int index;
        for(index = 0; exts_i[index]; index++) {
            const char *e = exts_i[index];
            if(strcmp(e, ext) == 0) {
                return 1;
            }
        }
    } else {
        const char *extensions;
        const char *loc;
        const char *terminator;
        extensions = exts;
        if(extensions == NULL || ext == NULL) {
            return 0;
        }
        while(1) {
            loc = strstr(extensions, ext);
            if(loc == NULL) {
                return 0;
            }
            terminator = loc + strlen(ext);
            if((loc == extensions || *(loc - 1) == ' ') &&
                (*terminator == ' ' || *terminator == '\0')) {
                return 1;
            }
            extensions = terminator;
        }
    }
    return 0;
}

static GLADapiproc glad_gl_get_proc_from_userptr(void *userptr, const char* name) {
    return (GLAD_GNUC_EXTENSION (GLADapiproc (*)(const char *name)) userptr)(name);
}

static int glad_gl_find_extensions_gles2(GladGLES2Context *context) {
    const char *exts = NULL;
    char **exts_i = NULL;
    if (!glad_gl_get_extensions(context, &exts, &exts_i)) return 0;

    context->AMD_compressed_3DC_texture = glad_gl_has_extension(exts, exts_i, "GL_AMD_compressed_3DC_texture");
    context->AMD_compressed_ATC_texture = glad_gl_has_extension(exts, exts_i, "GL_AMD_compressed_ATC_texture");
    context->AMD_framebuffer_multisample_advanced = glad_gl_has_extension(exts, exts_i, "GL_AMD_framebuffer_multisample_advanced");
    context->AMD_performance_monitor = glad_gl_has_extension(exts, exts_i, "GL_AMD_performance_monitor");
    context->AMD_program_binary_Z400 = glad_gl_has_extension(exts, exts_i, "GL_AMD_program_binary_Z400");
    context->ANDROID_extension_pack_es31a = glad_gl_has_extension(exts, exts_i, "GL_ANDROID_extension_pack_es31a");
    context->ANGLE_depth_texture = glad_gl_has_extension(exts, exts_i, "GL_ANGLE_depth_texture");
    context->ANGLE_framebuffer_blit = glad_gl_has_extension(exts, exts_i, "GL_ANGLE_framebuffer_blit");
    context->ANGLE_framebuffer_multisample = glad_gl_has_extension(exts, exts_i, "GL_ANGLE_framebuffer_multisample");
    context->ANGLE_instanced_arrays = glad_gl_has_extension(exts, exts_i, "GL_ANGLE_instanced_arrays");
    context->ANGLE_pack_reverse_row_order = glad_gl_has_extension(exts, exts_i, "GL_ANGLE_pack_reverse_row_order");
    context->ANGLE_program_binary = glad_gl_has_extension(exts, exts_i, "GL_ANGLE_program_binary");
    context->ANGLE_texture_compression_dxt3 = glad_gl_has_extension(exts, exts_i, "GL_ANGLE_texture_compression_dxt3");
    context->ANGLE_texture_compression_dxt5 = glad_gl_has_extension(exts, exts_i, "GL_ANGLE_texture_compression_dxt5");
    context->ANGLE_texture_usage = glad_gl_has_extension(exts, exts_i, "GL_ANGLE_texture_usage");
    context->ANGLE_translated_shader_source = glad_gl_has_extension(exts, exts_i, "GL_ANGLE_translated_shader_source");
    context->APPLE_clip_distance = glad_gl_has_extension(exts, exts_i, "GL_APPLE_clip_distance");
    context->APPLE_color_buffer_packed_float = glad_gl_has_extension(exts, exts_i, "GL_APPLE_color_buffer_packed_float");
    context->APPLE_copy_texture_levels = glad_gl_has_extension(exts, exts_i, "GL_APPLE_copy_texture_levels");
    context->APPLE_framebuffer_multisample = glad_gl_has_extension(exts, exts_i, "GL_APPLE_framebuffer_multisample");
    context->APPLE_rgb_422 = glad_gl_has_extension(exts, exts_i, "GL_APPLE_rgb_422");
    context->APPLE_sync = glad_gl_has_extension(exts, exts_i, "GL_APPLE_sync");
    context->APPLE_texture_format_BGRA8888 = glad_gl_has_extension(exts, exts_i, "GL_APPLE_texture_format_BGRA8888");
    context->APPLE_texture_max_level = glad_gl_has_extension(exts, exts_i, "GL_APPLE_texture_max_level");
    context->APPLE_texture_packed_float = glad_gl_has_extension(exts, exts_i, "GL_APPLE_texture_packed_float");
    context->ARM_mali_program_binary = glad_gl_has_extension(exts, exts_i, "GL_ARM_mali_program_binary");
    context->ARM_mali_shader_binary = glad_gl_has_extension(exts, exts_i, "GL_ARM_mali_shader_binary");
    context->ARM_rgba8 = glad_gl_has_extension(exts, exts_i, "GL_ARM_rgba8");
    context->ARM_shader_core_properties = glad_gl_has_extension(exts, exts_i, "GL_ARM_shader_core_properties");
    context->ARM_shader_framebuffer_fetch = glad_gl_has_extension(exts, exts_i, "GL_ARM_shader_framebuffer_fetch");
    context->ARM_shader_framebuffer_fetch_depth_stencil = glad_gl_has_extension(exts, exts_i, "GL_ARM_shader_framebuffer_fetch_depth_stencil");
    context->ARM_texture_unnormalized_coordinates = glad_gl_has_extension(exts, exts_i, "GL_ARM_texture_unnormalized_coordinates");
    context->DMP_program_binary = glad_gl_has_extension(exts, exts_i, "GL_DMP_program_binary");
    context->DMP_shader_binary = glad_gl_has_extension(exts, exts_i, "GL_DMP_shader_binary");
    context->EXT_EGL_image_array = glad_gl_has_extension(exts, exts_i, "GL_EXT_EGL_image_array");
    context->EXT_EGL_image_storage = glad_gl_has_extension(exts, exts_i, "GL_EXT_EGL_image_storage");
    context->EXT_EGL_image_storage_compression = glad_gl_has_extension(exts, exts_i, "GL_EXT_EGL_image_storage_compression");
    context->EXT_YUV_target = glad_gl_has_extension(exts, exts_i, "GL_EXT_YUV_target");
    context->EXT_base_instance = glad_gl_has_extension(exts, exts_i, "GL_EXT_base_instance");
    context->EXT_blend_func_extended = glad_gl_has_extension(exts, exts_i, "GL_EXT_blend_func_extended");
    context->EXT_blend_minmax = glad_gl_has_extension(exts, exts_i, "GL_EXT_blend_minmax");
    context->EXT_buffer_storage = glad_gl_has_extension(exts, exts_i, "GL_EXT_buffer_storage");
    context->EXT_clear_texture = glad_gl_has_extension(exts, exts_i, "GL_EXT_clear_texture");
    context->EXT_clip_control = glad_gl_has_extension(exts, exts_i, "GL_EXT_clip_control");
    context->EXT_clip_cull_distance = glad_gl_has_extension(exts, exts_i, "GL_EXT_clip_cull_distance");
    context->EXT_color_buffer_float = glad_gl_has_extension(exts, exts_i, "GL_EXT_color_buffer_float");
    context->EXT_color_buffer_half_float = glad_gl_has_extension(exts, exts_i, "GL_EXT_color_buffer_half_float");
    context->EXT_conservative_depth = glad_gl_has_extension(exts, exts_i, "GL_EXT_conservative_depth");
    context->EXT_copy_image = glad_gl_has_extension(exts, exts_i, "GL_EXT_copy_image");
    context->EXT_debug_label = glad_gl_has_extension(exts, exts_i, "GL_EXT_debug_label");
    context->EXT_debug_marker = glad_gl_has_extension(exts, exts_i, "GL_EXT_debug_marker");
    context->EXT_depth_clamp = glad_gl_has_extension(exts, exts_i, "GL_EXT_depth_clamp");
    context->EXT_discard_framebuffer = glad_gl_has_extension(exts, exts_i, "GL_EXT_discard_framebuffer");
    context->EXT_disjoint_timer_query = glad_gl_has_extension(exts, exts_i, "GL_EXT_disjoint_timer_query");
    context->EXT_draw_buffers = glad_gl_has_extension(exts, exts_i, "GL_EXT_draw_buffers");
    context->EXT_draw_buffers_indexed = glad_gl_has_extension(exts, exts_i, "GL_EXT_draw_buffers_indexed");
    context->EXT_draw_elements_base_vertex = glad_gl_has_extension(exts, exts_i, "GL_EXT_draw_elements_base_vertex");
    context->EXT_draw_instanced = glad_gl_has_extension(exts, exts_i, "GL_EXT_draw_instanced");
    context->EXT_draw_transform_feedback = glad_gl_has_extension(exts, exts_i, "GL_EXT_draw_transform_feedback");
    context->EXT_external_buffer = glad_gl_has_extension(exts, exts_i, "GL_EXT_external_buffer");
    context->EXT_float_blend = glad_gl_has_extension(exts, exts_i, "GL_EXT_float_blend");
    context->EXT_fragment_shading_rate = glad_gl_has_extension(exts, exts_i, "GL_EXT_fragment_shading_rate");
    context->EXT_framebuffer_blit_layers = glad_gl_has_extension(exts, exts_i, "GL_EXT_framebuffer_blit_layers");
    context->EXT_geometry_point_size = glad_gl_has_extension(exts, exts_i, "GL_EXT_geometry_point_size");
    context->EXT_geometry_shader = glad_gl_has_extension(exts, exts_i, "GL_EXT_geometry_shader");
    context->EXT_gpu_shader5 = glad_gl_has_extension(exts, exts_i, "GL_EXT_gpu_shader5");
    context->EXT_instanced_arrays = glad_gl_has_extension(exts, exts_i, "GL_EXT_instanced_arrays");
    context->EXT_map_buffer_range = glad_gl_has_extension(exts, exts_i, "GL_EXT_map_buffer_range");
    context->EXT_memory_object = glad_gl_has_extension(exts, exts_i, "GL_EXT_memory_object");
    context->EXT_memory_object_fd = glad_gl_has_extension(exts, exts_i, "GL_EXT_memory_object_fd");
    context->EXT_memory_object_win32 = glad_gl_has_extension(exts, exts_i, "GL_EXT_memory_object_win32");
    context->EXT_multi_draw_arrays = glad_gl_has_extension(exts, exts_i, "GL_EXT_multi_draw_arrays");
    context->EXT_multi_draw_indirect = glad_gl_has_extension(exts, exts_i, "GL_EXT_multi_draw_indirect");
    context->EXT_multisampled_compatibility = glad_gl_has_extension(exts, exts_i, "GL_EXT_multisampled_compatibility");
    context->EXT_multisampled_render_to_texture = glad_gl_has_extension(exts, exts_i, "GL_EXT_multisampled_render_to_texture");
    context->EXT_multisampled_render_to_texture2 = glad_gl_has_extension(exts, exts_i, "GL_EXT_multisampled_render_to_texture2");
    context->EXT_multiview_draw_buffers = glad_gl_has_extension(exts, exts_i, "GL_EXT_multiview_draw_buffers");
    context->EXT_multiview_tessellation_geometry_shader = glad_gl_has_extension(exts, exts_i, "GL_EXT_multiview_tessellation_geometry_shader");
    context->EXT_multiview_texture_multisample = glad_gl_has_extension(exts, exts_i, "GL_EXT_multiview_texture_multisample");
    context->EXT_multiview_timer_query = glad_gl_has_extension(exts, exts_i, "GL_EXT_multiview_timer_query");
    context->EXT_occlusion_query_boolean = glad_gl_has_extension(exts, exts_i, "GL_EXT_occlusion_query_boolean");
    context->EXT_polygon_offset_clamp = glad_gl_has_extension(exts, exts_i, "GL_EXT_polygon_offset_clamp");
    context->EXT_post_depth_coverage = glad_gl_has_extension(exts, exts_i, "GL_EXT_post_depth_coverage");
    context->EXT_primitive_bounding_box = glad_gl_has_extension(exts, exts_i, "GL_EXT_primitive_bounding_box");
    context->EXT_protected_textures = glad_gl_has_extension(exts, exts_i, "GL_EXT_protected_textures");
    context->EXT_pvrtc_sRGB = glad_gl_has_extension(exts, exts_i, "GL_EXT_pvrtc_sRGB");
    context->EXT_raster_multisample = glad_gl_has_extension(exts, exts_i, "GL_EXT_raster_multisample");
    context->EXT_read_format_bgra = glad_gl_has_extension(exts, exts_i, "GL_EXT_read_format_bgra");
    context->EXT_render_snorm = glad_gl_has_extension(exts, exts_i, "GL_EXT_render_snorm");
    context->EXT_robustness = glad_gl_has_extension(exts, exts_i, "GL_EXT_robustness");
    context->EXT_sRGB = glad_gl_has_extension(exts, exts_i, "GL_EXT_sRGB");
    context->EXT_sRGB_write_control = glad_gl_has_extension(exts, exts_i, "GL_EXT_sRGB_write_control");
    context->EXT_semaphore = glad_gl_has_extension(exts, exts_i, "GL_EXT_semaphore");
    context->EXT_semaphore_fd = glad_gl_has_extension(exts, exts_i, "GL_EXT_semaphore_fd");
    context->EXT_semaphore_win32 = glad_gl_has_extension(exts, exts_i, "GL_EXT_semaphore_win32");
    context->EXT_separate_depth_stencil = glad_gl_has_extension(exts, exts_i, "GL_EXT_separate_depth_stencil");
    context->EXT_separate_shader_objects = glad_gl_has_extension(exts, exts_i, "GL_EXT_separate_shader_objects");
    context->EXT_shader_framebuffer_fetch = glad_gl_has_extension(exts, exts_i, "GL_EXT_shader_framebuffer_fetch");
    context->EXT_shader_framebuffer_fetch_non_coherent = glad_gl_has_extension(exts, exts_i, "GL_EXT_shader_framebuffer_fetch_non_coherent");
    context->EXT_shader_group_vote = glad_gl_has_extension(exts, exts_i, "GL_EXT_shader_group_vote");
    context->EXT_shader_implicit_conversions = glad_gl_has_extension(exts, exts_i, "GL_EXT_shader_implicit_conversions");
    context->EXT_shader_integer_mix = glad_gl_has_extension(exts, exts_i, "GL_EXT_shader_integer_mix");
    context->EXT_shader_io_blocks = glad_gl_has_extension(exts, exts_i, "GL_EXT_shader_io_blocks");
    context->EXT_shader_non_constant_global_initializers = glad_gl_has_extension(exts, exts_i, "GL_EXT_shader_non_constant_global_initializers");
    context->EXT_shader_pixel_local_storage = glad_gl_has_extension(exts, exts_i, "GL_EXT_shader_pixel_local_storage");
    context->EXT_shader_pixel_local_storage2 = glad_gl_has_extension(exts, exts_i, "GL_EXT_shader_pixel_local_storage2");
    context->EXT_shader_samples_identical = glad_gl_has_extension(exts, exts_i, "GL_EXT_shader_samples_identical");
    context->EXT_shader_texture_lod = glad_gl_has_extension(exts, exts_i, "GL_EXT_shader_texture_lod");
    context->EXT_shadow_samplers = glad_gl_has_extension(exts, exts_i, "GL_EXT_shadow_samplers");
    context->EXT_sparse_texture = glad_gl_has_extension(exts, exts_i, "GL_EXT_sparse_texture");
    context->EXT_sparse_texture2 = glad_gl_has_extension(exts, exts_i, "GL_EXT_sparse_texture2");
    context->EXT_tessellation_point_size = glad_gl_has_extension(exts, exts_i, "GL_EXT_tessellation_point_size");
    context->EXT_tessellation_shader = glad_gl_has_extension(exts, exts_i, "GL_EXT_tessellation_shader");
    context->EXT_texture_border_clamp = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_border_clamp");
    context->EXT_texture_buffer = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_buffer");
    context->EXT_texture_compression_astc_decode_mode = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_compression_astc_decode_mode");
    context->EXT_texture_compression_bptc = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_compression_bptc");
    context->EXT_texture_compression_dxt1 = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_compression_dxt1");
    context->EXT_texture_compression_rgtc = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_compression_rgtc");
    context->EXT_texture_compression_s3tc = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_compression_s3tc");
    context->EXT_texture_compression_s3tc_srgb = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_compression_s3tc_srgb");
    context->EXT_texture_cube_map_array = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_cube_map_array");
    context->EXT_texture_filter_anisotropic = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_filter_anisotropic");
    context->EXT_texture_filter_minmax = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_filter_minmax");
    context->EXT_texture_format_BGRA8888 = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_format_BGRA8888");
    context->EXT_texture_format_sRGB_override = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_format_sRGB_override");
    context->EXT_texture_mirror_clamp_to_edge = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_mirror_clamp_to_edge");
    context->EXT_texture_norm16 = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_norm16");
    context->EXT_texture_query_lod = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_query_lod");
    context->EXT_texture_rg = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_rg");
    context->EXT_texture_sRGB_R8 = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_sRGB_R8");
    context->EXT_texture_sRGB_RG8 = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_sRGB_RG8");
    context->EXT_texture_sRGB_decode = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_sRGB_decode");
    context->EXT_texture_shadow_lod = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_shadow_lod");
    context->EXT_texture_storage = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_storage");
    context->EXT_texture_storage_compression = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_storage_compression");
    context->EXT_texture_type_2_10_10_10_REV = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_type_2_10_10_10_REV");
    context->EXT_texture_view = glad_gl_has_extension(exts, exts_i, "GL_EXT_texture_view");
    context->EXT_unpack_subimage = glad_gl_has_extension(exts, exts_i, "GL_EXT_unpack_subimage");
    context->EXT_win32_keyed_mutex = glad_gl_has_extension(exts, exts_i, "GL_EXT_win32_keyed_mutex");
    context->EXT_window_rectangles = glad_gl_has_extension(exts, exts_i, "GL_EXT_window_rectangles");
    context->FJ_shader_binary_GCCSO = glad_gl_has_extension(exts, exts_i, "GL_FJ_shader_binary_GCCSO");
    context->IMG_bindless_texture = glad_gl_has_extension(exts, exts_i, "GL_IMG_bindless_texture");
    context->IMG_framebuffer_downsample = glad_gl_has_extension(exts, exts_i, "GL_IMG_framebuffer_downsample");
    context->IMG_multisampled_render_to_texture = glad_gl_has_extension(exts, exts_i, "GL_IMG_multisampled_render_to_texture");
    context->IMG_program_binary = glad_gl_has_extension(exts, exts_i, "GL_IMG_program_binary");
    context->IMG_read_format = glad_gl_has_extension(exts, exts_i, "GL_IMG_read_format");
    context->IMG_shader_binary = glad_gl_has_extension(exts, exts_i, "GL_IMG_shader_binary");
    context->IMG_texture_compression_pvrtc = glad_gl_has_extension(exts, exts_i, "GL_IMG_texture_compression_pvrtc");
    context->IMG_texture_compression_pvrtc2 = glad_gl_has_extension(exts, exts_i, "GL_IMG_texture_compression_pvrtc2");
    context->IMG_texture_filter_cubic = glad_gl_has_extension(exts, exts_i, "GL_IMG_texture_filter_cubic");
    context->INTEL_blackhole_render = glad_gl_has_extension(exts, exts_i, "GL_INTEL_blackhole_render");
    context->INTEL_conservative_rasterization = glad_gl_has_extension(exts, exts_i, "GL_INTEL_conservative_rasterization");
    context->INTEL_framebuffer_CMAA = glad_gl_has_extension(exts, exts_i, "GL_INTEL_framebuffer_CMAA");
    context->INTEL_performance_query = glad_gl_has_extension(exts, exts_i, "GL_INTEL_performance_query");
    context->KHR_blend_equation_advanced = glad_gl_has_extension(exts, exts_i, "GL_KHR_blend_equation_advanced");
    context->KHR_blend_equation_advanced_coherent = glad_gl_has_extension(exts, exts_i, "GL_KHR_blend_equation_advanced_coherent");
    context->KHR_context_flush_control = glad_gl_has_extension(exts, exts_i, "GL_KHR_context_flush_control");
    context->KHR_debug = glad_gl_has_extension(exts, exts_i, "GL_KHR_debug");
    context->KHR_no_error = glad_gl_has_extension(exts, exts_i, "GL_KHR_no_error");
    context->KHR_parallel_shader_compile = glad_gl_has_extension(exts, exts_i, "GL_KHR_parallel_shader_compile");
    context->KHR_robust_buffer_access_behavior = glad_gl_has_extension(exts, exts_i, "GL_KHR_robust_buffer_access_behavior");
    context->KHR_robustness = glad_gl_has_extension(exts, exts_i, "GL_KHR_robustness");
    context->KHR_shader_subgroup = glad_gl_has_extension(exts, exts_i, "GL_KHR_shader_subgroup");
    context->KHR_texture_compression_astc_hdr = glad_gl_has_extension(exts, exts_i, "GL_KHR_texture_compression_astc_hdr");
    context->KHR_texture_compression_astc_ldr = glad_gl_has_extension(exts, exts_i, "GL_KHR_texture_compression_astc_ldr");
    context->KHR_texture_compression_astc_sliced_3d = glad_gl_has_extension(exts, exts_i, "GL_KHR_texture_compression_astc_sliced_3d");
    context->MESA_bgra = glad_gl_has_extension(exts, exts_i, "GL_MESA_bgra");
    context->MESA_framebuffer_flip_x = glad_gl_has_extension(exts, exts_i, "GL_MESA_framebuffer_flip_x");
    context->MESA_framebuffer_flip_y = glad_gl_has_extension(exts, exts_i, "GL_MESA_framebuffer_flip_y");
    context->MESA_framebuffer_swap_xy = glad_gl_has_extension(exts, exts_i, "GL_MESA_framebuffer_swap_xy");
    context->MESA_program_binary_formats = glad_gl_has_extension(exts, exts_i, "GL_MESA_program_binary_formats");
    context->MESA_sampler_objects = glad_gl_has_extension(exts, exts_i, "GL_MESA_sampler_objects");
    context->MESA_shader_integer_functions = glad_gl_has_extension(exts, exts_i, "GL_MESA_shader_integer_functions");
    context->MESA_texture_const_bandwidth = glad_gl_has_extension(exts, exts_i, "GL_MESA_texture_const_bandwidth");
    context->NVX_blend_equation_advanced_multi_draw_buffers = glad_gl_has_extension(exts, exts_i, "GL_NVX_blend_equation_advanced_multi_draw_buffers");
    context->NV_bindless_texture = glad_gl_has_extension(exts, exts_i, "GL_NV_bindless_texture");
    context->NV_blend_equation_advanced = glad_gl_has_extension(exts, exts_i, "GL_NV_blend_equation_advanced");
    context->NV_blend_equation_advanced_coherent = glad_gl_has_extension(exts, exts_i, "GL_NV_blend_equation_advanced_coherent");
    context->NV_blend_minmax_factor = glad_gl_has_extension(exts, exts_i, "GL_NV_blend_minmax_factor");
    context->NV_clip_space_w_scaling = glad_gl_has_extension(exts, exts_i, "GL_NV_clip_space_w_scaling");
    context->NV_compute_shader_derivatives = glad_gl_has_extension(exts, exts_i, "GL_NV_compute_shader_derivatives");
    context->NV_conditional_render = glad_gl_has_extension(exts, exts_i, "GL_NV_conditional_render");
    context->NV_conservative_raster = glad_gl_has_extension(exts, exts_i, "GL_NV_conservative_raster");
    context->NV_conservative_raster_pre_snap = glad_gl_has_extension(exts, exts_i, "GL_NV_conservative_raster_pre_snap");
    context->NV_conservative_raster_pre_snap_triangles = glad_gl_has_extension(exts, exts_i, "GL_NV_conservative_raster_pre_snap_triangles");
    context->NV_copy_buffer = glad_gl_has_extension(exts, exts_i, "GL_NV_copy_buffer");
    context->NV_coverage_sample = glad_gl_has_extension(exts, exts_i, "GL_NV_coverage_sample");
    context->NV_depth_nonlinear = glad_gl_has_extension(exts, exts_i, "GL_NV_depth_nonlinear");
    context->NV_draw_buffers = glad_gl_has_extension(exts, exts_i, "GL_NV_draw_buffers");
    context->NV_draw_instanced = glad_gl_has_extension(exts, exts_i, "GL_NV_draw_instanced");
    context->NV_draw_vulkan_image = glad_gl_has_extension(exts, exts_i, "GL_NV_draw_vulkan_image");
    context->NV_explicit_attrib_location = glad_gl_has_extension(exts, exts_i, "GL_NV_explicit_attrib_location");
    context->NV_fbo_color_attachments = glad_gl_has_extension(exts, exts_i, "GL_NV_fbo_color_attachments");
    context->NV_fence = glad_gl_has_extension(exts, exts_i, "GL_NV_fence");
    context->NV_fill_rectangle = glad_gl_has_extension(exts, exts_i, "GL_NV_fill_rectangle");
    context->NV_fragment_coverage_to_color = glad_gl_has_extension(exts, exts_i, "GL_NV_fragment_coverage_to_color");
    context->NV_fragment_shader_barycentric = glad_gl_has_extension(exts, exts_i, "GL_NV_fragment_shader_barycentric");
    context->NV_fragment_shader_interlock = glad_gl_has_extension(exts, exts_i, "GL_NV_fragment_shader_interlock");
    context->NV_framebuffer_blit = glad_gl_has_extension(exts, exts_i, "GL_NV_framebuffer_blit");
    context->NV_framebuffer_mixed_samples = glad_gl_has_extension(exts, exts_i, "GL_NV_framebuffer_mixed_samples");
    context->NV_framebuffer_multisample = glad_gl_has_extension(exts, exts_i, "GL_NV_framebuffer_multisample");
    context->NV_generate_mipmap_sRGB = glad_gl_has_extension(exts, exts_i, "GL_NV_generate_mipmap_sRGB");
    context->NV_geometry_shader_passthrough = glad_gl_has_extension(exts, exts_i, "GL_NV_geometry_shader_passthrough");
    context->NV_gpu_shader5 = glad_gl_has_extension(exts, exts_i, "GL_NV_gpu_shader5");
    context->NV_image_formats = glad_gl_has_extension(exts, exts_i, "GL_NV_image_formats");
    context->NV_instanced_arrays = glad_gl_has_extension(exts, exts_i, "GL_NV_instanced_arrays");
    context->NV_internalformat_sample_query = glad_gl_has_extension(exts, exts_i, "GL_NV_internalformat_sample_query");
    context->NV_memory_attachment = glad_gl_has_extension(exts, exts_i, "GL_NV_memory_attachment");
    context->NV_memory_object_sparse = glad_gl_has_extension(exts, exts_i, "GL_NV_memory_object_sparse");
    context->NV_mesh_shader = glad_gl_has_extension(exts, exts_i, "GL_NV_mesh_shader");
    context->NV_non_square_matrices = glad_gl_has_extension(exts, exts_i, "GL_NV_non_square_matrices");
    context->NV_pack_subimage = glad_gl_has_extension(exts, exts_i, "GL_NV_pack_subimage");
    context->NV_path_rendering = glad_gl_has_extension(exts, exts_i, "GL_NV_path_rendering");
    context->NV_path_rendering_shared_edge = glad_gl_has_extension(exts, exts_i, "GL_NV_path_rendering_shared_edge");
    context->NV_pixel_buffer_object = glad_gl_has_extension(exts, exts_i, "GL_NV_pixel_buffer_object");
    context->NV_polygon_mode = glad_gl_has_extension(exts, exts_i, "GL_NV_polygon_mode");
    context->NV_primitive_shading_rate = glad_gl_has_extension(exts, exts_i, "GL_NV_primitive_shading_rate");
    context->NV_read_buffer = glad_gl_has_extension(exts, exts_i, "GL_NV_read_buffer");
    context->NV_read_buffer_front = glad_gl_has_extension(exts, exts_i, "GL_NV_read_buffer_front");
    context->NV_read_depth = glad_gl_has_extension(exts, exts_i, "GL_NV_read_depth");
    context->NV_read_depth_stencil = glad_gl_has_extension(exts, exts_i, "GL_NV_read_depth_stencil");
    context->NV_read_stencil = glad_gl_has_extension(exts, exts_i, "GL_NV_read_stencil");
    context->NV_representative_fragment_test = glad_gl_has_extension(exts, exts_i, "GL_NV_representative_fragment_test");
    context->NV_sRGB_formats = glad_gl_has_extension(exts, exts_i, "GL_NV_sRGB_formats");
    context->NV_sample_locations = glad_gl_has_extension(exts, exts_i, "GL_NV_sample_locations");
    context->NV_sample_mask_override_coverage = glad_gl_has_extension(exts, exts_i, "GL_NV_sample_mask_override_coverage");
    context->NV_scissor_exclusive = glad_gl_has_extension(exts, exts_i, "GL_NV_scissor_exclusive");
    context->NV_shader_atomic_fp16_vector = glad_gl_has_extension(exts, exts_i, "GL_NV_shader_atomic_fp16_vector");
    context->NV_shader_noperspective_interpolation = glad_gl_has_extension(exts, exts_i, "GL_NV_shader_noperspective_interpolation");
    context->NV_shader_subgroup_partitioned = glad_gl_has_extension(exts, exts_i, "GL_NV_shader_subgroup_partitioned");
    context->NV_shader_texture_footprint = glad_gl_has_extension(exts, exts_i, "GL_NV_shader_texture_footprint");
    context->NV_shading_rate_image = glad_gl_has_extension(exts, exts_i, "GL_NV_shading_rate_image");
    context->NV_shadow_samplers_array = glad_gl_has_extension(exts, exts_i, "GL_NV_shadow_samplers_array");
    context->NV_shadow_samplers_cube = glad_gl_has_extension(exts, exts_i, "GL_NV_shadow_samplers_cube");
    context->NV_stereo_view_rendering = glad_gl_has_extension(exts, exts_i, "GL_NV_stereo_view_rendering");
    context->NV_texture_barrier = glad_gl_has_extension(exts, exts_i, "GL_NV_texture_barrier");
    context->NV_texture_border_clamp = glad_gl_has_extension(exts, exts_i, "GL_NV_texture_border_clamp");
    context->NV_texture_compression_s3tc_update = glad_gl_has_extension(exts, exts_i, "GL_NV_texture_compression_s3tc_update");
    context->NV_texture_npot_2D_mipmap = glad_gl_has_extension(exts, exts_i, "GL_NV_texture_npot_2D_mipmap");
    context->NV_timeline_semaphore = glad_gl_has_extension(exts, exts_i, "GL_NV_timeline_semaphore");
    context->NV_viewport_array = glad_gl_has_extension(exts, exts_i, "GL_NV_viewport_array");
    context->NV_viewport_array2 = glad_gl_has_extension(exts, exts_i, "GL_NV_viewport_array2");
    context->NV_viewport_swizzle = glad_gl_has_extension(exts, exts_i, "GL_NV_viewport_swizzle");
    context->OES_EGL_image = glad_gl_has_extension(exts, exts_i, "GL_OES_EGL_image");
    context->OES_EGL_image_external = glad_gl_has_extension(exts, exts_i, "GL_OES_EGL_image_external");
    context->OES_EGL_image_external_essl3 = glad_gl_has_extension(exts, exts_i, "GL_OES_EGL_image_external_essl3");
    context->OES_compressed_ETC1_RGB8_sub_texture = glad_gl_has_extension(exts, exts_i, "GL_OES_compressed_ETC1_RGB8_sub_texture");
    context->OES_compressed_ETC1_RGB8_texture = glad_gl_has_extension(exts, exts_i, "GL_OES_compressed_ETC1_RGB8_texture");
    context->OES_compressed_paletted_texture = glad_gl_has_extension(exts, exts_i, "GL_OES_compressed_paletted_texture");
    context->OES_copy_image = glad_gl_has_extension(exts, exts_i, "GL_OES_copy_image");
    context->OES_depth24 = glad_gl_has_extension(exts, exts_i, "GL_OES_depth24");
    context->OES_depth32 = glad_gl_has_extension(exts, exts_i, "GL_OES_depth32");
    context->OES_depth_texture = glad_gl_has_extension(exts, exts_i, "GL_OES_depth_texture");
    context->OES_draw_buffers_indexed = glad_gl_has_extension(exts, exts_i, "GL_OES_draw_buffers_indexed");
    context->OES_draw_elements_base_vertex = glad_gl_has_extension(exts, exts_i, "GL_OES_draw_elements_base_vertex");
    context->OES_element_index_uint = glad_gl_has_extension(exts, exts_i, "GL_OES_element_index_uint");
    context->OES_fbo_render_mipmap = glad_gl_has_extension(exts, exts_i, "GL_OES_fbo_render_mipmap");
    context->OES_fragment_precision_high = glad_gl_has_extension(exts, exts_i, "GL_OES_fragment_precision_high");
    context->OES_geometry_point_size = glad_gl_has_extension(exts, exts_i, "GL_OES_geometry_point_size");
    context->OES_geometry_shader = glad_gl_has_extension(exts, exts_i, "GL_OES_geometry_shader");
    context->OES_get_program_binary = glad_gl_has_extension(exts, exts_i, "GL_OES_get_program_binary");
    context->OES_gpu_shader5 = glad_gl_has_extension(exts, exts_i, "GL_OES_gpu_shader5");
    context->OES_mapbuffer = glad_gl_has_extension(exts, exts_i, "GL_OES_mapbuffer");
    context->OES_packed_depth_stencil = glad_gl_has_extension(exts, exts_i, "GL_OES_packed_depth_stencil");
    context->OES_primitive_bounding_box = glad_gl_has_extension(exts, exts_i, "GL_OES_primitive_bounding_box");
    context->OES_required_internalformat = glad_gl_has_extension(exts, exts_i, "GL_OES_required_internalformat");
    context->OES_rgb8_rgba8 = glad_gl_has_extension(exts, exts_i, "GL_OES_rgb8_rgba8");
    context->OES_sample_shading = glad_gl_has_extension(exts, exts_i, "GL_OES_sample_shading");
    context->OES_sample_variables = glad_gl_has_extension(exts, exts_i, "GL_OES_sample_variables");
    context->OES_shader_image_atomic = glad_gl_has_extension(exts, exts_i, "GL_OES_shader_image_atomic");
    context->OES_shader_io_blocks = glad_gl_has_extension(exts, exts_i, "GL_OES_shader_io_blocks");
    context->OES_shader_multisample_interpolation = glad_gl_has_extension(exts, exts_i, "GL_OES_shader_multisample_interpolation");
    context->OES_standard_derivatives = glad_gl_has_extension(exts, exts_i, "GL_OES_standard_derivatives");
    context->OES_stencil1 = glad_gl_has_extension(exts, exts_i, "GL_OES_stencil1");
    context->OES_stencil4 = glad_gl_has_extension(exts, exts_i, "GL_OES_stencil4");
    context->OES_surfaceless_context = glad_gl_has_extension(exts, exts_i, "GL_OES_surfaceless_context");
    context->OES_tessellation_point_size = glad_gl_has_extension(exts, exts_i, "GL_OES_tessellation_point_size");
    context->OES_tessellation_shader = glad_gl_has_extension(exts, exts_i, "GL_OES_tessellation_shader");
    context->OES_texture_3D = glad_gl_has_extension(exts, exts_i, "GL_OES_texture_3D");
    context->OES_texture_border_clamp = glad_gl_has_extension(exts, exts_i, "GL_OES_texture_border_clamp");
    context->OES_texture_buffer = glad_gl_has_extension(exts, exts_i, "GL_OES_texture_buffer");
    context->OES_texture_compression_astc = glad_gl_has_extension(exts, exts_i, "GL_OES_texture_compression_astc");
    context->OES_texture_cube_map_array = glad_gl_has_extension(exts, exts_i, "GL_OES_texture_cube_map_array");
    context->OES_texture_float = glad_gl_has_extension(exts, exts_i, "GL_OES_texture_float");
    context->OES_texture_float_linear = glad_gl_has_extension(exts, exts_i, "GL_OES_texture_float_linear");
    context->OES_texture_half_float = glad_gl_has_extension(exts, exts_i, "GL_OES_texture_half_float");
    context->OES_texture_half_float_linear = glad_gl_has_extension(exts, exts_i, "GL_OES_texture_half_float_linear");
    context->OES_texture_npot = glad_gl_has_extension(exts, exts_i, "GL_OES_texture_npot");
    context->OES_texture_stencil8 = glad_gl_has_extension(exts, exts_i, "GL_OES_texture_stencil8");
    context->OES_texture_storage_multisample_2d_array = glad_gl_has_extension(exts, exts_i, "GL_OES_texture_storage_multisample_2d_array");
    context->OES_texture_view = glad_gl_has_extension(exts, exts_i, "GL_OES_texture_view");
    context->OES_vertex_array_object = glad_gl_has_extension(exts, exts_i, "GL_OES_vertex_array_object");
    context->OES_vertex_half_float = glad_gl_has_extension(exts, exts_i, "GL_OES_vertex_half_float");
    context->OES_vertex_type_10_10_10_2 = glad_gl_has_extension(exts, exts_i, "GL_OES_vertex_type_10_10_10_2");
    context->OES_viewport_array = glad_gl_has_extension(exts, exts_i, "GL_OES_viewport_array");
    context->OVR_multiview = glad_gl_has_extension(exts, exts_i, "GL_OVR_multiview");
    context->OVR_multiview2 = glad_gl_has_extension(exts, exts_i, "GL_OVR_multiview2");
    context->OVR_multiview_multisampled_render_to_texture = glad_gl_has_extension(exts, exts_i, "GL_OVR_multiview_multisampled_render_to_texture");
    context->QCOM_YUV_texture_gather = glad_gl_has_extension(exts, exts_i, "GL_QCOM_YUV_texture_gather");
    context->QCOM_alpha_test = glad_gl_has_extension(exts, exts_i, "GL_QCOM_alpha_test");
    context->QCOM_binning_control = glad_gl_has_extension(exts, exts_i, "GL_QCOM_binning_control");
    context->QCOM_driver_control = glad_gl_has_extension(exts, exts_i, "GL_QCOM_driver_control");
    context->QCOM_extended_get = glad_gl_has_extension(exts, exts_i, "GL_QCOM_extended_get");
    context->QCOM_extended_get2 = glad_gl_has_extension(exts, exts_i, "GL_QCOM_extended_get2");
    context->QCOM_frame_extrapolation = glad_gl_has_extension(exts, exts_i, "GL_QCOM_frame_extrapolation");
    context->QCOM_framebuffer_foveated = glad_gl_has_extension(exts, exts_i, "GL_QCOM_framebuffer_foveated");
    context->QCOM_motion_estimation = glad_gl_has_extension(exts, exts_i, "GL_QCOM_motion_estimation");
    context->QCOM_perfmon_global_mode = glad_gl_has_extension(exts, exts_i, "GL_QCOM_perfmon_global_mode");
    context->QCOM_render_sRGB_R8_RG8 = glad_gl_has_extension(exts, exts_i, "GL_QCOM_render_sRGB_R8_RG8");
    context->QCOM_render_shared_exponent = glad_gl_has_extension(exts, exts_i, "GL_QCOM_render_shared_exponent");
    context->QCOM_shader_framebuffer_fetch_noncoherent = glad_gl_has_extension(exts, exts_i, "GL_QCOM_shader_framebuffer_fetch_noncoherent");
    context->QCOM_shader_framebuffer_fetch_rate = glad_gl_has_extension(exts, exts_i, "GL_QCOM_shader_framebuffer_fetch_rate");
    context->QCOM_shading_rate = glad_gl_has_extension(exts, exts_i, "GL_QCOM_shading_rate");
    context->QCOM_texture_foveated = glad_gl_has_extension(exts, exts_i, "GL_QCOM_texture_foveated");
    context->QCOM_texture_foveated2 = glad_gl_has_extension(exts, exts_i, "GL_QCOM_texture_foveated2");
    context->QCOM_texture_foveated_subsampled_layout = glad_gl_has_extension(exts, exts_i, "GL_QCOM_texture_foveated_subsampled_layout");
    context->QCOM_texture_lod_bias = glad_gl_has_extension(exts, exts_i, "GL_QCOM_texture_lod_bias");
    context->QCOM_tiled_rendering = glad_gl_has_extension(exts, exts_i, "GL_QCOM_tiled_rendering");
    context->QCOM_writeonly_rendering = glad_gl_has_extension(exts, exts_i, "GL_QCOM_writeonly_rendering");
    context->QCOM_ycbcr_degamma = glad_gl_has_extension(exts, exts_i, "GL_QCOM_ycbcr_degamma");
    context->VIV_shader_binary = glad_gl_has_extension(exts, exts_i, "GL_VIV_shader_binary");

    glad_gl_free_extensions(exts_i);

    return 1;
}

static int glad_gl_find_core_gles2(GladGLES2Context *context) {
    int i;
    const char* version;
    const char* prefixes[] = {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        "OpenGL SC ",
        NULL
    };
    int major = 0;
    int minor = 0;
    version = (const char*) context->GetString(GL_VERSION);
    if (!version) return 0;
    for (i = 0;  prefixes[i];  i++) {
        const size_t length = strlen(prefixes[i]);
        if (strncmp(version, prefixes[i], length) == 0) {
            version += length;
            break;
        }
    }

    GLAD_IMPL_UTIL_SSCANF(version, "%d.%d", &major, &minor);

    context->ES_VERSION_2_0 = (major == 2 && minor >= 0) || major > 2;
    context->ES_VERSION_3_0 = (major == 3 && minor >= 0) || major > 3;
    context->ES_VERSION_3_1 = (major == 3 && minor >= 1) || major > 3;
    context->ES_VERSION_3_2 = (major == 3 && minor >= 2) || major > 3;

    return GLAD_MAKE_VERSION(major, minor);
}

int gladLoadGLES2ContextUserPtr(GladGLES2Context *context, GLADuserptrloadfunc load, void *userptr) {
    int version;

    context->GetString = (PFNGLGETSTRINGPROC) load(userptr, "glGetString");
    if(context->GetString == NULL) return 0;
    version = glad_gl_find_core_gles2(context);

    glad_gl_load_GL_ES_VERSION_2_0(context, load, userptr);
    glad_gl_load_GL_ES_VERSION_3_0(context, load, userptr);
    glad_gl_load_GL_ES_VERSION_3_1(context, load, userptr);
    glad_gl_load_GL_ES_VERSION_3_2(context, load, userptr);

    if (!glad_gl_find_extensions_gles2(context)) return 0;
    glad_gl_load_GL_AMD_framebuffer_multisample_advanced(context, load, userptr);
    glad_gl_load_GL_AMD_performance_monitor(context, load, userptr);
    glad_gl_load_GL_ANGLE_framebuffer_blit(context, load, userptr);
    glad_gl_load_GL_ANGLE_framebuffer_multisample(context, load, userptr);
    glad_gl_load_GL_ANGLE_instanced_arrays(context, load, userptr);
    glad_gl_load_GL_ANGLE_translated_shader_source(context, load, userptr);
    glad_gl_load_GL_APPLE_copy_texture_levels(context, load, userptr);
    glad_gl_load_GL_APPLE_framebuffer_multisample(context, load, userptr);
    glad_gl_load_GL_APPLE_sync(context, load, userptr);
    glad_gl_load_GL_ARM_shader_core_properties(context, load, userptr);
    glad_gl_load_GL_EXT_EGL_image_storage(context, load, userptr);
    glad_gl_load_GL_EXT_base_instance(context, load, userptr);
    glad_gl_load_GL_EXT_blend_func_extended(context, load, userptr);
    glad_gl_load_GL_EXT_buffer_storage(context, load, userptr);
    glad_gl_load_GL_EXT_clear_texture(context, load, userptr);
    glad_gl_load_GL_EXT_clip_control(context, load, userptr);
    glad_gl_load_GL_EXT_copy_image(context, load, userptr);
    glad_gl_load_GL_EXT_debug_label(context, load, userptr);
    glad_gl_load_GL_EXT_debug_marker(context, load, userptr);
    glad_gl_load_GL_EXT_discard_framebuffer(context, load, userptr);
    glad_gl_load_GL_EXT_disjoint_timer_query(context, load, userptr);
    glad_gl_load_GL_EXT_draw_buffers(context, load, userptr);
    glad_gl_load_GL_EXT_draw_buffers_indexed(context, load, userptr);
    glad_gl_load_GL_EXT_draw_elements_base_vertex(context, load, userptr);
    glad_gl_load_GL_EXT_draw_instanced(context, load, userptr);
    glad_gl_load_GL_EXT_draw_transform_feedback(context, load, userptr);
    glad_gl_load_GL_EXT_external_buffer(context, load, userptr);
    glad_gl_load_GL_EXT_fragment_shading_rate(context, load, userptr);
    glad_gl_load_GL_EXT_framebuffer_blit_layers(context, load, userptr);
    glad_gl_load_GL_EXT_geometry_shader(context, load, userptr);
    glad_gl_load_GL_EXT_instanced_arrays(context, load, userptr);
    glad_gl_load_GL_EXT_map_buffer_range(context, load, userptr);
    glad_gl_load_GL_EXT_memory_object(context, load, userptr);
    glad_gl_load_GL_EXT_memory_object_fd(context, load, userptr);
    glad_gl_load_GL_EXT_memory_object_win32(context, load, userptr);
    glad_gl_load_GL_EXT_multi_draw_arrays(context, load, userptr);
    glad_gl_load_GL_EXT_multi_draw_indirect(context, load, userptr);
    glad_gl_load_GL_EXT_multisampled_render_to_texture(context, load, userptr);
    glad_gl_load_GL_EXT_multiview_draw_buffers(context, load, userptr);
    glad_gl_load_GL_EXT_occlusion_query_boolean(context, load, userptr);
    glad_gl_load_GL_EXT_polygon_offset_clamp(context, load, userptr);
    glad_gl_load_GL_EXT_primitive_bounding_box(context, load, userptr);
    glad_gl_load_GL_EXT_raster_multisample(context, load, userptr);
    glad_gl_load_GL_EXT_robustness(context, load, userptr);
    glad_gl_load_GL_EXT_semaphore(context, load, userptr);
    glad_gl_load_GL_EXT_semaphore_fd(context, load, userptr);
    glad_gl_load_GL_EXT_semaphore_win32(context, load, userptr);
    glad_gl_load_GL_EXT_separate_shader_objects(context, load, userptr);
    glad_gl_load_GL_EXT_shader_framebuffer_fetch_non_coherent(context, load, userptr);
    glad_gl_load_GL_EXT_shader_pixel_local_storage2(context, load, userptr);
    glad_gl_load_GL_EXT_sparse_texture(context, load, userptr);
    glad_gl_load_GL_EXT_tessellation_shader(context, load, userptr);
    glad_gl_load_GL_EXT_texture_border_clamp(context, load, userptr);
    glad_gl_load_GL_EXT_texture_buffer(context, load, userptr);
    glad_gl_load_GL_EXT_texture_storage(context, load, userptr);
    glad_gl_load_GL_EXT_texture_storage_compression(context, load, userptr);
    glad_gl_load_GL_EXT_texture_view(context, load, userptr);
    glad_gl_load_GL_EXT_win32_keyed_mutex(context, load, userptr);
    glad_gl_load_GL_EXT_window_rectangles(context, load, userptr);
    glad_gl_load_GL_IMG_bindless_texture(context, load, userptr);
    glad_gl_load_GL_IMG_framebuffer_downsample(context, load, userptr);
    glad_gl_load_GL_IMG_multisampled_render_to_texture(context, load, userptr);
    glad_gl_load_GL_INTEL_framebuffer_CMAA(context, load, userptr);
    glad_gl_load_GL_INTEL_performance_query(context, load, userptr);
    glad_gl_load_GL_KHR_blend_equation_advanced(context, load, userptr);
    glad_gl_load_GL_KHR_debug(context, load, userptr);
    glad_gl_load_GL_KHR_parallel_shader_compile(context, load, userptr);
    glad_gl_load_GL_KHR_robustness(context, load, userptr);
    glad_gl_load_GL_MESA_framebuffer_flip_y(context, load, userptr);
    glad_gl_load_GL_MESA_sampler_objects(context, load, userptr);
    glad_gl_load_GL_NV_bindless_texture(context, load, userptr);
    glad_gl_load_GL_NV_blend_equation_advanced(context, load, userptr);
    glad_gl_load_GL_NV_clip_space_w_scaling(context, load, userptr);
    glad_gl_load_GL_NV_conditional_render(context, load, userptr);
    glad_gl_load_GL_NV_conservative_raster(context, load, userptr);
    glad_gl_load_GL_NV_conservative_raster_pre_snap_triangles(context, load, userptr);
    glad_gl_load_GL_NV_copy_buffer(context, load, userptr);
    glad_gl_load_GL_NV_coverage_sample(context, load, userptr);
    glad_gl_load_GL_NV_draw_buffers(context, load, userptr);
    glad_gl_load_GL_NV_draw_instanced(context, load, userptr);
    glad_gl_load_GL_NV_draw_vulkan_image(context, load, userptr);
    glad_gl_load_GL_NV_fence(context, load, userptr);
    glad_gl_load_GL_NV_fragment_coverage_to_color(context, load, userptr);
    glad_gl_load_GL_NV_framebuffer_blit(context, load, userptr);
    glad_gl_load_GL_NV_framebuffer_mixed_samples(context, load, userptr);
    glad_gl_load_GL_NV_framebuffer_multisample(context, load, userptr);
    glad_gl_load_GL_NV_gpu_shader5(context, load, userptr);
    glad_gl_load_GL_NV_instanced_arrays(context, load, userptr);
    glad_gl_load_GL_NV_internalformat_sample_query(context, load, userptr);
    glad_gl_load_GL_NV_memory_attachment(context, load, userptr);
    glad_gl_load_GL_NV_memory_object_sparse(context, load, userptr);
    glad_gl_load_GL_NV_mesh_shader(context, load, userptr);
    glad_gl_load_GL_NV_non_square_matrices(context, load, userptr);
    glad_gl_load_GL_NV_path_rendering(context, load, userptr);
    glad_gl_load_GL_NV_polygon_mode(context, load, userptr);
    glad_gl_load_GL_NV_read_buffer(context, load, userptr);
    glad_gl_load_GL_NV_sample_locations(context, load, userptr);
    glad_gl_load_GL_NV_scissor_exclusive(context, load, userptr);
    glad_gl_load_GL_NV_shading_rate_image(context, load, userptr);
    glad_gl_load_GL_NV_texture_barrier(context, load, userptr);
    glad_gl_load_GL_NV_timeline_semaphore(context, load, userptr);
    glad_gl_load_GL_NV_viewport_array(context, load, userptr);
    glad_gl_load_GL_NV_viewport_swizzle(context, load, userptr);
    glad_gl_load_GL_OES_EGL_image(context, load, userptr);
    glad_gl_load_GL_OES_EGL_image_external(context, load, userptr);
    glad_gl_load_GL_OES_copy_image(context, load, userptr);
    glad_gl_load_GL_OES_draw_buffers_indexed(context, load, userptr);
    glad_gl_load_GL_OES_draw_elements_base_vertex(context, load, userptr);
    glad_gl_load_GL_OES_geometry_shader(context, load, userptr);
    glad_gl_load_GL_OES_get_program_binary(context, load, userptr);
    glad_gl_load_GL_OES_mapbuffer(context, load, userptr);
    glad_gl_load_GL_OES_primitive_bounding_box(context, load, userptr);
    glad_gl_load_GL_OES_sample_shading(context, load, userptr);
    glad_gl_load_GL_OES_tessellation_shader(context, load, userptr);
    glad_gl_load_GL_OES_texture_3D(context, load, userptr);
    glad_gl_load_GL_OES_texture_border_clamp(context, load, userptr);
    glad_gl_load_GL_OES_texture_buffer(context, load, userptr);
    glad_gl_load_GL_OES_texture_storage_multisample_2d_array(context, load, userptr);
    glad_gl_load_GL_OES_texture_view(context, load, userptr);
    glad_gl_load_GL_OES_vertex_array_object(context, load, userptr);
    glad_gl_load_GL_OES_viewport_array(context, load, userptr);
    glad_gl_load_GL_OVR_multiview(context, load, userptr);
    glad_gl_load_GL_OVR_multiview_multisampled_render_to_texture(context, load, userptr);
    glad_gl_load_GL_QCOM_alpha_test(context, load, userptr);
    glad_gl_load_GL_QCOM_driver_control(context, load, userptr);
    glad_gl_load_GL_QCOM_extended_get(context, load, userptr);
    glad_gl_load_GL_QCOM_extended_get2(context, load, userptr);
    glad_gl_load_GL_QCOM_frame_extrapolation(context, load, userptr);
    glad_gl_load_GL_QCOM_framebuffer_foveated(context, load, userptr);
    glad_gl_load_GL_QCOM_motion_estimation(context, load, userptr);
    glad_gl_load_GL_QCOM_shader_framebuffer_fetch_noncoherent(context, load, userptr);
    glad_gl_load_GL_QCOM_shading_rate(context, load, userptr);
    glad_gl_load_GL_QCOM_texture_foveated(context, load, userptr);
    glad_gl_load_GL_QCOM_tiled_rendering(context, load, userptr);



    return version;
}


int gladLoadGLES2Context(GladGLES2Context *context, GLADloadfunc load) {
    return gladLoadGLES2ContextUserPtr(context, glad_gl_get_proc_from_userptr, GLAD_GNUC_EXTENSION (void*) load);
}



 

#ifdef GLAD_GLES2

#ifndef GLAD_LOADER_LIBRARY_C_
#define GLAD_LOADER_LIBRARY_C_

#include <stddef.h>
#include <stdlib.h>

#if GLAD_PLATFORM_WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif


static void* glad_get_dlopen_handle(const char *lib_names[], int length) {
    void *handle = NULL;
    int i;

    for (i = 0; i < length; ++i) {
#if GLAD_PLATFORM_WIN32
  #if GLAD_PLATFORM_UWP
        size_t buffer_size = (strlen(lib_names[i]) + 1) * sizeof(WCHAR);
        LPWSTR buffer = (LPWSTR) malloc(buffer_size);
        if (buffer != NULL) {
            int ret = MultiByteToWideChar(CP_ACP, 0, lib_names[i], -1, buffer, buffer_size);
            if (ret != 0) {
                handle = (void*) LoadPackagedLibrary(buffer, 0);
            }
            free((void*) buffer);
        }
  #else
        handle = (void*) LoadLibraryA(lib_names[i]);
  #endif
#else
        handle = dlopen(lib_names[i], RTLD_LAZY | RTLD_LOCAL);
#endif
        if (handle != NULL) {
            return handle;
        }
    }

    return NULL;
}

static void glad_close_dlopen_handle(void* handle) {
    if (handle != NULL) {
#if GLAD_PLATFORM_WIN32
        FreeLibrary((HMODULE) handle);
#else
        dlclose(handle);
#endif
    }
}

static GLADapiproc glad_dlsym_handle(void* handle, const char *name) {
    if (handle == NULL) {
        return NULL;
    }

#if GLAD_PLATFORM_WIN32
    return (GLADapiproc) GetProcAddress((HMODULE) handle, name);
#else
    return GLAD_GNUC_EXTENSION (GLADapiproc) dlsym(handle, name);
#endif
}

#endif /* GLAD_LOADER_LIBRARY_C_ */

#if GLAD_PLATFORM_EMSCRIPTEN
#ifndef GLAD_EGL_H_
  typedef void (*__eglMustCastToProperFunctionPointerType)(void);
  typedef __eglMustCastToProperFunctionPointerType (GLAD_API_PTR *PFNEGLGETPROCADDRESSPROC)(const char *name);
#endif
  extern __eglMustCastToProperFunctionPointerType emscripten_GetProcAddress(const char *name);
#elif defined(GLAD_GLES2_USE_SYSTEM_EGL)
  #include <EGL/egl.h>
  typedef __eglMustCastToProperFunctionPointerType (GLAD_API_PTR *PFNEGLGETPROCADDRESSPROC)(const char *name);
#else
  #include <glad/egl.h>
#endif


struct _glad_gles2_userptr {
    void *handle;
    PFNEGLGETPROCADDRESSPROC get_proc_address_ptr;
};


static GLADapiproc glad_gles2_get_proc(void *vuserptr, const char* name) {
    struct _glad_gles2_userptr userptr = *(struct _glad_gles2_userptr*) vuserptr;
    GLADapiproc result = NULL;

#if GLAD_PLATFORM_EMSCRIPTEN
    GLAD_UNUSED(&glad_dlsym_handle);
#else
    result = glad_dlsym_handle(userptr.handle, name);
#endif
    if (result == NULL) {
        result = userptr.get_proc_address_ptr(name);
    }

    return result;
}


static void* glad_gles2_dlopen_handle(GladGLES2Context *context) {
#if GLAD_PLATFORM_EMSCRIPTEN
#elif GLAD_PLATFORM_APPLE
    static const char *NAMES[] = {"libGLESv2.dylib"};
#elif GLAD_PLATFORM_WIN32
    static const char *NAMES[] = {"GLESv2.dll", "libGLESv2.dll"};
#else
    static const char *NAMES[] = {"libGLESv2.so.2", "libGLESv2.so"};
#endif

#if GLAD_PLATFORM_EMSCRIPTEN
    GLAD_UNUSED(&glad_get_dlopen_handle);
    return NULL;
#else
    if (context->glad_loader_handle == NULL) {
        context->glad_loader_handle = glad_get_dlopen_handle(NAMES, sizeof(NAMES) / sizeof(NAMES[0]));
    }

    return context->glad_loader_handle;
#endif
}

static struct _glad_gles2_userptr glad_gles2_build_userptr(void *handle) {
    struct _glad_gles2_userptr userptr;
#if GLAD_PLATFORM_EMSCRIPTEN
    GLAD_UNUSED(handle);
    userptr.get_proc_address_ptr = emscripten_GetProcAddress;
#else
    userptr.handle = handle;
    userptr.get_proc_address_ptr = eglGetProcAddress;
#endif
    return userptr;
}

int gladLoaderLoadGLES2Context(GladGLES2Context *context) {
    int version = 0;
    void *handle = NULL;
    int did_load = 0;
    struct _glad_gles2_userptr userptr;

#if GLAD_PLATFORM_EMSCRIPTEN
    GLAD_UNUSED(handle);
    GLAD_UNUSED(did_load);
    GLAD_UNUSED(&glad_gles2_dlopen_handle);
    GLAD_UNUSED(&glad_gles2_build_userptr);
    userptr.get_proc_address_ptr = emscripten_GetProcAddress;
    version = gladLoadGLES2ContextUserPtr(context, glad_gles2_get_proc, &userptr);
#else
#ifndef GLAD_GLES2_USE_SYSTEM_EGL
    if (eglGetProcAddress == NULL) {
        return 0;
    }
#endif
    did_load = context->glad_loader_handle == NULL;
    handle = glad_gles2_dlopen_handle(context);
    if (handle != NULL) {
        userptr = glad_gles2_build_userptr(handle);

        version = gladLoadGLES2ContextUserPtr(context, glad_gles2_get_proc, &userptr);

        if (!version && did_load) {
            gladLoaderUnloadGLES2Context(context);
        }
    }
#endif

    return version;
}



void gladLoaderUnloadGLES2Context(GladGLES2Context *context) {
    if (context->glad_loader_handle != NULL) {
        glad_close_dlopen_handle(context->glad_loader_handle);
        context->glad_loader_handle = NULL;
    }
}

#endif /* GLAD_GLES2 */

#ifdef __cplusplus
}
#endif
