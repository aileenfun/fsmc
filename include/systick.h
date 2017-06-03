/*
 * FILE                : systick.h
 * DESCRIPTION         : This file is for systick.c
 * Author              : ysloveivy
 * Copyright           :
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 11/21/2015
 *
 * create.
 * --------------------
 */
#ifndef __systick_h__
#define __systick_h__

//--------------------------- Define ---------------------------//

//----------------------- Include files ------------------------//

//-------------------------- Typedef ---------------------------//
typedef struct {
	int (* initialize)(void);
	int second_flag;
	int ms_flag;
	int counter;
}SYSTICK_T;

//--------------------------- Extern ---------------------------//
extern SYSTICK_T systick;

#endif //__systick_h__
