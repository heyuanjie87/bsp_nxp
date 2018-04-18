/*
 * File      : drv_uart.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2013, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-10-10     Tanek        the first version
 * 2018-03-17     laiyiketang  Add other uart.
 */

#include "fsl_common.h"
#include "fsl_lpuart.h"
#include "fsl_iomuxc.h"
#include "drv_uart.h"
 #include <rtdevice.h>

#ifdef RT_USING_SERIAL

/* GPIO����ʱ�ӻ���LPUART_Init���Զ�����, ������������º��򲻻��Զ����� */
#if defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL
#error "Please don't define 'FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL'!"
#endif

#if !defined(RT_USING_UART1) && !defined(RT_USING_UART2) && \
    !defined(RT_USING_UART3) && !defined(RT_USING_UART4) && \
    !defined(RT_USING_UART5) && !defined(RT_USING_UART6) && \
    !defined(RT_USING_UART7) && !defined(RT_USING_UART8)
#error "Please define at least one UARTx"

#endif

/* imxrt uart driver */
struct imxrt_uart
{
    LPUART_Type * uart_base;
    IRQn_Type irqn;

    struct rt_serial_device * serial;
    char *device_name;
};

static void uart_isr(struct rt_serial_device *serial);

#if defined(RT_USING_UART1)
struct rt_serial_device serial1;

void LPUART1_IRQHandler(void)
{
    uart_isr(&serial1);
}

#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
struct rt_serial_device serial2;

void LPUART2_IRQHandler(void)
{
    uart_isr(&serial2);
}

#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART3)
struct rt_serial_device serial3;

void LPUART3_IRQHandler(void)
{
    uart_isr(&serial3);
}

#endif /* RT_USING_UART3 */

#if defined(RT_USING_UART4)
struct rt_serial_device serial4;

void LPUART4_IRQHandler(void)
{
    uart_isr(&serial4);
}
#endif /* RT_USING_UART4 */

#if defined(RT_USING_UART5)
struct rt_serial_device serial5;

void LPUART5_IRQHandler(void)
{
    uart_isr(&serial5);
}

#endif /* RT_USING_UART5 */

#if defined(RT_USING_UART6)
struct rt_serial_device serial6;

void LPUART6_IRQHandler(void)
{
    uart_isr(&serial6);
}

#endif /* RT_USING_UART6 */

#if defined(RT_USING_UART7)
struct rt_serial_device serial7;

void LPUART7_IRQHandler(void)
{
    uart_isr(&serial7);
}

#endif /* RT_USING_UART7 */

#if defined(RT_USING_UART8)
struct rt_serial_device serial8;

void LPUART8_IRQHandler(void)
{
    uart_isr(&serial8);
}

#endif /* RT_USING_UART8 */

static const struct imxrt_uart uarts[] = {
#ifdef RT_USING_UART1
    {
        LPUART1,
        LPUART1_IRQn,
        &serial1,
        "uart1",
    },
#endif
#ifdef RT_USING_UART2
    {
        LPUART2,
        LPUART2_IRQn,
        &serial2,
        "uart2",
    },
#endif
#ifdef RT_USING_UART3
    {
        LPUART3,
        LPUART3_IRQn,
        &serial3,
        "uart3",
    },
#endif
#ifdef RT_USING_UART4
    {
        LPUART4,
        LPUART4_IRQn,
        &serial4,
        "uart4",
    },
#endif
#ifdef RT_USING_UART5
    {
        LPUART5,
        LPUART5_IRQn,
        &serial5,
        "uart5",
    },
#endif
#ifdef RT_USING_UART6
    {
        LPUART6,
        LPUART6_IRQn,
        &serial6,
        "uart6",
    },
#endif
#ifdef RT_USING_UART7
    {
        LPUART7,
        LPUART7_IRQn,
        &serial7,
        "uart7",
    },
#endif
#ifdef RT_USING_UART8
    {
        LPUART8,
        LPUART8_IRQn,
        &serial8,
        "uart8",
    },
#endif

};

/* Get debug console frequency. */
uint32_t BOARD_DebugConsoleSrcFreq(void)
{
    uint32_t freq;

    /* To make it simple, we assume default PLL and divider settings, and the only variable
       from application is use PLL3 source or OSC source */
    if (CLOCK_GetMux(kCLOCK_UartMux) == 0) /* PLL3 div6 80M */
    {
        freq = (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / 6U) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    }
    else
    {
        freq = CLOCK_GetOscFreq() / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    }

    return freq;
}


