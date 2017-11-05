#ifndef __GLK_INCLUDE_H__
#define __GLK_INCLUDE_H__

#include "main_def.h"

#ifdef GLK_INCLUDE_GLEW
    #include <GL/glew.h>
    #include <GLFW/glfw3.h>
#elif defined(GLK_INCLUDE_EGL)
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#else
    #error "define either GLK_INCLUDE_EGL or GLK_INCLUDE_GLEW to " \
        "choose header implementation"
#endif

#endif // __GLK_INCLUDE_H__
