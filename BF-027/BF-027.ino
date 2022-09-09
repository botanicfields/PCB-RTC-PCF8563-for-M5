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

#include <WiFi.h>
#include <WiFiManager.h>
#include "BF_Pcf8563.h"
#include "BF_RtcxNtp.h"
#include "BF_Pcf8563Test.h"

//..:....1....:....2....:....3....:....4....:....5....:....6....:....7..
// for NTP
const char* time_zone  = "JST-9";
const char* ntp_server = "pool.ntp.org";
bool localtime_valid(false);

//..:....1....:....2....:....3....:....4....:....5....:....6....:....7..
// for WiFi
const int wifi_config_portal_timeout_sec(60);

wl_status_t wifi_status(WL_NO_SHIELD);

const char* wl_status_str[] = {
  "WL_IDLE_STATUS",      // 0
  "WL_NO_SSID_AVAIL",    // 1
  "WL_SCAN_COMPLETED",   // 2
  "WL_CONNECTED",        // 3
  "WL_CONNECT_FAILED",   // 4
  "WL_CONNECTION_LOST",  // 5
  "WL_DISCONNECTED",     // 6
  "WL_NO_SHIELD",        // 7 <-- 255
  "wl_status invalid",   // 8
};

const char* WlStatus(wl_status_t wl_status)
{
  if (wl_status >= 0 && wl_status <= 6) {
    return wl_status_str[wl_status];
  }
  if (wl_status == 255) {
    return wl_status_str[7];
  }
  return wl_status_str[8];
}

void WifiCheck()
{
  wl_status_t wifi_status_new = WiFi.status();
  if (wifi_status != wifi_status_new) {
    wifi_status = wifi_status_new;
    Serial.printf("[WiFi]%s\n", WlStatus(wifi_status));
  }
}

//..:....1....:....2....:....3....:....4....:....5....:....6....:....7..
// for loop control
const unsigned int loop_period_ms(100);  // 100ms
      unsigned int loop_last_ms;
const unsigned int compare_interval_ms(600000);  // 10 minutes
      unsigned int compare_last_ms;

void setup()
{
#ifdef M5STACK
  const bool lcd_enable(true);
  const bool sd_enable(true);
  const bool serial_enable(true);
  const bool i2c_enable(true);  // SCL = GPIO22, SDA = GPIO21, frequency = 100kHz;
  M5.begin(lcd_enable, sd_enable, serial_enable, i2c_enable);

  rtcx.Begin(Wire);
#endif

#ifdef M5ATOM
  const bool serial_enable(true);
  const bool i2c_enable(true);  // SCL = GPIO21, SDA = GPIO25, frequency = 100kHz;
  const bool display_enable(true);
  M5.begin(serial_enable, i2c_enable, !display_enable);

  const int      wire1_scl(32);       // GPIO32
  const int      wire1_sda(26);       // GPIO26
  const uint32_t wire1_freq(100000);  // 100kHz
  Wire1.begin(wire1_sda, wire1_scl, wire1_freq);

//  rtcx.Begin(Wire);  // connect to Pin Socket
  rtcx.Begin(Wire1);   // connect to GROVE
#endif

  SetTimeFromRtcx(time_zone);
  struct tm local_tm;
  getLocalTime(&local_tm);
  Serial.print(&local_tm, "localtime: %A, %B %d %Y %H:%M:%S\n");
  // print sample: must be < 64
  //....:....1....:....2....:....3....:....4....:....5....:....6....
  //localtime: Wednesday, September 11 2021 11:10:46

  // RTCx PCF8563: function tests
  TestControl();
  TestAlarm();
  TestClockOut();
  TestSetTimer();
  TestGetInterrupt();
  TestTimerPulse();

  // WiFi connect
  WiFiManager wm;  // blocking mode only
  wm.setConfigPortalTimeout(wifi_config_portal_timeout_sec);
  wm.autoConnect();
  WiFi.setSleep(false);  // https://macsbug.wordpress.com/2021/05/02/buttona-on-m5stack-does-not-work-properly/

  // NTP start
  NtpBegin(time_zone, ntp_server);

  // RTCx PCF8563: disable CLKO and INT
  const bool enable_clock_out(true);
  rtcx.ClockOutForTrimmer(!enable_clock_out);
  rtcx.DisableTimer();
  rtcx.DisableTimerInterrupt();

  // loop control
  loop_last_ms = millis();
  compare_last_ms = loop_last_ms;
}

void loop()
{
  M5.update();

  // WiFi status
  WifiCheck();

  // NTP sync
  RtcxUpdate();

  // compare RTCx to NTP
  unsigned int now_ms = millis();
  if (now_ms - compare_last_ms > compare_interval_ms) {
    compare_last_ms = now_ms;
    CompareRtcxTime();
  }

  // loop control
  unsigned int delay_ms(0);
  unsigned int elapse_ms = millis() - loop_last_ms;
  if (elapse_ms < loop_period_ms) {
    delay_ms = loop_period_ms - elapse_ms;
  }
  delay(delay_ms);
  loop_last_ms = millis();
//  Serial.printf("loop elapse = %dms\n", elapse_ms);  // monitor elapsed time
}