/**
* @brief UART MSP Initialization
*        This function configures the hardware resources used in this example:
*           - Peripheral's clock enable
*           - Peripheral's GPIO Configuration
*           - NVIC configuration for UART interrupt request enable
* @param huart: UART handle pointer
* @retval None
*/
void imxrt_uart_gpio_init(struct imxrt_uart *uart)
{
    if (uart->uart_base != RT_NULL)
    {
#ifdef RT_USING_UART1
        CLOCK_EnableClock(kCLOCK_Iomuxc);           /* iomuxc clock (iomuxc_clk_enable): 0x03u */

        IOMUXC_SetPinMux(
            IOMUXC_GPIO_AD_B0_12_LPUART1_TX,        /* GPIO_AD_B0_12 is configured as LPUART1_TX */
            0U);                                    /* Software Input On Field: Input Path is determined by functionality */
        IOMUXC_SetPinMux(
            IOMUXC_GPIO_AD_B0_13_LPUART1_RX,        /* GPIO_AD_B0_13 is configured as LPUART1_RX */
            0U);                                    /* Software Input On Field: Input Path is determined by functionality */
        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_AD_B0_12_LPUART1_TX,        /* GPIO_AD_B0_12 PAD functional properties : */
            0x10B0u);                               /* Slew Rate Field: Slow Slew Rate
                                                     Drive Strength Field: R0/6
                                                     Speed Field: medium(100MHz)
                                                     Open Drain Enable Field: Open Drain Disabled
                                                     Pull / Keep Enable Field: Pull/Keeper Enabled
                                                     Pull / Keep Select Field: Keeper
                                                     Pull Up / Down Config. Field: 100K Ohm Pull Down
                                                     Hyst. Enable Field: Hysteresis Disabled */
        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_AD_B0_13_LPUART1_RX,        /* GPIO_AD_B0_13 PAD functional properties : */
            0x10B0u);                               /* Slew Rate Field: Slow Slew Rate
                                                     Drive Strength Field: R0/6
                                                     Speed Field: medium(100MHz)
                                                     Open Drain Enable Field: Open Drain Disabled
                                                     Pull / Keep Enable Field: Pull/Keeper Enabled
                                                     Pull / Keep Select Field: Keeper
                                                     Pull Up / Down Config. Field: 100K Ohm Pull Down
                                                     Hyst. Enable Field: Hysteresis Disabled */
#endif
#ifdef RT_USING_UART2
        CLOCK_EnableClock(kCLOCK_Iomuxc);

        IOMUXC_SetPinMux(
            IOMUXC_GPIO_AD_B1_02_LPUART2_TX,
            0U);
        IOMUXC_SetPinMux(
            IOMUXC_GPIO_AD_B1_03_LPUART2_RX,
            0U);
        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_AD_B1_02_LPUART2_TX,
            0x10B0u);


        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_AD_B1_03_LPUART2_RX,
            0x10B0u);

#endif
#ifdef RT_USING_UART3
        CLOCK_EnableClock(kCLOCK_Iomuxc);

        IOMUXC_SetPinMux(
            IOMUXC_GPIO_AD_B1_06_LPUART3_TX,
            0U);
        IOMUXC_SetPinMux(
            IOMUXC_GPIO_AD_B1_07_LPUART3_RX,
            0U);
        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_AD_B1_06_LPUART3_TX,
            0x10B0u);

        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_AD_B1_07_LPUART3_RX,
            0x10B0u);
#endif
#ifdef RT_USING_UART4
        CLOCK_EnableClock(kCLOCK_Iomuxc);

        IOMUXC_SetPinMux(
            IOMUXC_GPIO_B1_00_LPUART4_TX,
            0U);
        IOMUXC_SetPinMux(
            IOMUXC_GPIO_B1_01_LPUART4_RX,
            0U);
        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_B1_00_LPUART4_TX,
            0x10B0u);

        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_B1_01_LPUART4_RX,
            0x10B0u);
#endif
#ifdef RT_USING_UART5
        CLOCK_EnableClock(kCLOCK_Iomuxc);

        IOMUXC_SetPinMux(
            IOMUXC_GPIO_B1_12_LPUART5_TX,
            0U);
        IOMUXC_SetPinMux(
            IOMUXC_GPIO_B1_13_LPUART5_RX,
            0U);
        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_B1_12_LPUART5_TX,
            0x10B0u);

        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_B1_13_LPUART5_RX,
            0x10B0u);
#endif
#ifdef RT_USING_UART6
        CLOCK_EnableClock(kCLOCK_Iomuxc);

        IOMUXC_SetPinMux(
            IOMUXC_GPIO_AD_B0_02_LPUART6_TX,
            0U);
        IOMUXC_SetPinMux(
            IOMUXC_GPIO_AD_B0_03_LPUART6_RX,
            0U);
        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_AD_B0_02_LPUART6_TX,
            0x10B0u);

        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_AD_B0_03_LPUART6_RX,
            0x10B0u);
#endif
#ifdef RT_USING_UART7
        CLOCK_EnableClock(kCLOCK_Iomuxc);

        IOMUXC_SetPinMux(
            IOMUXC_GPIO_EMC_31_LPUART7_TX,
            0U);
        IOMUXC_SetPinMux(
            IOMUXC_GPIO_EMC_32_LPUART7_RX,
            0U);
        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_EMC_31_LPUART7_TX,
            0x10B0u);

        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_EMC_32_LPUART7_RX,
            0x10B0u);
#endif
#ifdef RT_USING_UART8
        CLOCK_EnableClock(kCLOCK_Iomuxc);

        IOMUXC_SetPinMux(
            IOMUXC_GPIO_AD_B1_10_LPUART8_TX,
            0U);
        IOMUXC_SetPinMux(
            IOMUXC_GPIO_AD_B1_11_LPUART8_RX,
            0U);
        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_AD_B1_10_LPUART8_TX,
            0x10B0u);

        IOMUXC_SetPinConfig(
            IOMUXC_GPIO_AD_B1_11_LPUART8_RX,
            0x10B0u);
