1. make sure the config.ini file for this client is present and absolute path is provided for the client  either on class initialization or by setting ConfigPath value in utils.hp
2. start the client 
3. please make sure the user ( excuter user) has write access to /dev/shm

by passing following flags all logs can be monitors locally
export LOG_PRINT_LEVEL=0
export LOG_LEVEL=0