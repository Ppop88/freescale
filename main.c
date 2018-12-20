#include "pid.h"
#include "ftm.h"
#include "uart.h"
#include "shiboqi.h"
#include "pit.h"
#include "gpio.h"

wrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr
11331

/*    
			EN1:PD11
			PWM输出： FTM0_CH7_PD07
			UART通信: UART5_RX_PE09_TX_PE08
			编码器接口:PA08,PA09




*/

int16_t goal_speed;

float speed;



static void PIT_ISR(void)
{
	int value;
	uint8_t  direction;
	
	FTM_QD_GetData(HW_FTM1, &value, &direction);
	FTM_QD_ClearCount(HW_FTM1);
	speed=value;//(25*0.001);
	
	Motor_Control(goal_speed,speed);
	
}












int main(){
		
		GPIO_QuickInit(HW_GPIOD, 11, kGPIO_Mode_OPP);
		GPIO_QuickInit(HW_GPIOD, 9, kGPIO_Mode_OPP);

	
	
		goal_speed=73.0;
	
		DelayInit();
		
		//pid初始化
		Pid_All_Init();
	
		//uart初始化
		UART_QuickInit(UART5_RX_PE09_TX_PE08 ,115200);  

	
		//ftm初始化
		PDout(11)=0;     //EN1使能
		PDout(9)=0;
		FTM_PWM_QuickInit(FTM0_CH7_PD07, kPWM_EdgeAligned, 10000);
		FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH7, 0);     /* 0-10000 对应 0-100% */
	
    
		//初始化解码
		FTM_QD_QuickInit(FTM1_QD_PHA_PA08_PHB_PA09, kFTM_QD_NormalPolarity,kQD_PHABEncoding);
	
	//pit
    PIT_InitTypeDef PIT_InitStruct1;
    PIT_InitStruct1.chl = HW_PIT_CH0;
    PIT_InitStruct1.timeInUs = 25*1000;//25ms 
    PIT_Init(&PIT_InitStruct1);
    PIT_CallbackInstall(HW_PIT_CH0, PIT_ISR);
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);
		
		
	
		while(1){
			VisualScope_Output(speed,goal_speed,0,0);
		}      
	}
