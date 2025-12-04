#include "platform_nucleof411re_serial.h"

#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_usart.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static USART_TypeDef *m_serial;

static void init_uart_pin(const platform_nucleof411re_serial_PinConfig config) {
    LL_AHB1_GRP1_EnableClock(config.gpio_peripheral_enable);

    LL_GPIO_SetPinMode(config.gpio, config.pin, LL_GPIO_MODE_ALTERNATE);
    if(config.pin < LL_GPIO_PIN_8){
        LL_GPIO_SetAFPin_0_7(config.gpio, config.pin, config.af); 
    } else {
        LL_GPIO_SetAFPin_8_15(config.gpio, config.pin, config.af); 
    }
}

static void init_uart(const platform_nucleof411re_serial_UartConfig config) {
    LL_APB1_GRP1_EnableClock(config.uart_peripheral_enable);

    LL_USART_SetTransferDirection(config.uart, LL_USART_DIRECTION_TX);
    LL_USART_SetParity(config.uart, LL_USART_PARITY_NONE);
    LL_USART_SetDataWidth(config.uart, LL_USART_DATAWIDTH_8B);
    LL_USART_SetStopBitsLength(config.uart, LL_USART_STOPBITS_1);
    LL_USART_SetBaudRate(config.uart, SystemCoreClock/2, LL_USART_OVERSAMPLING_16, config.baudrate);

    LL_USART_Enable(config.uart);

    m_serial = config.uart;
}

static void send_byte_blocking(uint8_t byte)
{
    /* Wait for any previous tranmission to complete */
    while (!LL_USART_IsActiveFlag_TC(m_serial));
    /* Make sure transmit buffer is empty */
    while (!LL_USART_IsActiveFlag_TXE(m_serial));
    /* Send byte */
    LL_USART_TransmitData8(m_serial, byte);
}

void platform_nucleof411re_serial_init(const platform_nucleof411re_serial_Config config)
{
    init_uart_pin(config.tx_pin_config);
    init_uart(config.uart_config);
}

int _write(int file, char *ptr, int len)
{
    (void)file;

    for (uint8_t i = 0; i < len; i++)
    {
        const uint8_t byte = (uint8_t)ptr[i];
        send_byte_blocking(byte);
    }

    return len;
}
