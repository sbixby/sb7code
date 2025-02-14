/*
 * Copyright � 2012-2015 Graham Sellers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <sb7.h>
#include <vmath.h>
#include <string>

// Remove this to draw only a single cube!
// #define MANY_CUBES

class singlepoint_app : public sb7::application
{
	void init()
	{
		static const char title[] = "OpenGL SuperBible - Spinny Cube";

		sb7::application::init();

		memcpy(info.title, title, sizeof(title));
	}

	virtual void startup()
	{
		static const char* vs_source[] =
		{
			"#version 450 core                                                  \n"
			"                                                                   \n"
			"in vec4 position;                                                  \n"
			"                                                                   \n"
			"out VS_OUT                                                         \n"
			"{                                                                  \n"
			"    vec4 color;                                                    \n"
			"} vs_out;                                                          \n"
			"                                                                   \n"
			"uniform mat4 mv_matrix;                                            \n"
			"uniform mat4 proj_matrix;                                          \n"
			"                                                                   \n"
			"void main(void)                                                    \n"
			"{                                                                  \n"
			"    gl_Position = proj_matrix * mv_matrix * position;              \n"
			"    vs_out.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);      \n"
			"}                                                                  \n"
		};

		static const char* fs_source[] =
		{
			"#version 450 core                                                  \n"
			"                                                                   \n"
			"out vec4 color;                                                    \n"
			"                                                                   \n"
			"in VS_OUT                                                          \n"
			"{                                                                  \n"
			"    vec4 color;                                                    \n"
			"} fs_in;                                                           \n"
			"                                                                   \n"
			"void main(void)                                                    \n"
			"{                                                                  \n"
			"    color = fs_in.color;                                           \n"
			"}                                                                  \n"
		};

		program = glCreateProgram();
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, fs_source, NULL);
		glCompileShader(fs);

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, vs_source, NULL);
		glCompileShader(vs);

		glAttachShader(program, vs);
		glAttachShader(program, fs);

		glLinkProgram(program);

		mv_location = glGetUniformLocation(program, "mv_matrix");
		proj_location = glGetUniformLocation(program, "proj_matrix");

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		static const GLfloat vertex_positions[] =
		{
			-0.25f, 0.25f, -0.25f,
			-0.25f, -0.25f, -0.25f,
			0.25f, -0.25f, -0.25f,

			0.25f, -0.25f, -0.25f,
			0.25f, 0.25f, -0.25f,
			-0.25f, 0.25f, -0.25f,

			0.25f, -0.25f, -0.25f,
			0.25f, -0.25f, 0.25f,
			0.25f, 0.25f, -0.25f,

			0.25f, -0.25f, 0.25f,
			0.25f, 0.25f, 0.25f,
			0.25f, 0.25f, -0.25f,

			0.25f, -0.25f, 0.25f,
			-0.25f, -0.25f, 0.25f,
			0.25f, 0.25f, 0.25f,

			-0.25f, -0.25f, 0.25f,
			-0.25f, 0.25f, 0.25f,
			0.25f, 0.25f, 0.25f,

			-0.25f, -0.25f, 0.25f,
			-0.25f, -0.25f, -0.25f,
			-0.25f, 0.25f, 0.25f,

			-0.25f, -0.25f, -0.25f,
			-0.25f, 0.25f, -0.25f,
			-0.25f, 0.25f, 0.25f,

			-0.25f, -0.25f, 0.25f,
			0.25f, -0.25f, 0.25f,
			0.25f, -0.25f, -0.25f,

			0.25f, -0.25f, -0.25f,
			-0.25f, -0.25f, -0.25f,
			-0.25f, -0.25f, 0.25f,

			-0.25f, 0.25f, -0.25f,
			0.25f, 0.25f, -0.25f,
			0.25f, 0.25f, 0.25f,

			0.25f, 0.25f, 0.25f,
			-0.25f, 0.25f, 0.25f,
			-0.25f, 0.25f, -0.25f
		};

		const int arr_len = sizeof(vertex_positions) / sizeof(GLfloat);
		const int byte_len = sizeof(vertex_positions);

		GLfloat vp2[108];
		//auto vp2 = new GLfloat[arr_len];

		for (int i = 0; i < arr_len; i += 3)
		{
			vp2[i] = vertex_positions[i]+0.5;
			vp2[i + 1] = vertex_positions[i + 1]+0.5;
			vp2[i + 2] = vertex_positions[i + 2]+0.25;
		}

		tri_count = arr_len / 3 * 2;

		glCreateBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferStorage(GL_ARRAY_BUFFER, byte_len * 2, nullptr, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_DYNAMIC_STORAGE_BIT);

		// Dumps buffer before it was initialized
		// GLfloat* rm = static_cast<GLfloat*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY));
		// for(auto i =0;i<arr_len*2;i+=3)
		// {
		// 	OutputDebugString(std::to_string(rm[i]).c_str());
		// 	OutputDebugString(", ");
		// 	OutputDebugString(std::to_string(rm[i+1]).c_str());
		// 	OutputDebugString(", ");
		// 	OutputDebugString(std::to_string(rm[i+2]).c_str());
		// 	OutputDebugString("\n");
		// }
		// glUnmapBuffer(GL_ARRAY_BUFFER);

		// Named versions
		// glNamedBufferSubData(buffer, 0, sizeof(vertex_positions), vertex_positions);
		// glNamedBufferSubData(buffer, sizeof(vertex_positions), sizeof(vp2), vp2);

		// buffer versions
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_positions), vertex_positions);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions), sizeof(vp2), vp2);

		// memcpy version
		// void* p = glMapNamedBuffer(buffer, GL_WRITE_ONLY);
		// memcpy(p, vertex_positions, byte_len);
		// memcpy(static_cast<char*>(p) + byte_len, vp2, byte_len);
		// glUnmapNamedBuffer(buffer);
		// end memcpy version


		// glBufferData(GL_ARRAY_BUFFER,
		//              sizeof(vertex_positions),
		//              vertex_positions,
		//              GL_STATIC_DRAW);
		//              
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		aspect = static_cast<float>(info.windowWidth) / static_cast<float>(info.windowHeight);
		proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);
	}

	virtual void render(double currentTime)
	{
		static const GLfloat green[] = {0.0f, 0.25f, 0.0f, 1.0f};
		static const GLfloat one = 1.0f;

		glViewport(0, 0, info.windowWidth, info.windowHeight);
		glClearBufferfv(GL_COLOR, 0, green);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glUseProgram(program);

		glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);

#ifdef MANY_CUBES
        int i;
        for (i = 0; i < 24; i++)
        {
            float f = (float)i + (float)currentTime * 0.3f;
            vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -6.0f) *
                                    vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
                                    vmath::rotate((float)currentTime * 21.0f, 1.0f, 0.0f, 0.0f) *
                                    vmath::translate(sinf(2.1f * f) * 2.0f,
                                                     cosf(1.7f * f) * 2.0f,
                                                     sinf(1.3f * f) * cosf(1.5f * f) * 2.0f);
            glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
#else
		float f = (float)currentTime * 0.3f;
		vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -4.0f) *
			vmath::translate(sinf(2.1f * f) * 0.5f,
			                 cosf(1.7f * f) * 0.5f,
			                 sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
			vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);
		glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
		glDrawArrays(GL_TRIANGLES, 0, tri_count);
#endif
	}

	virtual void shutdown()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteProgram(program);
		glDeleteBuffers(1, &buffer);
	}

	void onResize(int w, int h)
	{
		sb7::application::onResize(w, h);

		aspect = (float)w / (float)h;
		proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);
	}

private:
	GLuint program;
	GLuint vao;
	GLuint buffer;
	GLint mv_location;
	GLint proj_location;
	GLint tri_count;

	float aspect;
	vmath::mat4 proj_matrix;
};

DECLARE_MAIN(singlepoint_app)
