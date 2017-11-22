/*********************************************************************************

Copyright (c) 2013-2014 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.  By using 
this software you agree to the terms of the associated Analog Devices Software 
License Agreement.

*********************************************************************************/

/*!
 ******************************************************************************
 * @file:    uart.c
 * @brief:   UART Device Implementations for ADuCxxx
 * @version: $Revision: 28544 $
 * @date:    $Date: 2014-11-13 13:09:44 -0500 (Thu, 13 Nov 2014) $
 *****************************************************************************/

/** \addtogroup UART_Driver UART Driver
 *  @{
 */

#define ASSERT_ADI_MISRA_SUPPRESSIONS  /*!< Apply ADI MISRA Suppressions */
#include "misra.h"

#include <stddef.h>                /* for NULL */
#include <string.h>                /* for memcpy */
#include "uart.h"
#include "gpio.h"      /* GPIO configuration */

#if (0 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) && (0 == ADI_UART_CFG_POLLED_MODE_SUPPORT)
#error Neither interrupt-mode nor polled-mode are enabled
#endif

#if (0 == ADI_UART_CFG_BLOCKING_MODE_SUPPORT) && (0 == ADI_UART_CFG_NONBLOCKING_MODE_SUPPORT)
#error Neither blocking nor non-blocking modes are enabled
#endif

#if (0 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
#define IS_INTERRUPT_MODE(hDevice) false
#elif (0 == ADI_UART_CFG_POLLED_MODE_SUPPORT)
#define IS_INTERRUPT_MODE(hDevice) true
#else
#define IS_INTERRUPT_MODE(hDevice) ((hDevice)->bInterruptMode)
#endif

#if (0 == ADI_UART_CFG_POLLED_MODE_SUPPORT)
#define IS_POLLED_MODE(hDevice) false
#elif (0 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
#define IS_POLLED_MODE(hDevice) true
#else
#define IS_POLLED_MODE(hDevice) (!(hDevice)->bInterruptMode)
#endif

#if (0 == ADI_UART_CFG_BLOCKING_MODE_SUPPORT)
#define IS_BLOCKING_MODE(hDevice) false
#elif (0 == ADI_UART_CFG_NONBLOCKING_MODE_SUPPORT)
#define IS_BLOCKING_MODE(hDevice) true
#else
#define IS_BLOCKING_MODE(hDevice) ((hDevice)->bBlockingMode)
#endif

#if (0 == ADI_UART_CFG_NONBLOCKING_MODE_SUPPORT)
#define IS_NONBLOCKING_MODE(hDevice) false
#elif (0 == ADI_UART_CFG_BLOCKING_MODE_SUPPORT)
#define IS_NONBLOCKING_MODE(hDevice) true
#else
#define IS_NONBLOCKING_MODE(hDevice) (!(hDevice)->bBlockingMode)
#endif

/*! \cond PRIVATE */

#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
/*!----------------------------------------------------------------------------
 * \struct ADI_UART_BUFFER
 *
 *  UART buffer
 *
 * In a transmit (Tx) buffer, the read index (RdIndx) is updated by the ISR and
 * the write index (WrIndex) is updated by do_write.
 *
 * In a receive (Rx) buffer, the write index (WrIndx) is updated by the ISR and
 * the read index (RdIndx) is updated by do_write.
 *
 * In either case there is no shared update between API level and interrupt
 * level, so synchronization by disabling interrupts is not required. Nor
 * do the read or write index need to be volatile, as they will not change
 * asynchonously.
 *
 * The 'NumAvailable' counter, on the other hand, is updated from both API
 * level and from interrupt level - for both transmit and receive buffers - and
 * so must be declared volatile and read-modify-writes of it from API level must
 * be done with the UART interrupt disabled.
 *
 * The 'Buffer' and 'BufSize' fields do not change after initialization, so
 * none of these issues apply to them.
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    uint8_t           *Buffer;                /*!< start of the buffer        */
    uint16_t          WrIndx;                 /*!< write index                */
    uint16_t          RdIndx;                 /*!< read index                 */
    uint16_t          BufSize;                /*!< buffer size                */
    volatile uint16_t NumAvailable;           /*!< number of bytes available  */

} ADI_UART_CIRC_BUFFER_TYPE;
#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */


/*!----------------------------------------------------------------------------
 *  \struct ADI_UART_DEV_DATA_TYPE
 *
 *  Configuration Structure
 *----------------------------------------------------------------------------*/
typedef struct ADI_UART_DEV_DATA_TYPE
{
    const IRQn_Type              UARTIRQn;              /*!< uart irq                   */
    ADI_UART_DRV_STATE_TYPE      DrvState;              /*!< driver state               */
    ADI_UART_TypeDef* const      pUartRegs;             /*!< uart register pointer      */
    bool_t                       bBlockingMode;         /*!< uart blocking flag         */
    bool_t                       bDMAMode;              /*!< uart dma mode              */
    bool_t                       bInterruptMode;        /*!< interrupt/polling mode     */
    uint8_t                      uLineStatus;           /*!< saved line status          */
    uint16_t                     defLineConfig;             /*!< default configuration      */
    uint16_t                     defModemConfig;        /*!< default configuration      */
    ADI_UART_BAUDRATE_TYPE       defBaud;               /*!< default baudrate           */

#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)

#  if (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
    /* OSAL */
    ADI_OSAL_SEM_HANDLE          hSem;                  /*!< Handle to semaphore        */
    uint8_t                      SemMemory[ADI_OSAL_MAX_SEM_SIZE_CHAR];
                                                        /*!< Semaphore Memory           */
#  else /* (0 == ADI_CFG_ENABLE_RTOS_SUPPORT) */
    volatile bool_t              bInterruptFlag;        /*!< interrupt control flag     */
#  endif /* (0 == ADI_CFG_ENABLE_RTOS_SUPPORT) */

    ADI_UART_CIRC_BUFFER_TYPE    RxBuffer;              /*!< receive buffer             */
    ADI_UART_CIRC_BUFFER_TYPE    TxBuffer;              /*!< transmit buffer            */
#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */

#if defined(ADI_DEBUG)
    ADI_UART_STATS_TYPE          Stats;                 /*!< uart statistics            */
#endif /* defined(ADI_DEBUG) */

} ADI_UART_DEV_DATA_TYPE;


/*!----------------------------------------------------------------------------
 *   UART Device Data Block
 *----------------------------------------------------------------------------*/
static ADI_UART_DEV_DATA_TYPE UART_DevData[] =
{
   /* UART-0 */
   {
       UART_IRQn,                                       /*!< uart irq priority          */
       ADI_UART_DRV_STATE_UNKNOWN,                      /*!< state                      */
       pADI_UART,                                       /*!< uart register pointer      */
       true,                                            /*!< blocking mode              */
       false,                                           /*!< dma mode                   */
       false,                                           /*!< interrupt mode             */
       0u,                                              /*!< null line status           */
       ADI_UART0_COMLCR_INITIALIZER,                    /*!< default configuration      */
       ADI_UART0_COMMCR_INITIALIZER,                    /*!< default configuration      */
       ADI_UART0_BAUD_INITIALIZER,                      /*!< default baudrate           */
#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
#if (1 == ADI_CFG_ENABLE_RTOS_SUPPORT)
       NULL,
       { 0 },
#else /* (0 == ADI_CFG_ENABLE_RTOS_SUPPORT) */
       false,                                           /*!< interrupt flag init state  */
#endif /* (0 == ADI_CFG_ENABLE_RTOS_SUPPORT) */
       {
           NULL,                                        /*!< rx buffer start address    */
           0,                                           /*!< rx buffer write index      */
           0,                                           /*!< rx buffer read index       */
           NULL,                                        /*!< rx buffer size             */
           0,                                           /*!< available bytes            */
       },
       {
           NULL,                                        /*!< tx buffer start address    */
           0,                                           /*!< tx buffer read index       */
           0,                                           /*!< tx buffer write index      */
           NULL,                                        /*!< tx buffer size             */
           NULL,                                        /*!< number of free elements    */
       },
#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */
#if defined(ADI_DEBUG)
       {0}                                              /*!< uart statistics            */
#endif /* defined(ADI_DEBUG) */
   }
};

/*!----------------------------------------------------------------------------
  UART Driver internal macros
*----------------------------------------------------------------------------*/
#define NUM_UART_INSTANCES (sizeof(UART_DevData) / sizeof(ADI_UART_DEV_DATA_TYPE))  /*!< Size of device driver instance data array */
#define UART_RX_FULL(hDevice)   (((hDevice)->pUartRegs->COMLSR) & COMLSR_DR)                          /*!< Returns true if rx data is available */
#define UART_TX_EMPTY(hDevice)  (((hDevice)->pUartRegs->COMLSR) & COMLSR_THRE)                        /*!< Returns true if tx hold register is empty */

/* debug helper to track statistics */
#if defined(ADI_DEBUG)
       #define ADI_UART_STAT_INC(x) (x++)                                           /*!< map the ADI_UART_STAT_INC macro */
#else
       #define ADI_UART_STAT_INC(x)                                                 /*!< compile ADI_UART_STAT_INC macro out to nothing */
#endif /* UART_DEBUG */

/*!----------------------------------------------------------------------------
  UART device driver internal functions
*----------------------------------------------------------------------------*/
/* resets circular buffer contents */
static void inline ResetCircularBuffers( ADI_UART_HANDLE const hDevice);

/* internal read and write functions */
static ADI_UART_RESULT_TYPE do_read (ADI_UART_HANDLE const hDevice, uint8_t *pRxData, const int16_t Size);
static ADI_UART_RESULT_TYPE do_write(ADI_UART_HANDLE const hDevice, uint8_t *pTxData, const int16_t Size);

/* Configures the read or write request depending on the current operating mode
 * i.e blocking, non-blocking, interrupt, polling and dma.
 */
static ADI_UART_RESULT_TYPE ProcessRequest( ADI_UART_HANDLE const hDevice, uint8_t *pData,
                                            int16_t  *pSize, bool_t  bIsRxBuffer
                                          );

/* rx status handler which processes overrun, parity and frame errors.
 */
static void UART_RX_Status_Handler(ADI_UART_HANDLE hDevice, const uint8_t uLineStatus);

#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
/* The interrupt handler cannot be static, as it must be externally visible so that
 * it can be linked to the vector table in the non-RTOS build case.
 */
ADI_INT_HANDLER(UART_Int_Handler);
#endif


