#ifndef LOGGER_
#define LOGGER_
#include <string>
// env values can be used to change config:
#define ENV_LOG_FILE "LOG_FILE"
#define ENV_LOG_LEVEL "LOG_LEVEL"
#define ENV_LOG_PRINT_LEVEL "LOG_PRINT_LEVEL"
#define ENV_LOG_FILE_MAX_SIZE "LOG_FILE_MAX_SIZE"
#define ENV_LOG_SYNC "LOG_SYNC"     // Set to anything for sync write
#define ENV_LOG_APPEND "LOG_APPEND" // set to one to stop log file overwritten

typedef enum { LEVEL_TRACE, LEVEL_DEBUG, LEVEL_INFO, LEVEL_WARN, LEVEL_ERROR, LEVEL_FATAL, ALL } ERROR_LEVEL;

#define LOG(level, ...) LOGGER(__FILE__, __func__, __LINE__, level, ##__VA_ARGS__);

#define LOG_TRACE(...) LOGGER(__FILE__, __func__, __LINE__, LEVEL_TRACE, ##__VA_ARGS__);

#define LOG_DEBUG(...) LOGGER(__FILE__, __func__, __LINE__, LEVEL_DEBUG, ##__VA_ARGS__);

#define LOG_INFO(...) LOGGER(__FILE__, __func__, __LINE__, LEVEL_INFO, ##__VA_ARGS__);
#define LOG_WARN(...) LOGGER(__FILE__, __func__, __LINE__, LEVEL_WARN, ##__VA_ARGS__);
#define LOG_ERROR(...) LOGGER(__FILE__, __func__, __LINE__, LEVEL_ERROR, ##__VA_ARGS__);
#define LOG_FATAL(...) LOGGER(__FILE__, __func__, __LINE__, LEVEL_FATAL, ##__VA_ARGS__);
void LOGGER(const char* file, const char* func, int line, ERROR_LEVEL level, const char* text, ...);

void CLOSE_LOGGER();
void FLUSH_LOGGER();
void OPEN_LOGGER();
void OPEN_LOGGER_APPEND();
//
const char* GET_LOGGER_FILENAME();

#endif
