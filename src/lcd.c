/*********************************************************************************

Copyright (c) 2013-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/******************************************************************************
 * @file:    lcd.c
 * @brief:   LCD API implementation file.
 * @version: $Revision: 28525 $
 * @date:    $Date: 2014-11-12 14:51:26 -0500 (Wed, 12 Nov 2014) $
******************************************************************************/

 /*! \addtogroup LCD_Driver LCD Driver
 *  @{
 */

/*! \cond PRIVATE */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /* Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stddef.h>		/* for 'NULL' */
#include <string.h>		/* for strlen */

#include "lcd.h"

/* forward declarations... */
/* These are needed in the case where ADI_CFG_ENABLE_RTOS_SUPPORT=1              */
/* The ADI_INSTALL_HANDLER macro will expand to a call to adi_int_InstallHandler */
/* and the handler will be referenced                                            */
/* The non-OSAL expansion of ADI_INSTALL_HANDLER results in a NULL string        */
ADI_INT_HANDLER(LCD_Int_Handler);

/*!
 *
 *****************************************************************************
 *  LCD driver state.
 *****************************************************************************/
typedef enum
{
    ADI_LCD_STATE_UNINITIALIZED = 0,  /*!< LCD is not initialized. */
    ADI_LCD_STATE_INITIALIZED,        /*!< LCD is initialized.  */
} ADI_LCD_STATE_TYPE;


/*!
 *
 * LCD Device instance data structure */
typedef struct ADI_LCD_DEV_DATA_TYPE
{
    ADI_LCD_STATE_TYPE         initState;             /*!< Device initialization state        */
    ADI_LCD_TypeDef*           pLCD;                  /*!< LCD  MMRs                          */
    bool_t                     bBlockingMode;         /*!< Blocking-Mode flag                 */
    IRQn_Type                  LCDIntID;              /*!< LCD interrupt ID.                  */
    ADI_CALLBACK               pfCallback;            /*!< Pointer to callback funcion.       */
    void *                     pCBParam;              /*!< Callback parameter.                */

} ADI_LCD_DEV_DATA_TYPE;


/*!
 * only one instance... */
static ADI_LCD_DEV_DATA_TYPE LCD_DevData[ADI_LCD_MAX_DEVID] =
{
    /* Instance 0 intialization */
    {
        ADI_LCD_STATE_UNINITIALIZED,    /* Init state */
        pADI_LCD,                       /* Pointer to the LCD MMR registers */
        LCD_IRQn,                       /* LCD Interrupt Number */
        NULL,                           /* Pointer to callback function */
        NULL,                           /* Callback parameter */
    }

    /* Add more instances here */

};

/*!
 * device data pointer for instance 0*/
static ADI_LCD_DEV_DATA_TYPE* const pLCD0_DevData = &LCD_DevData[0];


/*!
 *****************************************************************************
 * \struct ADI_LCD_CFG_TYPE
 *
 * LCD Configuration structure
 *****************************************************************************/
typedef struct
{
   uint16_t LCDCON;         /*!< LCD Configuration register */
   uint16_t LCDBLINK;       /*!< LCD Blink control register */
   uint16_t LCDCONTRAST;    /*!< LCD Contrast register */

} ADI_LCD_CFG_TYPE;



/*! LCD configuration arry for all the instances */
static const ADI_LCD_CFG_TYPE  gLCDCfg[ADI_LCD_MAX_DEVID] =
{

  /* LCD instance 0 configuration */
  {
    /**** LCDCON Register *****/

    (( LCD0_CFG_MUX_TYPE                        << BITP_LCD_LCDCON_LCDMUX       )|
     ( LCD0_CFG_SCREEN_SELECT                   << BITP_LCD_LCDCON_SCREENSEL    )|
     ( LCD0_CFG_FRAME_RATE                      << BITP_LCD_LCDCON_FRAMESEL     )|
     ( LCD0_CFG_FRAME_INVERT                    << BITP_LCD_LCDCON_FRAMEINV     )|
     ( LCD0_CFG_DATA_RESET                      << BITP_LCD_LCDCON_LCDRST       )|
     ( LCD0_CFG_ENABLE_FRAME_BOUNARY_INT        << BITP_LCD_LCDCON_FRAMEINT_EN  )|
     ( LCD0_CFG_ENABLE_CHARGE_PUMP_INT          << BITP_LCD_LCDCON_CPINT_EN     )|
     ( LCD0_CFG_ENABLE_BLINK                    << BITP_LCD_LCDCON_BLINKEN      )),

    /**** LCDBLINK register ****/

    (( LCD0_CFG_BLINK_MODE                      << BITP_LCD_LCDBLINK_BLKMOD     )|
     ( LCD0_CFG_BLINK_FREQ                      << BITP_LCD_LCDBLINK_BLKFREQ    )|
     ( LCD0_CFG_AUTOSWITCH                      << BITP_LCD_LCDBLINK_AUTOSWITCH )),

    /**** LCDCONTRAST register ****/

    (( LCD0_CFG_BIAS_LEVEL                      << BITP_LCD_LCDCONTRAST_BIASLVL )|
     ( LCD0_CFG_ENABLE_CHARGE_PUMP              << BITP_LCD_LCDCONTRAST_CP_EN   )|
     ( LCD0_CFG_POWER_DOWN_CHARGE_PUMP          << BITP_LCD_LCDCONTRAST_CP_PD   ))
  }
};


