#include "billboard.h"
#include "fslegl.h"
#include <vector>
#include <iostream>
#include <stdexcept>

Billboard::Billboard(GLsizei count)
 : m_ItemCount(count)
 , m_Width(0)
 , m_Height(1)
{
	InitEGL();

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glDepthFunc(GL_LESS);
	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	InitIndices();
	InitShaders();
}

Billboard::~Billboard()
{
	glDeleteBuffers(1, &m_Indices);
	glDeleteProgram(m_Program);
	
	CleanUpEGL();
}

void Billboard::InitEGL()
{
	m_NativeDisplay = fsl_getNativeDisplay();
	m_Display = eglGetDisplay(m_NativeDisplay);
	if (nullptr == m_Display)
	{
		throw std::runtime_error("Can't get display.");
	}
	eglInitialize(m_Display, nullptr, nullptr);
	eglBindAPI(EGL_OPENGL_ES_API);

	static const EGLint config[] =
	{
		EGL_RED_SIZE,	5,
		EGL_GREEN_SIZE, 6,
		EGL_BLUE_SIZE,	5,
		EGL_ALPHA_SIZE, 0,
		EGL_SAMPLES,	0,
		EGL_NONE,       EGL_NONE,
	};

	EGLint numconfigs;
	EGLConfig eglconfig;
	eglChooseConfig(m_Display, config, &eglconfig, 1, &numconfigs);
	if(0 == numconfigs)
	{
		throw std::runtime_error("Can't choose config.");
	}

	m_NativeWindow = fsl_createwindow(m_Display, m_NativeDisplay);	
	if ((EGLNativeWindowType)0 == m_NativeWindow)
	{
		throw std::runtime_error("Can't create native window.");
	}

	m_Surface = eglCreateWindowSurface(m_Display, eglconfig, m_NativeWindow, nullptr);
	if (nullptr == m_Surface)
	{
		throw std::runtime_error("Can't create surface.");
	}

	EGLint ContextAttribList[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
	m_Context = eglCreateContext( m_Display, eglconfig, EGL_NO_CONTEXT, ContextAttribList );
	eglMakeCurrent(m_Display, m_Surface, m_Surface, m_Context);

	std::cout << "Vendor:" << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer:" << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version:" << glGetString(GL_VERSION) << std::endl;
	std::cout << "Extensions:" << glGetString(GL_EXTENSIONS) << std::endl;

	EGLint width = 0;
	EGLint height = 0;
	eglQuerySurface(m_Display, m_Surface, EGL_WIDTH, &width);
	eglQuerySurface(m_Display, m_Surface, EGL_HEIGHT, &height);
	
	m_Width = width;
	m_Height = height;
}

void Billboard::CleanUpEGL()
{
	eglMakeCurrent(m_Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(m_Display, m_Context);
	eglDestroySurface(m_Display, m_Surface);	
	fsl_destroywindow(m_NativeWindow, m_NativeDisplay);
	eglTerminate(m_Display);
	eglReleaseThread();	
}

GLuint Billboard::CreateVAO(GLuint pt) const
{
	GLuint ret;
	glGenVertexArrays(1, &ret);
	if (0 == ret)
	{
		return ret;
	}
	glBindVertexArray(ret);
	
	glEnableVertexAttribArray(m_PTLocation);
	glBindBuffer(GL_ARRAY_BUFFER, pt);
	glVertexAttribPointer(m_PTLocation, 2, GL_UNSIGNED_SHORT, GL_TRUE, 0, (void*)0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Indices);

	glBindVertexArray(0);
	
	return ret;
}

GLuint Billboard::CreateVertices(GLsizei count) const
{
	GLuint ret;
	glGenBuffers(1, &ret);
	if (0 == ret)
	{
		return ret;
	}

	glBindBuffer(GL_ARRAY_BUFFER, ret);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * count, nullptr, GL_DYNAMIC_DRAW);
	
	return ret;
}

void Billboard::InitIndices()
{
	std::vector<uint16_t> indices(4 * m_ItemCount);
	for (GLsizei i = 0; i < 4 * m_ItemCount; ++i)
	{
		indices[i] = i;
	}

	glGenBuffers(1, &m_Indices);
	if (0 == m_Indices)
	{
		throw std::runtime_error("Can't initialize indices.");
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

void Billboard::InitShaders()
{
#define SHADER_SOURCE(x) #x
	const char* vss = 
	"#version 300 es\n"
	SHADER_SOURCE(
		\n
		in vec4 pt; \n
		\n
		out vec2 tc; \n
		\n
		void main()\n
		{ \n
			gl_Position = vec4(pt.xy, 0.0, 1.0); \n
			tc = pt.zw; \n
		}\n
	);

	const char* pss = 
	"#version 300 es\n"
	SHADER_SOURCE(
		precision mediump float; 
		uniform sampler2D tex; \n
		uniform float alpha; \n
		\n
		in vec2 tc; \n
		\n
		out vec4 fragColor; \n
		\n
		void main()\n
		{ \n
			fragColor = vec4(texture2D(tex, tc).rgb, alpha); \n
			if (fragColor.a < 0.01) \n
			{ \n
				discard; \n
			} \n
		}\n
	);

	m_Program = CreateProgram(vss, pss);
	glUseProgram(m_Program);

	m_PTLocation = glGetAttribLocation(m_Program, "pt");

	GLuint tex = glGetUniformLocation(m_Program, "tex");
	glUniform1i(tex, 0);

	m_AlphaLocation = glGetUniformLocation(m_Program, "alpha");
	glUniform1f(m_AlphaLocation, 1.0f);

	glUseProgram(m_Program);
}

GLuint Billboard::CreateProgram(const char* vss, const char* pss)
{
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	if (0 == vs)
	{
		throw std::runtime_error("Can't create vertex shader.");
	}
	glShaderSource(vs, 1, &vss, nullptr);
	glCompileShader(vs);

	GLint isCompiled = 0;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &isCompiled);

	if (0 == isCompiled)
	{
		GLint logLen = 0;

		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 1)
		{
			std::string log;
			log.resize(logLen);
			glGetShaderInfoLog(vs, logLen, &logLen, (GLchar*)(log.c_str()));
			std::cerr << "Error while compile vertex shader;" << log.c_str() << std::endl;
		}
		throw std::runtime_error("Can't compile vertex shader.");
	}

	GLuint ps = glCreateShader(GL_FRAGMENT_SHADER);
	if (0 == ps)
	{
		throw std::runtime_error("Can't create pixel shader.");
	}
	glShaderSource(ps, 1, &pss, nullptr);
	glCompileShader(ps);

	glGetShaderiv(ps, GL_COMPILE_STATUS, &isCompiled);

	if (0 == isCompiled)
	{
		GLint logLen = 0;

		glGetShaderiv(ps, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 1)
		{
			std::string log;
			log.resize(logLen);
			glGetShaderInfoLog(ps, logLen, &logLen, (GLchar*)(log.c_str()));
			std::cerr << "Error while compile pixel shader;" << log.c_str() << std::endl;
		}
		throw std::runtime_error("Can't compile pixel shader.");
	}

	GLuint program = glCreateProgram();
	if (0 == program)
	{
		glDeleteShader(ps);
		glDeleteShader(vs);
		throw std::runtime_error("Can't create program.");
	}
	glAttachShader(program, vs);
	glAttachShader(program, ps);
	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

	if (0 == isLinked)
	{
		GLint logLen = 0;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 1)
		{
			std::string log;
			log.resize(logLen);
			glGetProgramInfoLog(program, logLen, &logLen, (GLchar*)(log.c_str()));
			std::cerr << "Error while link program:" << log.c_str() << std::endl;
		}
		throw std::runtime_error("Error while link program.");
	}

	glDeleteShader(ps);
	glDeleteShader(vs);

	return program;
}

ItemInfo Billboard::CreateItems(GLsizei count) const
{
	ItemInfo ret = { count, 0, 0 };
	ret.PT = CreateVertices(count);
	if(0 != ret.PT)
	{
		ret.VAO = CreateVAO(ret.PT);
	}
	return ret;
}

void Billboard::SetItemsAttribute(const ItemInfo& items, const float* pt, size_t count) const
{
	glBindBuffer(GL_ARRAY_BUFFER, items.PT);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * count * 4, pt);
}

void Billboard::Begin() const
{
	glViewport(0, 0, m_Width, m_Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_Program);
}

void Billboard::SetAlpha(float alpha)
{
	glUniform1f(m_AlphaLocation, alpha);
}

void Billboard::Draw(const ItemInfo& items, size_t count)
{
	glBindVertexArray(items.VAO);
	glDrawElements(GL_TRIANGLE_STRIP, 4 * count, GL_UNSIGNED_SHORT, nullptr);
}

void Billboard::End() const
{
	eglSwapBuffers(m_Display, m_Surface);
}
