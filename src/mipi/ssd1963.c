/* Solomon Systech SSD1963 LCD Display Controller device-specific implementation file */

#include "device.h"
#include "ssd1963.h"
#include "dma.h"
#include "cdefADuCM350.h"
#include "startup.h"        /* For interrupt prototypes when doing OS builds */

/* Blocking semantics (controlled in adi_global_config.h) */
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)

    /* DMA complete pending semaphore */
    ADI_OSAL_SEM_HANDLE semDmaDone;
    uint8_t             semDmaDoneMem [ADI_OSAL_MAX_SEM_SIZE_CHAR];

    /* PDI complete pending semaphore */
    ADI_OSAL_SEM_HANDLE semPdiDone;
    uint8_t             semPdiDoneMem [ADI_OSAL_MAX_SEM_SIZE_CHAR];

#else

    /* non-OS uses a simple booleans to block */
    static volatile bool_t bDmaDone;
    static volatile bool_t bPdiDone;

#endif

/* Other file statics */
static uint16_t StartColumnAddress  = 0x00;
static uint16_t EndColumnAddress    = 0x00;
static uint16_t StartPageAddress    = 0x00;
static uint16_t EndPageAddress      = 0x00;
static uint8_t  bitsPerPixel        = 16;

ADI_PDI_RESULT_TYPE adi_DisplayDeviceInit (ADI_PDI_DEV_HANDLE const hDevice) 
{
    ADI_PDI_RESULT_TYPE result = ADI_PDI_SUCCESS;
    
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    /* Create the pend semaphores for blocking mode support. */
    if (ADI_OSAL_SUCCESS != adi_osal_SemCreateStatic(semDmaDoneMem, ADI_OSAL_MAX_SEM_SIZE_CHAR, &semDmaDone, 0))
    {
        return ADI_PDI_ERR_SEMAPHORE_FAILED;
    }
    if (ADI_OSAL_SUCCESS != adi_osal_SemCreateStatic(semPdiDoneMem, ADI_OSAL_MAX_SEM_SIZE_CHAR, &semPdiDone, 0))
    {
        return ADI_PDI_ERR_SEMAPHORE_FAILED;
    }
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT */

	/* Install interrupt handlers during device initialization */
	ADI_INSTALL_HANDLER (    PDI_IRQn,     PDI_Int_Handler);
	ADI_INSTALL_HANDLER (DMA_PDI_IRQn, DMA_PDI_Int_Handler);
	ADI_INSTALL_HANDLER (DMA_ERR_IRQn, DMA_Err_Int_Handler);
      
    uint8_t  paramBlock[4] = {0};

    /* Initialize PDI Interface Timing Register
     *     - bits[31:28] are reserved and should be set to 0
     *     - bits[27:24] = 0001 to set Chip Select Setup Time to 1 PCLK cycle
     *     - bits[23:16] = 00000001 to set Read/Write Control Pulse Low to 1 PCLK cycle
     *     - bits[15:8] = 00000001 to set Read/Write Control Pulse High to 1 PCLK cycle
     *     - bits[7:4] = 0001 to set Chip Select Wait Time to 1 PCLK cycle
     *     - bits[3:0] = 0001 to set Address Setup/Hold Time to 1 PCLK cycle 
     */
    if (ADI_PDI_SUCCESS != (result = adi_PDI_SetInterfaceTiming (hDevice, 0x01010111uL)))
    {
      return result;
    }

    /* Enable PDI for parallel DBI Type-B, 16-bit data bus, 16bpp colour depth, using DMA */
    if (ADI_PDI_SUCCESS != (result = adi_PDI_SetConfiguration (hDevice, 
                                                               (ADI_PDI_CONFIG_DMA_ENABLE | ADI_PDI_CONFIG_16BIT_16BPP | 
                                                                ADI_PDI_CONFIG_DBI_TYPE_B | ADI_PDI_CONFIG_PDI_ENABLE))))
    {
      return result;
    }

    /* Set the pixel interface format to 16Bit RGB565 format */
    paramBlock[0] = SSD1963_16BIT565;
    if (ADI_PDI_SUCCESS != (result = adi_PDI_DispatchMipiCommand (hDevice, MIPI_DCS1_SET_PIXEL_FORMAT, paramBlock)))
    {
        return result;
    }

    /* Set the display brightness to max */
    /* Set command, write 6 parameters */
    if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 6, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD1_SET_DBC_CONF))) 
      {
        return result;
      }
    /* Push 4 parameters */
    if (ADI_PDI_SUCCESS != (result = adi_PDI_PushParameterFIFO(hDevice, PDI_FIFO_PACK(0x0E, 
                                                                                      0xF0, 
                                                                                      (SSD1963_PWM_DBC_CONTROLLED | SSD1963_PWM_ENABLE), 
                                                                                      SSD1963_DBC_BRIGHTEST))))
      {
        return result;
      }
    /* Push 2 parameters */
    if (ADI_PDI_SUCCESS != (result = adi_PDI_PushParameterFIFO(hDevice, PDI_FIFO_PACK(0x00, 
                                                                                      0x0F, 
                                                                                      0x00, 
                                                                                      0x00)))) 
    {
        return result;
    }
      PDI_WAIT_CMD_DONE;

    /* Configure and enable Dynamic Backlight Control */
    /* Set command, write 1 parameter */
    if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 1, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD1_SET_DBC_CONF))) 
    {
      return result;
    }
    /* Push 1 parameter */
    if (ADI_PDI_SUCCESS != (result = adi_PDI_PushParameterFIFO(hDevice, PDI_FIFO_PACK((SSD1963_DBC_MANUAL_BRIGHTNESS_ENABLE |
                                                                                       SSD1963_DBC_TRANSITION_EFFECT_DISABLE |
                                                                                       SSD1963_ENERGY_SAVING_DISABLE |
                                                                                       SSD1963_DBC_MASTER_ENABLE), 
                                                                                      0, 
                                                                                      0, 
                                                                                      0)))) 
    {
      return result;
    }
    PDI_WAIT_CMD_DONE;
    
    /* Enable display */
    if (ADI_PDI_SUCCESS != (result = adi_PDI_DispatchMipiCommand (hDevice, MIPI_DCS0_SET_DISPLAY_ON, 0)))
    {
      return result;
    }

            return result;
}