/*!----------------------------------------------------------------------------
   UART internal functions
*----------------------------------------------------------------------------*/

/*!
 * @brief          RX status error handler
 *
 * @param[in] uLineStatus Line Status register contents
 *
 * @return         none
 *
 * @details        Line Status Error interrupt handlers gets invoked because of
 *                 any of the following events.
 *
 *                 1. Framing Error
 *                      Set when start bit has no associated stop bit.
 *                 2. Parity Error
 *                      Set when parity error occurs
 *                 3. Overrun error
 *                      Data is overwritten before being read
 *
 * @note           By default driver will trap the errors and waits in infinite loop.
 *                 Applications are expected to modify this function as required.
 */
static void UART_RX_Status_Handler(ADI_UART_HANDLE hDevice, const uint8_t uLineStatus)
{
#if defined(ADI_DEBUG)
     /* overflow error */
    if(uLineStatus & COMLSR_OE)
    {
        ADI_UART_STAT_INC(hDevice->Stats.OverrunErrorCnt);
    }

    /* parity error */
    if(uLineStatus & COMLSR_PE)
    {
        ADI_UART_STAT_INC(hDevice->Stats.ParityErrorCnt);
    }

    /* framing error */
    if(uLineStatus & COMLSR_FE)
    {
        ADI_UART_STAT_INC(hDevice->Stats.FrameErrorCnt);
    }
#endif /* defined(ADI_DEBUG) */

    hDevice->uLineStatus = (uLineStatus & (COMLSR_OE | COMLSR_PE | COMLSR_FE));
}

#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
/*!
* @brief            UART Interrupt handler
*
* @return           none
*
* @details         Handles rx line status, rx buffer full, modem status and tx buffer
*                  empty interrupts. When the driver is operating in interrupt mode these
*                  interrupts are enabled and data transmission or reception is done in
*                  the isr. In case of rx buffer full interrupt, data is fetched from the
*                  rx fifo and placed in the internal driver buffer. Applications will be
*                  able to get  this data by issuing adi_UART_BufRx api.
*
*                  If ADI_DEBUG is defined then appropriate statistics counters
*                  were updated.
*/
ADI_INT_HANDLER(UART_Int_Handler)
{
    ADI_UART_HANDLE       hDevice     = &UART_DevData[ADI_UART_DEVID_0];
    ADI_UART_TypeDef      *pUART_Regs = hDevice->pUartRegs;
    ADI_UART_CIRC_BUFFER_TYPE  *pBuf  = NULL;
    uint8_t  iir = hDevice->pUartRegs->COMIIR;
    uint8_t data;

    /* iir bit zero is 1 then there is no interrupt */
    if( iir & COMIIR_NINT )
        return;

    /* we got an uart interrupt, process it */
    switch( iir & COMIIR_STA_MSK )
    {
        /* rx line status interupt */
        case COMIIR_STA_RXLINESTATUS:
        {
            uint8_t status = pUART_Regs->COMLSR;

            ADI_UART_STAT_INC(hDevice->Stats.RxStatusIrqCnt);

            /* Trap overrun, parity, framing errors */
            if( status & (COMLSR_OE | COMLSR_PE | COMLSR_FE) )
                UART_RX_Status_Handler(hDevice, status);
        }
        break;

        /* rx buffer full interrupt */
        case COMIIR_STA_RXBUFFULL:
        {
            pBuf = &hDevice->RxBuffer;

            /* flush data if no space available */
            if( pBuf->NumAvailable == pBuf->BufSize )
            {
                ADI_UART_STAT_INC(hDevice->Stats.RxDroppedByteCnt);

               /* trap receive buffer overflow condition. This condition occurs when
                * internal driver buffer is full and more data is received.
                *
                * NOTE: If this condition occurs applications has to adjust the rx buffer size that
                * is passed via adi_UART_Init() call. The below while(1) is expected to be changed
                * depending on the application requirements.
                */
                while(1); /* internal driver buffer overflow */
            }

            /* adjust the write index if reaches the end */
            if( pBuf->WrIndx ==  pBuf->BufSize )
                pBuf->WrIndx = 0;

            /* read data to rx buffer */
            data = pUART_Regs->COMRX;
            pBuf->Buffer[pBuf->WrIndx++] = data;

            /* increment number of available bytes */
            pBuf->NumAvailable++;
            ADI_UART_STAT_INC(hDevice->Stats.RxBufferFullIrqCnt);
        }
        break;

        /* tx buffer empty interrupt */
        case COMIIR_STA_TXBUFEMPTY:
        {
              ADI_UART_STAT_INC(hDevice->Stats.TxBufferEmptyIrqCnt);

             pBuf = &hDevice->TxBuffer;

            if( pBuf->RdIndx >=  pBuf->BufSize )
                pBuf->RdIndx = 0;

             if(pBuf->NumAvailable < pBuf->BufSize)
             {
                hDevice->pUartRegs->COMTX = pBuf->Buffer[pBuf->RdIndx++];
                pBuf->NumAvailable++;
             }
             else
             {
                /* no more data to transmit so disable tx empty interrupt */
                pUART_Regs->COMIEN &= ~(COMIEN_ETBEI);
             }
        }
        break;

        /* modem status interrupt */
        case COMIIR_STA_MODEMSTATUS:
        {
              ADI_UART_STAT_INC(hDevice->Stats.ModemStatusIrqCnt);
        }
        break;

        default:
        break;
    }

#if (1 == ADI_CFG_ENABLE_RTOS_SUPPORT)
    adi_osal_SemPost(hDevice->hSem);
#else /* (0 == ADI_CFG_ENABLE_RTOS_SUPPORT) */
    /* Any interrupt that changes the UART device state needs to bring the
     * processor out of low-power mode, as functions may be waiting for the
     * state to change.
     */
    SystemExitLowPowerMode(&hDevice->bInterruptFlag);
#endif  /* (0 == ADI_CFG_ENABLE_RTOS_SUPPORT) */
}
#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */

/*!
* @brief            Reads data to user buffer
*
* @param[in]  hDevice   Handle to the device which is returned through adi_UART_Init()
* @param[in]  pRxData   Pointer to the user data area
* @param[in]  *pSize    Data pointer whose location contains the number of bytes to be read
*
* @return           0 upon success and -1 upon failure
*
* @details          Handles blocking/non-blocking with combination of interrupt and polling
*                   modes.
*/
static ADI_UART_RESULT_TYPE do_read(ADI_UART_HANDLE const hDevice, uint8_t *pRxData, const int16_t Size)
{
    int16_t i = 0;

#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
    if(IS_INTERRUPT_MODE(hDevice))
    {
        int16_t BytesToCopy   = 0;
        int16_t BytesRemaining= 0;
        ADI_UART_CIRC_BUFFER_TYPE *pBuf = &hDevice->RxBuffer;

        /* get number of bytes till end */
        BytesRemaining =  pBuf->BufSize - pBuf->RdIndx;

        /* interrupt mode */
        BytesToCopy = (BytesRemaining > Size) ? Size : BytesRemaining;

        /* TODO: check if memcpy is faster */
        for(i=0;i < BytesToCopy; i++)
            *pRxData++ = pBuf->Buffer[pBuf->RdIndx++];

        /* check if we have more to copy */
        if(BytesToCopy < Size)
        {
            BytesRemaining = Size - BytesToCopy;
            pBuf->RdIndx = 0;

            for(i=0;i<BytesRemaining;i++)
                *pRxData++ = pBuf->Buffer[pBuf->RdIndx++];
        }

        /* atomically adjust the NumAvailable */
        ADI_DISABLE_INT(hDevice->UARTIRQn);
        pBuf->NumAvailable -= Size;
        ADI_ENABLE_INT(hDevice->UARTIRQn);
    }
#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */

#if (1 == ADI_UART_CFG_POLLED_MODE_SUPPORT)
    /* polling mode */
    if(IS_POLLED_MODE(hDevice))
    {
        for(i=0; i < Size; i++)
        {
            /* blocking read */

            /* wait until a byte is available */
            while(1)
            {
                /* read the line status register */
                uint8_t status = hDevice->pUartRegs->COMLSR;

                /* Trap overflow, parity, frame errors */
                if(status & (COMLSR_OE | COMLSR_PE | COMLSR_FE))
                    UART_RX_Status_Handler(hDevice, status);

                /* check if data is available */
                if(status & COMLSR_DR)
                  break;
            }

            /* read byte from the COMRX register */
            *pRxData++ = hDevice->pUartRegs->COMRX;
            /* increment the pointer */
        }
    }
#endif /* (1 == ADI_UART_CFG_POLLED_MODE_SUPPORT) */

    return(ADI_UART_SUCCESS);
}


/*!
* @brief            Write data from the user buffer
*
* @param[in]  hDevice   Handle to the device which is returned through adi_UART_Init()
* @param[in]  pTxData   Pointer to the user data area
* @param[in]  Size      Size of the data to be sent
*
* @return           0 upon success and -1 upon failure
*
* @details          Handles blocking/non-blocking with combination of interrupt and polling
*                   modes.
*/
static ADI_UART_RESULT_TYPE do_write(ADI_UART_HANDLE const hDevice, uint8_t *pTxData, const int16_t Size)
{
#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
    if(IS_INTERRUPT_MODE(hDevice))
    {
        int16_t i = 0;
        int16_t BytesToCopy   = 0;
        int16_t BytesRemaining= 0;
        ADI_UART_CIRC_BUFFER_TYPE *pBuf = &hDevice->TxBuffer;

        i = pBuf->BufSize - pBuf->WrIndx;
        BytesToCopy = (i > Size) ? Size : i;

        /* TODO: check if memcpy is faster */
        for(i=0;i < BytesToCopy; i++)
            pBuf->Buffer[pBuf->WrIndx++] = *pTxData++;

        /* check if we have more to copy */
        if(BytesToCopy < Size)
        {
            BytesRemaining = Size - BytesToCopy;
            pBuf->WrIndx = 0;

            for(i=0;i<BytesRemaining;i++)
               pBuf->Buffer[pBuf->WrIndx++] = *pTxData++;
        }

        /* atomically adjust the NumAvailable */
        ADI_DISABLE_INT(hDevice->UARTIRQn);
        pBuf->NumAvailable -= Size;

        /* and enable tx empty interrupt */
        hDevice->pUartRegs->COMIEN |= COMIEN_ETBEI;
        ADI_ENABLE_INT(hDevice->UARTIRQn);
    }
#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */

#if (1 == ADI_UART_CFG_POLLED_MODE_SUPPORT)
    if(IS_POLLED_MODE(hDevice))
    {
        for(int i=0; i < Size; i++)
        {
            /* blocking write */

            /* wait until previous byte is transmitted */
            while(!(hDevice->pUartRegs->COMLSR & COMLSR_THRE));

            /* write byte to the COMTX register and increment the data pointer */
            hDevice->pUartRegs->COMTX = *pTxData++;
        }
    }
#endif /* (1 == ADI_UART_CFG_POLLED_MODE_SUPPORT) */

    return(ADI_UART_SUCCESS);
}


