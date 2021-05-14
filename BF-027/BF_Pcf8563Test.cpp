// Copyright 2021 BotanicFields, Inc.
// BF-027 PCF8563 RTC for Grove I2C
// test of BF_Pcf8563.h/cpp

#include "BF_Pcf8563Test.h"

//..:....1....:....2....:....3....:....4....:....5....:....6....:....7....:....8
void TestControl()
{
  Serial.println("\n[PCF8563] test: control");
  rtcx.ReadControl();  PrintControl();  Serial.println();

  rtcx.external_clock_test_mode = true;   PrintTestControl();
  rtcx.source_clock_stoped      = true;   PrintTestControl();
  rtcx.power_on_reset_override  = false;  PrintTestControl();
  rtcx.power_on_reset_override  = true;   PrintTestControl();
  rtcx.source_clock_stoped      = false;  PrintTestControl();
  rtcx.external_clock_test_mode = false;  PrintTestControl();
}

void PrintTestControl()
{
  rtcx.WriteControl();
  rtcx.ReadControl();  PrintControl();  Serial.println();
}

//..:....1....:....2....:....3....:....4....:....5....:....6....:....7....:....8
void CompareRtcxTime()
{
  struct tm tm_glt;
  struct tm tm_rtcx;

  PrintTimeval();
  getLocalTime(&tm_glt);
  Serial.print(&tm_glt, "getLocalTime = %A, %B %d %Y %H:%M:%S;  ");
  rtcx.ReadTime(&tm_rtcx);
  Serial.print(&tm_rtcx, "RTCx = %A, %B %d %Y %H:%M:%S;  ");
  // print sample: must be < 64
  //....:....1....:....2....:....3....:....4....:....5....:....6....
  //getLocalTime = Wednesday, September 11 2021 11:10:46;
  //RTCx = Wednesday, September 11 2021 11:10:46;

  Serial.printf("RTCx - gLT = %d\n", mktime(&tm_rtcx) - mktime(&tm_glt));
}

void PrintTimeval()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  Serial.printf("test: timeval = %8d.%06d;  ", tv.tv_sec, tv.tv_usec);
}

//..:....1....:....2....:....3....:....4....:....5....:....6....:....7....:....8
struct tm tm_test;

void TestAlarm()
{
  Serial.println("\n[PCF8563] test: alarm");
  rtcx.ReadAlarm();  PrintAlarm();  Serial.println();

  //            m   h   d   w  am  ah  ad  aw        match
  TestAlarmSub(59, 23, 11, 0, -1, -1, -1,  1);  // only wday:    2021.4.11(Sun) 23:59:57 --> Mon
  TestAlarmSub(59, 23, 11, 0, -1, -1, 12, -1);  // only day:     2021.4.11(Sun) 23:59:57 --> 12
  TestAlarmSub(59,  7, 11, 0, -1,  8, -1, -1);  // only hour:    2021.4.11(Sun)  7:59:57 -->  8:xx
  TestAlarmSub(50,  9, 11, 0, 51, -1, -1, -1);  // only minute:  2021.4.11(Sun)  9:50:57 -->  x:51
  TestAlarmSub(25, 15, 11, 0, 26, 16, -1, -1);  // hour, minute: 2021.4.11(Sun) 15:25:57 --> 16:26 (no alarm)
  TestAlarmSub(25, 15, 11, 0, 26, 15, -1, -1);  // hour, minute: 2021.4.11(Sun) 15:25:57 --> 15:26
  TestAlarmSub(25, 15, 11, 0, 26, 15, 11,  0);  // all:          2021.4.11(Sun) 15:25:57 --> 15:26
}

