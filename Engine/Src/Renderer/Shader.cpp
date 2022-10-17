#include "Shader.h"
#include "Src/Core.h"
#include "Src/FileReader.h"
#include "glad/glad.h"

namespace BEngine
{
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

		return 0;
	}

	Shader::Shader()
	{
	}

	Shader::Shader(const std::string& filename)
	{
		SetShader(filename);
	}

	void Shader::SetShader(const std::string& filename)
	{
		std::string src = FileReader::OpenFile(filename);
		m_Sources = PreProcess(src);
		Compile(m_Sources);
	}

	void Shader::Bind()
	{
		glUseProgram(m_RendererID);
	}

	std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> sources;

		// Set shader type prefix and get the length in elements.
		const char* shaderTypePrefix = "#type";
		uint32_t shaderTypeLength = strlen(shaderTypePrefix);

		// Find begin position of source files.
		uint32_t pos = source.find(shaderTypePrefix, 0);

		while (pos != std::string::npos)
		{
			// Find source begin
			uint32_t begin = pos + shaderTypeLength + 1;

			// find first newline starting search from pos.
			uint32_t endOfLine = source.find_first_of("\r\n", pos);

			// Get the opengl enum shader type as a string.
			std::string type = source.substr(begin, endOfLine - begin);

			// Get position of the next newline.
			uint32_t nextLinePos = source.find_first_of("\r\n", endOfLine);

			// Set pos to the next shaderTypePrefix.
			pos = source.find(shaderTypePrefix, nextLinePos);

			// Add shader type and source code to map.
			sources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return sources;
	}

	void Shader::Compile(const std::unordered_map<GLenum, std::string>& sources)
	{
		// Create shader program.
		GLuint program = glCreateProgram();

		std::vector<GLenum> shaderIds;

		for(std::pair<GLenum, std::string> source : sources)
		{
			// Create a gl shader object.
			GLuint shader;
			shader = glCreateShader(source.first);

			// Attach the shader source code to the shader object.
			const char* src = source.second.c_str();
			glShaderSource(shader, 1, &src, NULL);

			// Compile the shader.
			glCompileShader(shader);

			// Check if compilation was successful. if not assert and give the error.
			GLint success;
			char infoLog[512];
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 512, NULL, infoLog);
				BE_ASSERT(false, std::string(infoLog))
				glDeleteShader(shader);
			}

			// Attach shader to program.
			glAttachShader(program, shader);

			// Add compiled shader to temporary shader id collection.
			shaderIds.push_back(shader);
		}

		// Link all the compiled shader programs.
		glLinkProgram(program);


		GLint success;
		char infoLog[512];

		// Check if linking was successful.
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			BE_ASSERT(false, std::string(infoLog))
			glDeleteProgram(program);
		}

		// Delete shaders
		for (GLuint shaderId : shaderIds)
		{
			glDeleteShader(shaderId);
		}

		// Set program to render ID.
		m_RendererID = program;
	}

}