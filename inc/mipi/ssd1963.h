/* Solomon Systech SSD1963 LCD Display Controller include file */

#include "pdi.h"  /* To get MIPI commands that are sent by PDI command/fifo registers */

typedef enum  /* These commands are device-specific and should not be visible at the application layer */
{
    /* Note: "CMD0" is a zero-parameter command, "CMD1" is a one-parameter command, etc. and "CMDN" is an N-parameter command */

    /* The following commands map directly to the MIPI DCS Spec V1.02.00 (with some exceptions, see comments) */
    SSD1963_CMD0_ENTER_IDLE_MODE                = 0x39,
    SSD1963_CMD0_ENTER_INVERT_MODE              = 0x21,
    SSD1963_CMD0_ENTER_NORMAL_MODE              = 0x13,
    SSD1963_CMD0_ENTER_PARTIAL_MODE             = 0x12,
    SSD1963_CMD0_ENTER_SLEEP_MODE               = 0x10,
    SSD1963_CMD0_EXIT_IDLE_MODE                 = 0x38,
    SSD1963_CMD0_EXIT_INVERT_MODE               = 0x20,
    SSD1963_CMD0_EXIT_SLEEP_MODE                = 0x11,
    SSD1963_CMD1_GET_ADDRESS_MODE               = 0x0B,
    SSD1963_CMD1_GET_DISPLAY_MODE               = 0x0D,
    SSD1963_CMD1_GET_POWER_MODE                 = 0x0A,
    SSD1963_CMD2_GET_SCANLINE                   = 0x45,
    SSD1963_CMD1_GET_TEAR_EFFECT_STATUS         = 0x0E, // Equivalent to get_signal_mode in the MIPI DCS Spec V1.02.00
    SSD1963_CMD0_NOP                            = 0x00,
    SSD1963_CMD5_READ_DDB                       = 0xA1, // Equivalent to read_DDB_start in the MIPI DCS Spec V1.02.00
    SSD1963_CMDN_READ_MEMORY_CONTINUE           = 0x3E,
    SSD1963_CMDN_READ_MEMORY_START              = 0x2E,
    SSD1963_CMD1_SET_ADDRESS_MODE               = 0x36,
    SSD1963_CMD4_SET_COLUMN_ADDRESS             = 0x2A,
    SSD1963_CMD0_SET_DISPLAY_OFF                = 0x28,
    SSD1963_CMD0_SET_DISPLAY_ON                 = 0x29,
    SSD1963_CMD1_SET_GAMMA_CURVE                = 0x26,
    SSD1963_CMD4_SET_PAGE_ADDRESS               = 0x2B,
    SSD1963_CMD4_SET_PARTIAL_AREA               = 0x30, // Equivalent to set_partial_rows in the MIPI DCS Spec V1.02.00
    SSD1963_CMD6_SET_SCROLL_AREA                = 0x33,
    SSD1963_CMD2_SET_SCROLL_START               = 0x37,
    SSD1963_CMD0_SET_TEAR_OFF                   = 0x34,
    SSD1963_CMD1_SET_TEAR_ON                    = 0x35,
    SSD1963_CMD2_SET_TEAR_SCANLINE              = 0x44,
    SSD1963_CMD0_SOFT_RESET                     = 0x01,
    SSD1963_CMDN_WRITE_MEMORY_CONTINUE          = 0x3C,
    SSD1963_CMDN_WRITE_MEMORY_START             = 0x2C,

    /* The following commands are specific to the SSD1963 */
    SSD1963_CMD7_SET_LCD_MODE                   = 0xB0,  // Set the LCD panel mode and resolution.
    SSD1963_CMD7_GET_LCD_MODE                   = 0xB1,  // Get the current LCD panel mode and resolution.
    SSD1963_CMD8_SET_HORI_PERIOD                = 0xB4,  // Set front porch and back porch.
    SSD1963_CMD8_GET_HORI_PERIOD                = 0xB5,  // Get current front porch and back porch settings.
    SSD1963_CMD7_SET_VERT_PERIOD                = 0xB6,  // Set the vertical blanking interval between last scan line and next
                                                         // LFRAME pulse.
    SSD1963_CMD7_GET_VERT_PERIOD                = 0xB7,  // Get the vertical blanking interval between last scan line and next
                                                         // LFRAME pulse.
    SSD1963_CMD2_SET_GPIO_CONF                  = 0xB8,  // Set the GPIOs configuration . If the GPIOs are not used for LCD, 
                                                         // set the direction. Otherwise, they are toggled with LCD signals by 
                                                         // 0xC0 - 0xCF.
    SSD1963_CMD2_GET_GPIO_CONF                  = 0xB9,  // Get the current GPIOs configuration.
    SSD1963_CMD1_SET_GPIO_VALUE                 = 0xBA,  // Set GPIO value for GPIO configured as output.
    SSD1963_CMD1_GET_GPIO_STATUS                = 0xBB,  // Read current GPIO status. If the individula GPIO was configured as
                                                         // input, the value is the status of the corresponding pin. Otherwise
                                                         // it is the programmed value.
    SSD1963_CMD4_SET_POST_PROC                  = 0xBC,  // Set the image post processor.
    SSD1963_CMD4_GET_POST_PROC                  = 0xBD,  // Get the image post processor.
    SSD1963_CMD6_SET_PWM_CONF                   = 0xBE,  // Set the PWM configuration.
    SSD1963_CMD7_GET_PWM_CONF                   = 0xBF,  // Get the PWM configuration.
    SSD1963_CMD7_SET_LCD_GEN0                   = 0xC0,  // Set the rise, fall, period and toggling properties of LCD signal
                                                         // generator 0.
    SSD1963_CMD7_GET_LCD_GEN0                   = 0xC1,  // Get the rise, fall, period and toggling properties of LCD signal
                                                         // generator 0.
    SSD1963_CMD7_SET_LCD_GEN1                   = 0xC2,  // Set the rise, fall, period and toggling properties of LCD signal
                                                         // generator 1.
    SSD1963_CMD7_GET_LCD_GEN1                   = 0xC3,  // Get the rise, fall, period and toggling properties of LCD signal
                                                         // generator 1.
    SSD1963_CMD7_SET_LCD_GEN2                   = 0xC4,  // Set the rise, fall, period and toggling properties of LCD signal
                                                         // generator 2.
    SSD1963_CMD7_GET_LCD_GEN2                   = 0xC5,  // Get the rise, fall, period and toggling properties of LCD signal
                                                         // generator 2.
    SSD1963_CMD7_SET_LCD_GEN3                   = 0xC6,  // Set the rise, fall, period and toggling properties of LCD signal
                                                         // generator 3.
    SSD1963_CMD7_GET_LCD_GEN3                   = 0xC7,  // Get the rise, fall, period and toggling properties of LCD signal
                                                         // generator 3.
    SSD1963_CMD2_SET_GPIO0_ROP                  = 0xC8,  // Set the GPIO0 with respect to the LCD signal generators using ROP
                                                         // operation. No effect if the GPIO0 is configured as general GPIO.
    SSD1963_CMD2_GET_GPIO0_ROP                  = 0xC9,  // Get the GPIO0 properties with respect to the LCD signal generators.
    SSD1963_CMD2_SET_GPIO1_ROP                  = 0xCA,  // Set the GPIO1 with respect to the LCD signal generators using ROP
                                                         // operation. No effect if the GPIO1 is configured as general GPIO.
    SSD1963_CMD2_GET_GPIO1_ROP                  = 0xCB,  // Get the GPIO1 properties with respect to the LCD signal generators.
    SSD1963_CMD2_SET_GPIO2_ROP                  = 0xCC,  // Set the GPIO2 with respect to the LCD signal generators using ROP
                                                         // operation. No effect if the GPIO2 is configured as general GPIO.
    SSD1963_CMD2_GET_GPIO2_ROP                  = 0xCD,  // Get the GPIO2 properties with respect to the LCD signal generators.
    SSD1963_CMD2_SET_GPIO3_ROP                  = 0xCE,  // Set the GPIO3 with respect to the LCD signal generators using ROP
                                                         // operation. No effect if the GPIO3 is configured as general GPIO.
    SSD1963_CMD2_GET_GPIO3_ROP                  = 0xCF,  // Get the GPIO3 properties with respect to the LCD signal generators.
    SSD1963_CMD1_SET_DBC_CONF                   = 0xD0,  // Set the DBC (Dynamic Backlight Control) configuration.
    SSD1963_CMD1_GET_DBC_CONF                   = 0xD1,  // Get the DBC (Dynamic Backlight Control) configuration.
    SSD1963_CMD9_SET_DBC_TH                     = 0xD4,  // Set the threshold for each level of power saving.
    SSD1963_CMD9_GET_DBC_TH                     = 0xD5,  // Get the threshold for each level of power saving.
    SSD1963_CMD1_SET_PLL                        = 0xE0,  // Start the PLL. Before the start, the system was operated with the
                                                         // crystal oscillator or clock input.
    SSD1963_CMD3_SET_PLL_MN                     = 0xE2,  // Set the MN of the PLL.
    SSD1963_CMD3_GET_PLL_MN                     = 0xE3,  // Get the MN setting of the PLL.
    SSD1963_CMD1_GET_PLL_STATUS                 = 0xE4,  // Get the PLL status.
    SSD1963_CMD0_SET_DEEP_SLEEP                 = 0xE5,  // Set deep sleep mode. PLL would be stopped.
    SSD1963_CMD3_SET_LSHIFT_FREQ                = 0xE6,  // Set the LSHIFT (pixel clock) frequency.
    SSD1963_CMD3_GET_LSHIFT_FREQ                = 0xE7,  // Get the current LSHIFT (pixel clock) frequency setting.
    SSD1963_CMD1_SET_PIXEL_DATA_INTERFACE       = 0xF0,  // Set the pixel data format to 8-bit / 9-bit / 12-bit / 16-bit (565) /
                                                         // 18-bit / 24-bit in the parallel host processor interface. This
                                                         // command is used for display data only, the command format is
                                                         // always 8-bit.
    SSD1963_CMD1_GET_PIXEL_DATA_INTERFACE       = 0xF1,  // Get the current pixel data format settings in the host processor
                                                         // interface.
} SSD1963_CMD_ENUM;

