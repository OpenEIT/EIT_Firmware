#ifndef __ADI_TYPES_H__
#define __ADI_TYPES_H__

/* obtain integer types ... */
#include <stdint.h>

/* obtain boolean types ... */
#include <stdbool.h>

#if 0
...defer these to device.h
/*!
 * \typedef bool_t
 * boolean type, since use of C99 "bool" is not MISRA compliant
 */
typedef uint8_t  bool_t;

/* simple true/false defines for functional APIs */
#ifndef false
#define false 0u /*!< Create a boolean 'false' value */
#endif
#ifndef true
#define true 1u /*!< Create a boolean 'true' value */
#endif

#endif

/* define required types that are not provided by stdint.h or stdbool.h ... */
typedef char                char_t;
typedef float               float32_t;
#if !defined(__NO_FLOAT64)
typedef long double         float64_t;
#endif

#endif /* __ADI_TYPES_H__ */

