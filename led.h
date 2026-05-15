//----------------------------------------------------------------------------//
//                               OBJECT HISTORY                               //
//----------------------------------------------------------------------------//
//  REVISION |    DATE     |                               |      AUTHOR      //
//----------------------------------------------------------------------------//
//  1.00     | 15/Mai/2026 |                               | ALCP             //
// - First version                                                            //
//----------------------------------------------------------------------------//

#ifndef LED_H
#define LED_H

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------//
// EXTERNAL FUNCTIONS
//----------------------------------------------------------------------------//
/**
 * LED Initialization.
 * 
 * \param   nothing
 * \return  nothing
 */
extern void led_init(void);

/**
 * Periodic function.
 * 
 * \param   nothing
 * \return  nothing
 */
extern void led_periodic(void);

/**
 * Sets LED PWM Duty Cycle.
 * 
 * \param   pwm 0% (0) to 100% (255)
 * \return  nothing
 */
extern void led_setPWMDutyCycle(unsigned char pwm);

/**
 * Sets LED ON Time.
 * 
 * \param   on_time on a 20ms time basis
 * \return  nothing
 */
extern void led_setOnTime(unsigned char on_time);

/**
 * Sets LED OFF Time.
 * 
 * \param   off_time on a 20ms time basis
 * \return  nothing
 */
extern void led_setOffTime(unsigned char off_time);


#ifdef __cplusplus
}
#endif

#endif /* LED_H */

