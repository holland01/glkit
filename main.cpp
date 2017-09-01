#include "glk_atlas.h"
#include "glk_util.h"

#include <glm/gtx/string_cast.hpp>

#define KEY_PRESS(key) (glfwGetKey(window, (key)) == GLFW_PRESS)

int main(int argc, const char * argv[])
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_STICKY_KEYS, GLFW_TRUE);

    glk::camera_t camera(640, 480);

	GLFWwindow* window = glfwCreateWindow(camera.view_width(),
										  camera.view_height(),
										  "OpenGL",
										  nullptr, nullptr);

	glfwMakeContextCurrent(window);

	glewExperimental = true;
	GLenum res = glewInit();
	assert(res == GLEW_OK);

    GLK_H( glClearColor(0.0f, 0.0f, 0.0f, 1.0f) );

	size_t atlas_view_index = 0;
    std::array<glk::atlas_t, 2> atlasses;

    glk::make_atlas_from_dir(atlasses[0], "./textures/base_wall");

    atlasses[1].set_downscaled(true);
    glk::make_atlas_from_dir(atlasses[1], "./textures/base_wall");

    GLuint program = glk::link_program(glk::GLSL_VERTEX_SHADER, glk::GLSL_FRAGMENT_SHADER);

	GLuint vao;
    GLK_H( glGenVertexArrays(1, &vao) );
    GLK_H( glBindVertexArray(vao) );

	GLuint vbo;
    GLK_H( glGenBuffers(1, &vbo) );
    GLK_H( glBindBuffer(GL_ARRAY_BUFFER, vbo) );

    glk::vertex_t vbo_data[] = {
		{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }, { 255, 255, 255, 255 } },
		{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }, { 255, 255, 255, 255 } },
		{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 255, 255, 255, 255 } },
		{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, { 255, 255, 255, 255 } }
	};

    GLK_H( glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_data), &vbo_data[0],
					   GL_STATIC_DRAW) );

    GLK_H( glEnableVertexAttribArray(0) );
    GLK_H( glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vbo_data[0]),
                                (GLvoid*) offsetof(glk::vertex_t, position)) );

    GLK_H( glEnableVertexAttribArray(1) );
    GLK_H( glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vbo_data[0]),
                                (GLvoid*) offsetof(glk::vertex_t, st)) );

    GLK_H( glEnableVertexAttribArray(2) );
    GLK_H( glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vbo_data[0]),
                                (GLvoid*) offsetof(glk::vertex_t, color)) );

    GLK_H( glUseProgram(program) );

    atlasses[0].bind(0);

    GLK_H( glUniform1i(glGetUniformLocation(program, "sampler0"), 0) );

    bool use_perspective = true;
	camera.walk(-3.0f);

    const float CAMERA_STEP = 0.005f;

    uint32_t layer = 0;

	while (!KEY_PRESS(GLFW_KEY_ESCAPE)
		   && !glfwWindowShouldClose(window)
           && glk::g_state->running()) {

        if (use_perspective) {
            camera.perspective(40.0f, 0.01f, 10.0f);
            camera.set_scale(1.0f);
        } else {
            camera.ortho(0.01f, 10.0f);
            camera.set_scale(220.0f);
        }

        GLK_H( glUniformMatrix4fv(glGetUniformLocation(program, "modelView"),
								 1, GL_FALSE, glm::value_ptr(camera.model_to_view())) );

        GLK_H( glUniformMatrix4fv(glGetUniformLocation(program, "projection"),
								 1, GL_FALSE, glm::value_ptr(camera.view_to_clip())) );

        GLK_H( glClear(GL_COLOR_BUFFER_BIT) );
        GLK_H( glDrawArrays(GL_TRIANGLE_STRIP, 0, 4) );

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (KEY_PRESS(GLFW_KEY_W)) camera.walk(CAMERA_STEP);
		if (KEY_PRESS(GLFW_KEY_S)) camera.walk(-CAMERA_STEP);
		if (KEY_PRESS(GLFW_KEY_A)) camera.strafe(-CAMERA_STEP);
		if (KEY_PRESS(GLFW_KEY_D)) camera.strafe(CAMERA_STEP);
		if (KEY_PRESS(GLFW_KEY_SPACE)) camera.raise(CAMERA_STEP);
		if (KEY_PRESS(GLFW_KEY_LEFT_SHIFT)) camera.raise(-CAMERA_STEP);

        atlasses[atlas_view_index].bind(layer);

        if (KEY_PRESS(GLFW_KEY_RIGHT)) {
            atlas_view_index ^= 0x1;
        }

        if (KEY_PRESS(GLFW_KEY_LEFT)) {
            layer++;
            layer %= atlasses[atlas_view_index].num_layers();
        }

        if (KEY_PRESS(GLFW_KEY_UP)) {
            use_perspective = !use_perspective;
        }

        glk_logf("origin: %s", glm::to_string(camera.view_origin()).c_str());
	}

    GLK_H( glUseProgram(0) );
    GLK_H( glDeleteProgram(program) );

    GLK_H( glBindBuffer(GL_ARRAY_BUFFER, 0) );
    GLK_H( glDeleteBuffers(1, &vbo) );

    GLK_H( glBindVertexArray(0) );
    GLK_H( glDeleteVertexArrays(1, &vao) );

    for (glk::atlas_t& atlas: atlasses)
        atlas.free_memory();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