/************** Local APIs *******************************/
static void CommonIntHandler(ADI_LCD_DEV_DATA_TYPE* pDevData);
#ifdef ADI_DEBUG
static ADI_LCD_RESULT_TYPE ValidateDeviceHandle(ADI_LCD_DEV_HANDLE const hDevice);
#endif /* ADI_DEBUG */

/*! \endcond */

/*
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////   API IMPLEMENTATIONS   ///////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/


/*!
 * @brief  Initialize and allocate the LCD device.
 *
 * @param[in]        devID      Zero-based device index designating which device to initialize.
 * @param[out]       phDevice   The caller's device handle pointer for storing the initialized device instance data.
 *
 * @return            Status
 *                    - #ADI_LCD_ERR_ALREADY_INITIALIZED [D]    Specified device has already been initialized.
 *                    - #ADI_LCD_ERR_BAD_DEVICE_INDEX [D]       Invalid device index.
 *                    - #ADI_LCD_SUCCESS                        Call completed successfully.
 *
 * Initialize an LCD device and allocate the device for use.
 * The caller's device handle is written with the device instance data pointer.
 * The returned device handle is required to be passed to all subsequent APIs to convey which device instance to operate on.
 *
 * @note    The contents of phDevice will be set to NULL on failure.
 *
 * @sa      adi_LCD_UnInit()
 */
ADI_LCD_RESULT_TYPE adi_LCD_Init(ADI_LCD_DEV_ID_TYPE const devID, ADI_LCD_DEV_HANDLE* const phDevice)
{
    ADI_LCD_DEV_HANDLE hDevice;

#ifdef ADI_DEBUG
    if (devID >= ADI_LCD_MAX_DEVID)
        return ADI_LCD_ERR_BAD_DEVICE_INDEX;

    if (ADI_LCD_STATE_UNINITIALIZED != LCD_DevData[devID].initState)
        return ADI_LCD_ERR_ALREADY_INITIALIZED;
#endif

    /* set local device handle */
    hDevice = &LCD_DevData[devID];

    /* store a bad handle in case of failure */
    *phDevice = NULL;

    /* Reset the LCD data registers */
    hDevice->pLCD->LCDCON |= BITM_LCD_LCDCON_LCDRST;

	/*! LCD configuration arry for all the instances */
	ADI_LCD_CFG_TYPE const *pLCDCfg = &gLCDCfg[devID];

	/* Initialize the device based on the given configuration parameters */
	hDevice->pLCD->LCDCON         = pLCDCfg->LCDCON;
	hDevice->pLCD->LCDBLINK       = pLCDCfg->LCDBLINK;
	hDevice->pLCD->LCDCONTRAST    = pLCDCfg->LCDCONTRAST;

    /* Initalize device variables */
    hDevice->pfCallback = NULL;
    hDevice->pCBParam   = NULL;

    if(devID == 0)
    {
        /* Install the LCD interrupt handlers */
        ADI_INSTALL_HANDLER(ADI_INT_LCD, LCD_Int_Handler);
    }
    /*
        Add other instances here.
    */

    /* init successful */
    hDevice->initState = ADI_LCD_STATE_INITIALIZED;

    /* store the "device" handle last */
    *phDevice = hDevice;

    return ADI_LCD_SUCCESS;
}

/*!
 * @brief  Uninitialize and deallocate the LCD device.
 *
 * @param[in]   hDevice    Device handle obtained from adi_LCD_Init().
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 *
 * Uninitialize and release an allocated LCD device.
 *
 * @sa        adi_LCD_Init()
 */
ADI_LCD_RESULT_TYPE adi_LCD_UnInit(ADI_LCD_DEV_HANDLE const hDevice)
{
#ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    /* Disable the LCD interrupt */
    NVIC_DisableIRQ(hDevice->LCDIntID);

    /* UnInstall the LCD interrupt handlers */
    ADI_UNINSTALL_HANDLER(hDevice->LCDIntID);

    hDevice->initState = ADI_LCD_STATE_UNINITIALIZED;

    return ADI_LCD_SUCCESS;
}

/*
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////   CONFIGURATION APIS   ////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/

/*!
 * @brief  Sets the blink mode or disables it.
 *
 * The LCD blink/switch can be controlled by hardware (#ADI_LCD_BLINK_MODE_HARDWARE),
 * software (#ADI_LCD_BLINK_MODE_SOFTWARE) or disabled (ADI_LCD_BLINK_MODE_DISABLE).
 *\n
 * When blink is enabled (hardware/software controlled) and autoswitch is disabled
 * (using the API adi_LCD_SetAutoSwitch), the LCD controller will alternate between the
 * ON state and OFF state so that screen appears to blink.
 *
 * When blink is enabled (hardware/software controlled) and autoswitch is enabled the LCD
 * controller will alternate between screen 0 and screen 1.
 *
 * The blink can be automatically controlled using hardware at prefixed blink rates
 * or can be controlled by software.
 *
 * Hardware controlled Blink/Switch:
 *     To controll the blink/switch rate automatically using hardware call this API
 *     with BlinkMode set to ADI_LCD_BLINK_MODE_HARDWARE. Set the desired blink/switch
 *     frequency using API adi_LCD_SetBlinkFreq.
 *
 * Sotware controlled Blink/Switch:
 *     To controll the blink/switch rate automatically using hardware call this API
 *     with BlinkMode set to ADI_LCD_BLINK_MODE_SOFTWARE. Call the API adi_LCD_ToggleLCD
 *     to switch between screen 0 and screen 1 or between ON or OFF state.
 *
 * @param[in]   hDevice    Device handle obtained from adi_LCD_Init().
 * @param[in]   BlinkMode  Blink mode to be set.
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 *
 * @sa adi_LCD_SetBlinkFreq()
 * @sa adi_LCD_ToggleLCD()
 * @sa adi_LCD_SetAutoSwitch()
 */