#endif
    }
    else
    {
        RT_ASSERT(RT_NULL);
    }
}

static int imxrt_configure(rt_serial_t *serial, struct serial_configure *cfg)
{
    struct imxrt_uart *uart;
    lpuart_config_t config;

    uart = (struct imxrt_uart *)serial->parent.user_data;

    imxrt_uart_gpio_init(uart);

    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = cfg->baud_rate;

    config.dataBitsCount = kLPUART_EightDataBits;
    config.stopBitCount = kLPUART_OneStopBit;
    config.parityMode = kLPUART_ParityDisabled;

    config.enableTx = true;
    config.enableRx = true;

    LPUART_Init(uart->uart_base, &config, BOARD_DebugConsoleSrcFreq());

    return 0;
}

static int imxrt_control(rt_serial_t *serial, int cmd, void *arg)
{
    struct imxrt_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct imxrt_uart *)serial->parent.user_data;

    switch (cmd)
    {
    case SERIAL_CTRL_RXSTOP:
        LPUART_DisableInterrupts(uart->uart_base, kLPUART_RxDataRegFullInterruptEnable);
        break;
    case SERIAL_CTRL_RXSTART:
        /* enable interrupt */
        LPUART_EnableInterrupts(uart->uart_base, kLPUART_RxDataRegFullInterruptEnable);
        break;
	case SERIAL_CTRL_TXSTART:
		LPUART_EnableInterrupts(uart->uart_base, kLPUART_TxDataRegEmptyInterruptEnable);
	    break;
	case SERIAL_CTRL_TXSTOP:
		LPUART_DisableInterrupts(uart->uart_base, kLPUART_TxDataRegEmptyInterruptEnable);
	    break;	
    }

    return RT_EOK;
}

static int imxrt_putc(struct rt_serial_device *serial, char ch)
{
    struct imxrt_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct imxrt_uart *)serial->parent.user_data;

	while(!(LPUART_GetStatusFlags(uart->uart_base) & kLPUART_TxDataRegEmptyFlag));
    LPUART_WriteByte(uart->uart_base, ch);

    return 1;
}

static int imxrt_getc(struct rt_serial_device *serial)
{
    int ch;
    struct imxrt_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct imxrt_uart *)serial->parent.user_data;

    ch = -1;
    if (LPUART_GetStatusFlags(uart->uart_base) & kLPUART_RxDataRegFullFlag)
        ch = LPUART_ReadByte(uart->uart_base);
    return ch;
}

static int imxrt_init(rt_serial_t *serial)
{
    struct imxrt_uart *uart;

    uart = (struct imxrt_uart *)serial->parent.user_data;

    imxrt_uart_gpio_init(uart);
    EnableIRQ(uart->irqn);
	
	return 0;
}

static void imxrt_deinit(rt_serial_t *serial)
{

}

/**
 * Uart common interrupt process. This need add to uart ISR.
 *
 * @param serial serial device
 */
static void uart_isr(struct rt_serial_device *serial)
{
    struct imxrt_uart *uart;
    LPUART_Type *base;

    RT_ASSERT(serial != RT_NULL);

    uart = (struct imxrt_uart *) serial->parent.user_data;
    RT_ASSERT(uart != RT_NULL);

    base = uart->uart_base;
    RT_ASSERT(base != RT_NULL);

    /* enter interrupt */
    rt_interrupt_enter();

    /* UART in mode Receiver -------------------------------------------------*/
    if (LPUART_GetStatusFlags(base) & kLPUART_RxDataRegFullFlag)
    {
        rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_IND);
    }

    if (LPUART_GetStatusFlags(base) & kLPUART_TxDataRegEmptyInterruptEnable)
    {
        rt_hw_serial_isr(serial, RT_SERIAL_EVENT_TX_DONE);
    }

    /* If RX overrun. */
    if (LPUART_STAT_OR_MASK & base->STAT)
    {
        /* Clear overrun flag, otherwise the RX does not work. */
        base->STAT = ((base->STAT & 0x3FE00000U) | LPUART_STAT_OR_MASK);
    }

    /* leave interrupt */
    rt_interrupt_leave();
}

static const struct rt_uart_ops imxrt_uart_ops =
{
	imxrt_init,
	imxrt_deinit,
    imxrt_configure,
    imxrt_control,
    imxrt_putc,
    imxrt_getc,
};

int imxrt_hw_usart_init(void)
{
    int i;
    for (i = 0; i < sizeof(uarts) / sizeof(uarts[0]); i++)
    {
        uarts[i].serial->ops    = &imxrt_uart_ops;

        /* register UART1 device */
        rt_hw_serial_register(uarts[i].serial,
                              uarts[i].device_name,
                              0,
                              (void *)&uarts[i]);
    }

    return 0;
}
INIT_BOARD_EXPORT(imxrt_hw_usart_init);

#endif /*RT_USING_SERIAL */
