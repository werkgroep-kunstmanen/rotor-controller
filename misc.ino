/*******************************************************************
 * RCSId: $Id: misc.ino,v 1.1 2021/07/29 08:15:44 ralblas Exp $
 *
 * Project: rotordrive
 * Author: R. Alblas
 *
 * content: 
 *   some debug functions
 *
 * History: 
 * $Log: misc.ino,v $
 * Revision 1.1  2021/07/29 08:15:44  ralblas
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

// serial print without blocking: only print if enough buffer!
// (Otherwise big delays will occur if sent bits are not received by xtrack)
// hm, if not received at PC: filling up buffer in PC, if full: TX and RX cont. on.
// So if TX blocks also RX blocks??? 
int swrite(char *str)
{
#if USE_MON_UART
  if (!do_feedback) return 0;
  int bufspace=Serial.availableForWrite();
  if (strlen(str) < bufspace-2)
  {
    Serial.println(str);
    return 0;
  }
  return 1;
#else
  return 0;
#endif
}

// print to serial and/or display
void xprint(int x,int y,char *s)
{
#if USE_MON_UART
  swrite(s);
#endif

#if USE_DISPLAY
  lcd.setCursor(x, y);
  lcd.print(s);
#endif
}

// delay between 2 write actions, to prevent buffer overflow
#define SERDEL 100
void send_specs(ROTOR *AX_rot,ROTOR *EY_rot)
{
  char cmd[40];
  sprintf(cmd,"SPEC: Release %s\n",RELEASE);                  swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: AX_POffset     =%d\n",AX_POffset);       swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: EY_POffset     =%d\n",EY_POffset);       swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: AX_REFPOS      =%d\n",AX_REFPOS);        swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: EY_REFPOS      =%d\n",EY_REFPOS);        swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: AX_STEPS_DEGR  =%d\n",AX_STEPS_DEGR);    swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: EY_STEPS_DEGR  =%d\n",EY_STEPS_DEGR);    swrite(cmd); delay(SERDEL);
 #if MOTORTYPE == MOT_STEPPER
  sprintf(cmd,"SPEC: AX_MotorSpeed  =%d\n",AX_MotorSpeed);    swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: AX_MotorAccel  =%d\n",AX_MotorSpeed);    swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: EY_MotorSpeed  =%d\n",EY_MotorSpeed);    swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: EY_MotorAccel  =%d\n",EY_MotorSpeed);    swrite(cmd); delay(SERDEL);
 #else
  sprintf(cmd,"SPEC: MINSPEED       =%d\n",MINSPEED);         swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: MAXSPEED       =%d\n",MAXSPEED);         swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: L_DEGR_MAXSPEED=%d\n",L_DEGR_MAXSPEED);  swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: H_DEGR_MINSPEED=%d\n",H_DEGR_MINSPEED);  swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: D_DEGR_STOP    =%d\n",D_DEGR_STOP);      swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: PWMFreq        =%d\n",PWMFreq);          swrite(cmd); delay(SERDEL);
  sprintf(cmd,"SPEC: MAX_PWM        =%d\n",MAX_PWM);          swrite(cmd); delay(SERDEL);
 #endif
}

void send_stat(ROTOR *AX_rot,ROTOR *EY_rot)
{
  char cmd[30];
  int stat_ax=-1,stat_ey=-1;
  if (AX_rot) stat_ax=AX_rot->cal_status;
  if (EY_rot) stat_ey=EY_rot->cal_status;
  sprintf(cmd,"STAT: ax=%8d  ey=%8d\n",stat_ax,stat_ey); swrite(cmd);
}

void send_pos(ROTOR *AX_rot,ROTOR *EY_rot)
{
  char cmd[128]; // to be on the safe side, currently 22 characters + 4 floats en 2 ints
  float axpos_degr=0.,axreq_degr=0.,eypos_degr=0.,eyreq_degr=0.;
  int ax_speed=0,ey_speed=0;
  if (AX_rot) 
  {
    axpos_degr=AX_rot->degr;
    axreq_degr=AX_rot->req_degr;
    ax_speed  =AX_rot->speed;
  }
  if (EY_rot) 
  {
    eypos_degr=EY_rot->degr;
    eyreq_degr=EY_rot->req_degr;
    ey_speed  =EY_rot->speed;
  }
  sprintf(cmd,"pos=[");               dtostrf(axpos_degr,6, 1, cmd+strlen(cmd));
  sprintf(cmd+strlen(cmd),",");       dtostrf(eypos_degr,6, 1, cmd+strlen(cmd));
  sprintf(cmd+strlen(cmd),"] req=["); dtostrf(axreq_degr,6, 1, cmd+strlen(cmd));
  sprintf(cmd+strlen(cmd),",");       dtostrf(eyreq_degr,6, 1, cmd+strlen(cmd));
  sprintf(cmd+strlen(cmd),"] spd=[%d,%d]",ax_speed,ey_speed); 
  swrite(cmd);
}

void fdispl(int x,int y,char *s,float n)
{
  char str[256]; // XXX as we do not know size of s this is not safe
  sprintf(str,"%s=",s);
  dtostrf(n,6, 1, str+strlen(s)+1);
  xprint(x,y,str);
}

void idispl(int x,int y,char *s,int n)
{
  char str[256]; // XXX as we do not know size of s this is not safe
  sprintf(str,"%s=%4d",s,n);
  xprint(x,y,str);
}

void pos2displ(ROTOR *rot)
{
  int rot_degr=to_degr(rot);
  if (rot->id==EY_ID)
  {
    fdispl(0,3,rot->name,(float)(rot_degr));
    idispl(12,3,"spd",rot->speed);
  }
  else
  {
    fdispl(0,2,rot->name,(float)(rot_degr));
    idispl(12,2,"spd",rot->speed);
  }
}

void rec2displ(int ep,float ax,float ey)
{
  static int nr;
  char str[32],str1[10],str2[10];

  dtostrf(ax,5, 1, str1);
  dtostrf(ey,5, 1, str2);
  sprintf(str,"cmd: %d  %5s  %5s",ep,str1,str2);

  str[31]=0;
  //xprint(0,0,str);
  
  sprintf(str,"nr=%d",++nr);
  //xprint(0,1,str);
}

void blink(int n,int d)
{
  for (; n>0; n--)
  {
    digitalWrite(LED_BUILTIN, HIGH);   // LED on; command received
    delay(d);
    digitalWrite(LED_BUILTIN, LOW);    // LED off; command not (yet) received
    delay(d);
  }
}

void set_led(ROTOR *rot,int rgb,boolean enable)
{
  if (!rot) return;
  if (!enable) return;
  if (rot->id==EY_ID)
  {
    #ifdef PIN_Rey
    digitalWrite(PIN_Rey , (rgb&4? HIGH : LOW));
    digitalWrite(PIN_Gey , (rgb&2? HIGH : LOW));
    digitalWrite(PIN_Bey , (rgb&1? HIGH : LOW));
    #endif
  }
  else
  {
    #ifdef PIN_Rax
    digitalWrite(PIN_Rax , (rgb&4? HIGH : LOW));
    digitalWrite(PIN_Gax , (rgb&2? HIGH : LOW));
    digitalWrite(PIN_Bax , (rgb&1? HIGH : LOW));
    #endif
  }
}

#if MOTOR_SIM
#define PIN_SIMPLS_AX 11
#define PIN_SIMPLS_EY 8
#define PULS_WIDTH 1
#define SIM_AZIMUT_MIN  -20
#define SIM_AZIMUT_MAX  200
void motorpuls(ROTOR *rot)
{
  if (!rot) return;
  if (abs(rot->speed) > 0)
  {
    if (rot->id == AX_ID)
    {
      if (((rot->speed > 0) && (rot->degr < (SIM_AZIMUT_MAX))) ||
          ((rot->speed < 0) && (rot->degr > (SIM_AZIMUT_MIN))))
      {
        digitalWrite(PIN_SIMPLS_AX, HIGH);
        delay(PULS_WIDTH);
        digitalWrite(PIN_SIMPLS_AX, LOW);
      }
    }

    if (rot->id == EY_ID)
    {
      if (((rot->speed > 0) && (rot->degr < 180)) ||
          ((rot->speed < 0) && (rot->degr >   0)))
      {
        digitalWrite(PIN_SIMPLS_EY, HIGH);
        delay(PULS_WIDTH);
        digitalWrite(PIN_SIMPLS_EY, LOW);
      }
    }
  }
}
#endif
