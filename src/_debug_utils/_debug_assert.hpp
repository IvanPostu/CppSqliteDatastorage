#ifndef HEADER_DEBUG_ASSERT
#define HEADER_DEBUG_ASSERT

#include <iostream>



#ifdef NDEBUG
#define _debug_assert(condition, message) 0
#else
#define _debug_assert(condition, message)\
   (!(condition)) ?\
      (std::cerr << "Assertion failed: (" << #condition << "), "\
      << "function " << __FUNCTION__\
      << ", file " << __FILE__\
      << ", line " << __LINE__ << "."\
      << std::endl << message << std::endl, abort(), 0) : 1
#endif


#endif