ADI_LCD_RESULT_TYPE adi_LCD_SetBlinkMode(ADI_LCD_DEV_HANDLE const hDevice, ADI_LCD_BLINK_MODE_TYPE const BlinkMode)
{
    /* LCD MMR register value */
    uint16_t RegValue;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    /* Enable or disable blinking based on the chosen mode. If hardware or software mode is chosen enable blinking. */
    RegValue =  hDevice->pLCD->LCDCON;
    RegValue &= (~BITM_LCD_LCDCON_BLINKEN);
    RegValue |=  (BlinkMode == ADI_LCD_BLINK_MODE_DISABLE) ? 0 : BITM_LCD_LCDCON_BLINKEN;
    hDevice->pLCD->LCDCON = RegValue;

    /* Set the chosen hardware of software mode in blink control register. */
    if(ADI_LCD_BLINK_MODE_DISABLE != BlinkMode)
    {
        RegValue =  hDevice->pLCD->LCDBLINK;
        RegValue &=  (~BITM_LCD_LCDBLINK_BLKMOD);
        RegValue |=  BlinkMode;
        hDevice->pLCD->LCDBLINK = RegValue;
    }

    return ADI_LCD_SUCCESS;
}

/*!
 * @brief  Enables or disables the charge pump good interrupt.
 *
 * When enabled an interrupt is generated when the charge pump voltage falls below the
 * bias level for LCD contrast (set using API  #adi_LCD_SetContrast). Application
 * can get the charge pump good status using the API #adi_LCD_GetChargePumpGoodStatus.
 *
 * Application should enable the charge pump by calling the API #adi_LCD_EnableChargePump otherwise
 * nothing will be displayed on the LCD.
 *
 * @param[in]   hDevice    Device handle obtained from adi_LCD_Init().
 * @param[in]   bFlag      Flag wich indicates if charge pump good interrupt to be enabled or disabled.
 *                         'true'   -   Enable charge pump interrupt.
 *                         'false'  -   Disable charge pump interrupt.
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 *
 * @sa adi_LCD_SetContrast()
 * @sa adi_LCD_GetChargePumpGoodStatus()
 * @sa adi_LCD_EnableChargePump()
 */
ADI_LCD_RESULT_TYPE adi_LCD_SetChargePumpIntEnable(ADI_LCD_DEV_HANDLE const hDevice, const bool_t bFlag)
{
    /* LCD MMR register value */
    uint16_t RegValue;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    /* Enable or disable charge pump good interrupt. */
    RegValue =  hDevice->pLCD->LCDCON;
    RegValue &= (~BITM_LCD_LCDCON_CPINT_EN);
    RegValue |=  ((uint16_t) bFlag << BITP_LCD_LCDCON_CPINT_EN);
    hDevice->pLCD->LCDCON = RegValue;

    return ADI_LCD_SUCCESS;
}


/*!
 * @brief  Enables or disables the LCD frame boundary interrupt.
 *
 * When enabled, an interrupt is generated by the LCD controller just before the frame boundary
 * (after the data registers are accessed to display the last set of segments within a given frame).
 * The LCD frame boundary interrupt can be used to synchronize writing to the data registers
 * before the start of a new frame. Writing the data registers at the frame boundary allows a
 * crisp transition of the image displayed on the LCD.
 *
 * Alternatively application can use the API #adi_LCD_GetSafeToWriteStatus to query if it is
 * safe to update the LCD registers.
 *
 * @param[in]   hDevice    Device handle obtained from adi_LCD_Init().
 * @param[in]   bFlag      Flag wich indicates if LCD boundary interrupt to be enabled or disabled.
 *                         'true'   -   Enable the frame boundary interrupt.
 *                         'false'  -   Disable the frame boundary interrupt.
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 *
 * @sa adi_LCD_GetSafeToWriteStatus()
 */
ADI_LCD_RESULT_TYPE adi_LCD_SetFrameBoundaryIntEnable(ADI_LCD_DEV_HANDLE const hDevice, const bool_t bFlag)
{
    /* LCD MMR register value */
    uint16_t RegValue;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    /* Enable or disable frame boundary interrupt. */
    RegValue =  hDevice->pLCD->LCDCON;
    RegValue &= (~BITM_LCD_LCDCON_FRAMEINT_EN);
    RegValue |=  ((uint16_t) bFlag << BITP_LCD_LCDCON_FRAMEINT_EN);
    hDevice->pLCD->LCDCON = RegValue;

    return ADI_LCD_SUCCESS;
}


/*!
 * @brief  Enables or disables the frame inversion mode.
 *
 * Enabling frame inversion mode allows frames to be inverted every alternate
 * frame. If this mode is selected, the dc cancellation is done twice over two frames (once within
 * each frame and once frame-to-frame). This will be helpful to eliminate the dc offset that
 * occurs due to the difference in rise and fall times of the COM/SEG lines.
 *
 * @note This will effect only when in multiplex mode (set using API #adi_LCD_SetMuxType with ADI_LCD_MUX_4x).
 *       Enabling frame inversion mode will not have any effect when in static mode.
 *
 * @param[in]   hDevice    Device handle obtained from adi_LCD_Init().
 * @param[in]   bFlag      Flag wich indicates if frame inversion mode to be enabled or disabled.
 *                         'true'   -   Enable the frame inversion mode.
 *                         'false'  -   Disable the frame inversion mode.
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 */
ADI_LCD_RESULT_TYPE adi_LCD_SetFrameInversionModeEnable(ADI_LCD_DEV_HANDLE const hDevice, const bool_t bFlag)
{
    /* LCD MMR register value */
    uint16_t RegValue;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    /* Enable or disable frame inversion mode. */
    RegValue =  hDevice->pLCD->LCDCON;
    RegValue &= (~BITM_LCD_LCDCON_FRAMEINV);
    RegValue |=  ((uint16_t) bFlag << BITP_LCD_LCDCON_FRAMEINV);
    hDevice->pLCD->LCDCON = RegValue;

    return ADI_LCD_SUCCESS;
}

