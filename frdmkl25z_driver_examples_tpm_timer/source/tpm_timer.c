/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_tpm.h"
#include "fsl_gpio.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "fsl_adc16.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_ADC16_BASE ADC0
#define DEMO_ADC16_CHANNEL_GROUP 0U
#define DEMO_ADC16_USER_CHANNEL 0U /*PTE20, ADC0_SE0 */

#define EXT_BOARD_LED_G_GPIO EXT_BOARD_LED_G_RED_GPIO
#define EXT_BOARD_LED_G_GPIO_PIN EXT_BOARD_LED_G_RED_GPIO_PIN

#define EXT_BOARD_LED_F_GPIO EXT_BOARD_LED_F_RED_GPIO
#define EXT_BOARD_LED_F_GPIO_PIN EXT_BOARD_LED_F_RED_GPIO_PIN

#define EXT_BOARD_LED_A_GPIO EXT_BOARD_LED_A_RED_GPIO
#define EXT_BOARD_LED_A_GPIO_PIN EXT_BOARD_LED_A_RED_GPIO_PIN

#define EXT_BOARD_LED_B_GPIO EXT_BOARD_LED_B_RED_GPIO
#define EXT_BOARD_LED_B_GPIO_PIN EXT_BOARD_LED_B_RED_GPIO_PIN

#define EXT_BOARD_LED_E_GPIO EXT_BOARD_LED_E_RED_GPIO
#define EXT_BOARD_LED_E_GPIO_PIN EXT_BOARD_LED_E_RED_GPIO_PIN

#define EXT_BOARD_LED_D_GPIO EXT_BOARD_LED_D_RED_GPIO
#define EXT_BOARD_LED_D_GPIO_PIN EXT_BOARD_LED_D_RED_GPIO_PIN

#define EXT_BOARD_LED_C_GPIO EXT_BOARD_LED_C_RED_GPIO
#define EXT_BOARD_LED_C_GPIO_PIN EXT_BOARD_LED_C_RED_GPIO_PIN

#define EXT_BOARD_LED_DP_GPIO EXT_BOARD_LED_DP_RED_GPIO
#define EXT_BOARD_LED_DP_GPIO_PIN EXT_BOARD_LED_DP_RED_GPIO_PIN

#define DISP_1_ENABLE_GPIO DISP_1_UNIT_ENABLE_GPIO
#define DISP_1_ENABLE_GPIO_PIN DISP_1_UNIT_ENABLE_GPIO_PIN

#define DISP_2_ENABLE_GPIO DISP_2_DEC_ENABLE_GPIO
#define DISP_2_ENABLE_GPIO_PIN DISP_2_DEC_ENABLE_GPIO_PIN

#define DISP_3_ENABLE_GPIO DISP_3_CENT_ENABLE_GPIO
#define DISP_3_ENABLE_GPIO_PIN DISP_3_CENT_UNIT_ENABLE_GPIO_PIN

#define DISP_MODE_ENABLE_GPIO DISP_MODE_CENT_ENABLE_GPIO
#define DISP_MODE_ENABLE_GPIO_PIN DISP_MODE_CENT_UNIT_ENABLE_GPIO_PIN

#define BUTTON_PLUS BUTON_PLUS_1
#define BUTTON_PLUS_PIN BUTON_PLUS_PIN1

#define BUTTON_RESET BUTON_RESET_1
#define BUTTON_RESET_PIN BUTON_RESET_PIN1

#define BUTTON_SUB BUTON_SUB_1
#define BUTTON_SUB_PIN BUTON_SUB_PIN1

/* define instance */
#define BOARD_TPM TPM2
/* Interrupt number and interrupt handler for the TPM instance used */
#define BOARD_TPM_IRQ_NUM TPM2_IRQn
#define BOARD_TPM_HANDLER TPM2_IRQHandler
/* Get source clock for TPM driver */
#define TPM_SOURCE_CLOCK (CLOCK_GetFreq(kCLOCK_PllFllSelClk)/4)

