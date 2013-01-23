#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
/* TODO: Try not to depend on glew. */
#define GLEW_NO_GLU
#include <GL/glew.h>

/* Returns a non-zero shader ID for a compiled shader. Returns 0 on
 * error.
 */
GLuint load_shader(const char *path, GLenum shader_type)
{
	GLuint shader;
	int fd;
	long filesize;
	GLchar *srcbuf;
	GLint compile_status;
	struct stat st;

	/* Read shader code from source. */
	if (stat(path, &st) == -1) {
		perror("could not stat shader file");
		return 0;
	}

	filesize = st.st_size;

	fd = open(path, O_RDONLY);
	if (fd == -1) {
		perror("could not access shader source");
		return 0;
	}
	srcbuf = malloc(filesize + 1);
	if (read(fd, srcbuf, filesize) == -1) {
		perror("could not read shader file");
		free(srcbuf);
		return 0;
	}
	close(fd);
	srcbuf[filesize] = '\0';

	/* Create shader. */
	shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, (const GLchar **)&srcbuf, NULL);

	/* Compile shader. */
	glCompileShader(shader);

	/* Check compile status. */
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status == GL_FALSE) {
		GLint logsize;
		char *log;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);
		log = malloc(logsize);
		glGetShaderInfoLog(shader, logsize, NULL, log);
		fprintf(stderr, "%s: %s\n", path, log);

		free(log);
		glDeleteShader(shader);
		return 0;
	}

	glReleaseShaderCompiler();
	return shader;
}

GLuint link_program(const GLuint *shaders, size_t count)
{
	GLuint program;
	GLint link_status;

	/* Create a program */
	program = glCreateProgram();
	/* Attach all shaders to the program. */
	for (int i = 0; i < count; i++) {
		glAttachShader(program, shaders[i]);
	}

	/* Link the program. */
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &link_status);
	if (link_status == GL_FALSE) {
		GLint logsize;
		char *log;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logsize);
		log = malloc(logsize);
		glGetShaderInfoLog(program, logsize, NULL, log);
		fprintf(stderr, "program linker: %s\n", log);
		free(log);
		glDeleteProgram(program);
		return 0;
	}

	for (int i = 0; i < count; i++) {
		glDeleteShader(shaders[i]);
	}
	return program;
}