/*!
 * @brief  Sets the LCD frame refersh rate.
 *
 * An LCD segment acts like a capacitor that is charged and discharged at a certain rate. This
 * rate, called the refresh rate, determines the visual characteristics of the LCD. A slow refresh
 * rate results in flickering with the LCD blinking on and off between refreshes and is
 * disconcerting to the user. A fast refresh rate results in ghosting, where the LCD screen
 * appears to be continuously lit due to the segment not turning off. In addition, a faster refresh
 * rate consumes more power.
 *
 * @param[in]   hDevice    Device handle obtained from adi_LCD_Init().
 * @param[in]   FrameRate  Frame rate to be set.
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 */
ADI_LCD_RESULT_TYPE adi_LCD_SetFrameRate(ADI_LCD_DEV_HANDLE const hDevice, ADI_LCD_FRAME_RATE_TYPE const FrameRate)
{
    /* LCD MMR register value */
    uint16_t RegValue;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    /* Enable or disable frame inversion mode. */
    RegValue =  hDevice->pLCD->LCDCON;
    RegValue &= (~BITM_LCD_LCDCON_FRAMESEL);
    RegValue |=  ((uint16_t) FrameRate << BITP_LCD_LCDCON_FRAMESEL);
    hDevice->pLCD->LCDCON = RegValue;

    return ADI_LCD_SUCCESS;
}

/*!
 * @brief  Resets the LCD data registers.
 *
 * All the LCD data registers are set to 0.
 *
 * @param[in]   hDevice    Device handle obtained from adi_LCD_Init().
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 */
ADI_LCD_RESULT_TYPE adi_LCD_Reset(ADI_LCD_DEV_HANDLE const hDevice)
{
    /* LCD MMR register value */
    uint16_t RegValue;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    /* Enable or disable frame inversion mode. */
    RegValue =  hDevice->pLCD->LCDCON;
    RegValue |= BITM_LCD_LCDCON_LCDRST;  /* This bit is auto cleared so it is not clear before setting it. */
    hDevice->pLCD->LCDCON = RegValue;

    return ADI_LCD_SUCCESS;
}

/*!
 * @brief  Sets the screen to be displayed.
 *
 * Application can select the screen to be displayed or enable the Auto Switch so that the
 * LCD controller automatically switches between screen 0 and screen 1. The Auto Switch mode
 * can be enabled using the API #adi_LCD_SetAutoSwitch.
 *
 * @param[in]   hDevice    Device handle obtained from adi_LCD_Init().
 * @param[in]   Screen     Screen to be displayed.
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 */
ADI_LCD_RESULT_TYPE adi_LCD_SetScreenSelect(ADI_LCD_DEV_HANDLE const hDevice, ADI_LCD_SCREEN_TYPE const Screen)
{
    /* LCD MMR register value */
    uint16_t RegValue;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    /* Set the given frame for display. */
    RegValue =  hDevice->pLCD->LCDCON;
    RegValue &= (~BITM_LCD_LCDCON_SCREENSEL);
    RegValue |=  ((uint16_t) Screen << BITP_LCD_LCDCON_FRAMESEL);
    hDevice->pLCD->LCDCON = RegValue;

    return ADI_LCD_SUCCESS;
}

/*!
 * @brief  Sets the muxing type.
 *
 * The LCD controller can be configured to support two multiplex types.
 * 1. Static (only COM0 is used)
 * 2. 4 * multiplex (COM0 to COM3 are used)
 *
 * @param[in]   hDevice    Device handle obtained from adi_LCD_Init().
 * @param[in]   MuxType    Mux type to be set.
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 */
ADI_LCD_RESULT_TYPE adi_LCD_SetMuxType(ADI_LCD_DEV_HANDLE const hDevice, ADI_LCD_MUX_TYPE const MuxType)
{
    /* LCD MMR register value */
    uint16_t RegValue;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    /* Set the given mux type. */
    RegValue =  hDevice->pLCD->LCDCON;
    RegValue &= (~BITM_LCD_LCDCON_LCDMUX);
    RegValue |=  ((uint16_t) MuxType << BITP_LCD_LCDCON_LCDMUX);
    hDevice->pLCD->LCDCON = RegValue;

    return ADI_LCD_SUCCESS;
}

/*!
 * @brief  Enables or disables the LCD controller.
 *
 * Before enabling the LCD controller application should make sure that the LCD data registers are
 * written first. The LCD data registers can be written using the API #adi_LCD_SetDataReg
 *
 * @param[in]   hDevice    Device handle obtained from adi_LCD_Init().
 * @param[in]   bFlag      Flag which indicates if the LCD controller to be enabled or disabled.
 *                          'true'  -   Enable the LCD.
 *                          'false  -   Disable the LCD.
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 *
 * @sa adi_LCD_SetDataReg()
 */
