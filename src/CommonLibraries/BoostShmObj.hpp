#ifndef INCLUDE_BOOSTSHMOBJ_HPP_
#define INCLUDE_BOOSTSHMOBJ_HPP_

#include <cstring>
#include <fcntl.h>
#include <semaphore.h>
#include <stdexcept>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

#define MAXLEN_NAME 50
#define MAXLEN_VALUE 5000
#define MAXNUMBER_OBJECTS 50

#define SETVALUE_OK 0
#define SETVALUE_FAIL 1

struct C_sharedObjectElement {
    char name[MAXLEN_NAME];
    char value[MAXLEN_VALUE];
};

struct C_sharedObject {
    C_sharedObjectElement element[MAXNUMBER_OBJECTS];
};

class C_BoostShmObj {
  public:
    C_BoostShmObj(const char* name);
    C_BoostShmObj(const char* name, int elementNumber);
    ~C_BoostShmObj();

    int SetInt(const char* valueName, int newValue);
    int SetFloat(const char* valueName, float newValue);

    int GetInt(const char* valueName);
    float GetFloat(const char* valueName);

    int SetValue(const char* valueName, const char* newValue);
    int GetValue(const char* valueName, std::string& value);

    void Remove();

  private:
    std::string m_name;
    std::string m_guardName;
    C_sharedObject* m_pSharedObject;
    sem_t* m_guard; // Semaphore for inter-process synchronization
    int m_numberOfElements;
    int m_shmFd;
};

#endif /* INCLUDE_BOOSTSHMOBJ_HPP_ */
