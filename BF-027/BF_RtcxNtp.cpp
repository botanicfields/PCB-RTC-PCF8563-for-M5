// Copyright 2021 BotanicFields, Inc.
// routines for NTP and RTCx(PCf8563)

#include "BF_RtcxNtp.h"

const char* time_zone  = "JST-9";
const char* ntp_server = "pool.ntp.org";

int SetTimeFromRtcx()
{
  struct tm tm_init;

  setenv("TZ", time_zone, 1);
  tzset();  // Assign the local timezone from setenv for mktime()
  if (rtcx.ReadTime(&tm_init) == 0) {
    struct timeval tv = { mktime(&tm_init), 0 };
    settimeofday(&tv, NULL);
    Serial.print("Set time from RTCx: RTCx valid\n");
  }
  else
    Serial.print("Set time from RTCx: ERROR RTCx invalid\n");

  getLocalTime(&tm_init);
  Serial.print(&tm_init, "Set time from RTCx: %A, %B %d %Y %H:%M:%S\n");
  // print sample: must be < 64
  //....:....1....:....2....:....3....:....4....:....5....:....6....
  //Set time from RTCx: Wednesday, September 11 2021 11:10:46
}

void NtpBegin()
{
  configTzTime(time_zone, ntp_server);
  Serial.print("NtpBegin: Config TZ time\n");
  Serial.printf("NtpBegin: SNTP sync mode = %d (0:IMMED 1:SMOOTH)\n", sntp_get_sync_mode());
//  Serial.printf("NtpBegin: SNTP sync interval = %dms\n", sntp_get_sync_interval());  // not supported
  sntp_set_time_sync_notification_cb(SntpTimeSyncNotificationCallback);
}

void PrintSntpStatus(const char* header, sntp_sync_status_t sntp_sync_status)
{
  static const char* sntp_sync_status_str[] = {
    "SNTP_SYNC_STATUS_RESET       ",  // 0
    "SNTP_SYNC_STATUS_COMPLETED   ",  // 1
    "SNTP_SYNC_STATUS_IN_PROGRESS ",  // 2 
    "sntp_sync_status invalid     ",  // 3 
  };
  int sntp_sync_status_index = 3;
  if (sntp_sync_status >= 0 && sntp_sync_status <= 2)
    sntp_sync_status_index = sntp_sync_status;
  Serial.printf("%s SNTP sync status = %d: %s\n", header, sntp_sync_status, sntp_sync_status_str[sntp_sync_status_index]);
}

void SntpTimeSyncNotificationCallback(struct timeval *tv)
{
  sntp_sync_status_t sntp_sync_status = sntp_get_sync_status();
  PrintSntpStatus("SNTP callback:", sntp_sync_status);

  // synchronize time of RTCx 
  if (sntp_sync_status == SNTP_SYNC_STATUS_COMPLETED) {
    struct tm tm_sync;
    getLocalTime(&tm_sync);
    if (rtcx.WriteTime(&tm_sync) == 0)
      Serial.print("SNTP callback: RTCx time updated\n");
    else
      Serial.print("SNTP callback: RTCx update failed\n");
    Serial.print(&tm_sync, "SNTP callback: Sync = %A, %B %d %Y %H:%M:%S\n");
    // print sample: must be < 64
    //....:....1....:....2....:....3....:....4....:....5....:....6....
    //SNTP callback: Sync = Wednesday, September 11 2021 11:10:46
  }
}
