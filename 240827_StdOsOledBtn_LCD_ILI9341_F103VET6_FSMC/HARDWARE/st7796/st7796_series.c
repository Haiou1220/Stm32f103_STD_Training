/*
spi板: [IM0 IM1 IM2] = [ 1, IM1, 1],[IM1] => [R2短接=0] [R3短接=1],板载R3短接,
        [IM0 IM1 IM2] = [ 1, 1, 1]为4-Line SPI
并口板:[IM0 IM1 IM2] = [ IM0, 1, 0],[IM0]=>[R4短接=0][R5短接=1],板载R4短接,
        [IM0 IM1 IM2] = [ 0, 1, 0]为8080-series MCU 16-bit
*/  
/*
16-bit 8080-Series Interface
RESET CS DC WR RD DB[0-15] 
*/      