void TestAlarmSub(int m, int h, int d, int w, int am, int ah, int ad, int aw)
{
  tm_test.tm_sec  = 57;
  tm_test.tm_min  = m;
  tm_test.tm_hour = h;
  tm_test.tm_mday = d;
  tm_test.tm_wday = w;
  tm_test.tm_mon  = 4;
  tm_test.tm_year = 121;  // 2021.04.d(w) h:m:55
  rtcx.WriteTime(&tm_test);

  rtcx.SetAlarm(am, ah, ad, aw);
  Serial.println("[PCF8563] set alarm");
  PrintTestAlarm();

  rtcx.EnableAlarmInterrupt();
  Serial.println("[PCF8563] enable alarm interrupt");

  for (int i = 0; i < 5; ++i) {
    PrintTestAlarm();
    delay(1000);
  }
  int g = rtcx.GetInterrupt();
  Serial.printf("[PCF8563] check interrupt %d\n", g);
  PrintTestAlarm();

  rtcx.DisableAlarm();
  Serial.println("[PCF8563] disable alarm");
  PrintTestAlarm();

  rtcx.DisableAlarmInterrupt();
  Serial.println("[PCF8563] disable alarm interrupt");
  PrintTestAlarm();

  Serial.println();
}

void PrintTestAlarm()
{
  rtcx.ReadAlarm();  PrintAlarm();
  rtcx.ReadTime(&tm_test);  PrintTime();
  rtcx.ReadInterrupt();  PrintInterrupt();  Serial.println();
}

//..:....1....:....2....:....3....:....4....:....5....:....6....:....7....:....8
void TestClockOut()
{
  Serial.println("\n[PCF8563] test: clock out");
  rtcx.ReadClockOut();  PrintClockOut();  Serial.println();

  rtcx.clock_out = Pcf8563::fco_32768Hz;
  rtcx.clock_out_active = true;   PrintTestClockOut();  delay(5000);
  rtcx.clock_out_active = false;  PrintTestClockOut();  delay(2000);

  rtcx.clock_out = Pcf8563::fco_1024Hz;
  rtcx.clock_out_active = true;   PrintTestClockOut();  delay(5000);
  rtcx.clock_out_active = false;  PrintTestClockOut();  delay(2000);

  rtcx.clock_out = Pcf8563::fco_32Hz;
  rtcx.clock_out_active = true;   PrintTestClockOut();  delay(5000);
  rtcx.clock_out_active = false;  PrintTestClockOut();  delay(2000);

  rtcx.clock_out = Pcf8563::fco_1Hz;
  rtcx.clock_out_active = true;   PrintTestClockOut();  delay(5000);
  rtcx.clock_out_active = false;  PrintTestClockOut();  delay(2000);
}

void PrintTestClockOut()
{
  rtcx.WriteClockOut();
  rtcx.ReadClockOut();  PrintClockOut();  Serial.println();
}

//..:....1....:....2....:....3....:....4....:....5....:....6....:....7....:....8
void TestSetTimer()
{
  Serial.println("\n[CF8563] test: set-timer");
  rtcx.ReadTimer();  PrintTimer();  Serial.println();

  const double r = 1.01;  // common ratio of geometric progression
  double t(0.0002);       // the first term
  while (t < 20000.0) {   // the last term
    double v = rtcx.SetTimer(t);
    Serial.printf("Timer set t=%11.5f v=%11.5f ", t, v);
    rtcx.ReadTimer();  PrintTimer();  Serial.println();
    t *= r;
  }
  rtcx.DisableTimer();
}

//..:....1....:....2....:....3....:....4....:....5....:....6....:....7....:....8
int test_start_ms(0);