/*!
* @brief               Processes the receive or transmit request
*
* @param[in]  hDevice      Handle to the device which is returned through adi_UART_Init()
* @param[in]  pData        Pointer to rx or tx data
* @param[in]  *pSize       Pointer to the location of size
* @param[in]  pCircBuffer  Pointer to receive or transmit circular buffer
* @param[in]  bIsRxBuffer  Flag to control return of receive full or transmit empty state in blocking mode
*
* @return           0 upon success and -1 upon failure
*
* @details          Handles blocking/non-blocking with combination of interrupt and polling
*                   modes.
*/
static ADI_UART_RESULT_TYPE ProcessRequest(ADI_UART_HANDLE const hDevice,
                                           uint8_t *pData,
                                           int16_t *pSize,
                                           bool_t  bIsRxBuffer)
{
    ADI_UART_RESULT_TYPE result = ADI_UART_ERR_UNKNOWN;

#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
    /* check if we are in interrupt mode */
    if( IS_INTERRUPT_MODE(hDevice) )
    {
      ADI_UART_CIRC_BUFFER_TYPE *pCircBuffer = bIsRxBuffer ? &hDevice->RxBuffer : &hDevice->TxBuffer;

#if (1 == ADI_UART_CFG_BLOCKING_MODE_SUPPORT)
        /* blocking mode */
        if( IS_BLOCKING_MODE(hDevice) )
        {
           if (pCircBuffer->BufSize < *pSize)
           {
               /* If the caller is requesting more data than the circular buffer
                * can hold then the request can never be fulfilled.
                */
               result = ADI_UART_ERR_INVALID_BUFFER;
           }
           else
           {
               while(pCircBuffer->NumAvailable < *pSize)
               {
#if (1 == ADI_CFG_ENABLE_RTOS_SUPPORT)
                   adi_osal_SemPend(hDevice->hSem, ADI_OSAL_TIMEOUT_FOREVER);
#else /* (0 == ADI_CFG_ENABLE_RTOS_SUPPORT) */
                   /* Instead of busy-waiting, we enter core sleep mode while waiting. Any
                    * interrupt that changes NumAvailable will also bring the processor
                    * out of core sleep and cause this call to return.
                    */
                   SystemEnterLowPowerMode(ADI_SYS_MODE_CORE_SLEEP, /* wait in core sleep */
                                           &hDevice->bInterruptFlag,
                                           0);
#endif /* (0 == ADI_CFG_ENABLE_RTOS_SUPPORT) */
               }

                result = ADI_UART_SUCCESS;
           }
        }
#endif /* (1 == ADI_UART_CFG_BLOCKING_MODE_SUPPORT) */

#if (1 == ADI_UART_CFG_NONBLOCKING_MODE_SUPPORT)
        if( IS_NONBLOCKING_MODE(hDevice) ) /* non-blocking mode */
        {
           /* Adjust transfer size */
           int16_t BytesAvailable = pCircBuffer->NumAvailable;

           *pSize = (*pSize > BytesAvailable) ? BytesAvailable : *pSize;
            result = ADI_UART_SUCCESS;
        }
#endif /* (1 == ADI_UART_CFG_NONBLOCKING_MODE_SUPPORT) */
   }
#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */

#if (1 == ADI_UART_CFG_POLLED_MODE_SUPPORT)
   if (IS_POLLED_MODE(hDevice)) /* polling mode */
   {
#if (1 == ADI_UART_CFG_BLOCKING_MODE_SUPPORT)
      /* in non-blocking mode if there is no data pSize is set to 0 */
      if(IS_BLOCKING_MODE(hDevice))
      {
          /* No change to *pSize in polled-blocking mode, as we'll wait for the
           * specified number of bytes to be received or transmitted.
           */
            result = ADI_UART_SUCCESS;
      }
#endif /* (1 == ADI_UART_CFG_BLOCKING_MODE_SUPPORT) */

#if (1 == ADI_UART_CFG_NONBLOCKING_MODE_SUPPORT)
      if( IS_NONBLOCKING_MODE(hDevice) )
      {
          /* In polled-nonblocking mode, there can be at most one byte available */
          *pSize = (bIsRxBuffer ? UART_RX_FULL(hDevice) : UART_TX_EMPTY(hDevice)) ? 1 : 0;
            result = ADI_UART_SUCCESS;
      }
#endif /* (1 == ADI_UART_CFG_NONBLOCKING_MODE_SUPPORT) */
   }
#endif /* (1 == ADI_UART_CFG_POLLED_MODE_SUPPORT) */

   return(result);
}


/*!
* @brief            Resets the read and write indices of the buffer
*
* @param[in]  pBuffer   Pointer to the circular buffer
*
* @return None
*
*/
static inline void ResetCircularBuffers(ADI_UART_HANDLE const hDevice)
{
#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
    ADI_DISABLE_INT(hDevice->UARTIRQn);

    /* reset rx buffer contents */
    hDevice->RxBuffer.RdIndx = 0;
    hDevice->RxBuffer.WrIndx = 0;
    hDevice->RxBuffer.NumAvailable = 0;

    /* reset tx buffer contents */
    hDevice->TxBuffer.RdIndx = 0;
    hDevice->TxBuffer.WrIndx = 0;
    hDevice->TxBuffer.NumAvailable = hDevice->TxBuffer.BufSize;

    ADI_ENABLE_INT(hDevice->UARTIRQn);
#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */
}

/*! \endcond */

/*!
* @brief                 UART Initialization
*
* @param[in]   devID     Enumerated UART device identifier. This id is used to open the
*                        correct instance of the peripheral. ADI_UART_DEVID_0 is the only
*                        allowed value for the systems with a single UART.
* @param[out]  pHandle   Points to the location where the device handle will be stored upon
*                        successful initialization.
*
* @param[in]   pInitData Pointer to the #ADI_UART_INIT_DATA stucture. Application uses this structure to
*                        supply initialization parameters to the init function. If NULL is passed then
*                        UART driver operates in polling mode by polling the status bits. With valid receive
*                        and transmit buffers the driver operates in interrupt mode with internal buffering.
*
* @return      Status
*                        - #ADI_UART_SUCCESS                     upon success
*                        - #ADI_UART_ERR_INVALID_DEVID       [D] if invalid device id is passed
*                        - #ADI_UART_ERR_ALREADY_INITIALIZED [D] if the uart is already initialized
*                        - #ADI_UART_ERR_INVALID_BUFFER      [D] pInitData contains invalid buffer(s)
*
* @details
*                        Initializes and configures UART. Upon successful initialization a handle
*                        to the device is stored in the supplied pHandle.Applications must use this
*                        handle value in all subsequent api calls to this device. Device initialization
*                        includes configuring the GPIO pins, setting up UART with default baudrate,word
*                        length, parity and stop bits. Two buffers one for receive and one for transmit
*                        were also initialized.
*
*                        Init call sets up UART with default initialization values.Default UART values are
*                        baud rate -> 9600, word length -> 8, parity -> none, stop bits -> 1.
*                        Default initialization values for UART are controlled through the following macros
*                        in the UART header file.
*
*                        - #ADI_UART_COMLCR_INITIALIZER   configure word length and stop bits.
*                        - #ADI_UART_BAUD_9600            default baud rate
*
* @note                  Init call does not enable the UART device. Once configuration and other setup is complete
*                        applications has to explicitly enable UART via adi_UART_Enable call. Once UART is enabled
*                        changing the UART configuration such as baudrate, wordlength, operating mode (interrupt,poll,
*                        or dma) may result in un-desired behavior. Applications must disable UART before changing UART
*                        configuration and enable UART after setting the new configuration.
*
* @sa                    adi_UART_UnInit
* @sa                    adi_UART_Enable
*/
ADI_UART_RESULT_TYPE adi_UART_Init(const ADI_UART_DEV_ID_TYPE devID, ADI_UART_HANDLE* const pHandle,
                                   ADI_UART_INIT_DATA* const pInitData)
{
    ADI_UART_HANDLE hDevice     = NULL;
    ADI_UART_RESULT_TYPE Result = ADI_UART_SUCCESS;

    /* set handle to null */
    *pHandle = NULL;

#if defined(ADI_DEBUG)
    if(devID > NUM_UART_INSTANCES)
       return (ADI_UART_ERR_INVALID_DEVID);
#endif

    /* address of the device data block is the handle */
    hDevice = &UART_DevData[devID];

#if defined(ADI_DEBUG)
    /* if state is not unknown return error */
    if( ADI_UART_DRV_STATE_UNKNOWN != hDevice->DrvState )
        return(ADI_UART_ERR_ALREADY_INITIALIZED);
#endif /* defined(ADI_DEBUG) */

#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
    /* initialize uart using init data parameters */
    if(pInitData != NULL)
    {
#if defined(ADI_DEBUG)
        /* check init parameters */
        if(  ( pInitData->pRxBufferData == NULL ) || (pInitData->RxBufferSize == 0) ||
             ( pInitData->pTxBufferData == NULL ) || (pInitData->TxBufferSize == 0) )
            return(ADI_UART_ERR_INVALID_BUFFER);
#endif /* defined(ADI_DEBUG) */

#if (1 == ADI_CFG_ENABLE_RTOS_SUPPORT)
        /* Create the semaphore for blocking mode support. */
        if(adi_osal_SemCreateStatic(&hDevice->SemMemory, ADI_OSAL_MAX_SEM_SIZE_CHAR, &hDevice->hSem, 0u) != ADI_OSAL_SUCCESS)
        {
            return ADI_UART_ERR_SEMAPHORE_FAILED;
        }
#else /* (0 == ADI_CFG_ENABLE_RTOS_SUPPORT) */
        hDevice->bInterruptFlag = false;
#endif  /* (0 == ADI_CFG_ENABLE_RTOS_SUPPORT) */

        /* initialize rx buffer parameters */
        hDevice->RxBuffer.Buffer     = pInitData->pRxBufferData;
        hDevice->RxBuffer.BufSize    = pInitData->RxBufferSize;

        /* initialize tx buffer parameters */
        hDevice->TxBuffer.Buffer     = pInitData->pTxBufferData;
        hDevice->TxBuffer.BufSize    = pInitData->TxBufferSize;

        /* enable interrupt mode if internal buffering is enabled */
        hDevice->bInterruptMode = true;
        ADI_INSTALL_HANDLER(hDevice->UARTIRQn, UART_Int_Handler);
    }
#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */

    ResetCircularBuffers(hDevice);

    /* set uart state to initialized */
    hDevice->DrvState = ADI_UART_DRV_STATE_INITIALIZED;

    /* configure uart with default parameters */
    adi_UART_SetBaudRate(hDevice, hDevice->defBaud);
    hDevice->pUartRegs->COMLCR = hDevice->defLineConfig;
    hDevice->pUartRegs->COMMCR = hDevice->defModemConfig;

    /* set the handle */
    *pHandle = hDevice;

	/* enable the dedicated UART core clock */
    SystemEnableClock(ADI_SYS_CLOCK_GATE_UART, true);

    /* disable uart: applications might want to change the configuration */
    hDevice->pUartRegs->COMCON = COMCON_DISABLE;

#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
    /* enable uart interrupt */
    ADI_ENABLE_INT(hDevice->UARTIRQn);
#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */

    return(Result);
}


