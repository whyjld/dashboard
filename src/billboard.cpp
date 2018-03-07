#include "billboard.h"
#include <vector>
#include <iostream>
#include <stdexcept>

Billboard::Billboard(GLsizei count)
 : m_ItemCount(count)
 , m_Alpha(1.0f)
{
	InitGL();

	InitIndices();
	InitShaders();
}

Billboard::~Billboard()
{
	glDeleteBuffers(1, &m_Indices);
	glDeleteProgram(m_Program);
	
	CleanUpGL();
}

void Billboard::InitGL()
{
	std::cout << "Vendor:" << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer:" << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version:" << glGetString(GL_VERSION) << std::endl;
	const char* exts = (const char*)glGetString(GL_EXTENSIONS);
	if(nullptr != exts)
	{
		std::cout << "Extensions:" << exts << std::endl;
	}
	std::cout << "Width:" << m_Context.GetWidth() << ", Height:" << m_Context.GetHeight() << std::endl;
	
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glDepthFunc(GL_LESS);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Billboard::CleanUpGL()
{
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
	glVertexAttribPointer(m_PTLocation, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
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
		uniform vec4 trans;\n
		uniform vec4 vp;\n
		void main()\n
		{ \n
			gl_Position = vec4((pt.xy * trans.z + trans.xy) * vp.zw + vp.xy, 0.0, 1.0); \n
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
			fragColor = texture(tex, tc);\n
			fragColor.a *= alpha; \n
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
	glUniform1f(m_AlphaLocation, m_Alpha);
	
	m_TransLocation = glGetUniformLocation(m_Program, "trans");
	glUniform4f(m_TransLocation, 0.0f, 0.0f, 1.0f, 0.0f);

	m_VPLocation = glGetUniformLocation(m_Program, "vp");

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

void Billboard::SetItemsAttribute(const ItemInfo& items, const float* pt, GLsizei count) const
{
	glBindBuffer(GL_ARRAY_BUFFER, items.PT);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * count * 4, pt);
}

void Billboard::SetLogicSize(GLsizei width, GLsizei height)
{
	float r = float(width) / float(height);

	GLint vx, vy;
	GLsizei vw, vh;
	if(float(m_Context.GetWidth()) / float(m_Context.GetHeight()) > r)
	{
		vy = 0;
		vh = m_Context.GetHeight();
		vw = GLsizei(m_Context.GetHeight() * r);
		vx = GLint(m_Context.GetWidth() - vw) / 2;
	}
	else
	{
		vx = 0;
		vw = m_Context.GetWidth();
		vh = GLsizei(m_Context.GetWidth() / r);
		vy = GLint(m_Context.GetHeight() - vh) / 2;
	}
	glViewport(vx, vy, vw, vh);
	glUseProgram(m_Program);
	glUniform4f(m_VPLocation, -1.0f, -1.0f, 2.0f / width, 2.0f / height);
}

void Billboard::Begin() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_Program);
}

void Billboard::SetAlpha(float alpha)
{
    if(m_Alpha != alpha)
    {
        m_Alpha = alpha;
        if(m_Alpha > 1.0f)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    	    glUniform1f(m_AlphaLocation, m_Alpha - 1.0f);
        }
        else
        {
        	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    	    glUniform1f(m_AlphaLocation, m_Alpha);
        }
	}
}

void Billboard::SetTranslation(float x, float y, float s)
{
	glUniform4f(m_TransLocation, x, y, s, 0.0f);
}

void Billboard::Draw(const ItemInfo& items, GLsizei count)
{
	glBindVertexArray(items.VAO);
	glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_SHORT, nullptr);
}

void Billboard::End() const
{
	m_Context.SwapBuffers();
}
