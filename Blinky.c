/*----------------------------------------------------------------------------
 * Name:    BlinkySingleLED_commanded_IE.c
 * Purpose: Turn on/off LED depending on input, managed via interrupt
 *----------------------------------------------------------------------------*/

#include <stm32f10x.h>                       /* STM32F103 definitions         */
#include "stm32f10x_it.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include "RTL.h"
#include "EXTI_pins_configuration.h"
#include "pins_definition.h"

extern volatile int T1 = 0, IDLE = 1;

volatile int Eid = 0x01;

OS_TID Tid;         // Task1 (phase manager) id

volatile int current_Pin;

__task void Task1(void) {
	//wake up every 10 ms
	//capture events generated by ISR methods
	//react to events

	unsigned int last_Pin = PIN_IDLE_LEVER;
	//unsigned int last_time_max_accel;
	
	//os_itv_set(10);
	while(1) {
		T1 = 1; IDLE = 0;
		//os_itv_wait();
		if(current_Pin != last_Pin){
			switch(current_Pin) {
				case PIN_MAX_BRAKING:
					GPIOC->ODR	&= ~(1<<OUT_MED_BRAKING);			// Switch off the MED_POWER braking Pin
					GPIOC->ODR	|= (1<<OUT_MAX_BRAKING);			// Switch on the MAX_POWER braking Pin
					break;
				case PIN_MED_BRAKING:
					GPIOC->ODR	&= ~(1<<OUT_MAX_BRAKING);			// Switch off the MAX_POWER braking Pin
					GPIOC->ODR	&= ~(1<<OUT_MIN_BRAKING);			// Switch off the MIN_POWER braking Pin
					GPIOC->ODR	|= (1<<OUT_MED_BRAKING);			// Switch on the MED_POWER braking Pin
					break;
				case PIN_MIN_BRAKING:
					GPIOC->ODR	&= ~(1<<OUT_MED_BRAKING);			// Switch off the MED_POWER braking Pin
					GPIOC->ODR	|= (1<<OUT_MIN_BRAKING);			// Switch on the MIN_POWER braking Pin
					break;
				case PIN_IDLE_LEVER:
					GPIOC->ODR	&= ~(1<<OUT_MIN_BRAKING);			// Switch off the MIN_POWER braking Pin
					GPIOC->ODR	&= ~(1<<OUT_MIN_ACCEL);				// Switch off the MIN_POWER engine Pin
					break;
				case PIN_MIN_ACCEL:
					GPIOC->ODR	&= ~(1<<OUT_MED_ACCEL);				// Switch off the MED_POWER engine Pin
					GPIOC->ODR	|= (1<<OUT_MIN_ACCEL);				// Switch on the MIN_POWER engine Pin
					break;
				case PIN_MED_ACCEL:
					GPIOC->ODR	&= ~(1<<OUT_MAX_ACCEL);				// Switch off the MAX_POWER engine Pin
					GPIOC->ODR	&= ~(1<<OUT_MIN_ACCEL);				// Switch off the MIN_POWER engine Pin
					GPIOC->ODR	|= (1<<OUT_MED_ACCEL);				// Switch on the MED_POWER engine Pin
					break;
				case PIN_MAX_ACCEL:
					GPIOC->ODR	&= ~(1<<OUT_MED_ACCEL);				// Switch off the MED_POWER engine Pin
					GPIOC->ODR	|= (1<<OUT_MAX_ACCEL);				// Switch on the MIN_POWER engine Pin
					// update last_time_max_accel
					break;
			}
			last_Pin = current_Pin;
		}
		//check if current_Pin is MAX_ACCEL and last_time_max_accel - now > 4s
	}
}

__task void TaskInit(void) {
  //T2id = os_tsk_create( Task2, 10 );	
  Tid = os_tsk_create( Task1, 1 );
  //TBid = os_tsk_create( TaskB, 1 );
  //T3id = os_tsk_create( Task3, 80 );
	//TsimIdm = os_tsk_create( TaskSim, 99 );

  os_tsk_delete_self();      // kills self
}


/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/

int main(void) {
	
	RCC->APB2ENR	|=	RCC_APB2ENR_IOPCEN;			// Enable GPIOC clock
	GPIOC->CRL		=		0x00000333;							// PC.0..2 defined as Outputs (Signals to Engine)
	GPIOC->CRH		=		0x00003333;							// PC.8..11 defined as Outputs (Signals to Brakes)

  Configure_EXTI_GPIOB2();
	Configure_EXTI_GPIOB3();
	Configure_EXTI_GPIOB4();
	Configure_EXTI_GPIOB5();
	Configure_EXTI_GPIOB6();
	Configure_EXTI_GPIOB7();
	Configure_EXTI_GPIOB8();
	
	os_sys_init( TaskInit );
	//while(1){
	//		wait ();
	//		IN_INT=0;
	//}
}