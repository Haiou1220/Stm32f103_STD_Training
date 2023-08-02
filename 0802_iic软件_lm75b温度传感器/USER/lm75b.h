


 #ifndef __LM75B_H__
#define  __LM75B_H__

#include "stm32f10x.h"
 
 


//u8 lm75b_read_temp( float* temper)  ;
u8 lm75b_read_temp( float* temper,u16* temp_reg)  ;  

u8 lm75b_read_conf( u8* conf_reg )  ;

u8 lm75b_set_temper_over(s8 temper);
u8 lm75b_set_temper_hyster(s8 temper);

u8 lm75b_read_Tover_reg( u16* Tover_reg_9b );

u8 lm75b_read_Thys_reg_9b( u16* Thys_reg_9b ) ; 



#endif 