/* SSD1963_CMD1_SET_ADDRESS_MODE */
#define SSD1963_PAGE_ADDR_TOP_TO_BOTTOM         (0x0)
#define SSD1963_PAGE_ADDR_BOTTOM_TO_TOP         (0x01 << 7)
#define SSD1963_COLUMN_ADDR_LEFT_TO_RIGHT       (0x0)
#define SSD1963_COLUMN_ADDR_RIGHT_TO_LEFT       (0x01 << 6)
#define SSD1963_PAGE_COLUMN_ORDER_NORMAL        (0x00)
#define SSD1963_PAGE_COLUMN_ORDER_REVERSE       (0x01 << 5)
#define SSD1963_LCD_REFRESH_TOP_TO_BOTTOM       (0x00)
#define SSD1963_LCD_REFRESH_BOTTOM_TO_TOP       (0x01 << 4)

/* SSD1963_SET_GAMMA_CURVE */
#define SSD1963_NO_GAMMA_CURVE                  (0x0)
#define SSD1963_GAMMA_CURVE_0                   (0x1)
#define SSD1963_GAMMA_CURVE_1                   (0x1 << 1)
#define SSD1963_GAMMA_CURVE_2                   (0x1 << 2)
#define SSD1963_GAMMA_CURVE_3                   (0x1 << 3)

