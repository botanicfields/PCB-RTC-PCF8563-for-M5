// Copyright 2021 BotanicFields, Inc.
// BF-027 PCF8563 RTC for Grove I2C
// test and start example

//#define M5STACK
#define M5ATOM

#ifdef M5STACK
#include <M5Stack.h>
#endif
#ifdef M5ATOM
#include <M5Atom.h>
#endif

#include "BF_Wifi.h"
#include "BF_Pcf8563.h"
#include "BF_RtcxNtp.h"
#include "BF_Pcf8563Test.h"

// for loop control
const int loop_ms(20);          // 20ms
const int interval_ms(600000);  // 600sec
int loop_last_ms(0);
int interval_last_ms(0);

void setup()
{
#ifdef M5STACK
  const bool lcd_enable(true);
  const bool sd_enable(true);
  const bool serial_enable(true);
  const bool i2c_enable(true);  // SCL = GPIO22, SDA = GPIO21, frequency = 100kHz;
  M5.begin(lcd_enable, sd_enable, serial_enable, i2c_enable);
#endif
#ifdef M5ATOM
  const bool serial_enable(true);
  const bool i2c_enable(true);  // SCL = GPIO21, SDA = GPIO25, frequency = 100kHz;
  const bool display_enable(true);
  M5.begin(serial_enable, i2c_enable, !display_enable);
#endif

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
  WiFi.setSleep(false);  // https://macsbug.wordpress.com/2021/05/02/buttona-on-m5stack-does-not-work-properly/
  NtpBegin();

  // loop control
  loop_last_ms = millis();
  interval_last_ms = loop_last_ms;
}

void loop()
{
  M5.update();
  RtcxUpdate();  // for SNTP callback
//  WifiProcess();

  int now_ms = millis();
  if (now_ms - interval_last_ms > interval_ms) {
    interval_last_ms = now_ms;
    CompareRtcxTime();
  }

  delay(loop_ms + loop_last_ms - millis());
  loop_last_ms = millis();
}