ADI_LCD_RESULT_TYPE adi_LCD_Enable(ADI_LCD_DEV_HANDLE const hDevice, const bool_t bFlag)
{
    /* LCD MMR register value */
    uint16_t RegValue;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    /* Enable or disable the LCD. */
    RegValue =  hDevice->pLCD->LCDCON;
    RegValue &= (~BITM_LCD_LCDCON_LCDEN);
    RegValue |=  ((uint16_t) bFlag << BITP_LCD_LCDCON_LCDEN);
    hDevice->pLCD->LCDCON = RegValue;

    return ADI_LCD_SUCCESS;
}


/*!
 * @brief  Sets the LCD blink frequency.
 *
 * This sets the hardware controlled blink frequency. When using software based blinking, setting
 * the blink frequency will have no effect.
 *
 * Hardware based blink control can be enabled by using the API #adi_LCD_SetBlinkMode.
 *
 * @param[in]   hDevice    Device handle obtained from adi_LCD_Init().
 * @param[in]   BlinkFreq  Blink frequency to be set.
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 *
 * @sa adi_LCD_SetBlinkMode()
 */
ADI_LCD_RESULT_TYPE adi_LCD_SetBlinkFreq(ADI_LCD_DEV_HANDLE const hDevice, ADI_LCD_BLINK_FREQ_TYPE const BlinkFreq)
{
    /* LCD MMR register value */
    uint16_t RegValue;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    /* Set the given blink frequency. */
    RegValue =  hDevice->pLCD->LCDBLINK;
    RegValue &= (~BITM_LCD_LCDBLINK_BLKFREQ);
    RegValue |=  ((uint16_t) BlinkFreq << BITP_LCD_LCDBLINK_BLKFREQ);
    hDevice->pLCD->LCDBLINK = RegValue;

    return ADI_LCD_SUCCESS;
}

/*!
 * @brief  Enables or disables the LCD screen auto switch.
 *
 * When LCD blink is enabled and Auto switch is enabled the LCD will switch between Screen 0 and Screen 1.
 * When LCD blink is enabled and auto switch is disabled, the LCD will switch between ON state and
 * OFF state.
 *
 * The LCD blink, can be software controlled or hardware controlled. The blink control mode be set
 * using the API #adi_LCD_SetBlinkMode.
 *
 * @param[in]   hDevice     Device handle obtained from adi_LCD_Init().
 * @param[in]   bFlag       Flag which indicates whether the Auto Switch to be enabled or disabled.
 *                          'true'  -   Enable auto switch.
 *                          'false' -   Disable auto switch.
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 *
 * @sa adi_LCD_SetBlinkMode()
 */
ADI_LCD_RESULT_TYPE adi_LCD_SetAutoSwitch(ADI_LCD_DEV_HANDLE const hDevice, const bool_t bFlag)
{
    /* LCD MMR register value */
    uint16_t RegValue;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    /* Enable or disable the auto switch. */
    RegValue =  hDevice->pLCD->LCDBLINK;
    RegValue &= (~BITM_LCD_LCDBLINK_AUTOSWITCH);
    RegValue |=  ((uint16_t) bFlag << BITP_LCD_LCDBLINK_AUTOSWITCH);
    hDevice->pLCD->LCDBLINK = RegValue;

    return ADI_LCD_SUCCESS;
}


/*!
 * @brief  Sets the LCD contrast.
 *
 * The LCD contrast can be changed by software by setting the bias level The bias level.
 * The voltage step for the bias level is 38.7 mV (approximately 40 mV). The
 * bias voltage for a given BIASLVL value is given by the following equation:
 * Bias Voltage = 2.4 V + (0.0387) * BIASLVL.
 *
 * After setting the bias level application should make sure that the charge pump voltage reaches
 * the required bias voltage. This can be done by initiating a timeout for 62.5 ms using any of the GP Timers.
 * After receiving the timeout interrupt, the application should check if Charge Pump Good status is set (
 * use the API #adi_LCD_GetChargePumpGoodStatus ) . If the API returns false, there is an error in charge pump
 * or the LCD glass. In this case, nothing can be displayed on LCD.
 *
 * @note This process should be repeated every time the bias level is changed. Also during the startup when the
 *       charge pump is enabled (using API adi_LCD_EnableChargePump).
 *
 * @param[in]   hDevice     Device handle obtained from adi_LCD_Init().
 * @param[in]   BiasLevel   Bias level to be set for changing the LCD contrast. The value can range
 *                          between 0 to 31.
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_ERR_BAD_BIAS_LEVEL    [D]  The given bias level is outside the supported range.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 *
 * @sa adi_LCD_GetChargePumpGoodStatus()
 * @sa adi_LCD_EnableChargePump()
 */
ADI_LCD_RESULT_TYPE adi_LCD_SetContrast(ADI_LCD_DEV_HANDLE const hDevice, uint8_t const BiasLevel)
{
    /* LCD MMR register value */
    uint16_t RegValue;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;

    if(BiasLevel > ADI_LCD_MAX_BIAS_LEVEL)
        return ADI_LCD_ERR_BAD_BIAS_LEVEL;
#endif

    /* Set the given contrast level. */
    RegValue =  hDevice->pLCD->LCDCONTRAST;
    RegValue &= (~BITM_LCD_LCDCONTRAST_BIASLVL);
    RegValue |=  ((uint16_t) BiasLevel << BITP_LCD_LCDCONTRAST_BIASLVL);
    hDevice->pLCD->LCDCONTRAST = RegValue;

    return ADI_LCD_SUCCESS;
}