ADI_PDI_RESULT_TYPE adi_TranslateMipiCommand (ADI_PDI_DEV_HANDLE const hDevice, const ADI_MIPI_DCS_ENUM MipiCmd, const void* pMipiParams)
{
    ADI_PDI_RESULT_TYPE result = ADI_PDI_SUCCESS;
    uint32_t                value;
    uint32_t                parameter;
    uint32_t                byteCount;
    uint32_t                wordCount;
    ADI_DMA_TRANSFER_TYPE dmaBlock;
    uint32_t                status;
    uint32_t* const         pPullParamters = 0;
          uint8_t tmp0; 
          uint8_t tmp1;
          uint8_t tmp2;
          uint8_t tmp3;


    /* Application MIPI commands are sent here via the PDI layer for implimentation using the native device-specific    */
    /* commands.                                                                                                        */
    /* Commands are written to the PDI 1st, followed by the paramater FIFO read/write, which launches the transfer.     */

    switch (MipiCmd) {
      
        case MIPI_DCS0_ENTER_IDLE_MODE:
          /* Set command, no parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD0_ENTER_IDLE_MODE))) 
          {
              return result;
          }
          PDI_WAIT_CMD_DONE;
          break;
            
        case MIPI_DCS0_ENTER_INVERT_MODE:
          /* Set command, no parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD0_ENTER_INVERT_MODE))) 
          {
              return result;
          }
          PDI_WAIT_CMD_DONE;
          break;
            
        case MIPI_DCS0_ENTER_NORMAL_MODE:
          /* Set command, no parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD0_ENTER_NORMAL_MODE))) 
          {
              return result;
          }
          PDI_WAIT_CMD_DONE;
          break;
            
        case MIPI_DCS0_ENTER_PARTIAL_MODE:
          /* Set command, no parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD0_ENTER_PARTIAL_MODE))) 
          {
              return result;
          }
          PDI_WAIT_CMD_DONE;
          break;
            
        case MIPI_DCS0_ENTER_SLEEP_MODE:
          /* Set command, no parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD0_ENTER_SLEEP_MODE))) 
          {
              return result;
          }
          PDI_WAIT_CMD_DONE;
          break;
            
        case MIPI_DCS0_EXIT_IDLE_MODE:
          /* Set command, no parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD0_EXIT_IDLE_MODE))) 
          {
              return result;
          }
          PDI_WAIT_CMD_DONE;
          break;

        case MIPI_DCS0_EXIT_INVERT_MODE:
          /* Set command, no parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD0_EXIT_INVERT_MODE))) 
          {
              return result;
          }
          PDI_WAIT_CMD_DONE;
          break;

        case MIPI_DCS0_EXIT_SLEEP_MODE:
          /* Set command, no parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD0_EXIT_SLEEP_MODE))) 
          {
              return result;
          }
          PDI_WAIT_CMD_DONE;
          break;

        case MIPI_DCS1_GET_ADDRESS_MODE:
          /* Set command, read 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 1, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD1_GET_ADDRESS_MODE))) 
          {
            return result;
          }
          /* Pull 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PullParameterFIFO(hDevice, pPullParamters))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          ((uint32_t*) &pMipiParams)[0] = PDI_FIFO_UNPACK_A(*pPullParamters);
          break;
          
        /* Mipi commands not supported by SSD1963 */
        case MIPI_DCS1_GET_BLUE_CHANNEL:
        case MIPI_DCS1_GET_DIAGNOSTIC_MODE:
          return ADI_PDI_ERR_UNSUPPORTED_MIPI;
          break;

        case MIPI_DCS1_GET_DISPLAY_MODE:
          /* Set command, read 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 1, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD1_GET_DISPLAY_MODE))) 
          {
            return result;
          }
          /* Pull 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PullParameterFIFO(hDevice, pPullParamters))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          ((uint32_t*) &pMipiParams)[0] = PDI_FIFO_UNPACK_A(*pPullParamters);
          break;
        
        /* Mipi command not supported by SSD1963 */
        case MIPI_DCS1_GET_GREEN_CHANNEL:
          return ADI_PDI_ERR_UNSUPPORTED_MIPI;
          break;

        case MIPI_DCS1_GET_PIXEL_FORMAT:
          /* Set command, read 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 1, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD1_GET_PIXEL_DATA_INTERFACE))) 
          {
            return result;
          }
          /* Pull 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PullParameterFIFO(hDevice, pPullParamters))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          parameter = PDI_FIFO_UNPACK_A(*pPullParamters);
          
          /* Match DCS Spec. Pixel Format to SSD1963 Pixel Format */
          switch(parameter) {
            case SSD1963_8BIT:
              ((uint32_t*) &pMipiParams)[0] = MIPI_DCS_8BITS_PER_PIXEL;
              break;
              
            case SSD1963_12BIT:
              ((uint32_t*) &pMipiParams)[0] = MIPI_DCS_12BITS_PER_PIXEL;
              break;
              
            case SSD1963_16BIT_PACKED:
            case SSD1963_16BIT565:
              ((uint32_t*) &pMipiParams)[0] = MIPI_DCS_16BITS_PER_PIXEL;
              break;
               
            case SSD1963_18BIT:
              ((uint32_t*) &pMipiParams)[0] = MIPI_DCS_18BITS_PER_PIXEL;
              break;
               
            case SSD1963_24BIT:
              ((uint32_t*) &pMipiParams)[0] = MIPI_DCS_24BITS_PER_PIXEL;
              break;
              
            default:
              ((uint32_t*) &pMipiParams)[0] = 0x07;
              break;
          }
          break;
            
        case MIPI_DCS1_GET_POWER_MODE:
          /* Set command, read 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 1, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD1_GET_POWER_MODE))) 
          {
            return result;
          }
          /* Pull 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PullParameterFIFO(hDevice, pPullParamters))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          ((uint32_t*) &pMipiParams)[0] = PDI_FIFO_UNPACK_A(*pPullParamters);
          break;
          
        /* Mipi command not supported by SSD1963 */
        case MIPI_DCS1_GET_RED_CHANNEL:
          return ADI_PDI_ERR_UNSUPPORTED_MIPI;
          break;
            
        case MIPI_DCS2_GET_SCANLINE:
          /* Set command, read 2 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 2, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD2_GET_SCANLINE))) 
          {
            return result;
          }
          /* Pull 2 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PullParameterFIFO(hDevice, pPullParamters))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          ((uint32_t*) &pMipiParams)[0] = PDI_FIFO_UNPACK_A(*pPullParamters);
          ((uint32_t*) &pMipiParams)[1] = PDI_FIFO_UNPACK_B(*pPullParamters);
          break;
              
        case MIPI_DCS1_GET_SIGNAL_MODE:
          /* Set command, read 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 1, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD1_GET_TEAR_EFFECT_STATUS))) 
          {
            return result;
          }
          /* Pull 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PullParameterFIFO(hDevice, pPullParamters))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          ((uint32_t*) &pMipiParams)[0] = PDI_FIFO_UNPACK_A(*pPullParamters);
          break;
          
        case MIPI_DCS0_NOP:
          /* Set command, no parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD0_NOP))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          break;

        /* Mipi command not supported by SSD1963 */
        case MIPI_DCSN_READ_DDB_CONTINUE:
          return ADI_PDI_ERR_UNSUPPORTED_MIPI;
          break;
            
        case MIPI_DCSN_READ_DDB_START:
          /* Set command, read 5 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 5, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD5_READ_DDB))) 
          {
            return result;
          }
          /* Pull 4 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PullParameterFIFO(hDevice, pPullParamters))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          ((uint32_t*) &pMipiParams)[0] = PDI_FIFO_UNPACK_A(*pPullParamters);
          ((uint32_t*) &pMipiParams)[1] = PDI_FIFO_UNPACK_B(*pPullParamters);
          ((uint32_t*) &pMipiParams)[2] = PDI_FIFO_UNPACK_C(*pPullParamters);
          ((uint32_t*) &pMipiParams)[3] = PDI_FIFO_UNPACK_D(*pPullParamters);
          /* Pull 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PullParameterFIFO(hDevice, pPullParamters))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          ((uint32_t*) &pMipiParams)[4] = PDI_FIFO_UNPACK_A(*pPullParamters);
          break;
          
        case MIPI_DCSN_READ_MEMORY_CONTINUE:
            /* Setup and run the DMA engine to return display data
             *
             * PREREQUISETS:
             *        previous call to MIPI_DCSN_READ_MEMORY_CONTINUE or MIPI_DCSN_READ_MEMORY_START
             *        previous call to adi_PDI_SetFrameDataCount() to set transfer size (in bytes)
             */

            /* Allocate DMA channel */
            if (ADI_DMA_SUCCESS != adi_DMA_Init(PDI_CHANn, ADI_DMA_PRIORITY_RESET))
            {
                return ADI_PDI_ERR_DMA_OPEN;
            }

            /* Set data length */
            byteCount = DMA_TRANSFER_SIZE * 4;
            wordCount = DMA_TRANSFER_SIZE;
            
            /* Setup DMA descriptor block for 32-bit transfers from the PDI FIFO */
            dmaBlock.Chan       = PDI_CHANn;
            dmaBlock.CCD        = ADI_DMA_CCD_PRIMARY;
            dmaBlock.DataWidth  = ADI_DMA_WIDTH_WORD;
            dmaBlock.DataLength = wordCount;
            dmaBlock.pSrcData   = (void*)&(pADI_PDI->PDI_FIFO);
            dmaBlock.pDstData   = (uint32_t*)pMipiParams;       /* DMA transfers are always 32-bit wide */
            dmaBlock.SrcInc     = ADI_DMA_INCR_WORD;
            dmaBlock.DstInc     = ADI_DMA_INCR_NONE;
            dmaBlock.Protection = ADI_DMA_HPROT_NONE;
            dmaBlock.ArbitRate  = ADI_DMA_RPOWER_1;
            dmaBlock.Mode       = ADI_DMA_MODE_BASIC;           /* Less than 1024 */

            /* Submit descriptor block */
            if (ADI_DMA_SUCCESS != adi_DMA_SubmitTransfer(&dmaBlock))
            {
                return ADI_PDI_ERR_DMA_SUBMIT;
            }

            /* Program PDI config for DMA-driven Type B, 16-bit, 16 BPP DBI parallel transfer */
            if (ADI_PDI_SUCCESS != adi_PDI_SetConfiguration(hDevice, 
                                                            (ADI_PDI_CONFIG_DMA_ENABLE | 
                                                             ADI_PDI_CONFIG_16BIT_16BPP | 
                                                             ADI_PDI_CONFIG_DBI_TYPE_B | 
                                                             ADI_PDI_CONFIG_PDI_ENABLE)))
            {
                return result;
            }

            /* Enable PDI command done interrupt */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_InterruptEnable(hDevice, ADI_PDI_IRQ_CMD_DONE)))
            {
                return result;
            }
           
            /* Set the frame data byte count. Frame data count is always in terms of bytes. */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_SetFrameDataCount (hDevice, byteCount)))
            {
                return result;
            }
            
            /* Send the SSD1963 read memory continue command 
             * NOTE: data comes via DMA, not the SSD1963_CMDN_READ_MEMORY_CONTINUE command... so param count is zero) */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_DATA, PDI_DIR_WRITE, SSD1963_CMDN_READ_MEMORY_CONTINUE)))
            {
                return result;
            }

            /* Clear PDI interrupt */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_SetStatus(hDevice, ADI_PDI_IRQ_CMD_DONE)))
            {
                return result;
            }

            /* Enable DMA error Interrupt */
            NVIC_ClearPendingIRQ (DMA_ERR_IRQn);
            ADI_ENABLE_INT       (DMA_ERR_IRQn);

            /* Enable PDI interrupt */
            NVIC_ClearPendingIRQ (PDI_IRQn);
            ADI_ENABLE_INT       (PDI_IRQn);

            /* Enable PDI DMA interrupt */
            NVIC_ClearPendingIRQ (DMA_PDI_IRQn);
            ADI_ENABLE_INT       (DMA_PDI_IRQn);

            /* Wait for DMA and PDI interrupts */
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
			/* OS-friendly pend on semaphores */
            adi_osal_SemPend(semDmaDone, ADI_OSAL_TIMEOUT_FOREVER);
            adi_osal_SemPend(semPdiDone, ADI_OSAL_TIMEOUT_FOREVER);
