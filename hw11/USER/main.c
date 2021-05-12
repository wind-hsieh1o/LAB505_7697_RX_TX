#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "touch.h" 

//改自範例28
void led_mov(u8 choice);
void color_set(u8 x);
u8 pointI = 0;
u8 led_run = 0;
//清空屏幕並在右上角顯示"RST"
void Load_Drow_Dialog(void)
{
	u8 ci = 0;
	u8 xi = 0;
	LCD_Clear(WHITE);//清屏   
 	POINT_COLOR=BLUE;//設置字體為藍色 
	//LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//顯示清屏區域
	//***********************************************************************************************
	      //畫筆顏色
	
	for(ci = 0; ci<2; ci++){//verticle
		POINT_COLOR=CYAN; //btn block 
		LCD_DrawLine(0, 50+ci, 240, 50+ci); 
		LCD_DrawLine(0, 100+ci, 240, 100+ci); 
		LCD_DrawLine(0, 150+ci, 240, 150+ci);
		POINT_COLOR=GREEN;
		LCD_DrawLine(0, 150+ci+2, 240, 150+ci+2); //led block
		LCD_DrawLine(0, 180+ci, 240, 180+ci);
	}
	POINT_COLOR=CYAN; 
	for(ci = 0; ci<2; ci++){ //horizontal
		LCD_DrawLine(80-1+ci, 0, 80-1+ci, 150);
		LCD_DrawLine(160-1+ci, 0, 160-1+ci, 150);
	} 
	
	POINT_COLOR=GREEN;//POINT_COLOR=BRRED;      //畫筆顏色
	for(ci = 10; ci<15; ci++) LCD_Draw_Circle(40, 25 ,ci); //1
	for(ci = 10; ci<15; ci++) LCD_DrawRectangle(120-ci, 25-ci, 120+ci, 25+ci); //2 
	LCD_ShowString(185,20,60,16,16,"CLEAR"); //3
	
	POINT_COLOR=MAGENTA;      //畫筆顏色�
	LCD_ShowString(10,65,60,24,24,"Large");	 //4
	LCD_ShowString(185,70,60,12,12,"Small");  //6
	color_set(0);
	LCD_ShowString(100,70,60,16,16,"Color");  //5

	LCD_ShowString(100,115,60,16,16,"Pause");  //8
	POINT_COLOR=GREEN;
	LCD_ShowString(25,120,60,16,16,"<<<");	 //7
	LCD_ShowString(190,120,60,16,16,">>>");  //9
	
	//POINT_COLOR=CYAN; 
	POINT_COLOR=BRRED;
	for(ci = 0; ci<=9; ci++){ // 10 led
		//for(xi = 0; xi<3; xi++){
				LCD_Draw_Circle(ci*24 +12, 167 , 3);
				LCD_Draw_Circle(ci*24 +12, 167 , 4);
		//}
	}
	POINT_COLOR=RED;
	if(led_run == 1)
		for(xi = 0; xi<3; xi++) 
			LCD_Draw_Circle(pointI*24 +12, 167 , xi);
	
	//LCD_Draw_Circle(120 , 270, 30);
	//************************************************************************************************
}
////////////////////////////////////////////////////////////////////////////////
//電容觸摸屏專有部分
//畫水平線
//x0,y0:坐標
//len:線長度
//color:顏色
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//畫實心圓
//x0,y0:坐標
//r:半徑
//color:顏色
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  
//兩個數之差的絕對值 
//x1,x2：需取差值的兩個數
//返回值：|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  
//畫一條粗線
//(x1,y1),(x2,y2):線條的起始坐標
//size：線條的粗細程度
//color：線條的顏色
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //計算坐標增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //設置單步方向 
	else if(delta_x==0)incx=0;//垂直線 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平線 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //選取基本增量坐標軸 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//畫線輸出 
	{  
		gui_fill_circle(uRow,uCol,size,color);//畫點 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}   
////////////////////////////////////////////////////////////////////////////////
 //5個觸控點的顏色(電容觸摸屏用)												 
const u16 POINT_COLOR_TBL[OTT_MAX_TOUCH]={RED,GREEN,BLUE,BROWN,GRED};  
//電阻觸摸屏測試函數
void rtp_test(void)
{	/*
	while(1){
		tp_dev.scan(0); 		 
		if(tp_dev.sta&TP_PRES_DOWN)			//觸摸屏被按下
		{	
		 	if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{	
				if(tp_dev.x[0]>(lcddev.width-24)&&tp_dev.y[0]<16)Load_Drow_Dialog();//清除
				else TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],RED);		//畫圖	  			   
			}
		}else delay_ms(10);	//沒有按鍵按下的時候 	    
	}*/
}
//電容觸摸屏測試函數
void ctp_test(void)
{	
	/*
	u8 t=0;
	u8 i=0;	  	    
 	u16 lastpos[5][2];		//最後一次的數據 
	while(1)
	{
		tp_dev.scan(0);
		for(t=0;t<OTT_MAX_TOUCH;t++)
		{
			if((tp_dev.sta)&(1<<t))
			{
				if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)
				{
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
					//lcd_draw_bline(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],2,POINT_COLOR_TBL[t]);//畫線
					lastpos[t][0]=tp_dev.x[t];
					lastpos[t][1]=tp_dev.y[t];
					if(tp_dev.x[t]>(lcddev.width-24)&&tp_dev.y[t]<20)
					{
						Load_Drow_Dialog();//清除
					}
				}
			}else lastpos[t][0]=0XFFFF;
		}
		
		delay_ms(5);i++;
		if(i%20==0)LED0=!LED0;
	}	
	*/
}
	





	u16 circle_r = 30;
	u16 right_path = 0;
	u16 left_path = 0;
	u16 up_path = 0;
	u16 down_path = 0;
	u8 shape_now = 1;
	u8 my_color = 0;

void color_set(u8 x){
	if(x == 1){
		if(my_color < 12){
					my_color ++;
				}
				else my_color = 0;
				Load_Drow_Dialog();
	}
	switch(my_color)
				{
					case 0:POINT_COLOR=BLACK;break;
					case 1:POINT_COLOR=RED;break;
					case 2:POINT_COLOR=BRRED;break;
					case 3:POINT_COLOR=YELLOW;break;
					case 4:POINT_COLOR=GREEN;break;
					case 5:POINT_COLOR=CYAN;break;
					case 6:POINT_COLOR=BLUE;break; 
					case 7:POINT_COLOR=MAGENTA;break;
					case 9:POINT_COLOR=BROWN;break;
					case 10:POINT_COLOR=GRAY;break;
					case 11:POINT_COLOR=LGRAY;break;	
					case 12:POINT_COLOR=WHITE;break;
				}
}
void draw_contorl(u8 choice){
	u8 x = 0;
	x = choice;
	switch(x)
	{
			case 1: //DRAW CIRCLE
				Load_Drow_Dialog();	color_set(0);led_run = 0;
				shape_now = 1;
				circle_r = 30;
 				LCD_Draw_Circle(120 + right_path - left_path, 270+ up_path - down_path, circle_r);
				break;
				
			case 2: //DRAW Rectangle
				Load_Drow_Dialog();	color_set(0);led_run = 0;
				shape_now = 2;
				circle_r = 30;
				LCD_DrawRectangle(120-circle_r  + right_path - left_path, 270-circle_r+ up_path - down_path, 120+circle_r  + right_path - left_path, 270+circle_r+ up_path - down_path);
				break;
		
			case 8:
				led_mov(x);
				right_path = 0;
				left_path = 0;
				up_path = 0;
				down_path = 0;
				break;
			
			case 4: //LARGE
				if(circle_r < 90) circle_r ++;
				if(shape_now == 1){
					Load_Drow_Dialog();	color_set(0);led_run = 0;
					LCD_Draw_Circle(120 + right_path - left_path , 270+ up_path - down_path, circle_r);
				}
				else if(shape_now == 2){
						Load_Drow_Dialog();	color_set(0);led_run = 0;
						LCD_DrawRectangle(120-circle_r+ right_path - left_path, 270-circle_r+ up_path - down_path, 120+circle_r+ right_path - left_path, 270+circle_r+ up_path - down_path);
				}
				break;
					
			case 6: //SMALL
				if(circle_r > 5) circle_r --;
				if(shape_now == 1){
					Load_Drow_Dialog();	color_set(0);led_run = 0;
					LCD_Draw_Circle(120 + right_path - left_path , 270+ up_path - down_path, circle_r);
				}
				else if(shape_now == 2){
						Load_Drow_Dialog(); color_set(0);led_run = 0;
						LCD_DrawRectangle(120-circle_r+ right_path - left_path, 270-circle_r+ up_path - down_path, 120+circle_r+ right_path - left_path, 270+circle_r+ up_path - down_path);
				}
				break;
			case 3: //CLEAR 
				Load_Drow_Dialog();
				break; 
			
			case 7:	//<<
				led_run = 1;
				left_path++;
				if(shape_now == 1){
					Load_Drow_Dialog();	color_set(0);
					LCD_Draw_Circle(120 + right_path - left_path , 270+ up_path - down_path, circle_r);
				}
				else if(shape_now == 2){
						Load_Drow_Dialog();	color_set(0);
						LCD_DrawRectangle(120-circle_r+ right_path - left_path, 270-circle_r+ up_path - down_path, 120+circle_r+ right_path - left_path, 270+circle_r+ up_path - down_path);
				}
				led_mov(x);
				break;
			case 9: //>>
				led_run = 1;
				right_path ++;
				if(shape_now == 1){
					Load_Drow_Dialog();	color_set(0);
					LCD_Draw_Circle(120 + right_path - left_path , 270+ up_path - down_path, circle_r);
				}
				else if(shape_now == 2){
						Load_Drow_Dialog();	color_set(0);
						LCD_DrawRectangle(120-circle_r+ right_path - left_path, 270-circle_r+ up_path - down_path, 120+circle_r+ right_path - left_path, 270+circle_r+ up_path - down_path);
				}
				led_mov(x);
				break;
				
			case 5: //COLOR
				color_set(1);led_run = 0;
				if(shape_now == 1){
					LCD_Draw_Circle(120 + right_path - left_path , 270+ up_path - down_path, circle_r);
				}
				else if(shape_now == 2){
						LCD_DrawRectangle(120-circle_r+ right_path - left_path, 270-circle_r+ up_path - down_path, 120+circle_r+ right_path - left_path, 270+circle_r+ up_path - down_path);
				}
				break;
				
			case 10:	//<<
				up_path++; led_run = 0;
				if(shape_now == 1){
					Load_Drow_Dialog();	color_set(0);
					LCD_Draw_Circle(120 + right_path - left_path , 270 + up_path - down_path, circle_r);
				}
				else if(shape_now == 2){
						Load_Drow_Dialog();	color_set(0);
						LCD_DrawRectangle(120-circle_r+ right_path - left_path, 270-circle_r+ up_path - down_path, 120+circle_r+ right_path - left_path, 270+circle_r+ up_path - down_path);
				}
				break;
			
			case 11: //>>
				down_path ++; led_run = 0;
				if(shape_now == 1){
					Load_Drow_Dialog();	color_set(0);
					LCD_Draw_Circle(120 + right_path - left_path , 270+ up_path - down_path, circle_r);
				}
				else if(shape_now == 2){
						Load_Drow_Dialog();	color_set(0);
						LCD_DrawRectangle(120-circle_r+ right_path - left_path, 270-circle_r+ up_path - down_path, 120+circle_r+ right_path - left_path, 270+circle_r+ up_path - down_path);
				}
				break;		
		}
}


void rtp_control(void)
{
	u8 choice;
	u8 choice_from_de0;
	u8 t=0;
	u8 i=0;	  	    
 	//u16 lastpos[5][2];		//最後一次的數據 

	while(1)
	{
		tp_dev.scan(0);
		if(tp_dev.sta & TP_PRES_DOWN)
		{
				if(tp_dev.x[t]<lcddev.width && tp_dev.y[t]<lcddev.height)
				{
					if(tp_dev.x[0]>(lcddev.width-24)&&tp_dev.y[0]<16)Load_Drow_Dialog();//清除
					else TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],RED);		//畫圖
					
					if(tp_dev.x[0]>0 && tp_dev.x[0]<80 && tp_dev.y[0] > 0 && tp_dev.y[0]<50)	choice = 1;
					else if(tp_dev.x[0]>80 && tp_dev.x[0]<160 && tp_dev.y[0] > 0 && tp_dev.y[0]<50) choice = 2;
					else if(tp_dev.x[0]>160 && tp_dev.x[0]<240 && tp_dev.y[0] > 0 && tp_dev.y[0]<50) choice = 3;
					
					else if(tp_dev.x[0]>0 && tp_dev.x[0]<80 && tp_dev.y[0] > 50 && tp_dev.y[0]<100) choice = 4;
					else if(tp_dev.x[0]>80 && tp_dev.x[0]<160 && tp_dev.y[0] > 50 && tp_dev.y[0]<100) choice = 5;
					else if(tp_dev.x[0]>160 && tp_dev.x[0]<240 && tp_dev.y[0] > 50 && tp_dev.y[0]<100) choice = 6;
					
					else if(tp_dev.x[0]>0 && tp_dev.x[0]<80 && tp_dev.y[0] > 100 && tp_dev.y[0]<150) choice = 7;
					else if(tp_dev.x[0]>80 && tp_dev.x[0]<160 && tp_dev.y[0] > 100 && tp_dev.y[0]<150) choice = 8;
					else if(tp_dev.x[0]>160 && tp_dev.x[0]<240 && tp_dev.y[0] > 100 && tp_dev.y[0]<150) choice = 9;
					else choice = 0;
					
					draw_contorl(choice);
				}
		}
		// 讀入switch btn資料 變換led 跑馬燈
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) choice_from_de0 = 1;
		else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) choice_from_de0 = 2;
		else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 0) choice_from_de0 = 3;
		else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0) choice_from_de0 = 4;
		else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0) choice_from_de0 = 5;
		else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 0) choice_from_de0 = 6;
		
		else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0) choice_from_de0 = 7; //left
		else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == 0) choice_from_de0 = 8;
		else if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0) == 0) choice_from_de0 = 9; //E //right
		
		else if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1) == 0) choice_from_de0 = 10; //E //up
		else if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) == 0) choice_from_de0 = 11; //E //down

		else choice_from_de0 = 0;
		draw_contorl(choice_from_de0);
		
		GPIO_SetBits(GPIOB,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
		GPIO_SetBits(GPIOE,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
		
		delay_ms(100);
	}	
}