/*!
* @brief                 Un-Initialize UART
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
*
* @return      Status
*                        - #ADI_UART_SUCCESS                  upon success
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D] if invalid instance handle is passed
*                        - #ADI_UART_ERR_NOT_INITIALIZED  [D] if device is not initialized
*
* @details
*                        Closes UART device and resets it's internal structures. After closing
*                        the device it can be initialized again. Uninit disables UART interrupt.
*                        This call also puts UART in unknown state.Transmit and receive operations
*                        will result in error after UART is un-initialized.
*
* @sa                    adi_UART_Init
* @sa                    adi_UART_Enable
*/
ADI_UART_RESULT_TYPE adi_UART_UnInit(ADI_UART_HANDLE const hDevice)
{
#if defined(ADI_DEBUG)
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);

    if( hDevice->DrvState != ADI_UART_DRV_STATE_INITIALIZED )
        return ADI_UART_ERR_NOT_INITIALIZED;
#endif /* defined(ADI_DEBUG) */

#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
    ADI_UNINSTALL_HANDLER(hDevice->UARTIRQn);  /* uninstall handler */
    ADI_DISABLE_INT(hDevice->UARTIRQn);        /* disable uart interrupt */

#if (1 == ADI_CFG_ENABLE_RTOS_SUPPORT)
    adi_osal_SemDestroyStatic(hDevice->hSem);
#endif /* (1 == ADI_CFG_ENABLE_RTOS_SUPPORT) */

#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */

	/* disable the dedicated UART core clock */
    SystemEnableClock(ADI_SYS_CLOCK_GATE_UART, false);

    hDevice->pUartRegs->COMCON |= COMCON_DISABLE;    /* disable uart */
    hDevice->DrvState = ADI_UART_DRV_STATE_UNKNOWN;    /* reset uart state */

    return(ADI_UART_SUCCESS);
}


/*!
* @brief                Transmits data on UART
*
* @param[in]    hDevice Handle to the device which is returned through adi_UART_Init()
* @param[in]     pData  Data pointer which points to the to-be transmitted data.
* @param[in,out] pSize  Pointer to a location where number of bytes to be transmitted is specified.
*
* @return     Status
*                        - #ADI_UART_SUCCESS                  upon success
*                        - #ADI_UART_ERR_INVALID_PARAMS       if invalid parameters are passed
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D] if invalid instance handle is passed
*                        - #ADI_UART_ERR_NOT_INITIALIZED  [D] if device is not initialized
*
* @details
*                        Transmits data over UART. Actual data transmission is dependent on the operational
*                        mode of the driver. Application can set the driver to operate in blocking or non-blocking
*                        modes. Driver can be configured to use poll, interrupt or dma in conjuction with the
*                        operating modes.
*
*                        Blocking and Non-Blocking Modes:
*
*                        In blocking mode, this api will not return until entire data is transmitted. In case of
*                        non-blocking mode, this api returns immediately by transmitting the data completely
*                        or partially or without transmitting any data. In non-blocking mode pSize parameter is modified
*                        to reflect the number of bytes successfully transmitted.
*
*                        Both blocking and non-blocking modes can be used in conjuction with interrupt or polling
*                        configuration of the driver. In case of interrupt mode UART transmit interrupt is enabled
*                        and upon each transmit buffer empty interrupt a byte is fetched from internal driver buffer
*                        and transmitted on UART. In interrupt mode application data is copied in to driver's internal
*                        buffer.Applications have to carefully configure the tx internal buffer by using \e pTxBufferData
*                        and \e TxBufferSize which are members of #ADI_UART_INIT_DATA. This is input parameter for
*                        adi_UART_Init API.
*
*                        In interrupt mode applications can not write more than supplied buffer size of data in a
*                        single transmit call. As internal buffers are used by the time this call returns entire data
*                        might not have been transmitted on UART.
*                        In case of polled configuration status bits are used to determine the successful transfer.
*                        Internal buffers are not used in polled mode and by the time this API returns entire data is
*                        transmitted over UART.
*
*                        Applications can use adi_UART_GetNumTxBytes() call to get number of bytes that can be
*                        successfully transmitted.
*
*                        Following sections describe the different operating modes and configurations in detail.
*
*                        Blocked-Polling Mode:
*
*                        In blocked polling mode, this API will not return until entire data is transmitted.
*                        In polling mode UART devices status register is used to check success of the transmit.
*                        Internal buffering is not used in polling mode so application data buffer is directly
*                        by the driver.No memory copies are used in this mode. By the time this API returns
*                        entire data is successfully transmitted.
*
*                        Blocked-Interrupt Mode:
*
*                        In blocked interrupt mode, this API will not return until entire data is transmitted.
*                        In interrupt mode UART transmit buffer empty interrupt is used to check success of the
*                        transmit. Internal buffering is used in this mode.Application data is copied in to
*                        internal drivers data buffer and the call is returned. Upon each transmit buffer empty
*                        interrupt a byte is sent over UART. Because of the buffering by the time this call returns
*                        entire data might not have been sent over UART.Applications can reuse the data buffer as
*                        soon as the API returns. In this mode a single transmit operations should not write more
*                        than the size of supplied internal buffer.
*
*                        NonBlocked-Polling Mode:
*
*                        In non-blocked polling mode, this API returns immediately by transmitting data fully
*                        or partially or none. pSize parameter is changed in this mode reflecting the number of
*                        bytes transmitted.Internal buffering is not used in polled mode, so in this mode atmost
*                        one byte can be sent in a single transmit call. If transmit FIFO is not empty then the
*                        driver can not transmit any data so it returns by setting the pSize to zero. In the
*                        polling mode interrupts are not used and devices status registers are used to determine
*                        the success of the operation.
*
*                        NonBlocked-Interrupt Mode:
*
*                        In non-blocked interrupt mode, this API returns immediately by transmitting data fully
*                        or partially or none. pSize parameter is changed by reflecting the number of bytes
*                        transmitted.Internal buffering is used in interrupt mode, so in this application data
*                        is copied into internal driver buffer to the extent that is available. If incoming request
*                        is larger than available internal space then data is partially copied to the internal buffer.
*                        Upon each transmit buffer empty interrupt a byte is sent. pSize parameter is changed
*                        according to the number of bytes that are buffered. As internal buffering is used by the
*                        time this call returns data might not have been transmitted completely on UART.
*                        Applications can re-use its buffer as soon as this call returns.
*
*                        NonBlocked-Interrupt mode is best suited for applications that has parallel receive and
*                        transmit operations.
*
* @note                  Applications has to carefully configure the transmit buffer size depending on
*                        the application requirements and also depending on the other system latencies.
*                        Applications must enable UART by calling adi_UART_Enable before using this API.
*
* @par Side-Effects:
*                        In non-blocking mode *pSize parameter is changed with the number of bytes that are
*                        actually transmitted. So in non-blocking mode adviced to set this value each time
*                        this API is used.
*
* @sa                    adi_UART_Init
* @sa                    adi_UART_GetNumTxBytes
* @sa                    adi_UART_SetInterruptMode
* @sa                    adi_UART_SetBlockingMode
* @sa                    adi_UART_SetDmaMode
* @sa                    adi_UART_Enable
* @sa                    adi_UART_BufRx
*/
ADI_UART_RESULT_TYPE adi_UART_BufTx(ADI_UART_HANDLE const hDevice, const void* const pData, int16_t *pSize)
{
    ADI_UART_RESULT_TYPE Result = ADI_UART_ERR_UNKNOWN;

#if defined(ADI_DEBUG)
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);

    if( hDevice->DrvState != ADI_UART_DRV_STATE_INITIALIZED )
        return ADI_UART_ERR_NOT_INITIALIZED;

    /* validate incoming parameters */
    if( (pData == NULL) || (pSize == NULL) || (*pSize <= 0) )
        return(ADI_UART_ERR_INVALID_PARAMS);
#endif /* defined(ADI_DEBUG) */

    /* process the request depending on the mode of operation */
    Result = ProcessRequest(hDevice,(uint8_t*)pData,pSize,false);

    if (ADI_UART_SUCCESS == Result)
    {
        /* perform the actual transmit */
        Result = do_write(hDevice,(uint8_t*)pData,*pSize);
    }

    return(Result);
}