#else
            /* Non-OS spin on flags */
			bDmaDone = false;
			bPdiDone = false;
            while (!bPdiDone && !bDmaDone)
                ;
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */

            /* Final spin to drain FIFO */
            status = 0;
            while (!(ADI_PDI_STATUS_FIFO_EMPTY & status)) 
            {
                if (ADI_PDI_SUCCESS != (result = adi_PDI_GetStatus(hDevice, &status)))
                {
                    return result;
                }
            } ;

            /* Disable NVIC interrupts */
            ADI_DISABLE_INT (DMA_PDI_IRQn);
            ADI_DISABLE_INT (PDI_IRQn);
            ADI_DISABLE_INT (DMA_ERR_IRQn);

            /* Disable PDI interrupt */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_InterruptDisable(hDevice, ADI_PDI_IRQ_CMD_DONE)))
            {
                return result;
            }

            /* Release DMA channel */
            if (ADI_DMA_SUCCESS != adi_DMA_UnInit(PDI_CHANn))
            {
                return ADI_PDI_ERR_DMA_OPEN;
            }

            break;
            
        case MIPI_DCSN_READ_MEMORY_START:
            /* 
             * Setup and run the DMA engine to return display data
             *
             * PREREQUISETS:
             *        Previous call to MIPI_DCS4_SET_COLUMN_ADDRESS, as desired
             *        Previous call to MIPI_DCS4_SET_PAGE_ADDRESS, as desired
             *        Previous call to adi_PDI_SetFrameDataCount() to set transfer size (in bytes)
             */

            /* Allocate DMA channel */
            if (ADI_DMA_SUCCESS != adi_DMA_Init(PDI_CHANn, ADI_DMA_PRIORITY_RESET))
            {
                return ADI_PDI_ERR_DMA_OPEN;
            }

            /* Set data length */
            byteCount = DMA_TRANSFER_SIZE * 4;
            wordCount = DMA_TRANSFER_SIZE;
            
            /* Setup DMA descriptor block for 32-bit transfers from the PDI FIFO */
            dmaBlock.Chan       = PDI_CHANn;
            dmaBlock.CCD        = ADI_DMA_CCD_PRIMARY;
            dmaBlock.DataWidth  = ADI_DMA_WIDTH_WORD;
            dmaBlock.DataLength = wordCount;
            dmaBlock.pSrcData   = (void*)&(pADI_PDI->PDI_FIFO);
            dmaBlock.pDstData   = (uint32_t*)pMipiParams;       /* DMA transfers are always 32-bit wide */
            dmaBlock.SrcInc     = ADI_DMA_INCR_WORD;
            dmaBlock.DstInc     = ADI_DMA_INCR_NONE;
            dmaBlock.Protection = ADI_DMA_HPROT_NONE;
            dmaBlock.ArbitRate  = ADI_DMA_RPOWER_1;
            dmaBlock.Mode       = ADI_DMA_MODE_BASIC;           /* Less than 1024 */

            /* Submit descriptor block */
            if (ADI_DMA_SUCCESS != adi_DMA_SubmitTransfer(&dmaBlock))
            {
                return ADI_PDI_ERR_DMA_SUBMIT;
            }

            /* Program PDI config for DMA-driven Type B, 16-bit, 16 BPP DBI parallel transfer */
            if (ADI_PDI_SUCCESS != adi_PDI_SetConfiguration(hDevice, 
                                                            (ADI_PDI_CONFIG_DMA_ENABLE | 
                                                             ADI_PDI_CONFIG_16BIT_16BPP | 
                                                             ADI_PDI_CONFIG_DBI_TYPE_B | 
                                                             ADI_PDI_CONFIG_PDI_ENABLE)))
            {
                return result;
            }

            /* Enable PDI command done interrupt */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_InterruptEnable(hDevice, ADI_PDI_IRQ_CMD_DONE)))
            {
                return result;
            }
           
            /* Set the frame data byte count. Frame data count is always in terms of bytes. */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_SetFrameDataCount (hDevice, byteCount)))
            {
                return result;
            }

            /* Send the SSD1963 read memory start command 
             * NOTE: data comes via DMA, not the SSD1963_CMDN_READ_MEMORY_START command... so param count is zero) */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_DATA, PDI_DIR_WRITE, SSD1963_CMDN_READ_MEMORY_START)))
            {
                return result;
            }

            /* Clear PDI interrupt */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_SetStatus(hDevice, ADI_PDI_IRQ_CMD_DONE)))
            {
                return result;
            }

            /* Enable DMA error Interrupt */
            NVIC_ClearPendingIRQ (DMA_ERR_IRQn);
            ADI_ENABLE_INT       (DMA_ERR_IRQn);

            /* Enable PDI interrupt */
            NVIC_ClearPendingIRQ (PDI_IRQn);
            ADI_ENABLE_INT       (PDI_IRQn);

            /* Enable PDI DMA interrupt */
            NVIC_ClearPendingIRQ (DMA_PDI_IRQn);
            ADI_ENABLE_INT       (DMA_PDI_IRQn);

            /* Wait for DMA and PDI interrupts */
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
			/* OS-friendly pend on semaphores */
            adi_osal_SemPend(semDmaDone, ADI_OSAL_TIMEOUT_FOREVER);
            adi_osal_SemPend(semPdiDone, ADI_OSAL_TIMEOUT_FOREVER);
