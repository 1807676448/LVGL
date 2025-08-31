#ifndef __TOUCH_H
#define __TOUCH_H

#include "lcd.h"
#include "sys.h"
#include "main.h"
#include <stdlib.h>
#include <math.h>

#define T_CLK_Pin GPIO_PIN_13
#define T_CLK_GPIO_Port GPIOF
#define T_CS_Pin GPIO_PIN_14
#define T_CS_GPIO_Port GPIOF
#define T_DIN_Pin GPIO_PIN_15
#define T_DIN_GPIO_Port GPIOF
#define T_DO_Pin GPIO_PIN_0
#define T_DO_GPIO_Port GPIOG
#define T_IRQ_Pin GPIO_PIN_1
#define T_IRQ_GPIO_Port GPIOG

#define TP_PRES_DOWN 0x80  //触屏被按下	  
#define TP_CATH_PRES 0x40  //有按键按下了 	  
										    
//触摸屏控制器
typedef struct
{
	uint8_t (*init)(void);			//初始化触摸屏控制器
	uint8_t (*scan)(uint8_t);				//扫描触摸屏.0,屏幕扫描;1,物理坐标;	 
	uint16_t x0;						//原始坐标(第一次按下时的坐标)
	uint16_t y0;
	uint16_t x; 						//当前坐标(此次扫描时,触屏的坐标)
	uint16_t y;						   	    
	uint8_t  sta;					//笔的状态 
								//b7:按下1/松开0; 
	                            //b6:0,没有按键按下;1,有按键按下.         			  
////////////////////////触摸屏校准参数/////////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
//touchtype=0的时候,适合左右为X坐标,上下为Y坐标的TP.
//touchtype=1的时候,适合左右为Y坐标,上下为X坐标的TP.
	uint8_t touchtype;
}_m_tp_dev;

extern _m_tp_dev tp_dev;	 	//触屏控制器在touch.c里面定义

//与触摸屏芯片连接引脚	   
//与触摸屏芯片连接引脚

#define PEN  		PG_IN(1)  	//T_IRQ
#define DOUT 		PG_IN(0)   	//T_DO
#define TDIN(x) 	PF_OUT(15,x)  	//T_DIN
#define TCLK(x) 		PF_OUT(13,x)  	//T_CLK
#define TCS(x)  		PF_OUT(14,x)  	//T_CS  

uint16_t TP_Read_AD(uint8_t CMD);

uint16_t TP_Read_XOY(uint8_t xy);

uint8_t TP_Read_XY(uint16_t *x, uint16_t *y);

uint8_t TP_Read_XY2(uint16_t *x, uint16_t *y);

void TP_Drow_Touch_Point(uint16_t x, uint16_t y, uint16_t color);

void TP_Draw_Big_Point(uint16_t x, uint16_t y, uint16_t color);

uint8_t TP_Scan(uint8_t tp);

void TP_Save_Adjdata(void);

uint8_t TP_Get_Adjdata(void);

void TP_Adj_Info_Show(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, 
                      uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t fac);

void TP_Adjust(void);

uint8_t TP_Init(void);

#endif