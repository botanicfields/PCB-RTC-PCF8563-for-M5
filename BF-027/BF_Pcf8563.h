// Copyright 2021 BotanicFields, Inc.
// BF-027 PCF8563 RTC for Grove I2C of M5Stack, M5Atom

#ifndef INCLUDED_BF_PCF8563_H
#define INCLUDED_BF_PCF8563_H

#include <Arduino.h>
#include <Wire.h>

class Pcf8563 {
 public:
  int Begin(TwoWire &wire);
  int ReadTime(struct tm *tm_now);
  int WriteTime(struct tm *tm_now);

  int SetAlarm(int miniute = -1, int hour = -1, int day = -1, int weekday = -1);
  int DisableAlarm();
  int EnableAlarmInterrupt(bool enable_interrupt = true, bool keep_flag = false);
  int DisableAlarmInterrupt();

  double SetTimer(double timer_sec);
  int EnableTimer(bool enable_timer = true);
  int DisableTimer();
  int EnableTimerInterrupt(bool enable_interrupt = true, bool pulse_mode = false, bool keep_flag = false);
  int DisableTimerInterrupt();

  int GetInterrupt();  // check interrupt and clear

  int ClockOutForTrimmer();  // clock out 1Hz

  bool external_clock_test_mode;  // 00h Control_status_1 bit 7 TEST1
  bool source_clock_stoped;       // 00h Control_status_1 bit 5 STOP
  bool power_on_reset_override;   // 00h Control_status_1 bit 3 TESTC

  bool timer_interrupt_pulse_mode;  // 01h Control_status_2 bit 4 TI_TP
  bool alarm_flag_active;           // 01h Control_status_2 bit 3 AF
  bool timer_flag_active;           // 01h Control_status_2 bit 2 TF
  bool alarm_interrupt_enable;      // 01h Control_status_2 bit 1 AIE
  bool timer_interrupt_enable;      // 01h Control_status_2 bit 0 TIE

  bool alarm_minute_enable;   // 09h Minute_alarm   bit 7 !AE_M
  int alarm_minute;           // 09h Minute_alarm   0..59
  bool alarm_hour_enable;     // 0Ah Hour_alarm     bit 7 !AE_H
  int alarm_hour;             // 0Ah Hour_alarm     0..23
  bool alarm_day_enable;      // 0Bh Day_alarm      bit 7 !AE_D
  int alarm_day;              // 0Bh Day_alarm      1..31
  bool alarm_weekday_enable;  // 0Ch Weekday_alarm  bit 7 !AE_W
  int alarm_weekday;          // 0Ch Weekday_alarm  0..6

  enum FreqClockOut_t {      // 0Dh Clock_out_control FD  0..3
    fco_32768Hz,
    fco_1024Hz,
    fco_32Hz,
    fco_1Hz,
  };

  bool clock_out_active;     // 0Dh Clock_out_control bit 7 FE
  FreqClockOut_t clock_out;  // 0Dh Clock_out_control 0..3

  enum FreqTimerSource_t {         // 0Eh Timer_control TD  0..3
    fts_4096Hz,
    fts_64Hz,
    fts_1Hz,
    fts_1_60th_Hz,  // (1/60)Hz
  };

  bool timer_enable;               // 0Eh Timer_control bit 7 TE
  FreqTimerSource_t timer_source;  // 0Eh Timer_control 0..3
  int timer;                       // 0Fh Timer         0..255

  Pcf8563();
  ~Pcf8563();

  int ReadControl();
  int WriteControl();
  int ReadInterrupt();
  int WriteInterrupt();
  int ReadAlarm();
  int WriteAlarm();
  int ReadClockOut();
  int WriteClockOut();
  int ReadTimer();
  int WriteTimer();

 private:
  const int address_pcf8563 = 0x51;

  TwoWire *m_wire;
  uint8_t m_reg[0x10];

  size_t ReadReg(int reg_start, size_t reg_size);
  int WriteReg(int reg_start, size_t reg_size);
  int Int2Bcd(int int_num);
  int Bcd2Int(int bcd_num);
};

// an instance "rtc external"
extern Pcf8563 rtcx;
extern struct tm tm_rtcx;

#endif  // INCLUDED_BF_PCF8563_H