/*!
 * @brief  Enables and disables the charge pump.
 *
 * The LCD waveform voltages are generated using an internal charge pump circuitry and can
 * support bias levels from 2.4 V up to 3.6 V LCDs. Unless the charge pump is enabled nothing will be
 * displayed on the LCD.
 *
 * Application should check the charge pump level is good after enabling the charge pump. The
 * charge pump good status can be queried using the API #adi_LCD_GetChargePumpGoodStatus.
 * The charge pump good status should be checked after 62.5 ms after enabling it. A GP timer can
 * be used to initiate a timeout for 62.5 ms.
 *
 * @param[in]   hDevice     Device handle obtained from adi_LCD_Init().
 * @param[in]   bFlag       Flag which indicates if charge pump to be enabled or disabled.
 *                          'true'   - Enable charge pump.
 *                          'false'  - Disable charge pump.
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 *
 * @sa adi_LCD_GetChargePumpGoodStatus()
 * @sa adi_LCD_SetContrast()
 */
ADI_LCD_RESULT_TYPE adi_LCD_EnableChargePump(ADI_LCD_DEV_HANDLE const hDevice, const bool_t bFlag)
{
    /* LCD MMR register value */
    uint16_t RegValue;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    /* Enable or disable the auto switch. */
    RegValue =  hDevice->pLCD->LCDCONTRAST;
    RegValue &= ~(BITM_LCD_LCDCONTRAST_CP_PD | BITM_LCD_LCDCONTRAST_CP_EN);

    if(bFlag == true)
    {
        /* To enable CP_PD bit should be set to 0 and CP_EN bit should be set to 1 */
        RegValue |=  (1 << BITP_LCD_LCDCONTRAST_CP_EN);
    }
    else
    {
        /* To disable CP_PD bit should be set to 1 and CP_EN bit should be set to 0 */
        RegValue |=  (1 << BITP_LCD_LCDCONTRAST_CP_PD);
    }

    hDevice->pLCD->LCDCONTRAST = RegValue;

    return ADI_LCD_SUCCESS;
}

/*!
 * @brief  Sets the data registers for the given screen.
 *
 * Application should make sure that the LCD data registers for the selected screen are set, before
 * enabling the LCD. The LCD data registers should be updated at the frame boundary for a crisp
 * transition between the change. Application can either use the Frame Boundary interrupt
 * (can be enabled using adi_LCD_SetFrameBoundaryIntEnable) or use the API
 * #adi_LCD_GetSafeToWriteStatus to query if it is safe to update the LCD registers.
 *
 * @param[in]   hDevice     Device handle obtained from adi_LCD_Init().
 * @param[in]   eScreen     LCD screen number whose data registers to be updated.
 * @param[in]   pDataTable  Pointer to the table which holds the register number and data value to be written.
 * @param[in]   NumEntries Number of entries in the table.
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_ERR_NULL_POINTER      [D]  The given pointer to the data table is pointing to NULL.
 *                - #ADI_LCD_ERR_BAD_REGISTER_NUM  [D]  The given LCD data register number is inavlid.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 *
 * @sa adi_LCD_GetChargePumpGoodStatus()
 * @sa adi_LCD_SetContrast()
 */
ADI_LCD_RESULT_TYPE adi_LCD_SetDataReg(ADI_LCD_DEV_HANDLE const hDevice,  ADI_LCD_SCREEN_TYPE   const eScreen, ADI_LCD_DATA_TYPE * pDataTable, uint32_t const NumEntries)
{
   uint32_t     i;
   ADI_LCD_LCDDATA_P0_TypeDef     *pDataReg;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;

    if(pDataTable == NULL)
        return ADI_LCD_ERR_NULL_POINTER;

#endif

    /* Get the pointer to screen registers */
    pDataReg    =   (eScreen == ADI_LCD_SCREEN_0) ? hDevice->pLCD->LCDDATA_P0 : (ADI_LCD_LCDDATA_P0_TypeDef  *) hDevice->pLCD->LCDDATA_P1;

    for(i = 0; i < NumEntries; i++)
    {
 #ifdef ADI_DEBUG
        if(pDataTable->LCDRegNumber > ADI_LCD_NUM_LCD_DATA_REG)
            return ADI_LCD_ERR_BAD_REGISTER_NUM;
#endif /* ADI_DEBUG */

         pDataReg[pDataTable->LCDRegNumber].LCDDATA_S0 = pDataTable->RegValue;

         /* Move to the next table entry */
         pDataTable++;
    }

    return ADI_LCD_SUCCESS;
}


/*!
 * @brief  Gets the contents of the data registers for the given screen.
 *
 * @param[in]   hDevice     Device handle obtained from adi_LCD_Init().
 * @param[in]   eScreen     LCD screen number whose data registers to be queried.
 * @param[in]   pDataTable  Pointer to the table which holds the register number to be read and data where the register value will be written.
 * @param[in]   NumEntries  Number of entries in the table.
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_ERR_NULL_POINTER      [D]  The given pointer to the data table is pointing to NULL.
 *                - #ADI_LCD_ERR_BAD_REGISTER_NUM  [D]  The given LCD data register number is inavlid.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 *
 * @sa adi_LCD_GetChargePumpGoodStatus()
 * @sa adi_LCD_SetContrast()
 * @sa adi_LCD_SetDataReg()
 */
