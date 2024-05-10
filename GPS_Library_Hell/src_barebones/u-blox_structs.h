#pragma once

struct ubxAutomaticFlags
{
  union
  {
    uint8_t all;
    struct
    {
      uint8_t automatic : 1;         // Will this message be delivered and parsed "automatically" (without polling)
      uint8_t implicitUpdate : 1;    // Is the update triggered by accessing stale data (=true) or by a call to checkUblox (=false)
      uint8_t addToFileBuffer : 1;   // Should the raw UBX data be added to the file buffer?
      uint8_t callbackCopyValid : 1; // Is the copy of the data struct used by the callback valid/fresh?
    } bits;
  } flags;
};

typedef struct
{
  uint32_t iTOW; // GPS time of week of the navigation epoch: ms
  uint16_t year; // Year (UTC)
  uint8_t month; // Month, range 1..12 (UTC)
  uint8_t day;   // Day of month, range 1..31 (UTC)
  uint8_t hour;  // Hour of day, range 0..23 (UTC)
  uint8_t min;   // Minute of hour, range 0..59 (UTC)
  uint8_t sec;   // Seconds of minute, range 0..60 (UTC)
  union
  {
    uint8_t all;
    struct
    {
      uint8_t validDate : 1;     // 1 = valid UTC Date
      uint8_t validTime : 1;     // 1 = valid UTC time of day
      uint8_t fullyResolved : 1; // 1 = UTC time of day has been fully resolved (no seconds uncertainty).
      uint8_t validMag : 1;      // 1 = valid magnetic declination
    } bits;
  } valid;
  uint32_t tAcc;   // Time accuracy estimate (UTC): ns
  int32_t nano;    // Fraction of second, range -1e9 .. 1e9 (UTC): ns
  uint8_t fixType; // GNSSfix Type:
                   // 0: no fix
                   // 1: dead reckoning only
                   // 2: 2D-fix
                   // 3: 3D-fix
                   // 4: GNSS + dead reckoning combined
                   // 5: time only fix
  union
  {
    uint8_t all;
    struct
    {
      uint8_t gnssFixOK : 1; // 1 = valid fix (i.e within DOP & accuracy masks)
      uint8_t diffSoln : 1;  // 1 = differential corrections were applied
      uint8_t psmState : 3;
      uint8_t headVehValid : 1; // 1 = heading of vehicle is valid, only set if the receiver is in sensor fusion mode
      uint8_t carrSoln : 2;     // Carrier phase range solution status:
                                // 0: no carrier phase range solution
                                // 1: carrier phase range solution with floating ambiguities
                                // 2: carrier phase range solution with fixed ambiguities
    } bits;
  } flags;
  union
  {
    uint8_t all;
    struct
    {
      uint8_t reserved : 5;
      uint8_t confirmedAvai : 1; // 1 = information about UTC Date and Time of Day validity confirmation is available
      uint8_t confirmedDate : 1; // 1 = UTC Date validity could be confirmed
      uint8_t confirmedTime : 1; // 1 = UTC Time of Day could be confirmed
    } bits;
  } flags2;
  uint8_t numSV;    // Number of satellites used in Nav Solution
  int32_t lon;      // Longitude: deg * 1e-7
  int32_t lat;      // Latitude: deg * 1e-7
  int32_t height;   // Height above ellipsoid: mm
  int32_t hMSL;     // Height above mean sea level: mm
  uint32_t hAcc;    // Horizontal accuracy estimate: mm
  uint32_t vAcc;    // Vertical accuracy estimate: mm
  int32_t velN;     // NED north velocity: mm/s
  int32_t velE;     // NED east velocity: mm/s
  int32_t velD;     // NED down velocity: mm/s
  int32_t gSpeed;   // Ground Speed (2-D): mm/s
  int32_t headMot;  // Heading of motion (2-D): deg * 1e-5
  uint32_t sAcc;    // Speed accuracy estimate: mm/s
  uint32_t headAcc; // Heading accuracy estimate (both motion and vehicle): deg * 1e-5
  uint16_t pDOP;    // Position DOP * 0.01
  union
  {
    uint8_t all;
    struct
    {
      uint8_t invalidLlh : 1;        // 1 = Invalid lon, lat, height and hMSL
      uint8_t lastCorrectionAge : 4; // Age of the most recently received differential correction:
                                     // 0: Not available
                                     // 1: Age between 0 and 1 second
                                     // 2: Age between 1 (inclusive) and 2 seconds
                                     // 3: Age between 2 (inclusive) and 5 seconds
                                     // 4: Age between 5 (inclusive) and 10 seconds
                                     // 5: Age between 10 (inclusive) and 15 seconds
                                     // 6: Age between 15 (inclusive) and 20 seconds
                                     // 7: Age between 20 (inclusive) and 30 seconds
                                     // 8: Age between 30 (inclusive) and 45 seconds
                                     // 9: Age between 45 (inclusive) and 60 seconds
                                     // 10: Age between 60 (inclusive) and 90 seconds
                                     // 11: Age between 90 (inclusive) and 120 seconds
                                     // >=12: Age greater or equal than 120 seconds
    } bits;
  } flags3;
  uint8_t reserved1[5];
  int32_t headVeh; // Heading of vehicle (2-D): deg * 1e-5
  int16_t magDec;  // Magnetic declination: deg * 1e-2
  uint16_t magAcc; // Magnetic declination accuracy: deg * 1e-2
} UBX_NAV_PVT_data_t;

