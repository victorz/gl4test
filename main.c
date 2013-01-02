#include <stdlib.h>
#include <math.h>
#include <stdio.h>
/* TODO: try to eliminate the need for glew. */
#include <GL/glew.h>
#include <GL/glfw.h>

#include "shader.h"

static const double PI = 3.14159265358979323846;

typedef struct {
	GLuint vao;
	GLuint vertices;
	/* GLuint indicies; */
	/* GLuint normals; */
	/* GLuint texcoords; */
} vao_t;

GLenum init_window(int w, int h);
void gl_errors(size_t line);
void setup_projection(/* float aspect_ratio*/);
void gen_sphere(GLfloat r, GLuint lat_res, GLuint lng_res, vao_t *vao);

int main()
{
	int window_width = 500;
	int window_height = 500;

	if (init_window(window_width, window_height) != GL_TRUE) {
		return 1;
	}

	setup_projection();
	gl_errors(__LINE__);

	/* Sphere properties. */
	GLuint lat_res = 7;
	GLuint lng_res = 16;
	GLfloat radius = 0.9f;
	vao_t sphere;
	gen_sphere(radius, lat_res, lng_res, &sphere);

	/* OpenGL objects. */
	GLuint shaders[2];
	GLuint program;
	shaders[0] = load_shader("shaders/basic.vert", GL_VERTEX_SHADER);
	shaders[1] = load_shader("shaders/basic.frag", GL_FRAGMENT_SHADER);
	if (!shaders[0] || !shaders[1]) {
		fprintf(stderr, "could not compile shader(s)!\n");
		glfwTerminate();
		return 1;
	}
	program = link_program(shaders, 2);
	if (!program) {
		fprintf(stderr, "could not link program!\n");
		glfwTerminate();
		return 1;
	}
	glUseProgram(program);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	/* Has no effect? */
	/* glLineWidth(3.0f); */

	GLuint in_pos = glGetAttribLocation(program, "in_pos");
	glBindVertexArray(sphere.vao);
	glBindBuffer(GL_ARRAY_BUFFER, sphere.vertices);
	glEnableVertexAttribArray(in_pos);
	glVertexAttribPointer(in_pos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Main program loop.
	glClearColor(0.1f, 0.0f, 0.5f, 1.0f);
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// glfwWaitEvents();

		// Draw calls
		glDrawArrays(GL_TRIANGLE_FAN, 0, 1 + lng_res);
		gl_errors(__LINE__);

		// Event handling
		// glfwPollEvents();
		glfwSwapBuffers();
		glfwSleep(0.016);
	} while (glfwGetKey('Q') != GLFW_PRESS && glfwGetWindowParam(GLFW_OPENED) == GL_TRUE);

	glDeleteBuffers(1, &sphere.vertices);
	glDeleteVertexArrays(1, &sphere.vao);

	glfwTerminate();

	return 0;
}

/*
 * generates a UV-sphere (with longitudinal "slices" and latitudinal
 * "sectors" with geometric center at the origin.
 *
 * Input:
 * r: radius of the sphere
 * lat_res: number of "slices"; behavior undefined if non-zero.
 * lng_res: number of "sectors"; behavior undefined if non-zero.
 * vao: vertex array object to store result in.
 */
void gen_sphere(GLfloat r, GLuint lat_res, GLuint lng_res, vao_t *vao)
{
	size_t n_verts = 2 + lng_res * lat_res;
	GLfloat *verts;
	int vertex_index;

	GLfloat lat_delta = PI / (lat_res + 1);
	GLfloat lng_delta = 2 * PI / lng_res;

	/* GENERATE VERTICES */
	verts = calloc(n_verts * 3, sizeof(GLfloat));

	/* Poles */
	verts[1] = r;
	verts[(n_verts-1)*3 + 1] = -r;

	/* Latitudinal lines */
	vertex_index = 1;
	for (int lat = 0; lat < lat_res; lat++) {
		for (int lng = 0; lng < lng_res; lng++) {
			verts[vertex_index*3] = r * sinf((lat + 1) * lat_delta) * cosf(lng * lng_delta);
			verts[vertex_index*3 + 1] = r * cosf((lat + 1) * lat_delta);
			verts[vertex_index*3 + 2] = r * sinf((lat + 1) * lat_delta) * sinf(lng * lng_delta);
			fprintf(stderr, "verts[%3d]: (% f, % f, % f)\n", vertex_index,
			        verts[vertex_index*3], verts[vertex_index*3 + 1], verts[vertex_index*3 + 2]);
			vertex_index++;
		}
	}

	/* TODO: GENERATE INDICES */
	/* TODO: GENERATE NORMALS */
	/* TODO: GENERATE TEXCOORDS */

	glGenVertexArrays(1, &vao->vao);
	glBindVertexArray(vao->vao);

	/* Upload data to GPU. */
	glGenBuffers(1, &vao->vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vao->vertices);
	glBufferData(GL_ARRAY_BUFFER, n_verts * 3 * sizeof(GLfloat), verts, GL_STATIC_DRAW);

	/* Clean up. */
	free(verts);
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