#define TRUE		1u
#define FALSE		0u
#define RESETVALUE	0u
#define SECONDLOOP	1000U

#define T_SBYTE int8_t
#define	T_SLONG uint32_t
#define T_BIT bool


void Cuenta(void);
void Cuenta_Mode(void);
void ContadorManual(void);
void Disp_7_seg(T_SBYTE lsb_Digito);
void Compare(void);
void MODE(void);
void multiplexing(T_SBYTE rsb_Count_multiplexing);
void Display(void);
void ADC(void);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
T_BIT		rbi_tpmIsrFlag = FALSE;
T_SLONG		rsl_milisecondCounts = 0U;
T_SBYTE 	rsb_SecondCount= 0u;
T_SBYTE		rsb_DecadeSecondCount= 0u;
T_SBYTE		rsb_HundredSecondCount= 0u;

T_SBYTE		rsb_Type_Mode;
T_SBYTE		rsb_Digito_unidad = 0;
T_SBYTE		rsb_Digito_decena = 0;
T_SBYTE		rsb_Digito_centena = 0;
T_SLONG		rsl_valor=0;


/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
	/* define init structure for the ADC */
	adc16_config_t adc16ConfigStruct;
	adc16_channel_config_t adc16ChannelConfigStruct;


    /* Define the init structure for the output LED pin*/

	gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput, 0,
    };
    gpio_pin_config_t button_config = {
    	kGPIO_DigitalInput, 0,
        };

    tpm_config_t tpmInfo;

    /* Board pin, clock, debug console init */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* configure ADC*/
    adc16ConfigStruct.referenceVoltageSource = kADC16_ReferenceVoltageSourceVref;
    adc16ConfigStruct.clockSource = kADC16_ClockSourceAsynchronousClock;
    adc16ConfigStruct.enableAsynchronousClock = true;
    adc16ConfigStruct.clockDivider = kADC16_ClockDivider8;
    adc16ConfigStruct.resolution = kADC16_ResolutionSE12Bit;
    adc16ConfigStruct.longSampleMode = kADC16_LongSampleDisabled;
    adc16ConfigStruct.enableHighSpeed = false;
    adc16ConfigStruct.enableLowPower = false;
    adc16ConfigStruct.enableContinuousConversion = false;

    //ADC16_GetDefaultConfig(&adc16ConfigStruct);
#ifdef BOARD_ADC_USE_ALT_VREF
    adc16ConfigStruct.referenceVoltageSource = kADC16_ReferenceVoltageSourceValt;
#endif
    ADC16_Init(DEMO_ADC16_BASE, &adc16ConfigStruct);
    ADC16_EnableHardwareTrigger(DEMO_ADC16_BASE, false); /* Make sure the software trigger is used. */
#if defined(FSL_FEATURE_ADC16_HAS_CALIBRATION) && FSL_FEATURE_ADC16_HAS_CALIBRATION
    if (kStatus_Success == ADC16_DoAutoCalibration(DEMO_ADC16_BASE))
    {
        PRINTF("ADC16_DoAutoCalibration() Done.\r\n");
    }
    else
    {
        PRINTF("ADC16_DoAutoCalibration() Failed.\r\n");
    }
#endif /* FSL_FEATURE_ADC16_HAS_CALIBRATION */

    adc16ChannelConfigStruct.channelNumber = DEMO_ADC16_USER_CHANNEL;
    adc16ChannelConfigStruct.enableInterruptOnConversionCompleted = false;
#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
    adc16ChannelConfigStruct.enableDifferentialConversion = false;
