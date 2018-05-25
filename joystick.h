/*============================================================================================
 *
 *  (C) 2010, Phyton
 *
 *  ���������������� ������ ��� 1986BE91 testboard
 *
 *  ������ �� ��������������� "��� ����", �.�. ������������� ��� ������, ���������� ��������� 
 *  ������������� ���������� �� ���������� ��� ����������� Milandr 1986BE91T1. �������� Phyton 
 *  �� ����� ������� ��������������� �� ��������� ����������� ������������� �������, ��� 
 *  �������������� ������������� �� ��� ������, ��.
 *
 *--------------------------------------------------------------------------------------------
 *
 *  ���� joystick.h: ������ � ������� ������ ("����������")
 *
 *============================================================================================*/

#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "types.h"

/* "���� ������" */
typedef enum {
    NOKEY      = 0,
    SEL        = 1,
    RIGHT      = 2,
    LEFT       = 3,
    UP         = 4,
    DOWN       = 5,
    MULTIPLE   = 6,
    NUM_KEY_CODES
} KeyCode;

/* ����������� "����" �� ������� ������� */
KeyCode GetKey(void);

#define KEY_PRESSED(x)				(GetKey() == x)
#define WAIT_UNTIL_KEY_PRESSED(x)	while(!KEY_PRESSED(x)){}
#define WAIT_UNTIL_KEY_RELEASED(x)	while(KEY_PRESSED(x)){}
#define WAIT_UNTIL_ANY_KEY          while(GetKey() == NOKEY){}

#endif /* __JOYSTICK_H */

/*============================================================================================
 * ����� ����� joystick.h
 *============================================================================================*/


