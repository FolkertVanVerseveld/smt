/* draw blue triangle using shaders */
#include <GL/glew.h>
#include <smt/smt.h>
#include <stdio.h>
#include <stdlib.h>

GLuint gl_major, gl_minor;
GLuint gl_err;

#define PROGRAM_INVALID ((GLuint)-1)
#define SHADER_INVALID ((GLuint)-1)

GLuint program = PROGRAM_INVALID;
GLuint vs = SHADER_INVALID;
GLuint fs = SHADER_INVALID;

#define ATTR_COORD2D "coord2d"

GLint attribute_coord2d;

/**
\brief Provide pretty formatted OpenGL statistics and driver specs

A gl context must be created and set current before calling this function.
*/
static inline void gfxstat(GLenum experimental)
{
	smtGlAttrup(GL_MAJOR_VERSION, &gl_major);
	smtGlAttrup(GL_MINOR_VERSION, &gl_minor);
	printf("GL version: %u.%u\n", gl_major, gl_minor);
	glewExperimental = experimental;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "glew: init returned %u\n", err);
		return;
	}
	if (!GLEW_VERSION_1_1) {
		fputs("glew: seriously outdated or obscure OpenGL driver\n", stderr);
		return;
	}
	printf(
		"OpenGL Info\n"
		"version: %s\n"
		"vendor : %s\n"
		"render : %s\n"
		"shading: %s\n",
		glGetString(GL_VERSION),
		glGetString(GL_VENDOR),
		glGetString(GL_RENDERER),
		glGetString(GL_SHADING_LANGUAGE_VERSION)
	);
	struct glver {
		GLuint major, minor;
		int present;
	} vertbl[] = {
		{1, 1, 0}, {1, 2, 0}, {1, 3, 0}, {1, 4, 0}, {1, 5, 0},
		{2, 0, 0}, {2, 1, 0},
		{3, 0, 0}, {3, 1, 0}, {3, 2, 0}, {3, 3, 0},
		{4, 0, 0}, {4, 1, 0}, {4, 2, 0}, {4, 3, 0}, {4, 4, 0},
	};
	unsigned i = 0;
	#define prst(major,minor)vertbl[i++].present=GLEW_VERSION_##major##_##minor
	prst(1,1); prst(1,2); prst(1,3); prst(1,4); prst(1,5);
	prst(2,0); prst(2,1);
	prst(3,0); prst(3,1); prst(3,2); prst(3,3);
	prst(4,0); prst(4,1); prst(4,2); prst(4,3); prst(4,4);
	fputs("Available GL versions:", stdout);
	for (i = 0; i < (sizeof vertbl) / sizeof(vertbl[0]); ++i) {
		if (vertbl[i].present)
			printf(" %u.%u", vertbl[i].major, vertbl[i].minor);
	}
	putchar('\n');
}

static inline void cleanup(void)
{
	if (program != PROGRAM_INVALID) {
		if (fs != SHADER_INVALID)
			glDetachShader(program, fs);
		if (vs != SHADER_INVALID)
			glDetachShader(program, vs);
		glDeleteProgram(program);
		program = PROGRAM_INVALID;
	}
	if (fs != SHADER_INVALID) {
		glDeleteShader(fs);
		fs = SHADER_INVALID;
	}
	if (vs != SHADER_INVALID) {
		glDeleteShader(vs);
		vs = SHADER_INVALID;
	}
	if ((gl_err = glGetError()) != GL_NO_ERROR)
		printf("gl error: %X\n", gl_err);
}

static inline int init(void)
{
	GLint ok = GL_FALSE;
	GLsizei n;
	const GLchar *version = "#version 100\n";
	const GLchar *vs_src[] = {
		version,
		"attribute vec2 " ATTR_COORD2D ";\n"
		"void main(void) {\n"
		"\tgl_Position = vec4(" ATTR_COORD2D ", 0.0, 1.0);\n"
		"}\n"
	};
	const GLchar *fs_src[] = {
		version,
		"void main(void) {\n"
		"\tgl_FragColor[0] = 0.0;\n"
		"\tgl_FragColor[1] = 0.0;\n"
		"\tgl_FragColor[2] = 1.0;\n"
		"}\n"
	};
	vs = glCreateShader(GL_VERTEX_SHADER);
	n = (sizeof vs_src) / sizeof(vs_src[0]);
	glShaderSource(vs, n, vs_src, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
	atexit(cleanup);
	if (!ok) {
		fputs("vertex shader: compilation failed\n", stderr);
		return 1;
	}
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	n = (sizeof fs_src) / sizeof(fs_src[0]);
	glShaderSource(fs, n, fs_src, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
	if (!ok) {
		fputs("fragment shader: compilation failed\n", stderr);
		return 1;
	}
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &ok);
	if (!ok) {
		fputs("ld: incompatible shaders\n", stderr);
		return 1;
	}
	attribute_coord2d = glGetAttribLocation(program, ATTR_COORD2D);
	if (attribute_coord2d == -1) {
		fprintf(stderr, "%s: no such attrib\n", ATTR_COORD2D);
		return 1;
	}
	return 0;
}

void display(void)
{
	const GLfloat triangle[] = {
		 0.0,  0.8,
		-0.8, -0.8,
		 0.8, -0.8
	};
	const GLsizei n = (sizeof triangle) / sizeof(triangle[0]);
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord2d);
	glVertexAttribPointer(
		attribute_coord2d,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		triangle
	);
	glDrawArrays(GL_TRIANGLES, 0, n);
	glDisableVertexAttribArray(attribute_coord2d);
}

int main(int argc, char **argv) {
	int ret = 1;
	ret = smtInit(&argc, argv);
	if (ret != 0) goto fail;
	unsigned win, gl;
	smtGlAttru(GL_CONTEXT_PROFILE_MASK, SMT_CTX_GL_NEW);
	smtGlAttru(GL_MAJOR_VERSION, 2);
	smtGlAttru(GL_MINOR_VERSION, 1);
	ret = smtCreatewin(&win, 512, 512, "hello glew", SMT_WIN_VISIBLE | SMT_WIN_BORDER);
	if (ret != 0) goto fail;
	ret = smtCreategl(&gl, win);
	if (ret != 0) goto fail;
	gfxstat(GL_TRUE);
	if (init()) return 1;
	while (1) {
		unsigned ev;
		while ((ev = smtPollev()) != SMT_EV_DONE) {
			switch (ev) {
			case SMT_EV_QUIT: goto end;
			}
		}
		display();
		if ((gl_err = glGetError()) != GL_NO_ERROR)
			printf("gl error: %X\n", gl_err);
		smtSwapgl(win);
	}
end:
	ret = smtFreegl(gl);
	if (ret != 0) goto fail;
	ret = smtFreewin(win);
fail:
	return ret;
}
