/*********************************************************************************

Copyright (c) 2013-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement (See /LICENSES/ADI.LABLAB.LICENSE). 

Modified from the Analog Devices original.
Portions Copyright (c) 2018 Andrew Grosser, Moritz von Buttlar.

*********************************************************************************/

/******************************************************************************
 * @file:    lcd_VIM828.c
 * @brief:   VIM-828 LCD API implementation file.
 * @version: $Revision: 28570 $
 * @date:    $Date: 2014-11-14 12:55:46 -0500 (Fri, 14 Nov 2014) $
******************************************************************************/

/*! \addtogroup LCD_Driver LCD Driver
 *  @{
 */

/*! \cond PRIVATE */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /* Apply ADI MISRA Suppressions */
#include "misra.h" 

#include "device.h"
#include "lcd.h"
#include "lcd/lcd_VIM828.h"

   
/*! Encodes the register values to be written for the given charecter in to one value to save table space.*/
#define LCD_VIM828_CHAR(a,b,c,d)      ( (a << 12) | (b << 8) | (c << 4) | d )

/*! Gets back the register1 value */
#define LCD_VIM828_CHAR_REG1_VALUE(Value, Pos)  ((Pos & 0xF0) ? ((Pos & 0xF) ? (Value >> 8) : (Value & 0xFF00)) : ((Pos & 0xF) ? (Value << 8) : (Value & 0xFF)))

/*! Decimal value for the charecter 'A' */
#define LCDVIM828_ASCII_UPPERCASE_A_VALUE   (65u)

/*! Decimal value for the charecter '0' */
#define LCDVIM828_ASCII_ZERO_VALUE          (48u)

/*! Decimal value for decimal point' */
#define LCDVIM828_ASCII_DP_VALUE            (46u)

/*! Decimal value for minus sign' */
#define LCDVIM828_ASCII_MINUS_VALUE         (45u)

/*! Decimal value for space */
#define LCDVIM828_ASCII_SPACE_VALUE         (32u)

/*! Offset for upper case 'A' in the mapping table*/
#define LCDVIM828_UPPERCASE_A_OFFSET        (10u)

/*! Offset for decimal point in the mapping table*/
#define LCDVIM828_DP_OFFSET                 (36u)

/*! Offset for minus sign in the mapping table*/
#define LCDVIM828_MINUS_OFFSET              (37u)

/*! Offset for space in the mapping table*/
#define LCDVIM828_SPACE_OFFSET              (38u)

/*! Value to be subtracted from ASCII value to get the table index for numbers */
#define LCDVIM828_ASCII_NUMBER_TO_INDEX     (LCDVIM828_ASCII_ZERO_VALUE)

/*! Value to be subtracted from ASCII value to get the table index for upper case letters */
#define LCDVIM828_UPPERCASE_ASCII_TO_INDEX  (LCDVIM828_ASCII_UPPERCASE_A_VALUE - LCDVIM828_UPPERCASE_A_OFFSET)

/*! Value to be subtracted from ASCII value to get the table index for decimal point */
#define LCDVIM828_DP_ASCII_TO_INDEX         (LCDVIM828_ASCII_DP_VALUE - LCDVIM828_DP_OFFSET)

/*! Value to be subtracted from ASCII value to get the table index for minus sign */
#define LCDVIM828_MINUS_ASCII_TO_INDEX      (LCDVIM828_ASCII_MINUS_VALUE - LCDVIM828_MINUS_OFFSET)

/*! Value to be subtracted from ASCII value to get the table index for space */
#define LCDVIM828_SPACE_ASCII_TO_INDEX      (LCDVIM828_ASCII_SPACE_VALUE - LCDVIM828_SPACE_OFFSET)

