#include "stdafx.h"
#include "OGLUtilities.h"

const char * OGLUtilities::GLTypeEnumToCString(GLenum eType)
{
	switch (eType)
	{
	case GL_FLOAT: return "GL_FLOAT";
	case GL_FLOAT_VEC2: return "GL_FLOAT_VEC2";
	case GL_FLOAT_VEC3: return "GL_FLOAT_VEC3";
	case GL_FLOAT_VEC4: return "GL_FLOAT_VEC4";
	case GL_FLOAT_MAT2: return "GL_FLOAT_MAT2";
	case GL_FLOAT_MAT3: return "GL_FLOAT_MAT3";
	case GL_FLOAT_MAT4: return "GL_FLOAT_MAT4";
	case GL_FLOAT_MAT2x3: return "GL_FLOAT_MAT2x3";
	case GL_FLOAT_MAT2x4: return "GL_FLOAT_MAT2x4";
	case GL_FLOAT_MAT3x2: return "GL_FLOAT_MAT3x2";
	case GL_FLOAT_MAT3x4: return "GL_FLOAT_MAT3x4";
	case GL_FLOAT_MAT4x2: return "GL_FLOAT_MAT4x2";
	case GL_FLOAT_MAT4x3: return "GL_FLOAT_MAT4x3";
	case GL_INT: return "GL_INT";
	case GL_INT_VEC2: return "GL_INT_VEC2";
	case GL_INT_VEC3: return "GL_INT_VEC3";
	case GL_INT_VEC4: return "GL_INT_VEC4";
	case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
	case GL_UNSIGNED_INT_VEC2: return "GL_UNSIGNED_INT_VEC2";
	case GL_UNSIGNED_INT_VEC3: return "GL_UNSIGNED_INT_VEC3";
	case GL_UNSIGNED_INT_VEC4: return "GL_UNSIGNED_INT_VEC4";
	case GL_DOUBLE: return "GL_DOUBLE";
	case GL_DOUBLE_VEC2: return "GL_DOUBLE_VEC2";
	case GL_DOUBLE_VEC3: return "GL_DOUBLE_VEC3";
	case GL_DOUBLE_VEC4: return "GL_DOUBLE_VEC4";
	case GL_DOUBLE_MAT2: return "GL_DOUBLE_MAT2";
	case GL_DOUBLE_MAT3: return "GL_DOUBLE_MAT3";
	case GL_DOUBLE_MAT4: return "GL_DOUBLE_MAT4";
	case GL_DOUBLE_MAT2x3: return "GL_DOUBLE_MAT2x3";
	case GL_DOUBLE_MAT2x4: return "GL_DOUBLE_MAT2x4";
	case GL_DOUBLE_MAT3x2: return "GL_DOUBLE_MAT3x2";
	case GL_DOUBLE_MAT3x4: return "GL_DOUBLE_MAT3x4";
	case GL_DOUBLE_MAT4x2: return "GL_DOUBLE_MAT4x2";
	case GL_DOUBLE_MAT4x3: return "GL_DOUBLE_MAT4x3";
	default:
		return "Unknown type";
	}
}

const char * OGLUtilities::GLDebugEnumToCString(GLenum eType)
{
	switch (eType)
	{

	case GL_DEBUG_SOURCE_API: return "GL_DEBUG_SOURCE_API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
	case GL_DEBUG_SOURCE_SHADER_COMPILER: return "GL_DEBUG_SOURCE_SHADER_COMPILER";
	case GL_DEBUG_SOURCE_THIRD_PARTY: return "GL_DEBUG_SOURCE_THIRD_PARTY";
	case GL_DEBUG_SOURCE_APPLICATION: return "GL_DEBUG_SOURCE_APPLICATION";
	case GL_DEBUG_SOURCE_OTHER: return "GL_DEBUG_SOURCE_OTHER";

	case GL_DEBUG_TYPE_ERROR: return "GL_DEBUG_TYPE_ERROR";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
	case GL_DEBUG_TYPE_PORTABILITY: return "GL_DEBUG_TYPE_PORTABILITY";
	case GL_DEBUG_TYPE_PERFORMANCE: return "GL_DEBUG_TYPE_PERFORMANCE";
	case GL_DEBUG_TYPE_MARKER: return "GL_DEBUG_TYPE_MARKER";
	case GL_DEBUG_TYPE_PUSH_GROUP: return "GL_DEBUG_TYPE_PUSH_GROUP";
	case GL_DEBUG_TYPE_POP_GROUP: return "GL_DEBUG_TYPE_POP_GROUP";
	case GL_DEBUG_TYPE_OTHER: return "GL_DEBUG_TYPE_OTHER";

	case GL_DEBUG_SEVERITY_HIGH: return "GL_DEBUG_SEVERITY_HIGH";
	case GL_DEBUG_SEVERITY_MEDIUM: return "GL_DEBUG_SEVERITY_MEDIUM";
	case GL_DEBUG_SEVERITY_LOW: return "GL_DEBUG_SEVERITY_LOW";
	case GL_DEBUG_SEVERITY_NOTIFICATION: return "GL_DEBUG_SEVERITY_NOTIFICATION";

	default:
		return "Unknown debug enum entry";
	}
}

GLenum OGLUtilities::GetErrors()
{
	GLenum error = glGetError();
	do
	{		
		switch (error)
		{
		case GL_NO_ERROR:
			std::cout << "GL_NO_ERROR" << std::endl;
			break;
		case GL_INVALID_ENUM:
			std::cout << "GL_INVALID_ENUM" << std::endl;
			break;
		case GL_INVALID_VALUE:
			std::cout << "GL_INVALID_VALUE" << std::endl;
			break;
		case GL_INVALID_OPERATION:
			std::cout << "GL_INVALID_OPERATION" << std::endl;
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
			break;
		case GL_OUT_OF_MEMORY:
			std::cout << "GL_OUT_OF_MEMORY" << std::endl;
			break;
		case GL_STACK_UNDERFLOW:
			std::cout << "GL_STACK_UNDERFLOW" << std::endl;
			break;
		case GL_STACK_OVERFLOW:
			std::cout << "GL_STACK_OVERFLOW" << std::endl;
			break;
		default:
			std::cout << "Unknown error" << std::endl;
			break;
		}
	} while (error = glGetError() != GL_NO_ERROR);

	return error;
}

void APIENTRY OGLUtilities::DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const GLvoid * userParam)
{
	//printf("Debug message with source=0x%04X type=0x%04X, id %u, severity 0x%0X, '%s'\n" , source, type, id, severity, message);
	std::cout << "Debug message:\n"
			<< "\tSource: " << OGLUtilities::GLDebugEnumToCString(source) << "\n"
			<< "\tType: " << OGLUtilities::GLDebugEnumToCString(type) << "\n"
			<< "\tSeverity: " << OGLUtilities::GLDebugEnumToCString(severity) << "\n"
			<< "\tMessage: " << message << std::endl;
}
