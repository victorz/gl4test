#include <stdlib.h>
#include <math.h>
#include <stdio.h>
/* TODO: try to eliminate the need for glew. */
#include <GL/glew.h>
#include <GL/glfw.h>

static const double PI = 3.14159265358979323846;

GLenum init_window(int w, int h);
void gl_errors(size_t line);
void setup_projection();

int main()
{
	int window_width = 500;
	int window_height = 500;
	// Sphere radius properties.
	int resolution = 20;
	float radius = 0.5f;

	GLfloat cap[1 + resolution][3];
	float sector_delta = 2 * PI / resolution;

	if (init_window(window_width, window_height) != GL_TRUE) {
		return 1;
	}

	// Generate north polar cap mesh. Can be rotated to provide the
	// south cap.
	cap[0][0] = 0.0f;
	cap[0][1] = 1.0f;
	cap[0][2] = 0.0f;
	for (int i = 1; i <= resolution; i++) {
		cap[i][0] = radius * cosf(i * sector_delta);
		cap[i][1] = radius * cosf(sector_delta);
		cap[i][2] = radius * sinf(i * sector_delta);
	}

	GLuint vert_array;
	glGenVertexArrays(1, &vert_array);
	glBindVertexArray(vert_array);

	GLuint cap_vert_buf;
	glGenBuffers(1, &cap_vert_buf);
	glBindBuffer(GL_ARRAY_BUFFER, cap_vert_buf);
	glBufferData(GL_ARRAY_BUFFER, (1 + resolution) * 3 * sizeof(GLfloat), cap, GL_STATIC_DRAW);

	// Black clear color.
	glClearColor(0.1f, 0.0f, 0.5f, 1.0f);
	gl_errors(__LINE__);

	setup_projection();
	gl_errors(__LINE__);

	// Main program loop.
	do {
		glClear(GL_COLOR_BUFFER_BIT);
		// glfwWaitEvents();

		// Draw calls
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		gl_errors(__LINE__);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 1 + resolution);
		glDisableVertexAttribArray(0);
		gl_errors(__LINE__);

		// Event handling
		// glfwPollEvents();
		glfwSwapBuffers();
		glfwSleep(0.016);
	} while (glfwGetKey('Q') != GLFW_PRESS && glfwGetWindowParam(GLFW_OPENED) == GL_TRUE);

	glfwTerminate();

	return 0;
}

void setup_projection()
{
}

GLenum init_window(int w, int h)
{
	int major, minor, rev;
	GLenum glewResult;

	if (glfwInit() != GL_TRUE) {
		fprintf(stderr, "error: could not init GLFW\n");
		return GL_FALSE;
	}
	glfwGetVersion(&major, &minor, &rev);
	printf("GLFW %d.%d.%d\n", major, minor, rev);

	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	if (!glfwOpenWindow(w, h, 0,0,0,0, 32,0, GLFW_WINDOW)) {
		fprintf(stderr, "Could not open window\n");
		glfwTerminate();
		return GL_FALSE;
	}

	glfwGetGLVersion(&major, &minor, &rev);
	fprintf(stderr, "Context: OpenGL %d.%d.%d\n", major, minor, rev);

	glfwSetWindowTitle("Sphere Test");

	glewExperimental = GL_TRUE;
	glewResult = glewInit();
	gl_errors(__LINE__);
	if (glewResult != GLEW_OK) {
		fprintf(stderr, "error: could not init GLEW\n");
		fprintf(stderr, "error: %s\n", glewGetErrorString(glewResult));
		glfwTerminate();
		return GL_FALSE;
	}
	return GL_TRUE;
}

void gl_errors(size_t line)
{
	GLenum error;
	do {
		error = glGetError();
		switch (error) {
		case GL_INVALID_ENUM:
			fprintf(stderr, "invalid enum");
			break;
		case GL_INVALID_VALUE:
			fprintf(stderr, "invalid value");
			break;
		case GL_INVALID_OPERATION:
			fprintf(stderr, "invalid operator");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			fprintf(stderr, "invalid framebuffer operation");
			break;
		case GL_OUT_OF_MEMORY:
			fprintf(stderr, "out of memory");
			break;
		case GL_STACK_UNDERFLOW:
			fprintf(stderr, "stack underflow");
			break;
		case GL_STACK_OVERFLOW:
			fprintf(stderr, "stack overflow");
			break;
		}
		if (error != GL_NO_ERROR) {
			fprintf(stderr, " on line: %u\n", line);
		}
	} while (error != GL_NO_ERROR);
}
