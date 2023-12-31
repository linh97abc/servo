#include "fifo_uart_mock.h"
#include "LL/avl_fifo_uart_reg.h"
#include <iostream>
#include <string.h>

uint32_t sys_mock::FifoUart_MemMap::iord(unsigned reg)
{
    switch (reg)
    {
    case FIFO_UART_CR_REG:
        return this->BASE.cr.data;

    case FIFO_UART_IE_REG:
        return this->BASE.ie.data;

    case FIFO_UART_IE_DIS_REG:
        return 0;

    case FIFO_UART_FLAG_REG:
        return this->BASE.flag.data;
    case FIFO_UART_TX_COUNT_REG:
        return this->BASE.tx_cnt;

    case FIFO_UART_RX_COUNT_REG:
        return this->BASE.rx_cnt;

    case FIFO_UART_RX_THRESHOLD_REG:
        return this->BASE.rx_threshold;

    case FIFO_UART_TX_REG:
        return 0;

    case FIFO_UART_RX_REG:
        this->rx_val++;
        return (this->rx_val | FIFO_UART_RX_REG_VALID);

    default:
        break;
    }

    return 0;
}

void sys_mock::FifoUart_MemMap::iowr(unsigned reg, uint32_t data)
{
    switch (reg)
    {
    case FIFO_UART_CR_REG:
    {
        avl_fifo_uart_CR_REG crReg;
        crReg.data = data;

        this->BASE.cr.field.en = crReg.field.en;

        if (!this->BASE.cr.field.en)
        {
            this->BASE.cr.field.baudrate = crReg.field.baudrate;
        }

        break;
    }

    case FIFO_UART_IE_REG:
    {
        uint32_t ieReg = this->BASE.ie.data;
        ieReg |= data;
        this->BASE.ie.data = ieReg;

        if (this->BASE.ie.field.rx_valid)
        {
            this->BASE.flag.field.rx_valid = 1;
        }

        if (this->BASE.ie.field.tx_ready)
        {
            this->BASE.flag.field.tx_ready = 1;
        }

        sys_mock::IrqManager::GetInst()->Invoke(this->irq);

        this->BASE.flag.field.tx_ready = 0;
        this->BASE.flag.field.rx_valid = 0;

        break;
    }

    case FIFO_UART_IE_DIS_REG:
    {
        uint32_t ieReg = this->BASE.ie.data;
        ieReg &= ~data;
        this->BASE.ie.data = ieReg;

        break;
    }

    case FIFO_UART_FLAG_REG:
    {
        uint32_t ieReg = this->BASE.ie.data;
        ieReg &= ~(data & (FIFO_UART_FLAG_STOP_ERR |
                           FIFO_UART_FLAG_PARITY_ERR |
                           FIFO_UART_FLAG_OVERRUN_ERR));
        this->BASE.ie.data = ieReg;

        break;
    }

    case FIFO_UART_RX_THRESHOLD_REG:
    {
        this->BASE.rx_threshold = data;
        break;
    }

    case FIFO_UART_TX_REG:
    {
        std::cout << data << " ";
        break;
    }

    default:
        break;
    }
}