void led_mov(u8 choice){
		u8 x = 0;
	u8 ci, xi;
		x = choice;
		
		POINT_COLOR=RED;
	switch(x)
	{
			case 7: // left 讀入10
				GPIO_SetBits(GPIOF,GPIO_Pin_1);
				GPIO_ResetBits(GPIOF,GPIO_Pin_0);
				GPIO_SetBits(GPIOF,GPIO_Pin_0);
				  
				if(pointI == 0) pointI = 9;
				//for(xi = 0; xi<3; xi++) LCD_Draw_Circle(pointI*24 +12, 167 , 3);delay_ms(200);
				pointI--;
			
				break;
			case 8: // all light 讀入00
				GPIO_ResetBits(GPIOF,GPIO_Pin_0);
				GPIO_ResetBits(GPIOF,GPIO_Pin_1);
				GPIO_SetBits(GPIOF,GPIO_Pin_0);
				GPIO_SetBits(GPIOF,GPIO_Pin_1);

				//for(ci = 0; ci<=9; ci++) for(xi = 0; xi<3; xi++) LCD_Draw_Circle(pointI*24 +12, 167 , 3); delay_ms(200);
				
				break;
			case 9: // right 讀入01
				GPIO_SetBits(GPIOF,GPIO_Pin_0);
				GPIO_ResetBits(GPIOF,GPIO_Pin_1);
				GPIO_SetBits(GPIOF,GPIO_Pin_1);
			
				if(pointI == 9) pointI = 0;
				//for(xi = 0; xi<3; xi++) LCD_Draw_Circle(pointI*24 +12, 167 , 3);delay_ms(200);
				pointI++;
				
				break;
		}
}

