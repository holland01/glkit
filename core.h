#ifndef __GLK_LOG_H__
#define __GLK_LOG_H__

#include "glk_include_gl.h"

#include <vector>
#include <string>
#include <memory>

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>

#ifdef GLK_IO
#define glk_logf(...) glk::logf_impl(__LINE__, __FUNCTION__, __VA_ARGS__)
#else
#define glk_logf(...)
#endif

#ifdef GLK_DEBUG_OPENGL
#define GLK_H(expr) \
do { \
    ( expr ); \
    glk::exit_on_gl_error(__LINE__, __FUNCTION__, #expr); \
} while (0)
#else
#define GLK_H(expr) expr
#endif

namespace glk {

// It's bare bones now, but it eventually won't be.
class state_t {
    bool is_running = true;

public:
    bool running(void) const { return is_running; }
    void set_running(bool r) { is_running = r; }
};

static std::unique_ptr< state_t > g_state( new state_t() );

static glk_inline void exit_on_error(void)
{
#ifdef GLK_MAIN_LOOP
    g_state->set_running(false);
#else
    exit(1);
#endif
}

// We want to prevent spamming to stdout
static std::vector<std::string> g_gl_err_msg_cache;

static glk_inline void exit_on_gl_error(int line, const char* func,
    const char* expr)
{
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        char msg[256];
        memset(msg, 0, sizeof(msg));

#ifdef GLK_INCLUDE_GLEW
        sprintf(
            &msg[0],
            "GLK_ERROR (%x) in %s@%i [%s]: %s\n",
            err,
            func,
            line,
            expr,
            (const char* )gluErrorString(err));
#else // No error string implementation at the moment - some day...
        sprintf(
            &msg[0],
            "GLK_ERROR (%x) in %s@%i [%s]\n",
            err,
            func,
            line,
            expr);
#endif

        std::string smsg(msg);

        if (std::find(g_gl_err_msg_cache.begin(), g_gl_err_msg_cache.end(), smsg)
            == g_gl_err_msg_cache.end()) {
            printf("%s", smsg.c_str());
            g_gl_err_msg_cache.push_back(smsg);
        }

        exit_on_error();
    }
}

static glk_inline void logf_impl( int line, const char* func,
    const char* fmt, ... )
{
    va_list arg;

    va_start( arg, fmt );
    fprintf( stdout, "\n[ %s@%i ]: ", func, line );
    vfprintf( stdout, fmt, arg );
    fputs( "\n", stdout );
    va_end( arg );
}

static glk_inline GLuint compile_shader(const char* shader_src, GLenum shader_type)
{
    GLuint shader;
    GLK_H( shader = glCreateShader(shader_type) );
    GLK_H( glShaderSource(shader, 1, &shader_src, NULL) );
    GLK_H( glCompileShader(shader) );

    GLint compile_success;
    GLK_H( glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_success) );

    if (compile_success == GL_FALSE) {
        GLint info_log_len;
        GLK_H( glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_len) );

        std::vector<char> log_msg(info_log_len + 1, 0);
        GLK_H( glGetShaderInfoLog(shader, (GLsizei)(log_msg.size() - 1),
                                 NULL, &log_msg[0]) );

        glk_logf("COMPILE ERROR: %s\n\nSOURCE\n\n---------------\n%s\n--------------",
             &log_msg[0], shader_src);

        return 0;
    }

    return shader;
}

static glk_inline GLuint link_program(const char* vertex_src, const char* fragment_src)
{
    GLuint vertex, fragment;

    vertex = compile_shader(vertex_src, GL_VERTEX_SHADER);
    if (!vertex)
        goto fail;

    fragment = compile_shader(fragment_src, GL_FRAGMENT_SHADER);
    if (!fragment)
        goto fail;

    {
        GLuint program;
        GLK_H( program = glCreateProgram() );

        GLK_H( glAttachShader(program, vertex) );
        GLK_H( glAttachShader(program, fragment) );

        GLK_H( glLinkProgram(program) );

        GLK_H( glDetachShader(program, vertex) );
        GLK_H( glDetachShader(program, fragment) );

        GLK_H( glDeleteShader(vertex) );
        GLK_H( glDeleteShader(fragment) );

        GLint link_success;
        GLK_H( glGetProgramiv(program, GL_LINK_STATUS, &link_success) );

        if (link_success == GL_FALSE) {
            GLint info_log_len;
            GLK_H( glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_len) );

            std::vector<char> log_msg(info_log_len + 1, 0);
            GLK_H( glGetProgramInfoLog(program, (GLsizei)(log_msg.size() - 1),
                                      NULL, &log_msg[0]) );

            glk_logf("LINK ERROR:\n Program ID: %lu\n Error: %s",
                 program, &log_msg[0]);

            goto fail;
        }

        return program;
    }

fail:
    exit_on_error();
    return 0;
}

} // end namespace glk

#endif // __GLK_LOG_H__
