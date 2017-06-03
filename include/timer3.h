/*
 * FILE                : tiemr3.h
 * DESCRIPTION         : this file is for timer3.c
 * Author              : ysloveivy
 * Copyright           :
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 11/30/2015
 *
 * create.
 * --------------------
 */

#ifndef __timer3_h__
#define __timer3_h__

//--------------------------- Define ---------------------------//

//----------------------- Include files ------------------------//

//-------------------------- Typedef----------------------------//
typedef struct {
	int (* initialize)(void);
}TIMER3_T;

//--------------------------- Extern ---------------------------//
extern TIMER3_T timer3;

#endif //__led_h__