/*! Character to register setting mapping table for the VIM-828 14-Segment display */
static const uint16_t BigChar12Map[] = {
    /* Numbers */
    LCD_VIM828_CHAR(0x1,0x7,0x8,0xE),     /* 0 */
    LCD_VIM828_CHAR(0x0,0x6,0x0,0x0),     /* 1 */
    LCD_VIM828_CHAR(0x2,0x3,0x4,0xC),     /* 2 */
    LCD_VIM828_CHAR(0x2,0x7,0x4,0x8),     /* 3 */
    LCD_VIM828_CHAR(0x2,0x6,0x4,0x2),     /* 4 */
    LCD_VIM828_CHAR(0x1,0x5,0x4,0x8),     /* 5 */
    LCD_VIM828_CHAR(0x2,0x5,0x4,0xE),     /* 6 */
    LCD_VIM828_CHAR(0x0,0x7,0x0,0x0),     /* 7 */
    LCD_VIM828_CHAR(0x2,0x7,0x4,0xE),     /* 8 */
    LCD_VIM828_CHAR(0x2,0x7,0x4,0xA),     /* 9 */
    /* Upper case letters */ 
    LCD_VIM828_CHAR(0x2,0x7,0x4,0x6),     /* A */
    LCD_VIM828_CHAR(0x8,0x7,0x5,0x8),     /* B */
    LCD_VIM828_CHAR(0x0,0x1,0x0,0xE),     /* C */
    LCD_VIM828_CHAR(0x8,0x7,0x1,0x8),     /* D */
    LCD_VIM828_CHAR(0x2,0x1,0x0,0xE),     /* E */
    LCD_VIM828_CHAR(0x2,0x1,0x0,0x6),     /* F */
    LCD_VIM828_CHAR(0x0,0x5,0x4,0xE),     /* G */
    LCD_VIM828_CHAR(0x2,0x6,0x4,0x6),     /* H */
    LCD_VIM828_CHAR(0x8,0x1,0x1,0x8),     /* I */
    LCD_VIM828_CHAR(0x8,0x1,0x1,0xC),     /* J */
    LCD_VIM828_CHAR(0x2,0x0,0xA,0x6),     /* K */
    LCD_VIM828_CHAR(0x0,0x0,0x0,0xE),     /* L */
    LCD_VIM828_CHAR(0x1,0x6,0x2,0x6),     /* M */
    LCD_VIM828_CHAR(0x1,0x6,0x8,0x6),     /* N */
    LCD_VIM828_CHAR(0x0,0x7,0x0,0xE),     /* O */
    LCD_VIM828_CHAR(0x2,0x3,0x4,0x6),     /* P */
    LCD_VIM828_CHAR(0x0,0x7,0x8,0xE),     /* Q */
    LCD_VIM828_CHAR(0x2,0x3,0xC,0x6),     /* R */
    LCD_VIM828_CHAR(0x2,0x5,0x4,0xA),     /* S */
    LCD_VIM828_CHAR(0x8,0x1,0x1,0x0),     /* T */
    LCD_VIM828_CHAR(0x0,0x6,0x0,0xE),     /* U */
    LCD_VIM828_CHAR(0x4,0x0,0x2,0x6),     /* V */
    LCD_VIM828_CHAR(0x4,0x6,0x8,0x6),     /* W */
    LCD_VIM828_CHAR(0x5,0x0,0xA,0x0),     /* X */
    LCD_VIM828_CHAR(0x9,0x0,0x2,0x0),     /* Y */
    LCD_VIM828_CHAR(0x4,0x1,0x2,0x8),     /* Z */
    /* Decimal point */
    LCD_VIM828_CHAR(0x0,0x8,0x0,0x0),     /* 1DP */
    /* Minus Sign */
    LCD_VIM828_CHAR(0x0,0x0,0x4,0x0),     /* Minus Sign */
    /* Space */
    LCD_VIM828_CHAR(0x0,0x0,0x0,0x0),     /* Space */

};


/*!
   Structure will have the register numbers and the corresponding bit position information
   for the given display segment. This helps in setting the corresponding bits in the LCD
   registers to display a character/digit. Once the register values for a given
   character/digit are obtained from the mapping tables, the information in this
   structure provides the register number and bit position where the values to be
   written.  
   \n   
   The structure also will have the mask value which helps to clear the display
   for the correponding digit.
*/
typedef struct 
{
    uint8_t RegNum1;
    uint8_t BitPos1;
    uint8_t Mask1;
    uint8_t RegNum2;
    uint8_t BitPos2;
    uint8_t Mask2;
    uint16_t const *pMapTable;
    
} ADI_LCDVIM828_REG_INFO;


