/*

  u8x8_byte.c 
  
  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  
  
*/

#include "u8x8.h"

uint8_t u8x8_byte_SetDC(u8x8_t *u8x8, uint8_t dc)
{
  return u8x8->byte_cb(u8x8, U8X8_MSG_BYTE_SET_DC, dc, NULL);
}

uint8_t u8x8_byte_SendBytes(u8x8_t *u8x8, uint8_t cnt, uint8_t *data)
{
  return u8x8->byte_cb(u8x8, U8X8_MSG_BYTE_SEND, cnt, (void *)data);
}

uint8_t u8x8_byte_SendByte(u8x8_t *u8x8, uint8_t byte)
{
  return u8x8_byte_SendBytes(u8x8, 1, &byte);
}

uint8_t u8x8_byte_StartTransfer(u8x8_t *u8x8)
{
  return u8x8->byte_cb(u8x8, U8X8_MSG_BYTE_START_TRANSFER, 0, NULL);
}

uint8_t u8x8_byte_EndTransfer(u8x8_t *u8x8)
{
  return u8x8->byte_cb(u8x8, U8X8_MSG_BYTE_END_TRANSFER, 0, NULL);
}

/*=========================================*/

uint8_t u8x8_byte_empty(U8X8_UNUSED u8x8_t *u8x8, uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
  switch(msg)
  {
    case U8X8_MSG_BYTE_SEND:
    case U8X8_MSG_BYTE_INIT:
    case U8X8_MSG_BYTE_SET_DC:
    case U8X8_MSG_BYTE_START_TRANSFER:
    case U8X8_MSG_BYTE_END_TRANSFER:
      break;	/* do nothing */
  }
  return 1;	/* always succeed */
}


/*=========================================*/


/*
  Uses:
    u8x8->display_info->sda_setup_time_ns
    u8x8->display_info->sck_pulse_width_ns
    u8x8->display_info->spi_mode
    u8x8->display_info->chip_disable_level
    u8x8->display_info->chip_enable_level
    u8x8->display_info->post_chip_enable_wait_ns
    u8x8->display_info->pre_chip_disable_wait_ns
  Calls to GPIO and DELAY:
    U8X8_MSG_DELAY_NANO
    U8X8_MSG_GPIO_DC
    U8X8_MSG_GPIO_CS
    U8X8_MSG_GPIO_CLOCK
    U8X8_MSG_GPIO_DATA
  Handles:
    U8X8_MSG_BYTE_INIT
    U8X8_MSG_BYTE_SEND
    U8X8_MSG_BYTE_SET_DC
    U8X8_MSG_BYTE_START_TRANSFER
    U8X8_MSG_BYTE_END_TRANSFER
*/

/*=========================================*/



