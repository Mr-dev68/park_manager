1. make sure the config.ini file is present and the absolute path is provided for the server either on class initialization or by setting ConfigPath value in utils.hpp
2. make sure the server has write and read access to the path where config.ini is present
3. start the server
4. please make sure the user ( excuter user) has write access to /dev/shm

by passing following flags all logs can be monitors locally
export LOG_PRINT_LEVEL=0
export LOG_LEVEL=0
