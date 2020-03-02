#ifndef DEBUG_HPP_
#define DEBUG_HPP_

#include "includesOpengl.hpp"

GLenum  checkError_(const char *file, int line);
void    checkErrorExit_(const char *file, int line);

#define checkError() checkError_(__FILE__, __LINE__)
#define checkErrorExit() checkErrorExit_(__FILE__, __LINE__)

#endif  // DEBUG_HPP_