/*!< 
    Register number, bit position, mask and mapping table information for LCD display.
    Two consecutive segments are controlled by the same pair of registers with the 
    MS and LS bytes swapped.
    Example:
        Segment 1A: Reg7=0x2700 & Reg0=0x0046
        Segment 2A: Reg7=0x0027 & Reg0=0x4600 
    Bit position decides which encoding to be used in the macro LCD_VIM828_CHAR_REG1_VALUE. 
*/
static const ADI_LCDVIM828_REG_INFO SegmentRegInfo[] = 
{
    /* Segment 1  */
    {
        7,              /* Register Number */
        0x80,           /* Bit position */
        0xF,            /* Mask to clear digit */
        0,              /* Register Number */
        0x0,            /* Bit position */
        0xF,            /* Mask to clear digit */
        BigChar12Map    /* Mapping table for digits */
    },
    
    /* Segment 2 */
    {
        7,              /* Register Number */
        0x81,           /* Bit position */
        0xF,             /* Mask to clear digit */
        0,              /* Register Number */
        0x1,            /* Bit position */
        0xF,            /* Mask to clear digit */
        BigChar12Map    /* Mapping table for digits */
    },
    
    /*  Segment 3 */
    {
        6,              /* Register Number */
        0x80,           /* Bit position */
        0xF,            /* Mask to clear digit */
        1,              /* Register Number */
        0,              /* Bit position */
        0xF,            /* Mask to clear digit */
        BigChar12Map    /* Mapping table for digits */
    },
	
    /* Segment 4 */
	{
        6,              /* Register Number */
        0x81,           /* Bit position */
        0xF,            /* Mask to clear digit */
        1,              /* Register Number */
        0x1,            /* Bit position */
        0xF,            /* Mask to clear digit */
        BigChar12Map    /* Mapping table for digits */
    },
	
    /*Segment 5 */
	{
        5,              /* Register Number */
        0x80,           /* Bit position */
        0xF,            /* Mask to clear digit */
        2,              /* Register Number */
        0,              /* Bit position */
        0xF,            /* Mask to clear digit */
        BigChar12Map    /* Mapping table for digits */
    },
    
    /*Segment 6 */
	{
        5,              /* Register Number */
        0x81,           /* Bit position */
        0xF,            /* Mask to clear digit */
        2,              /* Register Number */
        1,              /* Bit position */
        0xF,            /* Mask to clear digit */
        BigChar12Map    /* Mapping table for digits */
    },
	
    /*Segment 7 */
	{
        4,              /* Register Number */
        0x80,           /* Bit position */
        0xF,            /* Mask to clear digit */
        3,              /* Register Number */
        0,              /* Bit position */
        0xF,            /* Mask to clear digit */
        BigChar12Map    /* Mapping table for digits */
    },
	
    /* Segment 8*/
	{
        4,              /* Register Number */
        0x81,           /* Bit position */
        0xF,            /* Mask to clear digit */
        3,              /* Register Number */
        1,              /* Bit position */
        0xF,            /* Mask to clear digit */
        BigChar12Map    /* Mapping table for digits */
    }
};

/* Local APIs */

/* Sets the LCD registers for the given charecter */
static ADI_LCDVIM828_RESULT_TYPE DisplayChar(ADI_LCD_DEV_HANDLE const       hDevice,
                                             ADI_LCD_SCREEN_TYPE const      Screen,
                                             ADI_LCDVIM828_REG_INFO const*  pRegInfo,
                                             uint32_t const                 CharIndex,
                                             bool_t                         dpReqd);
       
/* Clears all the characters on the specified display */       
static ADI_LCDVIM828_RESULT_TYPE ClearChars(ADI_LCD_DEV_HANDLE const        hDevice,
                                            ADI_LCD_SCREEN_TYPE const       Screen,
                                            ADI_LCDVIM828_REG_INFO const    *pRegInfoTable,
                                            uint8_t const                   NumChars);       

/*! \endcond */


/*!
 * @brief  Displays the given string.
 *   
 * Displays the given string on the VIM-828 display. The string length should be between 1 - 8 characters.
 * The allowed characters are 0-9, A-Z, space (' ') and decimal point ('.').
 *
 * Clears the previously displayed string before displaying the new string.
 *
 * @param[in]   hDevice             Device handle obtained from adi_LCD_Init().
 * @param[in]   Screen              LCD screen on which to display the measurement.
 * @param[in]   DisplayString       String to be displayed.
 *
 * @return      Status
 *                - #ADI_LCDVIM828_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCDVIM828_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCDVIM828_ERR_NULL_POINTER      [D]  The measurement string NULL.
 *                - #ADI_LCDVIM828_ERR_BAD_CHAR          [D]  One or more characters in the measurement string cannot be displayed.
 *                - #ADI_LCDVIM828_SUCCESS                    Call completed successfully.
 *
 * @sa adi_LCDVIM828_ClearDisplay()
 */
