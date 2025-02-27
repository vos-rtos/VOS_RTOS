

#include "vmisc.h"
#include "vtype.h"
#include "vos.h"
#include "stm32f4xx_hal.h"
#include "random_hw.h"

void SystemInit(void);
void HAL_IncTick(void);
void VOSExceptHandler(u32 *sp, s32 is_psp);

volatile s32 gUsbWorkMode = 0;


void SetUSBWorkMode(s32 mode)
{
	gUsbWorkMode = mode;
}

s32 GetUSBWorkMode()
{
	return gUsbWorkMode;
}

void SystemClock_Config(void)
{
#ifdef STM32F407xx
	  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	  /**Configure the main internal regulator output voltage
	  */
	  __HAL_RCC_PWR_CLK_ENABLE();
	  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	  /**Initializes the CPU, AHB and APB busses clocks
	  */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	  RCC_OscInitStruct.PLL.PLLM = 8;
	  RCC_OscInitStruct.PLL.PLLN = 336;
	  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	  RCC_OscInitStruct.PLL.PLLQ = 7;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    kprintf("error!\r\n");
	    while(1);
	  }
	  /**Initializes the CPU, AHB and APB busses clocks
	  */
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	  {
		    kprintf("error!\r\n");
		    while(1);
	  }
	//STM32F405x/407x/415x/417x 使能flash预取
		if (HAL_GetREVID() == 0x1001)
		{
			__HAL_FLASH_PREFETCH_BUFFER_ENABLE();
		}
#elif defined(STM32F429xx)
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/**Configure the main internal regulator output voltage
	*/
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/**Initializes the CPU, AHB and APB busses clocks
	*/
