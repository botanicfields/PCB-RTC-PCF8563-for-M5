// Copyright 2021 BotanicFields, Inc.
// BF-027 PCF8563 RTC for Grove I2C
// test and example

#include <M5Atom.h>
#include "BF_Wifi.h"
#include "BF_RtcxNtp.h"
#include "BF_Pcf8563Test.h"

// for M5Atom
const bool serial_enable(true);
const bool i2c_enable(true);
const bool display_enable(true);

// for I2C
const int wire_scl(21);       // GPIO21
const int wire_sda(25);       // GPIO25
const int wire_freq(100000);  // 100kHz

// for loop control
const int loop_ms(20);          // 20ms
const int interval_ms(600000);  // 600sec
int last_ms(0);
int interval_last_ms(0);

void setup()
{
  M5.begin(serial_enable, !i2c_enable, !display_enable);
  Wire.begin(wire_sda, wire_scl, wire_freq);

  // RTCx PCF8563: set I2C connected
  rtcx.Begin(Wire);

  // RTCx PCF8563: function tests
//  TestControl();
//  TestAlarm();
//  TestClockOut();
//  TestSetTimer();
//  TestGetInterrupt();
//  TestTimerPulse();

  // RTCx PCF8563: for adjusting trimmer capacitor
  rtcx.ClockOutForTrimmer();

  // set system time, connect Wi-Fi, start NTP
  SetTimeFromRtcx();
  WifiBegin();
  NtpBegin();

  // loop control
  last_ms = millis();
  interval_last_ms = last_ms;
}

void loop()
{
  M5.update();
  WifiProcess();

  int now_ms = millis();
  if (now_ms - interval_last_ms > interval_ms) {
    interval_last_ms = now_ms;
    CompareRtcxTime();
  }

  delay(loop_ms + last_ms - millis());
  last_ms = millis();
}
