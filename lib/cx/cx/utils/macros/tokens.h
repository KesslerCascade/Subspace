#pragma once

// token pasting and stringifcation macros

#define _tokconcat_actual(x,y) x##y
#define tokconcat(x,y) _tokconcat_actual(x,y)
#define tokcat2(x,y) _tokconcat_actual(x,y)
#define tokcat3(x1, x2, x3) tokcat2(tokcat2(x1, x2), x3)
#define tokcat4(x1, x2, x3, x4) tokcat2(tokcat3(x1, x2, x3), x4)
#define tokcat5(x1, x2, x3, x4, x5) tokcat2(tokcat4(x1, x2, x3, x4), x5)
#define _tokconcatU_actual(x,y) x##_##y
#define tokconcatU(x,y) _tokconcatU_actual(x,y)
#define tokcatU2(x,y) _tokconcatU_actual(x,y)
#define tokcatU3(x1, x2, x3) tokcatU2(tokcatU2(x1, x2), x3)
#define tokcatU4(x1, x2, x3, x4) tokcatU2(tokcatU3(x1, x2, x3), x4)
#define tokcatU5(x1, x2, x3, x4, x5) tokcatU2(tokcatU4(x1, x2, x3, x4), x5)
#define _tokstring_actual(x) #x
#define tokstring(x) _tokstring_actual(x)

// for splitting out tokens and dealing with MSVC weirdness mostly
#define tokeval(...) __VA_ARGS__