void TestGetInterrupt()
{
  Serial.println("\n[PCF8563] test: get-interrupt");
  rtcx.ReadTimer();  PrintTimer();  Serial.println();

  for (double t = 0.2; t < 5.0; t *= 2.0) {
    Serial.println();
    test_start_ms = millis();

    double v = rtcx.SetTimer(t);
    Serial.printf("elapse=%6dms timer set t=%11.5f v=%11.5f\n", millis() - test_start_ms, t, v);

    rtcx.EnableTimerInterrupt();  // enable timer interrupt
    PrintTestInterrupt("enable ");

    while (rtcx.GetInterrupt() == 0) {

      rtcx.DisableTimer();  // pause timer
      delay(1000);
      PrintTestInterrupt("pause  ");

      rtcx.EnableTimer();  // resume timer
      delay(1000);
      PrintTestInterrupt("resume ");
    }
    PrintTestInterrupt("get!   ");

    rtcx.DisableTimerInterrupt();
    PrintTestInterrupt("disable");
  }
  rtcx.DisableTimer();
}

void TestTimerPulse()
{
  Serial.println("\n[PCF8563] test: timer-pulse");
  rtcx.ReadTimer();  PrintTimer();  Serial.println();

  for (double t = 0.2; t < 3.0; t *= 1.5) {
    Serial.println();
    test_start_ms = millis();

    double v = rtcx.SetTimer(t);
    Serial.printf("elapse=%6dms, Timer set t=%11.5f v=%11.5f\n", millis() - test_start_ms, t, v);

    rtcx.EnableTimerInterrupt(true, true);  // enable timer interrupt, enable pulse mode
    delay(5000);
    PrintTestInterrupt("enabled ");

    rtcx.EnableTimerInterrupt(false, true);  // disable timer interrupt, enable pulse mode
    delay(5000);
    PrintTestInterrupt("disabled");
  }
  rtcx.DisableTimer();
}

void PrintTestInterrupt(const String s)
{
  Serial.printf("elapse=%6dms %s", millis() - test_start_ms, s);
  rtcx.ReadInterrupt();  PrintInterrupt();
  rtcx.ReadTimer();  PrintTimer();  Serial.println();
}

//..:....1....:....2....:....3....:....4....:....5....:....6....:....7....:....8
void PrintControl()
{
  Serial.printf("[PCF8563:control]TEST1=%d STOP=%d TESTC=%d ",
                rtcx.external_clock_test_mode,
                rtcx.source_clock_stoped,
                rtcx.power_on_reset_override);
}

void PrintInterrupt()
{
  Serial.printf("[PCF8563:interrupt]TI_TP=%d AF=%d TF=%d AIE=%d TIE=%d ",
                rtcx.timer_interrupt_pulse_mode,
                rtcx.alarm_flag_active,
                rtcx.timer_flag_active,
                rtcx.alarm_interrupt_enable,
                rtcx.timer_interrupt_enable);
}

void PrintTime()
{
  Serial.print(&tm_test, "[PCF8563:date-time]%A, %B %d %Y %H:%M:%S ");
}

void PrintAlarm()
{
  Serial.printf("[PCF8563:alarm]EM=%d EH=%d ED=%d EW=%d M=%d H=%d D=%d W=%d ",
                rtcx.alarm_minute_enable,
                rtcx.alarm_hour_enable,
                rtcx.alarm_day_enable,
                rtcx.alarm_weekday_enable,
                rtcx.alarm_minute,
                rtcx.alarm_hour,
                rtcx.alarm_day,
                rtcx.alarm_weekday);
}

void PrintClockOut()
{
  Serial.printf("[PCF8563:clock-out]FE=%d out=%d ",
                rtcx.clock_out_active,
                rtcx.clock_out);
}

void PrintTimer()
{
  double timer_remain(0.0);
  switch (rtcx.timer_source) {
    case 0: timer_remain = rtcx.timer / 4096.0; break;
    case 1: timer_remain = rtcx.timer / 64.0;   break;
    case 2: timer_remain = rtcx.timer / 1.0;    break;
    case 3: timer_remain = rtcx.timer * 60.0;   break;
    default: break;
  }
  Serial.printf("[PCF8563:timer]TE=%d src=%d timer=%3d remain=%11.5f ",
                rtcx.timer_enable,
                rtcx.timer_source,
                rtcx.timer,
                timer_remain);
}