void PBE_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOE時鐘
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB時鐘
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF時鐘
	
  //GPIOF9,F10初始化設置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;//DATA對應IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通輸出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽輸出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;//Addr1,Addr0對應IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通輸出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽輸出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;//Addr1,Addr0對應IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通輸出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽輸出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
	GPIO_SetBits(GPIOE,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
	GPIO_SetBits(GPIOF,GPIO_Pin_0 | GPIO_Pin_1);
	//GPIO_SetBits(GPIOE,GPIO_Pin_0 | GPIO_Pin_1);//GPIOE, set
	}

int main(void)
{ 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//設置系統中斷優先級分組2
	delay_init(168);  //初始化延時函數
	uart_init(115200);		//初始化串口波特率為115200
	
	LED_Init();					//初始化LED 
 	LCD_Init();					//LCD初始化 
	KEY_Init(); 				//按鍵初始化  
	tp_dev.init();				//觸摸屏初始化
	
	
	
 	POINT_COLOR=BLUE;
	LCD_ShowString(30,50,200,16,16,"0616");	
	LCD_ShowString(30,70,200,16,16,"Final Project!!!");	
	LCD_ShowString(30,90,200,16,16,"00552145");
	LCD_ShowString(30,110,200,16,16,"");
   	if(tp_dev.touchtype!=0XFF)LCD_ShowString(30,130,200,16,16,"Press KEY0 to Adjust");//電阻屏才顯示
	delay_ms(1500);
 	
	
	Load_Drow_Dialog();	 	
	if(tp_dev.touchtype&0X80){
		ctp_test();//電容屏測//觸控回饋
	}
	else {
		//rtp_test(); 					//電阻屏測試
		rtp_control(); // my code
	}
}