#endif /* FSL_FEATURE_ADC16_HAS_DIFF_MODE */


    /* Init output LED GPIO. */
       //GPIO_PinInit(BOARD_LED_GPIO, BOARD_LED_GPIO_PIN, &led_config);
    GPIO_PinInit( EXT_BOARD_LED_G_GPIO, EXT_BOARD_LED_G_GPIO_PIN, &led_config);
    GPIO_PinInit( EXT_BOARD_LED_F_GPIO, EXT_BOARD_LED_F_GPIO_PIN, &led_config);
    GPIO_PinInit( EXT_BOARD_LED_A_GPIO, EXT_BOARD_LED_A_GPIO_PIN, &led_config);
    GPIO_PinInit( EXT_BOARD_LED_B_GPIO, EXT_BOARD_LED_B_GPIO_PIN, &led_config);
    GPIO_PinInit( EXT_BOARD_LED_E_GPIO, EXT_BOARD_LED_E_GPIO_PIN, &led_config);
    GPIO_PinInit( EXT_BOARD_LED_D_GPIO, EXT_BOARD_LED_D_GPIO_PIN, &led_config);
    GPIO_PinInit( EXT_BOARD_LED_C_GPIO, EXT_BOARD_LED_C_GPIO_PIN, &led_config);
    GPIO_PinInit( EXT_BOARD_LED_DP_GPIO, EXT_BOARD_LED_DP_GPIO_PIN, &led_config);

    GPIO_PinInit( DISP_1_ENABLE_GPIO, DISP_1_ENABLE_GPIO_PIN, &led_config);
    GPIO_PinInit( DISP_2_ENABLE_GPIO, DISP_2_ENABLE_GPIO_PIN, &led_config);
    GPIO_PinInit( DISP_3_ENABLE_GPIO, DISP_3_ENABLE_GPIO_PIN, &led_config);
    GPIO_PinInit( DISP_MODE_ENABLE_GPIO, DISP_MODE_ENABLE_GPIO_PIN, &led_config);


    GPIO_PinInit( BUTTON_PLUS, BUTTON_PLUS_PIN, &button_config);
    GPIO_PinInit( BUTTON_RESET, BUTTON_RESET_PIN, &button_config);
    GPIO_PinInit( BUTTON_SUB, BUTTON_SUB_PIN, &button_config);

    /* Select the clock source for the TPM counter as kCLOCK_PllFllSelClk */
    CLOCK_SetTpmClock(1U);

//    /* Print a note to terminal */
//    PRINTF("\r\nTPM example to simulate a timer\r\n");

    TPM_GetDefaultConfig(&tpmInfo);
	
	/* TPM clock divide by 4 */
    tpmInfo.prescale = kTPM_Prescale_Divide_4;
	
    /* Initialize TPM module */
    TPM_Init(BOARD_TPM, &tpmInfo);

    /*
     * Set timer period.
     */
    TPM_SetTimerPeriod(BOARD_TPM, USEC_TO_COUNT(1000U, TPM_SOURCE_CLOCK));

    TPM_EnableInterrupts(BOARD_TPM, kTPM_TimeOverflowInterruptEnable);

    EnableIRQ(BOARD_TPM_IRQ_NUM);

    TPM_StartTimer(BOARD_TPM, kTPM_SystemClock);

    Disp_7_seg(rsb_Digito_unidad);
    while (true)
    {
    	if(GPIO_ReadPinInput(BUTTON_RESET, BUTTON_RESET_PIN)==TRUE)
    	{
    		MODES();
    	}
    		switch (rsb_Type_Mode)
    		{
    		case 0u:

    			Cuenta();
    			break;

    		case 1u:

    			ContadorManual();
    			break;

    		default:
    		    ADC16_SetChannelConfig(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP, &adc16ChannelConfigStruct);
    		    while (0U == (kADC16_ChannelConversionDoneFlag &
    		                  ADC16_GetChannelStatusFlags(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP)))
    		    {
    		    }
    			ADC();
    			break;

    		}


        __WFI();
    }
}

