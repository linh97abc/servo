#ifndef __FIFO_UART_MOCK_H__
#define __FIFO_UART_MOCK_H__

#include <system_mock.h>
#include <avl_fifo_uart.h>

namespace sys_mock
{
    class FifoUart_MemMap : public MemoryMap
    {
        struct tag_FifoUart_Reg BASE {0};
        uint8_t rx_val{0};
        // unsigned RX_FIFO_SIZE;
        // unsigned TX_FIFO_SIZE;

    public:
        FifoUart_MemMap(const char *name
                        // ,
                        //                 unsigned RX_FIFO_SIZE,
                        //                 unsigned TX_FIFO_SIZE
                        )
            : MemoryMap(name)
        // , RX_FIFO_SIZE(RX_FIFO_SIZE), TX_FIFO_SIZE(TX_FIFO_SIZE)
        {
            this->BASE.flag.field.rx_valid = 1;
            this->BASE.flag.field.tx_ready = 1;

        }

        uint32_t iord(unsigned reg);
        void iowr(unsigned reg, uint32_t data);

    };

} // namespace sys_mock
#endif // __FIFO_UART_MOCK_H__