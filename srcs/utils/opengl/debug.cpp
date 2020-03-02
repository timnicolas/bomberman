#include "debug.hpp"
#include "Logging.hpp"

GLenum checkError_(const char *file, int line) {
    GLenum errorCode;
    GLenum lastError = GL_NO_ERROR;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
            case 1283: error = "STACK_OVERFLOW"; break;  // GL_STACK_OVERFLOW
            case 1284:  error = "STACK_UNDERFLOW"; break;  // GL_STACK_UNDERFLOW
            case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        logErr("OpenGL error: " << error << " > " << file << " (" << line << ")");
        lastError = errorCode;
    }
    return lastError;
}

void checkErrorExit_(const char *file, int line) {
    GLenum ret = checkError_(file, line);
    if (ret != GL_NO_ERROR)
        exit(1);
}
