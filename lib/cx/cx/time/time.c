#include "time.h"

#define SECSPERMIN 60
#define SECSPERHOUR (60*60)
#define SECSPERDAY (24*60*60)
#define DAYSPERWEEK 7

#define EPOCH_WDAY Monday

strref timeDayName[7] = {
    (strref)"\xE1\xC1\x06""Sunday",
    (strref)"\xE1\xC1\x06""Monday",
    (strref)"\xE1\xC1\x07""Tuesday",
    (strref)"\xE1\xC1\x09""Wednesday",
    (strref)"\xE1\xC1\x08""Thursday",
    (strref)"\xE1\xC1\x06""Friday",
    (strref)"\xE1\xC1\x08""Saturday"
};
strref timeDayAbbrev[7] = {
    (strref)"\xE1\xC1\x03""Sun",
    (strref)"\xE1\xC1\x03""Mon",
    (strref)"\xE1\xC1\x03""Tue",
    (strref)"\xE1\xC1\x03""Wed",
    (strref)"\xE1\xC1\x03""Thu",
    (strref)"\xE1\xC1\x03""Fri",
    (strref)"\xE1\xC1\x03""Sat"
};
strref timeMonthName[13] = {
    (strref)"\xE1\xC1\x00",
    (strref)"\xE1\xC1\x07""January",
    (strref)"\xE1\xC1\x08""February",
    (strref)"\xE1\xC1\x05""March",
    (strref)"\xE1\xC1\x05""April",
    (strref)"\xE1\xC1\x03""May",
    (strref)"\xE1\xC1\x04""June",
    (strref)"\xE1\xC1\x04""July",
    (strref)"\xE1\xC1\x06""August",
    (strref)"\xE1\xC1\x09""September",
    (strref)"\xE1\xC1\x07""October",
    (strref)"\xE1\xC1\x08""November",
    (strref)"\xE1\xC1\x08""December"
};
strref timeMonthAbbrev[13] = {
    (strref)"\xE1\xC1\x00",
    (strref)"\xE1\xC1\x03""Jan",
    (strref)"\xE1\xC1\x03""Feb",
    (strref)"\xE1\xC1\x03""Mar",
    (strref)"\xE1\xC1\x03""Apr",
    (strref)"\xE1\xC1\x03""May",
    (strref)"\xE1\xC1\x03""Jun",
    (strref)"\xE1\xC1\x03""Jul",
    (strref)"\xE1\xC1\x03""Aug",
    (strref)"\xE1\xC1\x03""Sep",
    (strref)"\xE1\xC1\x03""Oct",
    (strref)"\xE1\xC1\x03""Nov",
    (strref)"\xE1\xC1\x03""Dec"
};

_Use_decl_annotations_
bool timeDecompose(TimeParts *out, int64 time)
{
    if (time < 0)
        return false;

    int64 secs;
    int32 days;
    int32 rem;

    // Julian date starts at noon, but the math is much easier if we start at
    // midnight, so sacrifice a tiny bit of range a few hundred thousand years
    // in the future to pretend that the epoch is actually midnight.
    time += 43200000000;

    // time of day is the easy part
    out->usec = time % 1000000;
    secs = time / 1000000;
    days = (int32)(secs / SECSPERDAY);
    rem = secs % SECSPERDAY;
    out->hour = (uint8)(rem / SECSPERHOUR);
    rem %= SECSPERHOUR;
    out->minute = (uint8)(rem / SECSPERMIN);
    out->second = rem % SECSPERMIN;
    out->wday = (EPOCH_WDAY + days) % DAYSPERWEEK;

    // now the hard part
    // implemented using the Fliegel-Van Flandern algorithm
    int32 p, q, r, s, t, u, v;
    p = days + 68569;
    q = 4 * p / 146097;
    r = p - (146097 * q + 3) / 4;
    s = 4000 * (r + 1) / 1461001;
    t = r - 1461 * s / 4 + 31;
    u = 80 * t / 2447;
    v = u / 11;

    out->year = 100 * (q - 49) + s + v;
    out->month = u + 2 - 12 * v;
    out->day = t - 2447 * u / 80;

    int32 ysd = 1461 * (out->year + 4799) / 4 - (3 * ((out->year + 4899) / 100)) / 4 - 31739;
    out->yday = days - ysd;

    return true;
}

_Use_decl_annotations_
int64 timeCompose(TimeParts *parts)
{
    int64 time;

    int32 m1 = (parts->month - 14) / 12;
    int32 y1 = parts->year + 4800;
    int32 j = 1461 * (y1 + m1) / 4 + 367 * (parts->month - 2 - 12 * m1) / 12
        - (3 * ((y1 + m1 + 100) / 100)) / 4 + parts->day - 32075;

    if (j < 0)
        return 0;

    time = (int64)j * SECSPERDAY;
    time -= 43200;      // julian half day adjustment

    int32 secs = (parts->hour) * SECSPERHOUR + (parts->minute) * SECSPERMIN + parts->second;
    time += (int64)secs;
    time *= 1000000;
    time += parts->usec;

    return time;
}