ADI_LCDVIM828_RESULT_TYPE adi_LCDVIM828_DisplayString(ADI_LCD_DEV_HANDLE    const  hDevice, 
                                                      ADI_LCD_SCREEN_TYPE   const Screen, 
                                                      uint8_t               const DisplayString[])
{
    int32_t     i;
    int32_t     j;
    int32_t     numDp = 0;
    uint8_t     tempDisplayString[ADI_LCDVIM828_NUM_SEGMENTS];
    uint8_t     charDP[] = {0,0,0,0,0,0,0,0}; /* Records the position of a decimal point */
    bool_t      addDP;
    uint32_t    charOffset;  /* Holds the offset of character in the mapping table */
    
#ifdef ADI_DEBUG
    if(DisplayString == NULL)
    {
        return ADI_LCDVIM828_ERR_NULL_POINTER;
    }
#endif /* ADI_DEBUG */
    
    /* Use a temporary variable so that the original string is not modified */
    for (i = 0; i < ADI_LCDVIM828_NUM_SEGMENTS; i++)
    {
        tempDisplayString[i] = DisplayString[i];

        if(tempDisplayString[i] == '.')
        {
            /* count the decimal points */
            numDp++;
        }
    }

    /* Clear the measurement display first */
    adi_LCDVIM828_ClearDisplay(hDevice, Screen);
    
    charOffset = LCDVIM828_SPACE_ASCII_TO_INDEX;
    
    /* Handle decimal point(s) display  
     *  cases:
     *  - numeric value with one decimal point (0.1)
     *  - decimal point in first position (.1)
     *  - multiple decimal points (0..1)
     */
    if ((numDp == 1) && (tempDisplayString[0] == '.'))
    {
        /* decimal in first position */
        tempDisplayString[0] = ' ';
        charDP[0] = 1;
    }
    else if (numDp == 1)
    {
        /* one decimal point not in first position (assume numeric value) 
         * shift the display characters after the decimal 
         */
        for (i = 1; i < ADI_LCDVIM828_NUM_SEGMENTS; i++)
        {
            if (tempDisplayString[i] == '.')
            {
                /* record the position and shift */
                charDP[i-1] = 1;
            
                for (j = i; j < ADI_LCDVIM828_NUM_SEGMENTS; j++)
                {
                    if (tempDisplayString[j + 1] == '\0')
                    {
                        tempDisplayString[j] = ' ';
                    }
                    else
                    {
                        tempDisplayString[j] = tempDisplayString[j + 1];
                    }
                }
                break;
            }
        }
    }
    else if (numDp > 1)
    {
        /* multiple decimal points - display as is */
        for (i = 0; i < ADI_LCDVIM828_NUM_SEGMENTS; i++)
        {
            if (tempDisplayString[i] == '.')
            {
               /* record the position */
               charDP[i] = 1;
               tempDisplayString[i] = ' ';
            }        
        }
    }
             
    /* Display the given string */
    for(i = 0; i < ADI_LCDVIM828_NUM_SEGMENTS;  i++)
    {
        /* Check if a decimal point is used */
        if(charDP[i])
        {
            addDP = true;
        }
        /* Check if character is valid and assign offset */
        if ((tempDisplayString[i] >= '0') && (tempDisplayString[i] <= '9'))
        {
            charOffset = LCDVIM828_ASCII_NUMBER_TO_INDEX;   
        }
        else if((tempDisplayString[i] >= 'A') && (tempDisplayString[i] <= 'Z'))
        {
            charOffset = LCDVIM828_UPPERCASE_ASCII_TO_INDEX;   
        }
        else if(tempDisplayString[i]=='-')
        {
            charOffset = LCDVIM828_MINUS_ASCII_TO_INDEX;   
        }
        else if(tempDisplayString[i]==' ')
        {
            charOffset = LCDVIM828_SPACE_ASCII_TO_INDEX;   
        }
        else
        {
            return ADI_LCDVIM828_ERR_BAD_CHAR;
        }

        DisplayChar(hDevice, Screen, &SegmentRegInfo[i], (tempDisplayString[i] - charOffset), addDP);
            
        addDP = false;
    }
    
    return ADI_LCDVIM828_SUCCESS;
}