#else
            /* Non-OS spin on flags */
			bDmaDone = false;
			bPdiDone = false;
            while (!bPdiDone && !bDmaDone)
                ;
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */

            /* Final spin to drain FIFO */
            status = 0;
            while (!(ADI_PDI_STATUS_FIFO_EMPTY & status)) 
            {
                if (ADI_PDI_SUCCESS != (result = adi_PDI_GetStatus(hDevice, &status)))
                {
                    return result;
                }
            } ;

            /* Disable NVIC interrupts */
            ADI_DISABLE_INT (DMA_PDI_IRQn);
            ADI_DISABLE_INT (PDI_IRQn);
            ADI_DISABLE_INT (DMA_ERR_IRQn);

            /* Disable PDI interrupt */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_InterruptDisable(hDevice, ADI_PDI_IRQ_CMD_DONE)))
            {
                return result;
            }

            /* Release DMA channel */
            if (ADI_DMA_SUCCESS != adi_DMA_UnInit(PDI_CHANn))
            {
                return ADI_PDI_ERR_DMA_OPEN;
            }

            break;
            
        case MIPI_DCS1_SET_ADDRESS_MODE:
          /* Set command, write 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 1, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD1_SET_ADDRESS_MODE))) 
          {
            return result;
          }
          /* Push 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PushParameterFIFO(hDevice, PDI_FIFO_PACK(((uint8_t*) pMipiParams)[0], 
                                                                                            0, 
                                                                                            0, 
                                                                                            0)))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          break;

        case MIPI_DCS4_SET_COLUMN_ADDRESS:
          /* Record Start Column and End Column addresses */
          tmp0 = (((uint8_t*) pMipiParams)[0]);
          tmp1 = (((uint8_t*) pMipiParams)[1]);
          tmp2 = (((uint8_t*) pMipiParams)[2]);
          tmp3 = (((uint8_t*) pMipiParams)[3]);
          StartColumnAddress  = (tmp0 << 8) + tmp1;
          EndColumnAddress    = (tmp2 << 8) + tmp3;

          /* Set command, write 4 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 4, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD4_SET_COLUMN_ADDRESS))) 
          {
            return result;
          }
          /* Push 4 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PushParameterFIFO(hDevice, PDI_FIFO_PACK(((uint8_t*) pMipiParams)[0], 
                                                                                            ((uint8_t*) pMipiParams)[1], 
                                                                                            ((uint8_t*) pMipiParams)[2], 
                                                                                            ((uint8_t*) pMipiParams)[3])))) 
          {
            return result;
          }
            PDI_WAIT_CMD_DONE;
            break;

        case MIPI_DCS0_SET_DISPLAY_OFF:
          /* Set command, no parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD0_SET_DISPLAY_OFF))) 
          {
              return result;
          }
          PDI_WAIT_CMD_DONE;
          break;
          
        case MIPI_DCS0_SET_DISPLAY_ON:
          /* Set command, no parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD0_SET_DISPLAY_ON))) 
          {
              return result;
          }
          PDI_WAIT_CMD_DONE;
          break;

        case MIPI_DCS1_SET_GAMMA_CURVE:
          /* Set command, write 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 1, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD1_SET_GAMMA_CURVE))) 
          {
            return result;
          }
          /* Push 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PushParameterFIFO(hDevice, PDI_FIFO_PACK(((uint8_t*) pMipiParams)[0], 
                                                                                            0, 
                                                                                            0, 
                                                                                            0)))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          break;

        case MIPI_DCS4_SET_PAGE_ADDRESS:
          /* Record Start Page and End Page addresses */
          tmp0 = (((uint8_t*) pMipiParams)[0]);
          tmp1 = (((uint8_t*) pMipiParams)[1]);
          tmp2 = (((uint8_t*) pMipiParams)[2]);
          tmp3 = (((uint8_t*) pMipiParams)[3]);
          StartPageAddress  = (tmp0 << 8) + tmp1;
          EndPageAddress    = (tmp2 << 8) + tmp3;

          /* Set command, write 4 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 4, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD4_SET_PAGE_ADDRESS))) 
          {
            return result;
          }
          /* Push 4 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PushParameterFIFO(hDevice, PDI_FIFO_PACK(((uint8_t*) pMipiParams)[0], 
                                                                                            ((uint8_t*) pMipiParams)[1], 
                                                                                            ((uint8_t*) pMipiParams)[2], 
                                                                                            ((uint8_t*) pMipiParams)[3])))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          break;

        /* Mipi command not supported by SSD1963 */
        case MIPI_DCS4_SET_PARTIAL_COLUMNS:
          return ADI_PDI_ERR_UNSUPPORTED_MIPI;
          break;

        case MIPI_DCS4_SET_PARTIAL_ROWS:
          /* Record Start Row and End Row addresses */
          /* Set command, write 4 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 4, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD4_SET_PARTIAL_AREA))) 
          {
            return result;
          }
          /* Push 4 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PushParameterFIFO(hDevice, PDI_FIFO_PACK(((uint8_t*) pMipiParams)[0], 
                                                                                            ((uint8_t*) pMipiParams)[1], 
                                                                                            ((uint8_t*) pMipiParams)[2], 
                                                                                            ((uint8_t*) pMipiParams)[3])))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          break;

        case MIPI_DCS1_SET_PIXEL_FORMAT:
          value = ((uint8_t*) pMipiParams)[0];

          /* Only looking at bits[2:0] */
          value &= 0x07;

          /* This exact command is not supported by SSD1963, but an equivalent is. */
          /* Match DCS Spec. Pixel Format to SSD1963 Pixel Format */
          switch(value) 
          {
            case MIPI_DCS_3BITS_PER_PIXEL:
              /* Not supported */
              parameter = 0x07;
              break;
            
            case MIPI_DCS_8BITS_PER_PIXEL:
              parameter = SSD1963_8BIT;
              bitsPerPixel = 8;
              break;
              
            case MIPI_DCS_12BITS_PER_PIXEL:
              parameter = SSD1963_12BIT;
              bitsPerPixel = 12;
              break;
              
            case MIPI_DCS_16BITS_PER_PIXEL:
              parameter = SSD1963_16BIT565;
              bitsPerPixel = 16;
              break;
               
            case MIPI_DCS_18BITS_PER_PIXEL:
              parameter = SSD1963_18BIT;
              bitsPerPixel = 18;
              break;
               
            case MIPI_DCS_24BITS_PER_PIXEL:
              parameter = SSD1963_24BIT;
              bitsPerPixel = 24;
              break;
              
            default:
              parameter = 0x07;
              bitsPerPixel = 24;
              break;
          }
          
          if (parameter != 0x07) 
          {
            /* Set command, write 1 parameter */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 1, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD1_SET_PIXEL_DATA_INTERFACE))) 
            {
              return result;
            }
            /* Push 1 parameter */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_PushParameterFIFO(hDevice, PDI_FIFO_PACK(((uint8_t*) pMipiParams)[0], 
                                                                                              0, 
                                                                                              0, 
                                                                                            0)))) 
            {
              return result;
            }
            PDI_WAIT_CMD_DONE;
          }
          else 
          {
            return ADI_PDI_ERR_UNSUPPORTED_MIPI;
          }
          break;

        case MIPI_DCS6_SET_SCROLL_AREA:
          /* Set command, write 6 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 6, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD6_SET_SCROLL_AREA))) 
          {
            return result;
          }
          /* Push 4 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PushParameterFIFO(hDevice, PDI_FIFO_PACK(((uint8_t*) pMipiParams)[0], 
                                                                                            ((uint8_t*) pMipiParams)[1], 
                                                                                            ((uint8_t*) pMipiParams)[2], 
                                                                                            ((uint8_t*) pMipiParams)[3])))) 
          {
            return result;
          }
          /* Push 2 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PushParameterFIFO(hDevice, PDI_FIFO_PACK(((uint8_t*) pMipiParams)[4], 
                                                                                            ((uint8_t*) pMipiParams)[5], 
                                                                                            0, 
                                                                                            0)))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          break;

        case MIPI_DCS2_SET_SCROLL_START:
          /* Set command, write 2 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 2, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD2_SET_SCROLL_START))) 
          {
            return result;
          }
          /* Push 2 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PushParameterFIFO(hDevice, PDI_FIFO_PACK(((uint8_t*) pMipiParams)[0], 
                                                                                            ((uint8_t*) pMipiParams)[1], 
                                                                                            0, 
                                                                                            0)))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          break;

        case MIPI_DCS0_SET_TEAR_OFF:
          /* Set command, no parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD0_SET_TEAR_OFF))) 
          {
              return result;
          }
          PDI_WAIT_CMD_DONE;
          break;
          
        case MIPI_DCS1_SET_TEAR_ON:
          /* Set command, write 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 1, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD1_SET_TEAR_ON))) 
          {
            return result;
          }
          /* Push 1 parameter */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PushParameterFIFO(hDevice, PDI_FIFO_PACK(((uint8_t*) pMipiParams)[0], 
                                                                                            0, 
                                                                                            0, 
                                                                                            0)))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          break;

        case MIPI_DCS2_SET_TEAR_SCANLINE:
          /* Set command, write 2 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 2, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD2_SET_TEAR_SCANLINE))) 
          {
            return result;
          }
          /* Push 2 parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_PushParameterFIFO(hDevice, PDI_FIFO_PACK(((uint8_t*) pMipiParams)[0], 
                                                                                            ((uint8_t*) pMipiParams)[1], 
                                                                                            0, 
                                                                                            0)))) 
          {
            return result;
          }
          PDI_WAIT_CMD_DONE;
          break;

        case MIPI_DCS0_SOFT_RESET:
          /* Set command, no parameters */
          if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_CONTROL, PDI_DIR_WRITE, SSD1963_CMD0_SOFT_RESET))) 
          {
              return result;
          }
          PDI_WAIT_CMD_DONE;
          break;
          
        /* Mipi command not supported by ADuCM350 */
        case MIPI_DCSN_WRITE_LUT:
          return ADI_PDI_ERR_UNSUPPORTED_MIPI;
          break;

        case MIPI_DCSN_WRITE_MEMORY_CONTINUE:
            /* 
             * Setup and run the DMA engine to pump display data
             *
             * PREREQUISETS:
             *        Previous call to MIPI_DCSN_WRITE_MEMORY_CONTINUE or MIPI_DCSN_WRITE_MEMORY_START
             *        Previous call to adi_PDI_SetFrameDataCount() to set transfer size (in bytes)
             */

            /* Allocate DMA channel */
            if (ADI_DMA_SUCCESS != adi_DMA_Init(PDI_CHANn, ADI_DMA_PRIORITY_RESET))
            {
                return ADI_PDI_ERR_DMA_OPEN;
            }

            /* Set data length */
            byteCount = DMA_TRANSFER_SIZE * 4;
            wordCount = DMA_TRANSFER_SIZE;
            
            /* Setup DMA descriptor block for 32-bit transfers to the PDI FIFO */
            dmaBlock.Chan       = PDI_CHANn;
            dmaBlock.CCD        = ADI_DMA_CCD_PRIMARY;
            dmaBlock.DataWidth  = ADI_DMA_WIDTH_WORD;
            dmaBlock.DataLength = wordCount;
            dmaBlock.pSrcData   = (uint32_t*)pMipiParams;       /* DMA transfers are always 32-bit wide */
            dmaBlock.pDstData   = (void*)&(pADI_PDI->PDI_FIFO);
            dmaBlock.SrcInc     = ADI_DMA_INCR_WORD; 
            dmaBlock.DstInc     = ADI_DMA_INCR_NONE;
            dmaBlock.Protection = ADI_DMA_HPROT_NONE;
            dmaBlock.ArbitRate  = ADI_DMA_RPOWER_1;
            dmaBlock.Mode       = ADI_DMA_MODE_BASIC;           /* Less than 1024 */

            /* Submit descriptor block */
            if (ADI_DMA_SUCCESS != adi_DMA_SubmitTransfer(&dmaBlock))
            {
                return ADI_PDI_ERR_DMA_SUBMIT;
            }

            /* Program PDI config for DMA-driven Type B, 16-bit, 16 BPP DBI parallel transfer */
            if (ADI_PDI_SUCCESS != adi_PDI_SetConfiguration(hDevice, 
                                                            (ADI_PDI_CONFIG_DMA_ENABLE | 
                                                             ADI_PDI_CONFIG_16BIT_16BPP | 
                                                             ADI_PDI_CONFIG_DBI_TYPE_B | 
                                                             ADI_PDI_CONFIG_PDI_ENABLE)))
            {
                return result;
            }

            /* Enable PDI command done interrupt */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_InterruptEnable(hDevice, ADI_PDI_IRQ_CMD_DONE)))
            {
                return result;
            }
           
            /* Set the frame data byte count. Frame data count is always in terms of bytes. */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_SetFrameDataCount (hDevice, byteCount)))
            {
                return result;
            }

            /* Send the SSD1963 write memory continue command 
             * NOTE: data comes via DMA, not the SSD1963_CMDN_WRITE_MEMORY_CONTINUE command... so param count is zero) */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_DATA, PDI_DIR_WRITE, SSD1963_CMDN_WRITE_MEMORY_CONTINUE)))
            {
                return result;
            }

            /* Clear PDI interrupt */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_SetStatus(hDevice, ADI_PDI_IRQ_CMD_DONE)))
            {
                return result;
            }

            /* Enable DMA error Interrupt */
            NVIC_ClearPendingIRQ (DMA_ERR_IRQn);
            ADI_ENABLE_INT       (DMA_ERR_IRQn);

            /* Enable PDI interrupt */
            NVIC_ClearPendingIRQ (PDI_IRQn);
            ADI_ENABLE_INT       (PDI_IRQn);

            /* Enable PDI DMA interrupt */
            NVIC_ClearPendingIRQ (DMA_PDI_IRQn);
            ADI_ENABLE_INT       (DMA_PDI_IRQn);

            /* Wait for DMA and PDI interrupts */
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
			/* OS-friendly pend on semaphores */
            adi_osal_SemPend(semDmaDone, ADI_OSAL_TIMEOUT_FOREVER);
            adi_osal_SemPend(semPdiDone, ADI_OSAL_TIMEOUT_FOREVER);
