#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED

#ifndef NDEBUG
#include <cstdio>
#define _NAMED_ARG(value) #value
#define DETAILED_ASSERT 1

#define Assert(expr) if(!(expr)) {                \
    if(DETAILED_ASSERT) {                         \
    fprintf(stderr, "Assertion failed: %s\n\n"    \
                    "Line: %d, File: %s\n\n",     \
            _NAMED_ARG(expr), __LINE__, __FILE__);\
    }                                             \
    *((int*)0) = 0;                               \
}
#else
#define Assert(expr)
#endif


#endif