typedef struct
{
  union
  {
    uint32_t all;
    struct
    {
      uint32_t all : 1;

      uint32_t iTOW : 1;
      uint32_t year : 1;
      uint32_t month : 1;
      uint32_t day : 1;
      uint32_t hour : 1;
      uint32_t min : 1;
      uint32_t sec : 1;

      uint32_t validDate : 1;
      uint32_t validTime : 1;
      uint32_t fullyResolved : 1;
      uint32_t validMag : 1;

      uint32_t tAcc : 1;
      uint32_t nano : 1;
      uint32_t fixType : 1;
      uint32_t gnssFixOK : 1;
      uint32_t diffSoln : 1;
      uint32_t psmState : 1;
      uint32_t headVehValid : 1;
      uint32_t carrSoln : 1;

      uint32_t confirmedAvai : 1;
      uint32_t confirmedDate : 1;
      uint32_t confirmedTime : 1;

      uint32_t numSV : 1;
      uint32_t lon : 1;
      uint32_t lat : 1;
      uint32_t height : 1;
      uint32_t hMSL : 1;
      uint32_t hAcc : 1;
      uint32_t vAcc : 1;
      uint32_t velN : 1;
      uint32_t velE : 1;
    } bits;
  } moduleQueried1;
  union
  {
    uint32_t all;
    struct
    {
      uint32_t velD : 1;
      uint32_t gSpeed : 1;
      uint32_t headMot : 1;
      uint32_t sAcc : 1;
      uint32_t headAcc : 1;
      uint32_t pDOP : 1;

      uint32_t invalidLlh : 1;

      uint32_t headVeh : 1;
      uint32_t magDec : 1;
      uint32_t magAcc : 1;
    } bits;
  } moduleQueried2;
} UBX_NAV_PVT_moduleQueried_t;


typedef struct
{
  ubxAutomaticFlags automaticFlags;
  UBX_NAV_PVT_data_t data;
  UBX_NAV_PVT_moduleQueried_t moduleQueried;
  void (*callbackPointerPtr)(UBX_NAV_PVT_data_t *);
  UBX_NAV_PVT_data_t *callbackData;
} UBX_NAV_PVT_t;

typedef struct
{
  union
  {
    uint32_t all;
    struct
    {
      uint32_t all : 1;

      uint32_t iTOW : 1;
      uint32_t ecefX : 1;
      uint32_t ecefY : 1;
      uint32_t ecefZ : 1;
      uint32_t pAcc : 1;
    } bits;
  } moduleQueried;
} UBX_NAV_POSECEF_moduleQueried_t;

typedef struct
{
  uint32_t iTOW; // GPS time of week of the HNR epoch: ms
  uint16_t year; // Year (UTC)
  uint8_t month; // Month, range 1..12 (UTC)
  uint8_t day;   // Day of month, range 1..31 (UTC)
  uint8_t hour;  // Hour of day, range 0..23 (UTC)
  uint8_t min;   // Minute of hour, range 0..59 (UTC)
  uint8_t sec;   // Seconds of minute, range 0..60 (UTC)
  union
  {
    uint8_t all;
    struct
    {
      uint8_t validDate : 1;     // 1 = Valid UTC Date
      uint8_t validTime : 1;     // 1 = Valid UTC Time of Day
      uint8_t fullyResolved : 1; // 1 = UTC Time of Day has been fully resolved
    } bits;
  } valid;
  int32_t nano;   // Fraction of second (UTC): ns
  uint8_t gpsFix; // GPSfix Type, range 0..5
                  // 0x00 = No Fix
                  // 0x01 = Dead Reckoning only
                  // 0x02 = 2D-Fix
                  // 0x03 = 3D-Fix
                  // 0x04 = GPS + dead reckoning combined
                  // 0x05 = Time only fix
                  // 0x06..0xff: reserved
  union
  {
    uint8_t all;
    struct
    {
      uint8_t gpsFixOK : 1;     // >1 = Fix within limits (e.g. DOP & accuracy)
      uint8_t diffSoln : 1;     // 1 = DGPS used
      uint8_t WKNSET : 1;       // 1 = Valid GPS week number
      uint8_t TOWSET : 1;       // 1 = Valid GPS time of week (iTOW & fTOW)
      uint8_t headVehValid : 1; // 1= Heading of vehicle is valid
    } bits;
  } flags;
  uint8_t reserved1[2];
  int32_t lon;      // Longitude: Degrees * 1e-7
  int32_t lat;      // Latitude: Degrees * 1e-7
  int32_t height;   // Height above ellipsoid: mm
  int32_t hMSL;     // Height above MSL: mm
  int32_t gSpeed;   // Ground Speed (2-D): mm/s
  int32_t speed;    // Speed (3-D): mm/s
  int32_t headMot;  // Heading of motion (2-D): Degrees * 1e-5
  int32_t headVeh;  // Heading of vehicle (2-D): Degrees * 1e-5
  uint32_t hAcc;    // Horizontal accuracy: mm
  uint32_t vAcc;    // Vertical accuracy: mm
  uint32_t sAcc;    // Speed accuracy: mm/s
  uint32_t headAcc; // Heading accuracy: Degrees * 1e-5
  uint8_t reserved2[4];
} UBX_HNR_PVT_data_t;
  // Additional flags and pointers that need to be stored with each message type