#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

/******************************************************************************
 * AUTOSAR Version Information
 ******************************************************************************/

#define PLATFORM_TYPES_AR_RELEASE_MAJOR_VERSION      4U
#define PLATFORM_TYPES_AR_RELEASE_MINOR_VERSION      4U
#define PLATFORM_TYPES_AR_RELEASE_REVISION_VERSION   0U

/******************************************************************************
 * CPU Type
 ******************************************************************************/

#define CPU_TYPE_8      8U
#define CPU_TYPE_16     16U
#define CPU_TYPE_32     32U
#define CPU_TYPE_64     64U

#define CPU_TYPE        CPU_TYPE_32

/******************************************************************************
 * Bit Order
 ******************************************************************************/

#define MSB_FIRST       0U
#define LSB_FIRST       1U

#define CPU_BIT_ORDER   LSB_FIRST

/******************************************************************************
 * Byte Order
 ******************************************************************************/

#define HIGH_BYTE_FIRST 0U
#define LOW_BYTE_FIRST  1U

#define CPU_BYTE_ORDER  LOW_BYTE_FIRST

/******************************************************************************
 * Boolean
 ******************************************************************************/

typedef unsigned char boolean;

#ifndef TRUE
#define TRUE    ((boolean)1U)
#endif

#ifndef FALSE
#define FALSE   ((boolean)0U)
#endif

/******************************************************************************
 * Integer Types
 ******************************************************************************/

typedef signed char             sint8;
typedef unsigned char           uint8;

typedef signed short            sint16;
typedef unsigned short          uint16;

typedef signed int              sint32;
typedef unsigned int            uint32;

typedef signed long long        sint64;
typedef unsigned long long      uint64;

/******************************************************************************
 * Least Types
 ******************************************************************************/

typedef signed char             sint8_least;
typedef unsigned char           uint8_least;

typedef signed short            sint16_least;
typedef unsigned short          uint16_least;

typedef signed int              sint32_least;
typedef unsigned int            uint32_least;

/******************************************************************************
 * Floating Point Types
 ******************************************************************************/

typedef float                   float32;
typedef double                  float64;

#endif /* PLATFORM_TYPES_H */