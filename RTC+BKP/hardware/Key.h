#ifndef __KEY_H
#define __KEY_H

typedef enum{
    KEY_NO = 0,
    KEY1,
    KEY2,
    KEY3,
} KEY_NUM;

void Key_Init(void);
KEY_NUM Key_GetNum(void);

#endif