#else
            /* Non-OS spin on flags */
			bDmaDone = false;
			bPdiDone = false;
            while (!bPdiDone && !bDmaDone)
                ;
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */

            /* Final spin to drain FIFO */
            status = 0;
            while (!(ADI_PDI_STATUS_FIFO_EMPTY & status)) 
            {
                if (ADI_PDI_SUCCESS != (result = adi_PDI_GetStatus(hDevice, &status)))
                {
                    return result;
                }
            } ;

            /* Disable NVIC interrupts */
            ADI_DISABLE_INT (DMA_PDI_IRQn);
            ADI_DISABLE_INT (PDI_IRQn);
            ADI_DISABLE_INT (DMA_ERR_IRQn);

            /* Disable PDI interrupt */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_InterruptDisable(hDevice, ADI_PDI_IRQ_CMD_DONE)))
            {
                return result;
            }

            /* Release DMA channel */
            if (ADI_DMA_SUCCESS != adi_DMA_UnInit(PDI_CHANn))
            {
                return ADI_PDI_ERR_DMA_OPEN;
            }

            break;
          
        case MIPI_DCSN_WRITE_MEMORY_START:
            /* 
             * Setup and run the DMA engine to pump display data
             *
             * PREREQUISETS:
             *        Previous call to MIPI_DCS4_SET_COLUMN_ADDRESS, as desired
             *        Previous call to MIPI_DCS4_SET_PAGE_ADDRESS, as desired
             *        Previous call to adi_PDI_SetFrameDataCount() to set transfer size (in bytes)
             */
          
            /* Allocate DMA channel */
            if (ADI_DMA_SUCCESS != adi_DMA_Init(PDI_CHANn, ADI_DMA_PRIORITY_RESET))
            {
                return ADI_PDI_ERR_DMA_OPEN;
            }

            /* Set data length */
            byteCount = DMA_TRANSFER_SIZE * 4;
            wordCount = DMA_TRANSFER_SIZE;
            
            /* Setup DMA descriptor block for 32-bit transfers to the PDI FIFO */
            dmaBlock.Chan       = PDI_CHANn;
            dmaBlock.CCD        = ADI_DMA_CCD_PRIMARY;
            dmaBlock.DataWidth  = ADI_DMA_WIDTH_WORD;
            dmaBlock.DataLength = wordCount;
            dmaBlock.pSrcData   = (uint32_t*)pMipiParams;       /* DMA transfers are always 32-bit wide */
            dmaBlock.pDstData   = (void*)&(pADI_PDI->PDI_FIFO);
            dmaBlock.SrcInc     = ADI_DMA_INCR_WORD; 
            dmaBlock.DstInc     = ADI_DMA_INCR_NONE;
            dmaBlock.Protection = ADI_DMA_HPROT_NONE;
            dmaBlock.ArbitRate  = ADI_DMA_RPOWER_1;
            dmaBlock.Mode       = ADI_DMA_MODE_BASIC;           /* Less than 1024 */

            /* Submit descriptor block */
            if (ADI_DMA_SUCCESS != adi_DMA_SubmitTransfer(&dmaBlock))
            {
                return ADI_PDI_ERR_DMA_SUBMIT;
            }

            /* Program PDI config for DMA-driven Type B, 16-bit, 16 BPP DBI parallel transfer */
            if (ADI_PDI_SUCCESS != adi_PDI_SetConfiguration(hDevice, 
                                                            (ADI_PDI_CONFIG_DMA_ENABLE | 
                                                             ADI_PDI_CONFIG_16BIT_16BPP | 
                                                             ADI_PDI_CONFIG_DBI_TYPE_B | 
                                                             ADI_PDI_CONFIG_PDI_ENABLE)))
            {
                return result;
            }

            /* Enable PDI command done interrupt */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_InterruptEnable(hDevice, ADI_PDI_IRQ_CMD_DONE)))
            {
                return result;
            }
           
            /* Set the frame data byte count. Frame data count is always in terms of bytes. */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_SetFrameDataCount (hDevice, byteCount)))
            {
                return result;
            }

            /* Send the SSD1963 write memory start command 
             * NOTE: data comes via DMA, not the SSD1963_CMDN_WRITE_MEMORY_START command... so param count is zero) */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_SetCommand(hDevice, 0, PDI_CMD_DATA, PDI_DIR_WRITE, SSD1963_CMDN_WRITE_MEMORY_START)))
            {
                return result;
            }

            /* Clear PDI interrupt */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_SetStatus(hDevice, ADI_PDI_IRQ_CMD_DONE)))
            {
                return result;
            }

            /* Enable DMA error Interrupt */
            NVIC_ClearPendingIRQ (DMA_ERR_IRQn);
            ADI_ENABLE_INT       (DMA_ERR_IRQn);

            /* Enable PDI interrupt */
            NVIC_ClearPendingIRQ (PDI_IRQn);
            ADI_ENABLE_INT       (PDI_IRQn);

            /* Enable PDI DMA interrupt */
            NVIC_ClearPendingIRQ (DMA_PDI_IRQn);
            ADI_ENABLE_INT       (DMA_PDI_IRQn);

            /* Wait for DMA and PDI interrupts */