/*!
* @brief                  Receives Data from UART
*
* @param[in]      hDevice Handle to the device which is returned through adi_UART_Init()
* @param[out]     pData   Pointer to the memory where received bytes are stored
* @param[in,out]  pSize   Pointer to a location where number of bytes to be received is specified.
*
* @return         Status
*                        - #ADI_UART_SUCCESS                  upon success
*                        - #ADI_UART_ERR_INVALID_PARAMS   [D] if invalid parameters are passed
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D] if invalid instance handle is passed
*
* @details
*                        Receives data from the UART. Actual reception of data is dependent on the
*                        operational mode of the driver. Applications can se the driver to operate in
*                        blocking or non-blocking modes. Driver can be configured to use poll, interrupt
*                        or dma in conjuction with the operating modes.
*
*                        Blocked and Non-blocking modes:
*
*                        In blocking mode this api will wait until entire data is obtained. In case of
*                        non-blocking mode this api returns immediately by obtaining data completely or
*                        partially or with no data. In non-blocking mode pSize will be changed with the
*                        number of bytes that are successfully received. In blocking mode pSize parameter
*                        is not changed.
*
*                        Both blocking and non-blocking modes can be used in conjuction with interrupt
*                        or polling configurations. In case of interrupt mode UART receive interrupt is
*                        enabled and upon each receive buffer full interrupt a byte is fetched from the
*                        RX FIFO and stored in the internal buffer. Once internal buffer is full any incoming
*                        data is discarded. Applications can configure the size of the rx internal buffer using
*                        \e pRxBufferData, \e RxBufferSize members of #ADI_UART_INIT_DATA. This data structure is
*                        taken as input in #adi_UART_Init call. In interrupt mode data is accumulated in to
*                        the internal buffer once UART is enabled. So applications need not explicitly invoke
*                        this API to trigger the receive operation. In case of polled configuration internal
*                        buffer is not used and data is not accumulated. Applications have to explicitly issue
*                        this API inorder to receive data. In polled configuration at mode one byte
*                        can be accumulated as the FIFO length is one byte.
*
*                        Applications can use adi_UART_GetNumRxBytes() call to get number of bytes that are
*                        available to read. In case of interrupt configuration this API returns the number of
*                        bytes avaiable in the internal buffer where as in case of polled  operation it returns
*                        the number of bytes in the FIFO.
*
*                        Following sections describe the different operating modes and configurations in detail.
*
*                        Blocked-polling Mode:
*
*                        In blocked-polling mode this API will not return until entire data is completely obtained.
*                        In polling configuration devices status register is used to determine the success of the
*                        transfer. Internal buffering is not used so application's buffer is directly used to store
*                        the incoming data. No memory copies are used in this mode. Data is not accumulated, so
*                        active adi_UART_BufRx will read data in to application buffer. UART has one byte FIFO so
*                        at most one byte is held if there is no active read.
*
*                        Blocked-interrupt Mode:
*
*                        In blocked interrupt mode this API will not return until entire data is obtained. In
*                        interrupt mode UART receive buffr full interrupt is used to get the next byte. Internal
*                        buffering is used in interrupt mode so received data gets accumulated as soon as UART is
*                        enabled. If internal buffer has greater than or equal to the requested number of bytes then
*                        this API immediately returns with the data.Applications has to configure the size of the
*                        rx internal buffer using \e pRxBufferData, \e RxBufferSize members of #ADI_UART_INIT_DATA.
*                        This data structure is taken as input in #adi_UART_Init call.
*
*                        Nonblocked-polling Mode:
*
*                        In non-blocked polling mode, this API returns immediately by receiving data fully or
*                        partially or with no data. pSize parameter is changed reflecting the number of successfully
*                        received bytes. Internal buffering is not used in polled mode so at most one byte be received
*                        in a single receive call. If data is not available by the time this call is issued pSize is
*                        set to zero. In this mode devices status registers are used to determine the success of the
*                        operation.
*
*                        NonBlocked-Interrupt Mode:
*
*                        In non-blocked polling mode, this API returns immediately by receiving data fully or
*                        partially or with no data. pSize parameter is changed reflecting the number of bytes that
*                        are successfully received. In interrupt mode internal buffering is enabled so data can be
*                        accumulated once UART is enabled. The data present in the internal buffer is used for the
*                        read request.
*
* @note                  Applications has to configure the size of the rx internal buffer using \e pRxBufferData,
*                        \e RxBufferSize members of #ADI_UART_INIT_DATA. This data structure is taken as input
*                        in #adi_UART_Init call. The rx buffer size is dependent on the application requirements
*                        and other system latencies. Applications must enable UART using adi_UART_Enable before
*                        using this API.
*
* @par Side-Effects:
*                        In non-blocking mode *pSize parameter is changed with the number of bytes that are
*                        actually transmitted.
*
* @sa                    adi_UART_Init
* @sa                    adi_UART_BufTx
* @sa                    adi_UART_GetNumRxBytes
*/
ADI_UART_RESULT_TYPE adi_UART_BufRx(ADI_UART_HANDLE const hDevice, const void *pData, int16_t *pSize)
{
    ADI_UART_RESULT_TYPE Result = ADI_UART_ERR_UNKNOWN;

#if defined(ADI_DEBUG)
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);

    if( hDevice->DrvState != ADI_UART_DRV_STATE_INITIALIZED )
        return ADI_UART_ERR_NOT_INITIALIZED;

    /* validate incoming parameters */
    if( (pData == NULL) || (pSize == NULL) || (*pSize <= 0) )
        return(ADI_UART_ERR_INVALID_PARAMS);
#endif /* defined(ADI_DEBUG) */


    /* process read request */
    Result = ProcessRequest(hDevice,(uint8_t*)pData,pSize,true);

    if (ADI_UART_SUCCESS == Result)
    {
        /* perform the actual read */
        Result = do_read(hDevice,(uint8_t*)pData,*pSize);
    }

    return(Result);
}


/*!
* @brief                 Flush UART FIFOS
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
*
* @return     Status
*                        - #ADI_UART_SUCCESS                  upon success
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D] if invalid instance handle is passed
*                        - #ADI_UART_ERR_NOT_INITIALIZED  [D] if driver is not initialized
* @details
*                        Performs a dummy read if data is present in rx fifo. Also waits for the
*                        pending transmit operations to finish. Reset the internal driver buffer
*                        entities to their default values.
*
* @sa                    adi_UART_Enable
*/
ADI_UART_RESULT_TYPE adi_UART_BufFlush(ADI_UART_HANDLE const hDevice)
{
#if defined(ADI_DEBUG)
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);

    if( hDevice->DrvState != ADI_UART_DRV_STATE_INITIALIZED )
        return ADI_UART_ERR_NOT_INITIALIZED;
#endif /* defined(ADI_DEBUG) */

    /* check if there is any data to read - if so issue a dummy read */
    if( UART_RX_FULL(hDevice) )
        hDevice->pUartRegs->COMRX;

    /* wait until transmit empty bit set
     * if UART is not enabled then TEMT will never be set which will block
     * this API infinitely so only check Tx FIFO only if UART is enabled.
     */
    while (( !(hDevice->pUartRegs->COMCON & COMCON_DISABLE)) &&
           ( !(hDevice->pUartRegs->COMLSR & COMLSR_TEMT) ));

    ResetCircularBuffers(hDevice);

    return(ADI_UART_SUCCESS);
}


/*!
* @brief                 Set Baud Rate
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
* @param[in]  BaudRateIndex   Specifies the baud rate index
*
* @return     Status
*                        - #ADI_UART_SUCCESS                  upon success
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D] if invalid instance handle is passed
*                        - #ADI_UART_ERR_NOT_INITIALIZED  [D] if driver is not initialized
*                        - #ADI_UART_ERR_INVALID_PARAMS   [D] if invalid parameters were passed
* @details
*                        Configures relevent UART registers pertaining to the given buad rate.
*                        ADI_UART_BAUDRATE_TYPE enumeration defines the currently supported baud rates.
*
* @note                  Custom baud rate configuration is not supported, direct access to the COMDIV or fractional
*                        baud rate registers is not supported. DIVM and DIVN values for the standard baudrates is
*                        chosen from the specification. Applications must disable UART prior to changing the baud rate
*                        and then re-enable it after setting it. UART can be enabled or disabled using
*                        adi_UART_Enable API.
*
* @sa                    adi_UART_GetBaudRate
* @sa                    adi_UART_Enable
*/
ADI_UART_RESULT_TYPE adi_UART_SetBaudRate(ADI_UART_HANDLE const hDevice, const ADI_UART_BAUDRATE_TYPE BaudRateIndex)
{
    uint32_t incoming_clock;

    /* common baud rates and the associated comdiv, divm, divn values from the specification */
    /* other values can be optimized for minimal error based on HRM equation:                */
    /* BaudRate = UCLK / (2*16*COMDIV) / (M+(N/2048)),                                       */
    /*    with:                                                                              */
    /*        1<=COMDIV<=65536,                                                              */
    /*        1<=M<=3, and                                                                   */
    /*        0<=N<=2047                                                                     */
    /* these constants are optimized for and valid only for either a 16MHz or 1MHz clock      */
    typedef struct {
       uint16_t divC;
       uint16_t divM;
       uint16_t divN;
    } DIV_TYPE;

    /* create local divisor tables on stack */
    DIV_TYPE DivMap[][ADI_UART_BAUD_MAX_ENTRIES] = {
		{
            /* the 16MHz table... */
			{ 49, 1, 129 },   /* 0.005937% error */
			{ 19, 1, 759 },   /* 0.000625% error */
			{  4, 3, 523 },   /* 0.005000% error */
			{  8, 1, 174 },   /* 0.010001% error */
			{  3, 1, 915 },   /* 0.001250% error */
			{  2, 1, 174 },   /* 0.010001% error */
			{  1, 1, 174 },   /* 0.010001% error */
		},
		{
            /* the 1MHz table... */
			{ 1, 3,   523 },  /* 0.005000% error */
			{ 1, 1,  1285 },  /* 0.010001% error */
			{ 1, 1,     0 },  /* 18.6% error, nonrealizable solution */
			{ 1, 1,     0 },  /* 45.7% error, nonrealizable solution */
			{ 1, 1,     0 },  /* 72.9% error, nonrealizable solution */
			{ 1, 1,     0 },  /* 86.4% error, nonrealizable solution */
			{ 1, 1,     0 },  /* 93.2% error, nonrealizable solution */
		},
                {
            /* the 20MHz table... */
			{ 17, 3, 1699 },   /* 0.005937% error */
			{ 10, 3, 523 },    /* 0.000625% error */
			{  7, 2, 666 },   /* 0.005000% error */
			{  3, 3, 1263 },   /* 0.010001% error */
			{  2, 2, 1460 },   /* 0.001250% error */
			{  1, 2, 1460 },   /* 0.010001% error */
			{  1, 1, 730 },    /* 0.010001% error */
		}

	};

    /* pointer to table entries */
    DIV_TYPE *pMap;

#if defined(ADI_DEBUG)
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);

    if( hDevice->DrvState != ADI_UART_DRV_STATE_INITIALIZED )
        return ADI_UART_ERR_NOT_INITIALIZED;

    /* validate baudrate index */
    if (BaudRateIndex >= ADI_UART_BAUD_MAX_ENTRIES )
        return ADI_UART_ERR_INVALID_PARAMS;
