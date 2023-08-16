NOTE: AVR Code addresses are word addresses
All other addresses (including ones in FLASH) are byte addresses

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                             lit   00E4   0004 =      4. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       00E4  __lit_start
       00E8  __lit_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                           idata   00E8   0004 =      4. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       00E8  __idata_start
       00EC  __idata_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            text   00EC   08CE =   2254. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       0076  __text_start
       0076  __start
       0098  _exit
       0099  _delay_1us
       009F  _delay_nus
       00AD  _delay_1ms
       00B7  _delay_nms
       00C5  _I2C_Write
       00F9  _I2C_Read
       0147  _LCD_init
       015C  _LCD_en_write
       0162  _LCD_write_command
       0180  _LCD_write_data
       019E  _LCD_set_xy
       01AD  _LCD_write_string
       01C2  _LCD_write_char
       01CE  _conversion
       01F9  _display_x
       025F  _display_y
       02C5  _display_z
       032B  _ADXL345_init
       0341  _main
       034F  mod16u
       0351  div16u
       0352  xdiv16u
       036B  elpm32
       037F  push_xgset300C
       0384  pop_xgset300C
       0389  push_xgsetF00C
       0390  pop_xgsetF00C
       0397  push_xgset303C
       039E  pop_xgset303C
       03A5  neg32
       03AE  fpint
       03DA  uint2fp
       03DC  int2fp
       03E5  ulong2fp
       03E7  long2fp
       03FA  unpacks
       0402  tstR16_R19
       040A  unpacku
       041F  normalize_and_pack
       044D  pack
       0457  fpneg
       045E  fpneg2
       0464  saveR8toR25
       0473  restoreR8toR25
       0482  fpmule
       04D2  fpmule2
       04DD  __text_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            data   0200   0004 =      4. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       0200  __data_start
       0200  _display
       0204  __data_end

Area                               Start  End    Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                          vector   0000   0004 =      4. bytes (abs,ovr,rom)

Files Linked      [ module(s) ]

C:\iccv7avr\lib\crtatmega.o	[ crtatmega.s ]
AVR_GY-29.o	[ AVR_GY ]
<library>	[ div16u.s, elpm32.s, gpushx.s, gpushz.s, gpusha.s, lneg.s, fpint.s, int2fp.s, fputil.s, fpmule.s, fpmule2.s ]


User Global Definitions

ram_end = 0x21ff
hwstk_size = 0x1e

User Base Address Definitions

func_lit = 0xe4
data = 0x200
eeprom:0.4096