#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
			/* OS-friendly pend on semaphores */
            adi_osal_SemPend(semDmaDone, ADI_OSAL_TIMEOUT_FOREVER);
            adi_osal_SemPend(semPdiDone, ADI_OSAL_TIMEOUT_FOREVER);
#else
            /* Non-OS spin on flags */
			bDmaDone = false;
			bPdiDone = false;
            while (!bPdiDone && !bDmaDone)
                ;
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */

            /* Final spin to drain FIFO */
            status = 0;
            while (!(ADI_PDI_STATUS_FIFO_EMPTY & status)) 
            {
                if (ADI_PDI_SUCCESS != (result = adi_PDI_GetStatus(hDevice, &status)))
                {
                    return result;
                }
            } ;

            /* Disable NVIC interrupts */
            ADI_DISABLE_INT (DMA_PDI_IRQn);
            ADI_DISABLE_INT (PDI_IRQn);
            ADI_DISABLE_INT (DMA_ERR_IRQn);

            /* Disable PDI interrupt */
            if (ADI_PDI_SUCCESS != (result = adi_PDI_InterruptDisable(hDevice, ADI_PDI_IRQ_CMD_DONE)))
            {
                return result;
            }

            /* Release DMA channel */
            if (ADI_DMA_SUCCESS != adi_DMA_UnInit(PDI_CHANn))
            {
                return ADI_PDI_ERR_DMA_OPEN;
            }
            
            break;
          
        /* Unknown or invalid MIPI commands */
        default:
            return (ADI_PDI_ERR_UNKNOWN_MIPI);
            break;
    }
    return result;
}