/*!
 * @brief  Clears the display.
 *
 * The display is also cleared by the driver before displaying the newly given string. 
 * So it is not required for the application to use this API to clear the measurement 
 * display before displaying the new string. This API can be useful to turn off the display.
 *   
 * @param[in]   hDevice     Device handle obtained from adi_LCD_Init().
 * @param[in]   Screen      LCD screen on which to clear the mesurement display.
  *
 * @return      Status
 *                - #ADI_LCDVIM828_ERR_BAD_DEVICE_HANDLE [D]  Invalid device handle parameter.
 *                - #ADI_LCDVIM828_ERR_NOT_INITIALIZED   [D]  Device has not been previously configured for use.
 *                - #ADI_LCDVIM828_SUCCESS                    Call completed successfully.
 *
 * @sa adi_LCDVIM828_DisplayMeasurement()
 */
ADI_LCDVIM828_RESULT_TYPE adi_LCDVIM828_ClearDisplay(ADI_LCD_DEV_HANDLE const   hDevice, 
                                                     ADI_LCD_SCREEN_TYPE const  Screen)
{
    return(ClearChars(hDevice, Screen, SegmentRegInfo, ADI_LCDVIM828_NUM_SEGMENTS));
}

/* Sets the LCD registers to clear the characters */
static ADI_LCDVIM828_RESULT_TYPE ClearChars(ADI_LCD_DEV_HANDLE const        hDevice,
                                            ADI_LCD_SCREEN_TYPE const       Screen,
                                            ADI_LCDVIM828_REG_INFO const    *pRegInfoTable,
                                            uint8_t const                   NumChars)
{
    ADI_LCD_DATA_TYPE               DataTable[2];
    uint8_t                         i;
    ADI_LCDVIM828_REG_INFO const    *pRegInfo;
              
    for(i = 0; i < NumChars; i++)
    {   
        pRegInfo = &pRegInfoTable[i];
        
        /* Fill the register table whose contents to be read */
        DataTable[0].LCDRegNumber = pRegInfo->RegNum1;
        DataTable[1].LCDRegNumber = pRegInfo->RegNum2;
    
        /* Read the register contents */
        adi_LCD_GetDataReg(hDevice, Screen, DataTable, 2);
    
        /* Set the new data */
        DataTable[0].RegValue &= 0;
        DataTable[1].RegValue &= 0;
    
        /* Set back the register contents */
        adi_LCD_SetDataReg(hDevice, Screen, DataTable, 2);   
    }
    
    return ADI_LCDVIM828_SUCCESS;
}

/* Sets the LCD registers for the given character */
static ADI_LCDVIM828_RESULT_TYPE DisplayChar(ADI_LCD_DEV_HANDLE const       hDevice,
                                             ADI_LCD_SCREEN_TYPE const      Screen,
                                             ADI_LCDVIM828_REG_INFO const   *pRegInfo,
                                             uint32_t                       CharIndex,
                                             bool_t                         dpReqd)
{
    ADI_LCD_DATA_TYPE   DataTable[2];
    uint16_t            MapValue = pRegInfo->pMapTable[CharIndex];
    uint16_t            temp1; 
    uint16_t            temp2; 
                        
    /* If a decimal point is required, amend the map value */
    if (dpReqd)
    {
        MapValue |= pRegInfo->pMapTable[LCDVIM828_DP_OFFSET];
    }

    /* Fill the register table whose contents to be read */
    DataTable[0].LCDRegNumber = pRegInfo->RegNum1;
    DataTable[1].LCDRegNumber = pRegInfo->RegNum2;
    
    /* Read the register contents */
    adi_LCD_GetDataReg(hDevice, Screen, DataTable, 2);

    temp1 = DataTable[0].RegValue;
    temp2 = DataTable[1].RegValue;

    /* Set the new data */
    DataTable[0].RegValue = temp1 | LCD_VIM828_CHAR_REG1_VALUE(MapValue, pRegInfo->BitPos1);
    DataTable[1].RegValue = temp2 | LCD_VIM828_CHAR_REG1_VALUE(MapValue, pRegInfo->BitPos2);

    /* Set back the register contents */
    adi_LCD_SetDataReg(hDevice, Screen, DataTable, 2);   
    
    return ADI_LCDVIM828_SUCCESS;
	
	}
       
/*@}*/
    
#define REVERT_ADI_MISRA_SUPPRESSIONS
#include "misra.h"
