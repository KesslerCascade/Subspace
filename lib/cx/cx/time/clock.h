#pragma once

#include <cx/cx.h>

// Gets the current "wall time" (system time). Returns the number of microseconds
// since the julian date epoch.
int64 clockWall();

// Similar to clockWall, but adjust the time for local time zone rules currently
// in effect. This is not a proper time reference and may be discontinuous -- it
// is intended only for displaying the current date and time.
int64 clockWallLocal();

// Gets a monotonically increasing time that is guaranteed to not go backwards or
// skip around when the clock is reset. The actual value does not have any
// particular meaning other than being measured in microseconds, and may not be
// unique across operating system restarts. This should be used for timing events.
int64 clockTimer();