#endif /* defined(ADI_DEBUG) */


    /* get the system clock */
        // 'M350/360 have dedicated system clocks for UART
        incoming_clock = SystemGetClockFrequency(ADI_SYS_CLOCK_UART);

    /* only supporting 16MHz and 1MHz at this time... other clocks require new div table entries */
	if        (incoming_clock == 16000000) { pMap = &DivMap[0][BaudRateIndex];
	} else if (incoming_clock ==  1000000) { pMap = &DivMap[1][BaudRateIndex];
	} else if (incoming_clock ==  20000000) { pMap = &DivMap[2][BaudRateIndex];
	} else                                 { return ADI_UART_ERR_INVALID_PARAMS; }


    hDevice->pUartRegs->COMDIV = pMap->divC; /* set the main baudrate divider */
    hDevice->pUartRegs->COMFBR =      /* set the fractional baudrate dividers */
        pMap->divN | (pMap->divM << BITP_UART_COMFBR_DIVM) | COMFBR_ENABLE;

    return(ADI_UART_SUCCESS);
}


/*!
* @brief                 Get Baudrate
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
*
* @return                Returns current baud rate
*
* @details
*                        Returns the current baud rate. The returned value might be different from the
*                        value set by adi_UART_SetBaudRate API. This API computes the baud rate from the
*                        COMDIV, fractional baud rate register values.
*
* @sa                    adi_UART_SetBaudRate
*
*/
uint32_t  adi_UART_GetBaudRate(const ADI_UART_HANDLE hDevice)
{
    uint32_t incoming_clock;
    uint16_t ComDiv = hDevice->pUartRegs->COMDIV;
    uint16_t divN   = hDevice->pUartRegs->COMFBR & COMFBR_DIVN_MSK;
    uint8_t  divM   = (hDevice->pUartRegs->COMFBR & COMFBR_DIVM_MSK) >> 11;

    /* get the system clock */
        // 'M350/360 have dedicated system clocks for UART
        incoming_clock = SystemGetClockFrequency(ADI_SYS_CLOCK_UART);

   /* Formula to compute Baud Rate =  (UCLK) / ( 2 x 16 x (M + N/2048) * COMDIV )
    * rearranged to remove integer division, Baud Rate = (UCLK << 6) / (((M << 11) + N) * COMDIV)
    */
    return ((incoming_clock  << 6) / ( ( (divM << 11) + divN) * ComDiv));
}


/*!
* @brief                 Set Generic Settings
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
* @param[in]  pSettings  Pointer to the generic settings
*
* @return     Status
*                        - #ADI_UART_SUCCESS                  upon success
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D] if invalid instance handle is passed
*                        - #ADI_UART_ERR_NOT_INITIALIZED  [D] if driver is not initialized
*                        - #ADI_UART_ERR_INVALID_PARAMS   [D] if invalid parameters were passed
* @details
*                        Generic UART settings can be changed using this API. The generic settings
*                        include baud rate, word length, parity. Driver operating mode could also be
*                        changed using this API. This API has to be called only when UART is disabled and
*                        re-enable UART once new settings are set.
*
* @par Side-Effects:     This API uses the passed in values, so all elements of ADI_ADC_DEV_GENERIC_SETTINGS_TYPE
*                        structure must be initialized to the desired values else they may get propaged with
*                        un-intended values.
*
* @sa                    adi_UART_GetGenericSettings
* @sa                    adi_UART_Enable
*/
ADI_UART_RESULT_TYPE adi_UART_SetGenericSettings( ADI_UART_HANDLE const hDevice,
                                                 ADI_UART_GENERIC_SETTINGS_TYPE* const pSettings )
{
    ADI_UART_RESULT_TYPE Result = ADI_UART_ERR_UNKNOWN;

#if defined(ADI_DEBUG)
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);

    if( hDevice->DrvState != ADI_UART_DRV_STATE_INITIALIZED )
        return ADI_UART_ERR_NOT_INITIALIZED;

    if( pSettings == NULL)
        return ADI_UART_ERR_INVALID_PARAMS;
#endif /* defined(ADI_DEBUG) */

    Result = adi_UART_SetBaudRate(hDevice,pSettings->BaudRate);

    if (ADI_UART_SUCCESS != Result)
        return Result;

    Result = adi_UART_SetWordLength(hDevice,pSettings->WordLength);

    if (ADI_UART_SUCCESS != Result)
        return Result;

    Result = adi_UART_SetParityMode(hDevice,pSettings->Parity);

    if (ADI_UART_SUCCESS != Result)
        return Result;

    Result = adi_UART_SetBlockingMode(hDevice,pSettings->bBlockingMode);

    if (ADI_UART_SUCCESS != Result)
        return Result;

    adi_UART_SetInterruptMode(hDevice,pSettings->bInterruptMode);

    return(Result);
}


/*!
* @brief                 Get Generic Settings
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
* @param[in]  pSettings  Pointer to the memory location where the current setting will be returned.
*
* @return     Status
*                        - #ADI_UART_SUCCESS                  upon success
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D] if invalid instance handle is passed
*                        - #ADI_UART_ERR_NOT_INITIALIZED  [D] if driver is not initialized
*                        - #ADI_UART_ERR_INVALID_PARAMS   [D] if invalid parameters were passed
* @details
*                        Generic UART settings can be retrieved using this API. The generic settings
*                        include baud rate, word length, parity. Driver operating mode could also be
*                        retrieved using this API.
*
* @note                  Note that this API will not return the enumerated BaudRate but rather computes
*                        the actual configured baudrate and returns using ConfiguredBaudRate parameter.
*                        BaudRate member will not be set.
*
* @sa                    adi_UART_SetGenericSettings
* @sa                    adi_UART_Enable
*/
ADI_UART_RESULT_TYPE adi_UART_GetGenericSettings( ADI_UART_HANDLE const hDevice,
                                                 ADI_UART_GENERIC_SETTINGS_TYPE* const pSettings )
{
#if defined(ADI_DEBUG)
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);

    if( hDevice->DrvState != ADI_UART_DRV_STATE_INITIALIZED )
        return ADI_UART_ERR_NOT_INITIALIZED;

    if( pSettings == NULL)
        return ADI_UART_ERR_INVALID_PARAMS;
#endif /* defined(ADI_DEBUG) */

     pSettings->ConfiguredBaudRate  = adi_UART_GetBaudRate(hDevice);
     pSettings->WordLength          = adi_UART_GetWordLength(hDevice);
     pSettings->Parity              = adi_UART_GetParityMode(hDevice);
     pSettings->bBlockingMode       = adi_UART_GetBlockingMode(hDevice);
     pSettings->bInterruptMode      = adi_UART_GetInterruptMode(hDevice);
     pSettings->bDmaMode            = adi_UART_GetDmaMode(hDevice);

     return ADI_UART_SUCCESS;
}


/*!
* @brief                 Set Loopback
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
* @param[in]  bFlag      True then the driver operates in loopback mode
*
* @return     Status
*                        - #ADI_UART_SUCCESS                  upon success
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D] if invalid instance handle is passed
* @details
*                        Sets the driver to operate in loopback mode. This mode can be used with
*                        out a UART cable connected to host pc or hyperterminal. Applications must
*                        disable UART prior to changing the loopback mode and re-enable it after
*                        setting the value. UART can be enabled or disabled using adi_UART_Enable API.
*
* @sa                    adi_UART_GetLoopback
* @sa                    adi_UART_Enable
*/
ADI_UART_RESULT_TYPE adi_UART_SetLoopback(ADI_UART_HANDLE const hDevice, const bool_t bFlag)
{
#if defined(ADI_DEBUG)
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);
#endif /* defined(ADI_DEBUG) */

    if( bFlag )
        hDevice->pUartRegs->COMMCR |= COMMCR_LOOPBACK;
    else
        hDevice->pUartRegs->COMMCR &= ~COMMCR_LOOPBACK;

    return(ADI_UART_SUCCESS);
}


/*!
* @brief                 Get Loopback
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
*
* @return                True if driver is operating in loopback mode else False is returned.
*
* @details
*                        Returns true if driver is operating in loopback mode. False if driver is not
*                        operating in loopback mode. In loopback mode transmitted bytes are put on
*                        receive channel of the UART.
*
* @sa                    adi_UART_SetLoopback
*/
bool_t adi_UART_GetLoopback(const ADI_UART_HANDLE hDevice)
{
    return( (hDevice->pUartRegs->COMMCR & COMMCR_LOOPBACK) ? true : false);
}


/*!
* @brief                 Set Stop Bit
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
* @param[in]  bFlag      If bFlag is true then STOP bit is enabled, if false then stop bit is disabled
*
* @return     Status
*                        - #ADI_UART_SUCCESS                  upon success
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D] if invalid instance handle is passed
*
* @details
*                        Enables or disables stop bit. Number of stop bits used depends on the word length (WL)
*                        # WL of 5 bits - 1.5 stop bits
*                        # WL of 6,7,8 bits - 2 stop bits
*
* @note                  Hardware generates the required number of stop bits depending on the word length when
*                        stop enable bit is set in comlcr. Applications must disable UART prior to setting the
*                        stop bit and re-enable it after setting. UART can be enabled or disabled using
*                        adi_UART_Enable API.
*
* @sa                    adi_UART_GetStopBit
*/
ADI_UART_RESULT_TYPE adi_UART_SetStopBit(ADI_UART_HANDLE const hDevice, const bool_t bFlag)
{
#if defined(ADI_DEBUG)
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);
#endif /* defined(ADI_DEBUG) */

    if( bFlag )
         hDevice->pUartRegs->COMLCR |= (COMLCR_STOP);
    else
         hDevice->pUartRegs->COMLCR &= ~(COMLCR_STOP);

    return(ADI_UART_SUCCESS);
}


/*!
* @brief                 Get Stop bit
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
*
* @return                True if stop bit is enabled false otherwise
*
* @details               Returns true if stop bits are enabled. Number of stop bits is dependent
*                        on the word length.
*
* @sa                    adi_UART_SetStopBit
*/
bool_t adi_UART_GetStopBit(ADI_UART_HANDLE const hDevice)
{
    return( (hDevice->pUartRegs->COMLCR & COMLCR_STOP) ? true : false);
}