ADI_LCD_RESULT_TYPE adi_LCD_GetDataReg(ADI_LCD_DEV_HANDLE const hDevice,  ADI_LCD_SCREEN_TYPE   const eScreen, ADI_LCD_DATA_TYPE * pDataTable, uint32_t const NumEntries)
{
   uint32_t     i;
   ADI_LCD_LCDDATA_P0_TypeDef     *pDataReg;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;

    if(pDataTable == NULL)
        return ADI_LCD_ERR_NULL_POINTER;

#endif

    /* Get the pointer to screen registers */
    pDataReg    =   (eScreen == ADI_LCD_SCREEN_0) ? hDevice->pLCD->LCDDATA_P0 : (ADI_LCD_LCDDATA_P0_TypeDef  *) hDevice->pLCD->LCDDATA_P1;

    for(i = 0; i < NumEntries; i++)
    {
 #ifdef ADI_DEBUG
        if(pDataTable->LCDRegNumber > ADI_LCD_NUM_LCD_DATA_REG)
            return ADI_LCD_ERR_BAD_REGISTER_NUM;
#endif /* ADI_DEBUG */

         pDataTable->RegValue = pDataReg[pDataTable->LCDRegNumber].LCDDATA_S0;

         /* Move to the next table entry */
         pDataTable++;
    }

    return ADI_LCD_SUCCESS;
}


/*!
 * @brief  Toggles the LCD display.
 *
 * Toggles the LCD between ON/OFF state or between Screen 1 or Screen 2. This API is useful in software
 * controlled blink mode. Depending upon the auto switch settings (set using the API #adi_LCD_SetAutoSwitch)
 * the LCD will toggle between ON/OFF or Screen 1/Screen 2. Hardware and Software based blink mode can be
 * set using the API #adi_LCD_SetBlinkMode.
 *
 * @note This API will not have any effect if hardware based blink mode is chosen.
 *
 * @param[in]   hDevice     Device handle obtained from adi_LCD_Init().
 *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 *
 * @sa adi_LCD_SetAutoSwitch()
 * @sa adi_LCD_SetBlinkMode()
 */
ADI_LCD_RESULT_TYPE adi_LCD_ToggleLCD(ADI_LCD_DEV_HANDLE const hDevice)
{

    /* LCD MMR register value */
    uint16_t RegValue;

 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    /* Toggle the LCD. */
    RegValue =  hDevice->pLCD->LCDBLINK;
    RegValue ^= 1;                          /* Flip the Blink mode bit to toggle the LCD */
    hDevice->pLCD->LCDBLINK = RegValue;

    return ADI_LCD_SUCCESS;
}

/*
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////   Status Query APIS   ////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/

/*!
 * @brief  Gets the idle state of the LCD.
 *
 * Checks if LCD is actively displaying or in idle state.
 *
 * @param[in]   hDevice     Device handle obtained from adi_LCD_Init().
 * @param[out]  pStatus     Pointer to the status flag where the LCD status is written.
 *                          'true'  -   is written if LCD is in idle state.
 *                          'false' -   is written if LCD is actively displaying. *
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NULL_POINTER      [D]  The given pointer to status flag is pointing to NULL.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 */
ADI_LCD_RESULT_TYPE adi_LCD_GetIdleStatus(ADI_LCD_DEV_HANDLE const hDevice, bool_t * const pStatus)
{
 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;

    if(NULL == pStatus)
        return ADI_LCD_ERR_NULL_POINTER;
#endif

     /* Query the Idle status bit */
    *pStatus = ((hDevice->pLCD->LCDSTAT & BITM_LCD_LCDSTAT_LCD_IDLE) == 0) ? false : true ;

    return ADI_LCD_SUCCESS;
}


/*!
 * @brief  Gets the VLCD OK state of the LCD.
 *
 * Gets the state of the VLCD on the Reserviour Capacitor is equal to the Bias Level.
 *
 * @param[in]   hDevice     Device handle obtained from adi_LCD_Init().
 * @param[out]  pStatus     Pointer to the status flag where the VLCD OK status is written.
 *                          'true'  -   is written if VLCD on reservoir capacitor is equal to BIAS level.
 *                          'false' -   is written if VLCD on reservoir capacitor is not equal to BIAS level.
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NULL_POINTER      [D]  The given pointer to status flag is pointing to NULL.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 */
ADI_LCD_RESULT_TYPE adi_LCD_GetVLCDStatus(ADI_LCD_DEV_HANDLE const hDevice, bool_t * const pStatus)
{
 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;

    if(NULL == pStatus)
        return ADI_LCD_ERR_NULL_POINTER;
#endif

     /* Query the VLCD_OK status bit */
    *pStatus = ((hDevice->pLCD->LCDSTAT & BITM_LCD_LCDSTAT_VLCD_OK) == 0) ? false : true ;

    return ADI_LCD_SUCCESS;
}


/*!
 * @brief  Gets the charge pump good status.
 *
 * After changing the Bias level (by using API #adi_LCD_SetContrast) or after enabling the charge pump
 * (by using API #adi_LCD_EnableChargePump) application should check the charge pump status. The
 * charge pump status should be checked after 62.5 ms starting from changing the bias level or
 * enabing the charge pump.
 *
 * If the Charge Pump Good status is false after 62.5 ms, there is an error in charge pump or the LCD glass.
 * In this case, nothing can be displayed on LCD.
 *
 * @param[in]   hDevice     Device handle obtained from adi_LCD_Init().
 * @param[out]  pStatus     Pointer to the status flag where the Charge Pump good status is written.
 *                          'true'  -   is written if charge pump good is set.
 *                          'false' -   is written if charge pump good is not set.
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NULL_POINTER      [D]  The given pointer to status flag is pointing to NULL.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 */
ADI_LCD_RESULT_TYPE adi_LCD_GetChargePumpGoodStatus(ADI_LCD_DEV_HANDLE const hDevice, bool_t * const pStatus)
{
 #ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;

    if(NULL == pStatus)
        return ADI_LCD_ERR_NULL_POINTER;
#endif

     /* Query the Charge pump good status bit */
    *pStatus = ((hDevice->pLCD->LCDSTAT & BITM_LCD_LCDSTAT_CP_GD) == 0) ? false : true ;

    return ADI_LCD_SUCCESS;
}

