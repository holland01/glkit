#ifndef __GLK_UTIL_H__
#define __GLK_UTIL_H__

#include "include_gl.h"
#include "core.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#define GLK_UTIL_SHADER(s) "#version " GLK_GLSL_VERSION " core\n"#s

namespace glk {

//------------------------------------------------------------------------------------
// shader progs and related subroutines
//------------------------------------------------------------------------------------

static const char* GLSL_VERTEX_SHADER = GLK_UTIL_SHADER(
                                               layout(location = 0) in vec3 position;
                                               layout(location = 1) in vec2 st;
                                               layout(location = 2) in vec4 color;

                                               uniform mat4 modelView;
                                               uniform mat4 projection;

                                               out vec4 vary_Color;
                                               out vec2 vary_St;

                                               void main(void) {
                                                   gl_Position = projection * modelView * vec4(position, 1.0);
                                                   vary_Color = color;
                                                   vary_St = st;
                                               }
                                            );

static const char* GLSL_FRAGMENT_SHADER = GLK_UTIL_SHADER(
                                                 in vec4 vary_Color;
                                                 in vec2 vary_St;
                                                 out vec4 out_Fragment;

                                                 uniform sampler2D sampler0;

                                                 void main(void) {
                                                     out_Fragment = vary_Color * vec4(texture(sampler0, vary_St).rgb, 1.0);
                                                 }
                                            );

class input_pole_t
{
    enum {
        state_left = 0,
        state_forward,
        state_right,
        state_backward,
        state_up,
        state_down
    };

    std::array<bool, 6> input_state;

public:
    input_pole_t(void)
    {
        input_state.fill(false);
    }

    void set_moving_left(bool t) { input_state[state_left] = t; }
    void set_moving_forward(bool t) { input_state[state_forward] = t; }
    void set_moving_right(bool t) { input_state[state_right] = t; }
    void set_moving_backward(bool t) { input_state[state_backward] = t; }
    void set_moving_up(bool t) { input_state[state_up] = t; }
    void set_moving_down(bool t) { input_state[state_down] = t; }

    bool moving_left(void) const { return input_state[state_left]; }
    bool moving_forward(void) const { return input_state[state_forward]; }
    bool moving_right(void) const { return input_state[state_right]; }
    bool moving_backward(void) const { return input_state[state_backward]; }
    bool moving_up(void) const { return input_state[state_up]; }
    bool moving_down(void) const { return input_state[state_down]; }
};

class camera_t
{
    bool is_ortho;

    uint16_t screen_width, screen_height;

    glm::vec3 origin;
    glm::vec3 translate_scale;
    glm::mat4 view;
    glm::mat4 projection;

public:
    camera_t(uint16_t screen_w, uint16_t screen_h)
        :   is_ortho(false),
            screen_width(screen_w), screen_height(screen_h),
            origin(0.0f), translate_scale(1.0f),
            view(1.0f), projection(1.0f)

    {}

    void perspective(float fovy, float znear, float zfar)
    {
        projection = glm::perspective(glm::radians(fovy),
                                      static_cast<float>(screen_width) / static_cast<float>(screen_height),
                                      znear, zfar);

        is_ortho = false;
    }

    void ortho(float znear, float zfar)
    {
        float fw = static_cast<float>(screen_width) * 0.5f;
        float fh = static_cast<float>(screen_height) * 0.5f;

        ortho(-fw, fw, -fh, fh, znear, zfar);
    }

    void ortho(float left, float right, float bottom, float top, float znear, float zfar)
    {
        projection = glm::ortho(left, right, bottom, top, znear, zfar);

        is_ortho = true;
    }

    void strafe(float t)
    {
        origin.x += t;
    }

    void raise(float t)
    {
        origin.y += t;
    }

    void walk(float t)
    {
        origin.z -= t; // negative z is the forward axis
    }

    void set_translate_scale(float s)
    {
        translate_scale = glm::vec3(s, s, s);
    }

    void set_origin(const glm::vec3& v)
    {
        origin = v;
    }

    void update_from_input(const input_pole_t& pole)
    {
        if (pole.moving_forward()) walk(translate_scale.z);
        if (pole.moving_backward()) walk(-translate_scale.z);
        if (pole.moving_up()) raise(translate_scale.y);
        if (pole.moving_down()) raise(-translate_scale.y);
        if (pole.moving_right()) strafe(translate_scale.x);
        if (pole.moving_left()) strafe(-translate_scale.x);
    }

    glm::mat4 model_to_view(void)
    {
     //   view[0] = glm::vec4(scale.x, 0.0f, 0.0f, 0.0f);
     //   view[1] = glm::vec4(0.0f, scale.y, 0.0f, 0.0f);
     //   view[2] = glm::vec4(0.0f, 0.0f, scale.z, 0.0f);

        glm::vec3 o(origin.x, origin.y, origin.z);

        view[3] = glm::vec4(/*is_ortho? (*/-o/*): (-o)*/, 1.0f); // flip for view space translation

        return view;
    }

    const glm::mat4& view_to_clip(void) const { return projection; }

    uint16_t view_width(void) const { return screen_width; }

    uint16_t view_height(void) const { return screen_height; }

    const glm::vec3& view_origin(void) const { return origin; }

    void log_model_to_view(void) const
    {
        glk_logf("model_to_view:\n%s", glm::to_string(view).c_str());
    }
};

struct vertex_t {
    GLfloat position[3];
    GLfloat st[2];
    GLubyte color[4];
};

} // end namespace glk

#endif // __GLK_UTIL_H__
