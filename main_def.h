#ifndef __GLK_MAIN_DEF_H__
#define __GLK_MAIN_DEF_H__

#ifdef _WIN32	
#define GLK_PATH_SEP '\\'
#define GLK_PATH_SEP_STR "\\"
#else
#define GLK_PATH_SEP '/'
#define GLK_PATH_SEP_STR "/"
#endif

#define GLM_ENABLE_EXPERIMENTAL

#define GLK_MAIN_LOOP
#define GLK_INCLUDE_GLEW
#define glk_inline inline

#define GLK_FUNC static glk_inline

#define GLK_IO
#define GLK_DEBUG_OPENGL

#define GLK_ATLAS_TEX_FORMAT GL_RGBA
#define GLK_ATLAS_DESIRED_BPP 4

#define GLK_GLSL_VERSION "430"

#define GLK_UNUSED(v) (void)(v)

#endif // __GLK_MAIN_DEF_H__