/*!
 * @brief  Gets the safe to write status.
 *
 * Application should update the LCD data registers at the LCD frame boundary to make sure
 * a crisp transition. Application can make use of the frame boundary interrupt or use this
 * API to know if it is safe to update the LCD data registers.
 *
 * @param[in]   hDevice     Device handle obtained from adi_LCD_Init().
 * @param[out]  pStatus     Pointer to the status flag where the safe to write status is written.
 *                          'true'  -   is written if it is safe to write.
 *                          'false' -   is written if it is not safe to write.
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NULL_POINTER      [D]  The given pointer to status flag is pointing to NULL.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 */
ADI_LCD_RESULT_TYPE adi_LCD_GetSafeToWriteStatus(ADI_LCD_DEV_HANDLE const hDevice, bool_t * const pStatus)

{
#ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;

    if(NULL == pStatus)
        return ADI_LCD_ERR_NULL_POINTER;
#endif

     /* Query the safe to write status bit */
    *pStatus = ((hDevice->pLCD->LCDSTAT & BITM_LCD_LCDSTAT_SAFE_TO_WR) == 0) ? false : true ;

    return ADI_LCD_SUCCESS;
}

/*!
 * @brief  Register or unregister callback function.
 *
 * Application can register a callback function with the driver which will be called back to notify
 * events when an interrupt occurs. For example when a frame boundary interrupt occurs, it will be
 * notified to the application via the registered callback with the event type #ADI_LCD_EVENT_FRAME_BOUNDARY.
 *
 * @param[in]   hDevice     Device handle obtained from adi_LCD_Init().
 * @param[in]   pfCallback  Pointer to the callback function. It can be set to NULL to unregister a previously
 *                          registered callback.
 * @param[in]   pCBParam    Callback parameter which will be passed back to the application when the callback
 *                          is called.
 * @return      Status
 *                - #ADI_LCD_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCD_ERR_NULL_POINTER      [D]  The given pointer to status flag is pointing to NULL.
 *                - #ADI_LCD_SUCCESS                    Call completed successfully.
 */
ADI_LCD_RESULT_TYPE adi_LCD_RegisterCallback(ADI_LCD_DEV_HANDLE const hDevice, ADI_CALLBACK pfCallback, void *pCBParam)
{

#ifdef ADI_DEBUG
    if (ADI_LCD_SUCCESS != ValidateDeviceHandle(hDevice))
        return ADI_LCD_ERR_BAD_DEVICE_HANDLE;

    if (ADI_LCD_STATE_UNINITIALIZED == hDevice->initState)
        return ADI_LCD_ERR_NOT_INITIALIZED;
#endif

    ADI_ENTER_CRITICAL_REGION();

    /* Save the given callback and callback parameters */
    hDevice->pfCallback = pfCallback;
    hDevice->pCBParam   = pCBParam;

    ADI_EXIT_CRITICAL_REGION();

    return ADI_LCD_SUCCESS;
}

/*
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////   Local APIs  /////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
*/

#ifdef ADI_DEBUG
/* Validates the given device handle */
static ADI_LCD_RESULT_TYPE ValidateDeviceHandle(ADI_LCD_DEV_HANDLE const hDevice)
{
    uint32_t i;

    /* Search through the device instances and find if any of the device instance
       matches the given device handle */
    for(i =0; i < ADI_LCD_MAX_DEVID; i++)
    {
        /* If found a match return success */
        if(&LCD_DevData[i] == hDevice)
        {
            return ADI_LCD_SUCCESS;
        }
    }

    /* Did not find any match return failure */
    return ADI_LCD_ERR_BAD_DEVICE_HANDLE;
}
#endif /* ADI_DEBUG */


/*!
 * @brief  LCD interrupt handler.
 *
 * @return void.
 *
 * Override of default (WEAK) LCD interrupt handler.
 */

ADI_INT_HANDLER(LCD_Int_Handler)
{
    CommonIntHandler(pLCD0_DevData);
}

/* Common interrupt handler */
static void CommonIntHandler(ADI_LCD_DEV_DATA_TYPE* pDevData)
{
    uint16_t Status = pDevData->pLCD->LCDSTAT;
    ADI_LCD_EVENT_TYPE eEvent ;


    /* Clear the events */
    pDevData->pLCD->LCDSTAT = Status;

    /* Check what is the cause for interrupt */
    if(Status & BITM_LCD_LCDSTAT_FRAMEINT)
    {
        eEvent = ADI_LCD_EVENT_FRAME_BOUNDARY;
    }
    else if (Status & BITM_LCD_LCDSTAT_CP_GD)
    {
        eEvent = ADI_LCD_EVENT_CP_GD;
    }

    /* If a callback is registered report the events via the callback */
    if(pDevData->pfCallback != NULL)
    {
        pDevData->pfCallback(pDevData->pCBParam, (uint32_t) eEvent, NULL);
    }

    return;
}

/*
** EOF
*/

/*@}*/

#define REVERT_ADI_MISRA_SUPPRESSIONS  /* Revert ADI MISRA Suppressions */
#include "misra.h"

