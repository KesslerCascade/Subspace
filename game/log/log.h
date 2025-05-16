#pragma once

// notify log system that control client thread is up and running and it should switch to that instead of direct sends
void log_client();

enum LOG_LEVEL_ENUM {
    LOG_Fatal,
    LOG_Error,
    LOG_Warn,
    LOG_Notice,
    LOG_Info,
    LOG_Verbose,
    LOG_Diag,           // Release build diagnostics not normally needed
    LOG_Debug,          // Compiled out of non-development builds
    LOG_Trace,          // Only available in debug builds

    LOG_Count           // Not a real level, for static assertions
};

int log_set_max_level(int level);
void log_str(int level, const char* str);
void log_fmt(int level, const char* fmt, ...);