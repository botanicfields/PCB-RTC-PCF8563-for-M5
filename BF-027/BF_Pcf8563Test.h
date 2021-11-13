// Copyright 2021 BotanicFields, Inc.
// BF-027 PCF8563 RTC for Grove I2C
// test of BF_Pcf8563.h/cpp

#pragma once

void TestControl();
void PrintTestControl();

void CompareRtcxTime();
void PrintTimeval();

void TestAlarm();
void TestAlarmSub(int m, int h, int d, int w, int am, int ah, int ad, int aw);
void PrintTestAlarm();

void TestClockOut();
void PrintTestClockOut();

void TestSetTimer();

void TestGetInterrupt();
void TestTimerPulse();
void PrintTestInterrupt(const char* s);

void PrintControl();
void PrintInterrupt();
void PrintTime();
void PrintAlarm();
void PrintClockOut();
void PrintTimer();
