/*******************************************************************
 * RCSId: $Id: pins_pwm.ino,v 1.1 2021/07/29 08:11:12 ralblas Exp $
 *
 * Project: rotordrive
 * Author: R. Alblas
 *
 * content: 
 *   pin def. for motors
 *
 * public functions:
 *   AX_set_pins(ROTOR *rot)
 *   EY_set_pins(ROTOR *rot)
 *
 * History: 
 * $Log: pins_pwm.ino,v $
 * Revision 1.1  2021/07/29 08:11:12  ralblas
 * Initial revision
 *
 *
 *******************************************************************/
/*******************************************************************
 * Copyright (C) 2020 R. Alblas. 
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software. If not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
 * 02111-1307, USA.
 ********************************************************************/
#include "rotorctrl.h"
#define set_pinmode(p,t) (p>=0? pinMode(p,t) : void())

// Define pins for rotor AX
void AX_set_pins(ROTOR *rot)
{
  if (!rot) return;
  rot->pin_pwm=PIN_ROTPWM_AX;
  rot->pin_dir=PIN_ROTDIR_AX;
  rot->pin_din=PIN_ROTDIN_AX;
  rot->pin_lsp=PIN_LOWSPD_AX;

  set_pinmode(PIN_ROTDIR_AX, OUTPUT);   // rotor direction
  set_pinmode(PIN_ROTDIN_AX , OUTPUT);  // rotor direction inversed
  set_pinmode(PIN_ROTPWM_AX, OUTPUT);   // rotor speed
  set_pinmode(PIN_ROTPLS_AX, INPUT);    // rotor move
  set_pinmode(PIN_LOWSPD_AX, OUTPUT);   // rotor low speed
  set_pinmode(PIN_Rax, OUTPUT);         // led indication
  set_pinmode(PIN_Gax, OUTPUT);         // led indication
  set_pinmode(PIN_Bax, OUTPUT);         // led indication

  #if MOTORTYPE == MOT_DC_PWM
    // Setup For PWM
    InitTimersSafe();
    SetPinFrequencySafe(PIN_ROTPWM_AX,PWMFreq);
  #endif
}

// Define pins for rotor EY
void EY_set_pins(ROTOR *rot)
{
  if (!rot) return;
  rot->pin_pwm=PIN_ROTPWM_EY;
  rot->pin_dir=PIN_ROTDIR_EY;
  rot->pin_din=PIN_ROTDIN_EY;
  rot->pin_lsp=PIN_LOWSPD_EY;

  set_pinmode(PIN_ROTDIR_EY, OUTPUT);   // rotor direction
  set_pinmode(PIN_ROTDIN_EY, OUTPUT);   // rotor direction inversed
  set_pinmode(PIN_ROTPWM_EY, OUTPUT);   // rotor speed
  set_pinmode(PIN_ROTPLS_EY, INPUT);    // rotor move
  set_pinmode(PIN_LOWSPD_EY, OUTPUT);   // rotor low speed

  set_pinmode(PIN_Rey, OUTPUT);         // led indication
  set_pinmode(PIN_Gey, OUTPUT);         // led indication
  set_pinmode(PIN_Bey, OUTPUT);         // led indication

  #if MOTORTYPE == MOT_DC_PWM
    // Setup For PWM
    InitTimersSafe();
    SetPinFrequencySafe(PIN_ROTPWM_EY,PWMFreq);
  #endif
}

