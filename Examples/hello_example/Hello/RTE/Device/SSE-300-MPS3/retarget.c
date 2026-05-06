// Copyright (c) 2024 Arm Limited. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include "Driver_USART.h"
#include "retarget_stdout.h"

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>STDOUT USART Interface

//   <o>Connect to hardware via Driver_USART# <0-255>
//   <i>Select driver control block for USART interface
#define USART_DRV_NUM 0

//   <o>Baudrate
#define USART_BAUDRATE 115200

// </h>

#define _USART_Driver_(n) Driver_USART##n
#define USART_Driver_(n)  _USART_Driver_(n)

extern ARM_DRIVER_USART USART_Driver_(USART_DRV_NUM);
#define ptrUSART (&USART_Driver_(USART_DRV_NUM))

int stdout_init(void)
{
    int32_t status;

    status = ptrUSART->Initialize(NULL);
    if (status != ARM_DRIVER_OK)
        return (-1);

    status = ptrUSART->PowerControl(ARM_POWER_FULL);
    if (status != ARM_DRIVER_OK)
        return (-1);

    status = ptrUSART->Control(ARM_USART_MODE_ASYNCHRONOUS | ARM_USART_DATA_BITS_8 | ARM_USART_PARITY_NONE
                                   | ARM_USART_STOP_BITS_1 | ARM_USART_FLOW_CONTROL_NONE,
                               USART_BAUDRATE);
    if (status != ARM_DRIVER_OK)
        return (-1);

    status = ptrUSART->Control(ARM_USART_CONTROL_TX, 1);
    if (status != ARM_DRIVER_OK)
        return (-1);

    return (0);
}

int stdout_putchar(int ch)
{
    uint8_t buf[1];

    buf[0] = ch;
    if (ptrUSART->Send(buf, 1) != ARM_DRIVER_OK) {
        return (-1);
    }
    while (ptrUSART->GetTxCount() != 1)
        ;
    return (ch);
}