void Compare(void){

	if(rsb_Digito_unidad>9)
	{
		rsb_Digito_unidad=0;
	    rsb_Digito_decena++;
   	}
	if(rsb_Digito_decena>9)
	{
		rsb_Digito_decena=0;
		rsb_Digito_centena++;
	}
	if(rsb_Digito_centena>9)
	{
		rsb_Digito_centena=0;
		rsb_Digito_decena=0;
		rsb_Digito_unidad=0;
	}


	if(rsb_Digito_unidad<0)
	{
		rsb_Digito_unidad=9;
		rsb_Digito_decena--;
	}

	if(rsb_Digito_decena<0)
	{
		rsb_Digito_decena=9;
		rsb_Digito_centena--;
	}

	if(rsb_Digito_centena<0)
	{
		rsb_Digito_centena=9;
		rsb_Digito_decena=9;
		rsb_Digito_unidad=9;
	}

}
void Cuenta(void){

	if (rbi_tpmIsrFlag)
    {

		rsl_milisecondCounts ++;
        rbi_tpmIsrFlag = false;
        if(rsl_milisecondCounts >= SECONDLOOP)
        {
//
        	rsb_SecondCount++;
            rsl_milisecondCounts = 0U;
        }
        if(rsb_SecondCount>9u)
        {
        	rsb_DecadeSecondCount++;
        	rsb_SecondCount=0;
        }
        if(rsb_DecadeSecondCount>9)
        {
        	rsb_HundredSecondCount++;
        	rsb_DecadeSecondCount=0;
        }
    }

    rsb_Digito_unidad=rsb_SecondCount;
    rsb_Digito_decena=rsb_DecadeSecondCount;
	rsb_Digito_centena=rsb_HundredSecondCount;

}
void Cuenta_Mode(void){

	if (rbi_tpmIsrFlag)
    {

		rsl_milisecondCounts ++;
        rbi_tpmIsrFlag = false;
        if(rsl_milisecondCounts >= SECONDLOOP)
        {

        	rsb_SecondCount++;
            rsl_milisecondCounts = 0U;
        }
    }
}
void Display (void)
{
	multiplexing(0);
	Disp_7_seg(rsb_Digito_unidad);
	GPIO_SetPinsOutput(EXT_BOARD_LED_DP_GPIO, 1u << EXT_BOARD_LED_DP_GPIO_PIN);
	multiplexing(1);
	Disp_7_seg(rsb_Digito_decena);
	multiplexing(2);
	Disp_7_seg(rsb_Digito_centena);
	if(rsb_Type_Mode==2)
		{
			GPIO_SetPinsOutput(EXT_BOARD_LED_DP_GPIO, 1u << EXT_BOARD_LED_DP_GPIO_PIN);
		}
	multiplexing(3);
	Disp_7_seg(rsb_Type_Mode);
	multiplexing(4);
}
void multiplexing(T_SBYTE rsb_Count_multiplexing)
{
	GPIO_ClearPinsOutput(EXT_BOARD_LED_DP_GPIO, 1u << EXT_BOARD_LED_DP_GPIO_PIN);
	GPIO_ClearPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
	GPIO_ClearPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
	GPIO_ClearPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
	GPIO_ClearPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
	GPIO_ClearPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
	GPIO_ClearPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
	GPIO_ClearPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

	if(rsb_Count_multiplexing==0)
	{
		GPIO_ClearPinsOutput(DISP_1_ENABLE_GPIO, 1u << DISP_1_ENABLE_GPIO_PIN);
		GPIO_SetPinsOutput(DISP_2_ENABLE_GPIO, 1u << DISP_2_ENABLE_GPIO_PIN);
		GPIO_SetPinsOutput(DISP_3_ENABLE_GPIO, 1u << DISP_3_ENABLE_GPIO_PIN);
		GPIO_SetPinsOutput(DISP_MODE_ENABLE_GPIO, 1u << DISP_MODE_ENABLE_GPIO_PIN);
	}
	else if(rsb_Count_multiplexing==1)
	{
		GPIO_SetPinsOutput(DISP_1_ENABLE_GPIO, 1u << DISP_1_ENABLE_GPIO_PIN);
		GPIO_ClearPinsOutput(DISP_2_ENABLE_GPIO, 1u << DISP_2_ENABLE_GPIO_PIN);
		GPIO_SetPinsOutput(DISP_3_ENABLE_GPIO, 1u << DISP_3_ENABLE_GPIO_PIN);
		GPIO_SetPinsOutput(DISP_MODE_ENABLE_GPIO, 1u << DISP_MODE_ENABLE_GPIO_PIN);
	}
	else if(rsb_Count_multiplexing==2)
	{
		GPIO_SetPinsOutput(DISP_1_ENABLE_GPIO, 1u << DISP_1_ENABLE_GPIO_PIN);
		GPIO_SetPinsOutput(DISP_2_ENABLE_GPIO, 1u << DISP_2_ENABLE_GPIO_PIN);
		GPIO_ClearPinsOutput(DISP_3_ENABLE_GPIO, 1u << DISP_3_ENABLE_GPIO_PIN);
		GPIO_SetPinsOutput(DISP_MODE_ENABLE_GPIO, 1u << DISP_MODE_ENABLE_GPIO_PIN);
	}
	else if(rsb_Count_multiplexing==3)
	{
		GPIO_SetPinsOutput(DISP_1_ENABLE_GPIO, 1u << DISP_1_ENABLE_GPIO_PIN);
		GPIO_SetPinsOutput(DISP_2_ENABLE_GPIO, 1u << DISP_2_ENABLE_GPIO_PIN);
		GPIO_SetPinsOutput(DISP_3_ENABLE_GPIO, 1u << DISP_3_ENABLE_GPIO_PIN);
		GPIO_ClearPinsOutput(DISP_MODE_ENABLE_GPIO, 1u << DISP_MODE_ENABLE_GPIO_PIN);
	}
	else
	{
		GPIO_SetPinsOutput(DISP_1_ENABLE_GPIO, 1u << DISP_1_ENABLE_GPIO_PIN);
		GPIO_SetPinsOutput(DISP_2_ENABLE_GPIO, 1u << DISP_2_ENABLE_GPIO_PIN);
		GPIO_SetPinsOutput(DISP_3_ENABLE_GPIO, 1u << DISP_3_ENABLE_GPIO_PIN);
		GPIO_SetPinsOutput(DISP_MODE_ENABLE_GPIO, 1u << DISP_MODE_ENABLE_GPIO_PIN);
		rsb_Count_multiplexing=0;
	}

}