/* Write to the SINT035TFT display with data specified by pData */
ADI_PDI_RESULT_TYPE writeDisplay (ADI_PDI_DEV_HANDLE hDevice, uint8_t page, uint32_t* pData) {
    ADI_PDI_RESULT_TYPE result = ADI_PDI_SUCCESS;
    uint8_t             paramBlock[4];

    if (0 == page)
    {
        /* There is a delay needed before the first pages are written*/
        for (int delay = 0; delay < 0xFFFF; delay++);

        /* Set page address */
        paramBlock[0] = 0x00;   /* SP(MSB) = 0x00 */
        paramBlock[1] = 0x00;   /* SP(LSB) = 0x00 */
        paramBlock[2] = 0x00;   /* EP(MSB) = 0x00 */
        paramBlock[3] = 0xEF;   /* EP(LSB) = 0xEF */
   
        if (ADI_PDI_SUCCESS != (result = adi_PDI_DispatchMipiCommand (hDevice, MIPI_DCS4_SET_PAGE_ADDRESS, paramBlock)))
        {
            return result;
        }
        
        /* Set column address */
        paramBlock[0] = 0x00;   /* SP(MSB) = 0x00 */
        paramBlock[1] = 0x00;   /* SP(LSB) = 0x00 */
        paramBlock[2] = 0x01;   /* EP(MSB) = 0x00 */
        paramBlock[3] = 0x3F;   /* EP(LSB) = 0xF0 */
      
        if (ADI_PDI_SUCCESS != (result = adi_PDI_DispatchMipiCommand (hDevice, MIPI_DCS4_SET_COLUMN_ADDRESS, paramBlock)))
        {
            return result;
        }

        /* Start writing to memory */
        if (ADI_PDI_SUCCESS != (result = adi_PDI_DispatchMipiCommand (hDevice, MIPI_DCSN_WRITE_MEMORY_START, pData)))
        {
            return result;
        }
    }
    else
    {
        /* Continue writing to memory */
        if (ADI_PDI_SUCCESS != (result = adi_PDI_DispatchMipiCommand (hDevice, MIPI_DCSN_WRITE_MEMORY_CONTINUE, pData)))
       {
            return result;
        }
    }
    return result;
}

