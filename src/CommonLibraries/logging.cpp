
#include "logging.hpp"
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
static const char* ERROR_LEVEL_TEXT[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

class log_config {
  public:
    static log_config& get_instance() {
        static log_config Instance;
        return Instance;
    }
    int get_log_lvl() { return m_log_level; }
    void set_log_lvl(u_int16_t in) { m_log_level = in; }
    int get_print_log_lvl() { return m_print_log_level; }
    void set_print_log_lvl(u_int16_t in) { m_print_log_level = in; }
    int get_log_file_size() { return m_log_file_size; }
    void set_log_file_size(u_int16_t in) { m_log_file_size = in; }
    std::string get_log_filename() { return m_log_filename; }
    void set_log_filename(std::string in, std::ios_base::openmode mode) {
        m_log_filename = in;
        m_logfile.close();
        m_logfile.open(m_log_filename, mode | std::ios::in);
    }
    std::fstream& get_log_file() { return m_logfile; }

  private:
    u_int16_t m_log_level;
    u_int16_t m_print_log_level;
    u_int16_t m_log_file_size; // not set
    std::string m_log_filename;
    std::fstream m_logfile;
    log_config() : m_log_level(2), m_log_file_size(0), m_log_filename(""){};
    ~log_config() {
        if (m_logfile.is_open())
            m_logfile.flush();
        m_logfile.close();
    };
};

std::streamsize getFileSize(std::fstream& file) {
    std::streampos currentPos = file.tellg();
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(currentPos);

    return size; // File size in bytes
}
void optimize_log_file(std::fstream& file, const char* filename, int max_size) {
    char new_text[1024];
    long current_size = getFileSize(file);
    file.seekp(max_size / 2, std::ios::beg);

    if (ftruncate(fileno(fopen(filename, "r+")), max_size / 2)) {
        sprintf(new_text, "\n\n error reducing log size %ld to %d: %s\n\n", current_size, max_size / 2, strerror(errno));
        file << new_text;

    } else {
        sprintf(new_text, "\n\n reducing log size %ld to %d \n\n", current_size, max_size / 2);
        file << new_text;
    }

    file.flush();
}

void OPEN_LOGGER_INTERNAL(bool append) {

    if (log_config::get_instance().get_log_filename().empty()) {
        const char* tmpFilename = getenv(ENV_LOG_FILE);
        //  printf(" log file %s \n", tmpFilename.c_str());
        std::ofstream file(tmpFilename, std::ios::app);
        if (!file.is_open()) {
            tmpFilename = "log ";
            file.open(tmpFilename, std::ios::app);
            if (!file.is_open())
                tmpFilename = NULL;
        }
        file.close();
        if (tmpFilename) {
            if (append)
                log_config::get_instance().set_log_filename(realpath(tmpFilename, NULL), std::ios::app);
            else
                log_config::get_instance().set_log_filename(realpath(tmpFilename, NULL), std::ios::out);
        }

        const char* str = getenv(ENV_LOG_LEVEL);
        u_int16_t log_level(2), print_level(2);
        if (str)
            log_level = atoi(str);
        str = getenv(ENV_LOG_LEVEL);
        if (str)
            print_level = atoi(str);
        log_config::get_instance().set_log_lvl((log_level < print_level) ? log_level : print_level);
        str = getenv(ENV_LOG_PRINT_LEVEL);
        if (str)
            print_level = atoi(str);
        log_config::get_instance().set_print_log_lvl((log_level < print_level) ? log_level : print_level);
        if (log_config::get_instance().get_print_log_lvl() < log_config::get_instance().get_log_lvl())
            log_config::get_instance().set_log_lvl(log_config::get_instance().get_print_log_lvl());
        str = getenv(ENV_LOG_FILE_MAX_SIZE);
        if (str)
            log_config::get_instance().set_log_file_size(atol(str));
    }
}

void LOGGER(const char* file, const char* func, int line, ERROR_LEVEL level, const char* text, ...) {

    if (!log_config::get_instance().get_log_file().is_open()) {
        int logAppend = 0;
        char* a = getenv(ENV_LOG_APPEND);
        if (a)
            logAppend = atoi(a);
        if (logAppend)
            OPEN_LOGGER_INTERNAL(true);
        else
            OPEN_LOGGER_INTERNAL(false);
    }
    if (level >= log_config::get_instance().get_log_lvl()) {
        char new_text[1024]; // Adjust size based on the maximum size needed
        // Check log size
        if (log_config::get_instance().get_log_file_size() &&
            getFileSize(log_config::get_instance().get_log_file()) > log_config::get_instance().get_log_file_size()) {

            optimize_log_file(log_config::get_instance().get_log_file(), log_config::get_instance().get_log_filename().c_str(),
                              log_config::get_instance().get_log_file_size());
        }
        // Write log entry
        char time_buffer[50];
        struct timeval t;
        gettimeofday(&t, NULL);
        strftime(time_buffer, 40, "%H:%M:%S", gmtime(&(t.tv_sec)));
        if (level < ALL) {

            snprintf(new_text, sizeof(new_text), "%s.%04ld %s %s[%s:%d]: ", time_buffer, t.tv_usec / 100, ERROR_LEVEL_TEXT[level], func, file, line);
        } else {
            snprintf(new_text, sizeof(new_text), "%s.%04ld  %s[%s:%d]: ", time_buffer, t.tv_usec / 100, func, file, line);
        }
        va_list argzeiger;
        va_start(argzeiger, text);
        strncat(new_text, text, sizeof(new_text) - strlen(new_text) - 1);
        char buffer[1024];
        vsprintf(buffer, new_text, argzeiger);
        log_config::get_instance().get_log_file() << buffer;
        if (level >= LEVEL_INFO)
            log_config::get_instance().get_log_file().flush();
        if (level >= log_config::get_instance().get_print_log_lvl()) {
            fprintf(stderr, "%s", buffer);
        }
        va_end(argzeiger);
    }
}

void CLOSE_LOGGER() { log_config::get_instance().get_log_file().close(); }

void OPEN_LOGGER() { OPEN_LOGGER_INTERNAL("w"); }

void OPEN_LOGGER_APPEND() { OPEN_LOGGER_INTERNAL("a"); }

void FLUSH_LOGGER() {
    if (log_config::get_instance().get_log_file().is_open())
        log_config::get_instance().get_log_file().flush();
}

const char* GET_LOGGER_FILENAME() { return log_config::get_instance().get_log_filename().c_str(); }