/* SSD1963_CMD1_SET_TEAR_ON */
#define SSD1963_TEAR_EFFECT_VBLANK              (0x0)
#define SSD1963_TEAR_EFFECT_VBLANK_HBLANK       (0x1)

/* SSD1963_CMD6_SET_PWM_CONF */
#define SSD1963_PWM_HOST_CONTROLLED             (0x0)
#define SSD1963_PWM_DBC_CONTROLLED              (0x1 << 3)
#define SSD1963_PWM_DISABLE                     (0x0)
#define SSD1963_PWM_ENABLE                      (0x1)
#define SSD1963_DBC_BRIGHTEST                   (0xFF)
#define SSD1963_DBC_DIMMEST                     (0x00)

/* SSD1963_CMD1_SET_DBC_CONF */
#define SSD1963_DBC_MANUAL_BRIGHTNESS_ENABLE    (0x0)
#define SSD1963_DBC_MANUAL_BRIGHTNESS_DISABLE   (0x1 << 6)
#define SSD1963_DBC_TRANSITION_EFFECT_ENABLE    (0x1 << 5)
#define SSD1963_DBC_TRANSITION_EFFECT_DISABLE   (0x0)
#define SSD1963_ENERGY_SAVING_DISABLE           (0x0)
#define SSD1963_ENERGY_SAVING_CONSERVATIVE      (0x1 << 2)
#define SSD1963_ENERGY_SAVING_NORMAL            (0x2 << 2)
#define SSD1963_ENERGY_SAVING_AGGRESSIVE        (0x3 << 2)
#define SSD1963_DBC_MASTER_ENABLE               (0x1)
#define SSD1963_DBC_MASTER_DISABLE              (0x0)

/* Pixel Data Interface Format */
#define SSD1963_8BIT                            (0x00)
#define SSD1963_12BIT                           (0x01)
#define SSD1963_16BIT_PACKED                    (0x02)
#define SSD1963_16BIT565                        (0x03)
#define SSD1963_18BIT                           (0x04)
#define SSD1963_24BIT                           (0x05)
#define SSD1963_9BIT                            (0x06)

/* The following constants pertain to the Displaytech DT035TFT LCD physical display */
#define LCD_HEIGHT                              (240u)
#define LCD_WIDTH                               (320u)
#define NUM_BITS_PER_PIXEL                      (16u)     // Color depth

#define ROWS_PER_DMA_TRANSFER                   (3u)
#define DMA_TRANSFER_SIZE                       (LCD_WIDTH * ROWS_PER_DMA_TRANSFER) /* Cannot be greater than 1024 */

/* Device-specific initializations and configurations */
extern  ADI_PDI_RESULT_TYPE adi_DisplayDeviceInit   (ADI_PDI_DEV_HANDLE const hDevice);
extern  ADI_PDI_RESULT_TYPE writeDisplay            (ADI_PDI_DEV_HANDLE hDevice, 
                                                     uint8_t page, 
                                                     uint32_t* pData);

/* 
 * PDI calls into the device-specific driver through this API.
 * The device-specific driver translates the MIPI copmmand and posts a device-specific fullfillment command sequence
 * and parameters through the PDI register API for SetCommand and PushFifo
 * Transmit and deceive data will go to/from data addresses (parameter FIFO) via the DMA engine
 */
extern ADI_PDI_RESULT_TYPE adi_TranslateMipiCommand (ADI_PDI_DEV_HANDLE const hDevice, const ADI_MIPI_DCS_ENUM MipiCmd, const void* pMipiParamBlock);