/* PDI interrupt handler */
ADI_INT_HANDLER (PDI_Int_Handler)
{
    uint32_t status = pADI_PDI->PDI_STAT;

    /* Verify expected PDI command done interrupt */
    if (ADI_PDI_IRQ_CMD_DONE & status) 
    {

        /* Clear it */
        pADI_PDI->PDI_INT_CLR = ADI_PDI_IRQ_CMD_DONE;

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
	    /* Post the semaphore */
        adi_osal_SemPost(semPdiDone);
#else
        /* or mark it done */
        bPdiDone = true;
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */
    } 
    else 
    {
		/* Unexpected interrupt */
		while (1)
			;
        }
}

/* DMA interrupt handler */
ADI_INT_HANDLER (DMA_PDI_Int_Handler)
{
    uint16_t tmp;

#if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
	/* Post the semaphore */
    adi_osal_SemPost(semDmaDone);
#else
    /* or mark it done */
    bDmaDone = true;
#endif /* ADI_CFG_ENABLE_RTOS_SUPPORT  */
    
    /* change in a tmp variable and write entire new value all at once */
    tmp = pADI_PDI->PDI_CFG;            /* Read the register */
    tmp |= ADI_PDI_CONFIG_END_TRANSFER; /* Set the bit */
    pADI_PDI->PDI_CFG = tmp;            /* Write it */
}

void DMA_Err_Int_Handler(void)
{
    /* We should never get here! */
    while (1)
        ;
}