void Disp_7_seg(T_SBYTE lsb_Digito){

	if(lsb_Digito==0)
	{
        GPIO_ClearPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

        GPIO_SetPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);


	}
	else if(lsb_Digito==1)
	{
        GPIO_ClearPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

        GPIO_SetPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);


	}
	else if(lsb_Digito==2)
	{
        GPIO_ClearPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

        GPIO_SetPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);


	}
	else if(lsb_Digito==3)
	{
        GPIO_ClearPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

        GPIO_SetPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

	}
	else if(lsb_Digito==4)
	{
        GPIO_ClearPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

        GPIO_SetPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

	}

	else if(lsb_Digito==5)
	{
        GPIO_ClearPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

        GPIO_SetPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

	}
	else if(lsb_Digito==6)
	{
        GPIO_ClearPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

        GPIO_SetPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

	}
	else if(lsb_Digito==7)
	{
        GPIO_ClearPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

        GPIO_SetPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

	}
	else if(lsb_Digito==8)
	{
        GPIO_ClearPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

        GPIO_SetPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

	}
	else if(lsb_Digito==9)
	{
        GPIO_ClearPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_E_GPIO, 1u << EXT_BOARD_LED_E_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_D_GPIO, 1u << EXT_BOARD_LED_D_GPIO_PIN);
        GPIO_ClearPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

        GPIO_SetPinsOutput(EXT_BOARD_LED_G_GPIO, 1u << EXT_BOARD_LED_G_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_F_GPIO, 1u << EXT_BOARD_LED_F_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_A_GPIO, 1u << EXT_BOARD_LED_A_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_B_GPIO, 1u << EXT_BOARD_LED_B_GPIO_PIN);
        GPIO_SetPinsOutput(EXT_BOARD_LED_C_GPIO, 1u << EXT_BOARD_LED_C_GPIO_PIN);

	}
	else
	{

	}
}