/*!
* @brief                 Set Break bit
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
* @param[in]  bFlag      If bFlag is true then break bit is set, if false then break control is disabled
*
* @return     Status
*                        - #ADI_UART_SUCCESS                  upon success
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D] if invalid instance handle is passed
*
* @details               Set by user to force the TxD to 0. Applications must disable UART before setting the
*                        break bit and re-enable after setting it. UART can be enabled or disabled by using
*                        adi_UART_Enable API.
*
* @sa                    adi_UART_GetBreakBit
* @sa                    adi_UART_Enable
*/
ADI_UART_RESULT_TYPE adi_UART_SetBreakBit(ADI_UART_HANDLE const hDevice, const bool_t bFlag)
{
#if defined(ADI_DEBUG)
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);
#endif /* defined(ADI_DEBUG) */

    if( bFlag )
         hDevice->pUartRegs->COMLCR |= (COMLCR_BRK);
    else
         hDevice->pUartRegs->COMLCR &= ~(COMLCR_BRK);

    return(ADI_UART_SUCCESS);
}


/*!
* @brief                 Get break bit
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
*
* @return                True if break bit is enabled false otherwise
*
* @details               Returns true if break bit is enabled.
*
* @sa                    adi_UART_SetBreakBit
*/
bool_t adi_UART_GetBreakBit(ADI_UART_HANDLE const hDevice)
{
    return( (hDevice->pUartRegs->COMLCR & COMLCR_BRK) ? true : false);
}


/*!
* @brief                 Set parity mode
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
* @param[in]  Parity     Enumeration value that specifies the parity that needs to be set
*
*                        Valid Parity values are
*
*                        - #ADI_UART_PARITY_NONE
*                        - #ADI_UART_PARITY_ODD
*                        - #ADI_UART_PARITY_EVEN
*
* @return     Status
*                        - #ADI_UART_SUCCESS                  upon success
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D] if invalid instance handle is passed
*
* @details
*                        API is used to set the parity. Supported parity values are enumerated using the
*                        ADI_UART_PARITY_TYPE. If odd or even parity is selected parity is automatically
*                        enabled. This api does not force the parity,i.e stick parity bit of COMLCR is not
*                        changed by this API. Applications has to disable UART prior to setting the parity
*                        mode and re-enable UART after setting the parity mode. UART can be enabled or disabled
*                        by adi_UART_Enable API.
*
* @sa                    adi_UART_GetParityMode
* @sa                    adi_UART_Enable
*/
ADI_UART_RESULT_TYPE adi_UART_SetParityMode(ADI_UART_HANDLE const hDevice, const ADI_UART_PARITY_TYPE Parity)
{
#if defined(ADI_DEBUG)
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);
#endif /* defined(ADI_DEBUG) */

    /* configure parity bits */
    switch( Parity )
    {
        /* no parity  */
        case ADI_UART_PARITY_NONE:
            hDevice->pUartRegs->COMLCR &= ~(COMLCR_PEN | COMLCR_EPS | COMLCR_SP);
            break;

        /* odd parity */
        case ADI_UART_PARITY_ODD:
            hDevice->pUartRegs->COMLCR &= ~(COMLCR_EPS | COMLCR_SP);
            hDevice->pUartRegs->COMLCR |=  (COMLCR_PEN);
            break;

        /* even parity */
        case ADI_UART_PARITY_EVEN:
            hDevice->pUartRegs->COMLCR |= ( COMLCR_PEN | COMLCR_EPS | COMLCR_SP );
            break;

        default:
            return(ADI_UART_ERR_INVALID_PARAMS);
    }
    return(ADI_UART_SUCCESS);
}


/*!
* @brief            Get parity mode
*
* @param  hDevice   Handle to the device which is returned through adi_UART_Init()
*
* @return           Returns the enumeration specifying the current parity
*                   Valid Parity values are
*                   - #ADI_UART_PARITY_NONE
*                   - #ADI_UART_PARITY_ODD
*                   - #ADI_UART_PARITY_EVEN
*
* @details
*
* @sa               adi_UART_SetParityMode
*/
ADI_UART_PARITY_TYPE adi_UART_GetParityMode(const ADI_UART_HANDLE hDevice)
{
    /* if parity bit is not enabled return ADI_UART_PARITY_NONE */
    if( !(hDevice->pUartRegs->COMLCR & COMLCR_PEN) )
        return(ADI_UART_PARITY_NONE);

    /* check EPS bit in COMLCR for even or odd parity */
    return( (hDevice->pUartRegs->COMLCR & COMLCR_EPS)
           ? ADI_UART_PARITY_EVEN
           : ADI_UART_PARITY_ODD);
}


/*!
* @brief            Set word length
*
* @param  hDevice   Handle to the device which is returned through adi_UART_Init()
* @param WordLength Enumeration value that specifies the number of bits in a word
*                   Valid \em Parity values are
*                   - #ADI_UART_WLS_5
*                   - #ADI_UART_WLS_6
*                   - #ADI_UART_WLS_7
*                   - #ADI_UART_WLS_8
*
* @return Status
*                  - #ADI_UART_SUCCESS                    upon success
*                  - #ADI_UART_ERR_INVALID_INSTANCE  [D]  if invalid instance handle is passed
*                  - #ADI_UART_ERR_INVALID_PARAMS    [D]  if invalid parameter is passed
*
* @details
*                  set word length select bits.
*
* @sa               adi_UART_GetWordLength
*/
ADI_UART_RESULT_TYPE adi_UART_SetWordLength(ADI_UART_HANDLE const hDevice, const ADI_UART_WLS_TYPE WordLength)
{
#if defined(ADI_DEBUG)
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);
#endif /* defined(ADI_DEBUG) */

    /* Configure data bits */
    switch( WordLength )
    {
        /* This implementation depends on the ADI_UART_WLS_TYPE enum using the
         * same numeric codes for the word lengths as the COMLCR_WLS_ macros.
         * adi_UART_GetWordLength(), below, also makes the same assumption.
         */
        case ADI_UART_WLS_5:        /* word length of 5 bits */
        case ADI_UART_WLS_6:        /* word length of 6 bits */
        case ADI_UART_WLS_7:        /* word length of 7 bits */
        case ADI_UART_WLS_8:        /* word length of 8 bits */
            hDevice->pUartRegs->COMLCR &= ~(COMLCR_WLS_MSK); /* clear the bits */
            hDevice->pUartRegs->COMLCR |= (uint16_t)WordLength;
            break;

#if defined(ADI_DEBUG)
        default:
            return(ADI_UART_ERR_INVALID_PARAMS);
#endif /* defined(ADI_DEBUG) */

    }
    return(ADI_UART_SUCCESS);
}


/*!
* @brief            Get Word length
*
* @param  hDevice   Handle to the device which is returned through adi_UART_Init()
*
* @return           WordLength  Enumeration value that specifies the number of bits in a word
*                   Valid Parity values are
*                   - #ADI_UART_WLS_5
*                   - #ADI_UART_WLS_6
*                   - #ADI_UART_WLS_7
*                   - #ADI_UART_WLS_8
*
* @sa               adi_UART_SetWordLength
*/
ADI_UART_WLS_TYPE adi_UART_GetWordLength(const ADI_UART_HANDLE hDevice)
{
    return((ADI_UART_WLS_TYPE)(hDevice->pUartRegs->COMLCR & COMLCR_WLS_MSK));
}


/*!
* @brief                 Enables DMA
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
* @param[in]  bFlag      if bFlag is true then the driver uses DMA to transfer data
*
* @return     Status
*                        - #ADI_UART_SUCCESS                  upon success
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D] if invalid instance handle is passed
*
* @details
*                        Enables DMA with in the driver
*
* @sa                    adi_UART_GetDmaMode
* @sa                    adi_UART_Enable
*/
ADI_UART_RESULT_TYPE adi_UART_SetDmaMode(ADI_UART_HANDLE const hDevice, const bool_t bFlag)
{
#if defined(ADI_DEBUG)
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);
#endif /* defined(ADI_DEBUG) */

    /* dma is not supported */
    return(ADI_UART_ERR_NOT_SUPPORTED);
}


/*!
* @brief                 Get DMA Mode
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
*
* @return                True  if DMA is enabled
*                        False if DMA is disabled
*
* @details
*                        Returns true if driver is operating in loopback mode.
*
* @sa                    adi_UART_SetDmaMode
*/
bool_t adi_UART_GetDmaMode(const ADI_UART_HANDLE hDevice)
{
    return(hDevice->bDMAMode);
}

/*!
* @brief                 Enable UART
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
* @param[in]  bFlag      Flag is true then UART is enabled, if it is false then UART is disabled
*
* @return     Status
*                        - #ADI_UART_SUCCESS                  upon success
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D] if invalid instance handle is passed
*
* @details
*
* @sa                    adi_UART_GetEnable
*/
ADI_UART_RESULT_TYPE adi_UART_Enable(ADI_UART_HANDLE const hDevice, const bool_t bFlag)
{
#if defined(ADI_DEBUG)
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);
#endif /* defined(ADI_DEBUG) */

    /* enable or disable uart */
    if(bFlag)
        hDevice->pUartRegs->COMCON &= ~(COMCON_DISABLE);
    else
        hDevice->pUartRegs->COMCON |= (COMCON_DISABLE);

	/* gate the dedicated UART core clock on/off as well, starting with the ADuCM360 */
    SystemEnableClock(ADI_SYS_CLOCK_GATE_UART, bFlag);

    return(ADI_UART_SUCCESS);
}


/*!
* @brief                 Get Enable
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
*
* @return                True  if uart is enabled
*                        False if uart is disabled
*
* @details
*
* @sa                    adi_UART_Enable
*/
bool_t adi_UART_GetEnable(const ADI_UART_HANDLE hDevice)
{
  return ((hDevice->pUartRegs->COMCON & COMCON_DISABLE) ? false : true);
}


