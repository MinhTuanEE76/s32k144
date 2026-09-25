#ifndef __MCUCFG_H__
#define __MCUCFG_H__

#include "Mcu_Types.h"

#define MCU_CLOCK_SETTING_ID0   (Mcu_ClockType)(0U)
#define MCU_CLOCK_SETTING_ID1   (Mcu_ClockType)(1U)


/* -------------------------------------------------------------------------- */
/* Bus Clock Divider                                                          */
/* -------------------------------------------------------------------------- */

#define MCU_CLOCK_BUS_DIV1          (0U)
#define MCU_CLOCK_BUS_DIV2          (1U)
#define MCU_CLOCK_BUS_DIV3          (2U)
#define MCU_CLOCK_BUS_DIV4          (3U)
#define MCU_CLOCK_BUS_DIV5          (4U)
#define MCU_CLOCK_BUS_DIV6          (5U)
#define MCU_CLOCK_BUS_DIV7          (6U)
#define MCU_CLOCK_BUS_DIV8          (7U)
#define MCU_CLOCK_BUS_DIV9          (8U)
#define MCU_CLOCK_BUS_DIV10         (9U)
#define MCU_CLOCK_BUS_DIV11         (10U)
#define MCU_CLOCK_BUS_DIV12         (11U)
#define MCU_CLOCK_BUS_DIV13         (12U)
#define MCU_CLOCK_BUS_DIV14         (13U)
#define MCU_CLOCK_BUS_DIV15         (14U)
#define MCU_CLOCK_BUS_DIV16         (15U)


/* -------------------------------------------------------------------------- */
/* Core Clock Divider                                                         */
/* -------------------------------------------------------------------------- */

#define MCU_CLOCK_CORE_DIV1         (0U)
#define MCU_CLOCK_CORE_DIV2         (1U)
#define MCU_CLOCK_CORE_DIV3         (2U)
#define MCU_CLOCK_CORE_DIV4         (3U)
#define MCU_CLOCK_CORE_DIV5         (4U)
#define MCU_CLOCK_CORE_DIV6         (5U)
#define MCU_CLOCK_CORE_DIV7         (6U)
#define MCU_CLOCK_CORE_DIV8         (7U)
#define MCU_CLOCK_CORE_DIV9         (8U)
#define MCU_CLOCK_CORE_DIV10        (9U)
#define MCU_CLOCK_CORE_DIV11        (10U)
#define MCU_CLOCK_CORE_DIV12        (11U)
#define MCU_CLOCK_CORE_DIV13        (12U)
#define MCU_CLOCK_CORE_DIV14        (13U)
#define MCU_CLOCK_CORE_DIV15        (14U)
#define MCU_CLOCK_CORE_DIV16        (15U)


/* -------------------------------------------------------------------------- */
/* Slow Clock Divider                                                         */
/* -------------------------------------------------------------------------- */

#define MCU_CLOCK_SLOW_DIV1         (0U)
#define MCU_CLOCK_SLOW_DIV2         (1U)
#define MCU_CLOCK_SLOW_DIV3         (2U)
#define MCU_CLOCK_SLOW_DIV4         (3U)
#define MCU_CLOCK_SLOW_DIV5         (4U)
#define MCU_CLOCK_SLOW_DIV6         (5U)
#define MCU_CLOCK_SLOW_DIV7         (6U)
#define MCU_CLOCK_SLOW_DIV8         (7U)
#define MCU_CLOCK_SLOW_DIV9         (8U)
#define MCU_CLOCK_SLOW_DIV10        (9U)
#define MCU_CLOCK_SLOW_DIV11        (10U)
#define MCU_CLOCK_SLOW_DIV12        (11U)
#define MCU_CLOCK_SLOW_DIV13        (12U)
#define MCU_CLOCK_SLOW_DIV14        (13U)
#define MCU_CLOCK_SLOW_DIV15        (14U)
#define MCU_CLOCK_SLOW_DIV16        (15U)


/* -------------------------------------------------------------------------- */
/* SPLL VCO Multiply Factor                                                   */
/* -------------------------------------------------------------------------- */

#define MCU_SPLL_MULT16             ((uint8)0U)
#define MCU_SPLL_MULT17             ((uint8)1U)
#define MCU_SPLL_MULT18             ((uint8)2U)
#define MCU_SPLL_MULT19             ((uint8)3U)
#define MCU_SPLL_MULT20             ((uint8)4U)
#define MCU_SPLL_MULT21             ((uint8)5U)
#define MCU_SPLL_MULT22             ((uint8)6U)
#define MCU_SPLL_MULT23             ((uint8)7U)
#define MCU_SPLL_MULT24             ((uint8)8U)
#define MCU_SPLL_MULT25             ((uint8)9U)
#define MCU_SPLL_MULT26             ((uint8)10U)
#define MCU_SPLL_MULT27             ((uint8)11U)
#define MCU_SPLL_MULT28             ((uint8)12U)
#define MCU_SPLL_MULT29             ((uint8)13U)
#define MCU_SPLL_MULT30             ((uint8)14U)
#define MCU_SPLL_MULT31             ((uint8)15U)
#define MCU_SPLL_MULT32             ((uint8)16U)
#define MCU_SPLL_MULT33             ((uint8)17U)
#define MCU_SPLL_MULT34             ((uint8)18U)
#define MCU_SPLL_MULT35             ((uint8)19U)
#define MCU_SPLL_MULT36             ((uint8)20U)
#define MCU_SPLL_MULT37             ((uint8)21U)
#define MCU_SPLL_MULT38             ((uint8)22U)
#define MCU_SPLL_MULT39             ((uint8)23U)
#define MCU_SPLL_MULT40             ((uint8)24U)
#define MCU_SPLL_MULT41             ((uint8)25U)
#define MCU_SPLL_MULT42             ((uint8)26U)
#define MCU_SPLL_MULT43             ((uint8)27U)
#define MCU_SPLL_MULT44             ((uint8)28U)
#define MCU_SPLL_MULT45             ((uint8)29U)
#define MCU_SPLL_MULT46             ((uint8)30U)
#define MCU_SPLL_MULT47             ((uint8)31U)


/* -------------------------------------------------------------------------- */
/* SPLL Reference Clock Divider                                               */
/* -------------------------------------------------------------------------- */

#define MCU_SPLL_PREDIV1            ((uint8)0U)
#define MCU_SPLL_PREDIV2            ((uint8)1U)
#define MCU_SPLL_PREDIV3            ((uint8)2U)
#define MCU_SPLL_PREDIV4            ((uint8)3U)
#define MCU_SPLL_PREDIV5            ((uint8)4U)
#define MCU_SPLL_PREDIV6            ((uint8)5U)
#define MCU_SPLL_PREDIV7            ((uint8)6U)
#define MCU_SPLL_PREDIV8            ((uint8)7U)


#endif /* __MCUCFG_H__ */