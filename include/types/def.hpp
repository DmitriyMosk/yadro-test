#pragma once 

#include <cstdint> 

/**
 * Same aliases
 */
using byte  = unsigned char;

/**
 * complex templates
 */
#define COMPLEX_TEMPLATES(classname)                    \
    template class classname<float>;                    \
    template class classname<double>;                   \
    template class classname<int8_t>;                   \
    template class classname<int16_t>;                  \
    template class classname<int32_t>;                  \
    template class classname<int64_t>;

/**
 * channel templates
 */
#define CHANNEL_TEMPLATES(classname)                    \
    template class classname<float>;                    \
    template class classname<double>;

/**
 * phys qam templates
 */
#define QAM_TEMPLATES(classname, order)                 \
    template class classname<float, order>;             \
    template class classname<double, order>;


#define QAM_MODEM_TEMPLATES(classname)                  \
    template class classname<float>;                    \
    template class classname<double>;