/*!
* @brief                 Set interrupt mode
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
* @param[in]  bFlag      Specifies the driver interrupt mode. If true driver operates in
*                        interrupt mode else it operates in polling mode.
*
* @return     Status
*                        - #ADI_UART_SUCCESS                  upon success
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D] if invalid instance handle is passed
*                        - #ADI_UART_ERR_INVALID_BUFFER   [D] if buffers are not supplied in #adi_UART_Init
*
* @details
*                        Set interrupt mode API allows applications to change the driver operating mode
*                        to interrupt. By default driver operates in polled mode where status registers
*                        are used to determine the transfer completion. In interrupt mode interrupts are
*                        used for transmitting or receiving data. Transmit and receive operations can be
*                        carried out in interrupt mode. The advantage of interrupt mode is data is transmitted
*                        or received asynchronously. In case of receive operations, data is accumulated into
*                        an internal driver buffer upon each rx interrupt. When application issues BufRx api
*                        this data is copied to user buffer.  Similarly, transmitted data is copied to drivers
*                        internal data buffer and upon each transmit empty interrupt another byte is sent across.
*                        Interrupt mode is more efficient as it reduces the processor loading. Applications can
*                        use either interrupt mode or polled mode. Both transmit and receive operations will use
*                        the same mode, i.e either polling mode or interrupt mode.
* @note
*                        Applications has to disable UART before changing the interrupt mode and re-enable
*                        it after changing.
*
* @sa                   adi_UART_GetInterruptMode
* @sa                   adi_UART_Enable
*/
ADI_UART_RESULT_TYPE adi_UART_SetInterruptMode(ADI_UART_HANDLE const hDevice, const bool_t bFlag)
{
#if defined(ADI_DEBUG)
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);
#endif /* defined(ADI_DEBUG) */

#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)

#if defined(ADI_DEBUG)
    /* check whether buffer is supplied by the application */
    if( bFlag && ((hDevice->RxBuffer.Buffer == NULL) || ( hDevice->TxBuffer.Buffer == NULL)))
        return(ADI_UART_ERR_INVALID_BUFFER);
#endif /* defined(ADI_DEBUG) */

    /* set the interrupt mode */
    hDevice->bInterruptMode = bFlag;

    /* Enable Receive interrupt */
    if(bFlag)
        hDevice->pUartRegs->COMIEN |= (COMIEN_ERBFI /*| COMIEN_ETBEI*/);
    else
        hDevice->pUartRegs->COMIEN &= ~(COMIEN_ERBFI /*| COMIEN_ETBEI*/);

    /* enable rx line status interrupt to trap error conditions */
    hDevice->pUartRegs->COMIEN |= COMIEN_ELSI;

#else /* (0 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */

    if (bFlag)
        return(ADI_UART_ERR_NOT_SUPPORTED);

#endif /* !(1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */


    return(ADI_UART_SUCCESS);
}


/*!
* @brief                 Get Interrupt Mode
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
*
* @return                True  if driver is operating in interrupt mode
*                        False if driver is operating in polled mode (default)
*
* @details               Returns true if driver is operating in interrupt mode, false if driver
*                        is operating in polling mode.
*
* @sa                    adi_UART_SetInterruptMode
*/
bool_t adi_UART_GetInterruptMode(const ADI_UART_HANDLE hDevice)
{
    return IS_INTERRUPT_MODE(hDevice);
}


/*!
* @brief                 Set blocking mode
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
* @param[in]  bFlag      if true driver operates in blocking mode. if false driver operates
*                        in non-blocking mode.
*
* @return     Status
*                        - #ADI_UART_SUCCESS                   upon success
*                        - #ADI_UART_ERR_INVALID_INSTANCE [D]  if invalid instance handle is passed
*
* @details               Puts the driver in blocking mode. In Blocking mode receive and transmit APIs will not
*                        return until requested data has been received or transmitted.
* @note
*                        Applications has to disable UART before changing the blocking mode and re-enable
*                        it after changing.
*
* @sa                    adi_UART_GetBlockingMode
* @sa                    adi_UART_Enable
*/
ADI_UART_RESULT_TYPE adi_UART_SetBlockingMode(ADI_UART_HANDLE const hDevice, const bool_t bFlag)
{
#if defined(ADI_DEBUG)
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);
#endif /* defined(ADI_DEBUG) */

    hDevice->bBlockingMode = bFlag;

    return(ADI_UART_SUCCESS);
}


/*!
* @brief                 Get Blocking Mode
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
*
* @return                True  if driver is operating in blocking mode
*                        False if driver is operating in non-blocking mode
*
* @details               Returns true if driver is operating in blocking mode, false if driver
*                        is operating in non-blocking mode.
*
* @sa                    adi_UART_SetBlockingMode
*
*/
bool_t adi_UART_GetBlockingMode(const ADI_UART_HANDLE hDevice)
{
    return IS_BLOCKING_MODE(hDevice);
}


/*!
* @brief                 Set Stick parity
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
* @param[in]  bFlag      If true stick parity bit is set, else stick parity is cleared.
*
* @return     Status
*                        - #ADI_UART_SUCCESS                upon success
*                        - #ADI_UART_ERR_INVALID_INSTANCE   if invalid instance handle is passed
*
* @details
*                        Stick parity (SP) bit of COMLCR forces the parity to the defined values
*                        SP is 1 if EPS = 1 and PEN = 1  (Even parity )
*                        SP is 0 if EPS = 0 and PEN = 1  (Odd  parity )
*
* @note                  Applications and issue transmit and receive only after UART is enabled.
*
* @sa                    adi_UART_GetStickParity
* @sa                    adi_UART_Enable
*/
ADI_UART_RESULT_TYPE adi_UART_SetStickParity(ADI_UART_HANDLE const hDevice, const bool_t bFlag)
{
#if defined(ADI_DEBUG)
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);
#endif /* defined(ADI_DEBUG) */

    if(bFlag)
        hDevice->pUartRegs->COMLCR |= COMLCR_SP;
    else
        hDevice->pUartRegs->COMLCR &= ~COMLCR_SP;

    return(ADI_UART_SUCCESS);
}


/*!
* @brief                 Get stick parity
*
* @param[in] hDevice     Handle to the device which is returned through adi_UART_Init()
*
* @return                True  if stick parity bit is set
*                        False if stick parity bit is cleared
*
* @details               Returns true then uart is using even parity (EPS=1 PEN=1 else
*                        uart is using odd parity (EPS=0 and PEN = 1)
*
* @sa                    adi_UART_SetStickParity
* @sa                    adi_UART_Enable
*/
bool_t adi_UART_GetStickParity(const ADI_UART_HANDLE hDevice)
{
    return( (hDevice->pUartRegs->COMLCR & COMLCR_SP) ? true : false);
}


/*!
* @brief                 Checks for the data availability
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
*
* @return                0 if no data is availble or
*                        number of bytes available
*
* @details               In case of interrupt mode the number of bytes accumulated in the
*                        buffer were returned. In case of polling mode at most one byte
*                        can present in the receive fifo. So in polling mode this call
*                        may return 0 if there is no data in the fifo or 1 if a byte is
*                        present in the rx fifo.
*
* @sa                    adi_UART_GetNumTxBytes
* @sa                    adi_UART_BufRx
*/
uint16_t adi_UART_GetNumRxBytes(const ADI_UART_HANDLE hDevice)
{
    int16_t BytesAvailable = 0;

#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
    /* interrupt mode data is buffered */
    if( IS_INTERRUPT_MODE(hDevice) )
    {
        BytesAvailable =  hDevice->RxBuffer.NumAvailable;
    }
#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */
#if (1 == ADI_UART_CFG_POLLED_MODE_SUPPORT)
    if( IS_POLLED_MODE(hDevice) ) /* polling mode - check the LSR */
    {
        BytesAvailable = (UART_RX_FULL(hDevice) ? 1 : 0);
    }
#endif /* (1 == ADI_UART_CFG_POLLED_MODE_SUPPORT) */

    return(BytesAvailable);
}


/*!
* @brief                 Returns number of bytes that can be transmitted
*
* @param[in]  hDevice    Handle to the device which is returned through adi_UART_Init()
*
* @return                0 if none can be transmitted
*                        number of bytes that can be transmitted
*
* @details
*
*/
uint16_t adi_UART_GetNumTxBytes(const ADI_UART_HANDLE hDevice)
{
    int16_t BytesAvailable = 0;

#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
    /* interrupt mode data is buffered */
    if( IS_INTERRUPT_MODE(hDevice) )
    {
        BytesAvailable =  hDevice->TxBuffer.NumAvailable;
    }
#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */

#if (1 == ADI_UART_CFG_POLLED_MODE_SUPPORT)
    if( IS_POLLED_MODE(hDevice) ) /* polling mode - check the LSR */
    {
        BytesAvailable = (hDevice->pUartRegs->COMLSR & COMLSR_THRE) ? 1 : 0;
    }
#endif /* (1 == ADI_UART_CFG_POLLED_MODE_SUPPORT) */

    return(BytesAvailable);
}


#if defined(ADI_DEBUG)
/*!
* @brief                  Returns the statistics
*
* @param[in]   hDevice    Handle to the device which is returned through adi_UART_Init()
* @param[out]  pUartStats Data pointer to where the statistics were collected.
*
* @return                 ADI_UART_ERR_NOT_SUPPORTED when ADI_DEBUG is not defined
*                         ADI_UART_SUCCESS           upon successful copy of the data
*
* @details                This API returns the currently collected UART statistics. ADI_UART_STATS_TYPE structure
*                         lists down the various statistical entities that are collected by the driver. This API
*                         is only valid in debug build.
*
* @note                   Once data has been copied to user data area the internal structure
*                         is reset to zero
*/
ADI_UART_RESULT_TYPE adi_UART_GetStatistics(ADI_UART_HANDLE const hDevice,ADI_UART_STATS_TYPE* const pUartStats)
{
    /* check the instance */
    if( hDevice != &UART_DevData[ADI_UART_DEVID_0] )
        return(ADI_UART_ERR_INVALID_INSTANCE);

#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
    ADI_DISABLE_INT(hDevice->UARTIRQn);
#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */

    /* copy the structure to the user data block */
    memcpy(pUartStats,&hDevice->Stats,sizeof(ADI_UART_STATS_TYPE));
    /* reset statistical counters to 0 */
    memset(&hDevice->Stats,0,sizeof(ADI_UART_STATS_TYPE));

#if (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT)
    ADI_ENABLE_INT(hDevice->UARTIRQn);
#endif /* (1 == ADI_UART_CFG_INTERRUPT_MODE_SUPPORT) */

    return(ADI_UART_SUCCESS);
}
#endif /* defined(ADI_DEBUG) */


#define REVERT_ADI_MISRA_SUPPRESSIONS  /*!< Revert ADI MISRA Suppressions */
#include "misra.h"

/*
** EOF
*/

/*@}*/
