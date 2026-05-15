//----------------------------------------------------------------------------//
//                               OBJECT HISTORY                               //
//----------------------------------------------------------------------------//
//  REVISION |    DATE     |                               |      AUTHOR      //
//----------------------------------------------------------------------------//
//  1.00     | 15/Mai/2026 |                               | ALCP             //
// - First version                                                            //
//----------------------------------------------------------------------------//

#ifndef IO_H
#define IO_H

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------//
// EXTERNAL DEFINITIONS
//----------------------------------------------------------------------------//
#define IO_INACTIVE 0
#define IO_ACTIVE   1

//----------------------------------------------------------------------------//
// EXTERNAL FUNCTIONS
//----------------------------------------------------------------------------//
/**
 * IO Initialization.
 * 
 * \param   nothing
 * \return  nothing
 */
extern void io_init(void);

/**
 * Sets SHDN.
 * 
 * \param   IO_INACTIVE if inactive (SHDN will be set as input), 
 *          IO_ACTIVE if active (STDH will be set as output with 0)
 * \return  nothing
 */
extern void io_setSHDN(unsigned char state);

/**
 * Sets LED Duty Cycle.
 * 
 * \param   duty: 0 = 0% ... 255 = 100%
 * \return  nothing
 */
extern void io_setLED(unsigned char duty);

#ifdef __cplusplus
}
#endif

#endif /* IO_H */