#if 0
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 360;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 8;
#elif 0
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 386;//360;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 8;//4;
#else //降频168MHz
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
#endif
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		while(1);
	}
	/**Activate the Over-Drive mode
	*/
	if (HAL_PWREx_EnableOverDrive() != HAL_OK)
	{
		while(1);
	}
	/**Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		while(1);
	}
#endif
}

void systick_init()
{
	u32 cnts = CPU_HZ/1000000;
	u32 reload;

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTick频率为HCLK

	reload = cnts;					    //每秒钟的计数次数 单位为K
	reload *= 1000000/OS_TICKS_PER_SEC;	//根据delay_ostickspersec设定溢出时间
											//reload为24位寄存器,最大值:16777216,在180M下,约合0.745s左右
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;//开启SYSTICK中断
	SysTick->LOAD = reload; 					//每1/OS_TICKS_PER_SEC秒中断一次
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //开启SYSTICK
}

void misc_init()
{
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
 	//TIM3_Int_Init(5000-1,8400-1);
	systick_init();
	HwRandomInit();
}


void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}

void HAL_Delay(uint32_t Delay)
{
	if (Delay > 20) {
		VOSTaskDelay(Delay);
	}
	else {
		VOSDelayUs(Delay*1000);
	}
}

uint32_t HAL_GetTick(void)
{
	return VOSGetTicks();
}



void __attribute__ ((section(".after_vectors")))
SysTick_Handler()
{
	VOSIntEnter();
	VOSSysTick();
	//HAL_IncTick();
	VOSIntExit ();
}


void SVC_Handler_C(u32 *svc_args, s32 is_psp)
{

	VOSIntEnter();
	StVosSysCallParam *psa;
	u8 svc_number;
	u32 irq_save;
	u32 offset = 0;
	if (!is_psp) {
		offset = 1;
	}
	irq_save = __local_irq_save();
	svc_number = ((char *)svc_args[6+offset])[-2];
	switch(svc_number) {
	case VOS_SVC_NUM_SYSCALL:
//		psa = (StVosSysCallParam *)svc_args[0+offset];
//		VOSSysCall(psa);
		break;

	case VOS_SVC_PRIVILEGED_MODE:
		svc_args[0+offset] = __switch_privileged_mode();
		break;

	default:
		kprintf("ERROR: SVC_Handler_C!\r\n");
		while (1) ;
		break;
	}

	__local_irq_restore(irq_save);
	VOSIntExit ();
}
void __attribute__ ((section(".after_vectors"),noreturn))
WWDG_IRQHandler()
{
	kprintf("WWDG_IRQHandler!\r\n");
}

void __attribute__ ((section(".after_vectors"),noreturn))
Reset_Handler ()
{
	vos_start ();
}

void __attribute__ ((section(".after_vectors"),weak))
HardFault_Handler ()
{
	asm volatile(
		  " push 	{lr}    \n"
		  " tst 	lr, #4  \n"
		  " ite 	eq      \n"
		  " mrseq 	r0, msp \n"
		  " mrsne 	r0, psp \n"
		  " mrs 	r1, msp \n"
		  " sub 	r1, r0  \n"
		  " bl 		VOSExceptHandler		\n"
		  " b		. 		\n"
		  " pop		{pc}	\n"
	);
}


void __attribute__ ((section(".after_vectors"),weak))
MemManage_Handler ()
{
	asm volatile(
		  " push 	{lr}    \n"
		  " tst 	lr, #4  \n"
		  " ite 	eq      \n"
		  " mrseq 	r0, msp \n"
		  " mrsne 	r0, psp \n"
		  " mrs 	r1, msp \n"
		  " sub 	r1, r0  \n"
		  " bl 		VOSExceptHandler		\n"
		  " b		. 		\n"
		  " pop		{pc}	\n"
	);
}


void __attribute__ ((section(".after_vectors"),weak,naked))
BusFault_Handler ()
{
	asm volatile(
		  " push 	{lr}    \n"
		  " tst 	lr, #4  \n"
		  " ite 	eq      \n"
		  " mrseq 	r0, msp \n"
		  " mrsne 	r0, psp \n"
		  " mrs 	r1, msp \n"
		  " sub 	r1, r0  \n"
		  " bl 		VOSExceptHandler		\n"
		  " b		. 		\n"
		  " pop		{pc}	\n"
	);
}

void __attribute__ ((section(".after_vectors"),weak,naked))
UsageFault_Handler ()
{
	asm volatile(
		  " push 	{lr}    \n"
		  " tst 	lr, #4  \n"
		  " ite 	eq      \n"
		  " mrseq 	r0, msp \n"
		  " mrsne 	r0, psp \n"
		  " mrs 	r1, msp \n"
		  " sub 	r1, r0  \n"
		  " bl 		VOSExceptHandler		\n"
		  " b		. 		\n"
		  " pop		{pc}	\n"
	);
}


void __attribute__ ((section(".after_vectors"),weak))
DebugMon_Handler ()
{
	asm volatile(
		  " push 	{lr}    \n"
		  " tst 	lr, #4  \n"
		  " ite 	eq      \n"
		  " mrseq 	r0, msp \n"
		  " mrsne 	r0, psp \n"
		  " mrs 	r1, msp \n"
		  " sub 	r1, r0  \n"
		  " bl 		VOSExceptHandler		\n"
		  " b		. 		\n"
		  " pop		{pc}	\n"
	);
}


void __attribute__ ((section(".after_vectors"),weak))
NMI_Handler ()
{
	asm volatile(
		  " push 	{lr}    \n"
		  " tst 	lr, #4  \n"
		  " ite 	eq      \n"
		  " mrseq 	r0, msp \n"
		  " mrsne 	r0, psp \n"
		  " mrs 	r1, msp \n"
		  " sub 	r1, r0  \n"
		  " bl 		VOSExceptHandler		\n"
		  " b		. 		\n"
		  " pop		{pc}	\n"
	);
}


extern HCD_HandleTypeDef hhcd_USB_OTG_FS;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
void OTG_FS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_FS_IRQn 0 */
  VOSIntEnter();

  if (GetUSBWorkMode()==USB_WORK_AS_DEVICE) {
	  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
  }
  else {
	  HAL_HCD_IRQHandler(&hhcd_USB_OTG_FS);
  }
  /* USER CODE BEGIN OTG_FS_IRQn 1 */
  VOSIntExit ();
  /* USER CODE END OTG_FS_IRQn 1 */
}

extern HCD_HandleTypeDef hhcd_USB_OTG_HS;
void OTG_HS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_HS_IRQn 0 */
  VOSIntEnter();
  /* USER CODE END OTG_HS_IRQn 0 */
  HAL_HCD_IRQHandler(&hhcd_USB_OTG_HS);
  /* USER CODE BEGIN OTG_HS_IRQn 1 */
  VOSIntExit ();
  /* USER CODE END OTG_HS_IRQn 1 */
}