void MODES(void)
{

		rsb_SecondCount=0;
		while(GPIO_ReadPinInput(BUTTON_RESET, BUTTON_RESET_PIN)==TRUE)
	    	{	//wait
				Cuenta_Mode();
				rsb_Digito_unidad=RESETVALUE;
      			rsb_Digito_decena=RESETVALUE;
      			rsb_Digito_centena=RESETVALUE;


	    		if(rsb_SecondCount>=2)
	 			{
	   				break;
	 			}
	    		}
  		while(GPIO_ReadPinInput(BUTTON_RESET, BUTTON_RESET_PIN)==TRUE && rsb_SecondCount>=2u)
	   		{
	   		    rsb_Type_Mode++;
	   		    if(rsb_Type_Mode>2)
	   		    {
    		    	rsb_Type_Mode=0;
	   		    }
    		    break;
	   		}
   		//PRINTF("%i",rsb_Type_Mode);
   		rsb_SecondCount=RESETVALUE;
   		rsb_DecadeSecondCount=RESETVALUE;
   		rsb_HundredSecondCount=RESETVALUE;
  		rsb_Digito_unidad=RESETVALUE;
  		rsb_Digito_decena=RESETVALUE;
  		rsb_Digito_centena=RESETVALUE;
}

void ContadorManual(void)
{
	Compare();
	rsb_SecondCount=0;
	rsl_milisecondCounts=0;
	    	if(GPIO_ReadPinInput(BUTTON_PLUS, BUTTON_PLUS_PIN)==TRUE)
	    	{
	    		while(GPIO_ReadPinInput(BUTTON_PLUS, BUTTON_PLUS_PIN)==TRUE)
	    		{
	    			//wait
	    							Cuenta_Mode();

	    				    		if(rsb_SecondCount>=1)
	    				 			{
	    				   				break;

	    				 			}
	    		}
//	    		while(GPIO_ReadPinInput(BUTTON_PLUS, BUTTON_PLUS_PIN)==TRUE && rsb_SecondCount>=1u)
//	    			    		{
//	    			rsb_Digito_unidad++;
//	    			break;
//	    			    		}
	    		rsb_Digito_unidad++;
	    	}

	    	if(GPIO_ReadPinInput(BUTTON_SUB, BUTTON_SUB_PIN)==TRUE)
	    	{
	    		while(GPIO_ReadPinInput(BUTTON_SUB, BUTTON_SUB_PIN)==TRUE)
	    		{
	    			//wait
	    							Cuenta_Mode();

	    				    		if(rsb_SecondCount>=1)
	    				 			{
	    				    			rsb_SecondCount=0;
	    				   				break;
	    				 			}
	    		}
//	    		while(GPIO_ReadPinInput(BUTTON_SUB, BUTTON_SUB_PIN)==TRUE && SecondCount>=1u)
//	    			    		{
//	    			rsb_Digito_unidad--;
//	    			break;
//	    			    		}
	    		rsb_Digito_unidad--;
	    	}
}
void ADC(void)
{

    rsl_valor=(uint32_t)ADC16_GetChannelConversionValue(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP)*.080566;

    rsb_Digito_centena=(uint32_t)rsl_valor/100;
    rsb_Digito_decena=(uint32_t)((rsl_valor-(rsb_Digito_centena*100))/10);
    rsb_Digito_unidad=(uint32_t)(rsl_valor-(rsb_Digito_centena*100)-(rsb_Digito_decena*10));

}
void BOARD_TPM_HANDLER(void)
{
    /* Clear interrupt flag.*/
	Display ();
    TPM_ClearStatusFlags(BOARD_TPM, kTPM_TimeOverflowFlag);
    rbi_tpmIsrFlag = TRUE;
}

