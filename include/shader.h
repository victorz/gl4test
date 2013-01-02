#ifndef SHADER_H
#define SHADER_H

GLuint load_shader(const char *path, GLenum shader_type);
GLuint link_program(const GLuint *shaders, size_t count);

#endif
