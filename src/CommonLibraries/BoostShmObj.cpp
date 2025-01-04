#include "BoostShmObj.hpp"
#include <iostream>
#include <sstream>

C_BoostShmObj::C_BoostShmObj(const char* name) : m_name(name), m_guardName(m_name + "::Guard"), m_numberOfElements(MAXNUMBER_OBJECTS) {
    // Create or open shared memory
    m_shmFd = shm_open(m_name.c_str(), O_CREAT | O_RDWR, 0666);
    if (m_shmFd == -1) {
        throw std::runtime_error("Failed to create or open shared memory");
    }

    // Resize shared memory
    size_t shmSize = sizeof(C_sharedObject);
    if (ftruncate(m_shmFd, shmSize) == -1) {
        throw std::runtime_error("Failed to resize shared memory");
    }

    // Map shared memory to process address space
    void* addr = mmap(nullptr, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_shmFd, 0);
    if (addr == MAP_FAILED) {
        throw std::runtime_error("Failed to map shared memory");
    }

    m_pSharedObject = static_cast<C_sharedObject*>(addr);
    memset(m_pSharedObject, 0, shmSize);

    // Create or open the semaphore
    m_guard = sem_open(m_guardName.c_str(), O_CREAT, 0666, 1);
    if (m_guard == SEM_FAILED) {
        throw std::runtime_error("Failed to create or open semaphore");
    }
}

C_BoostShmObj::C_BoostShmObj(const char* name, int elementNumber) : m_name(name), m_guardName(m_name + "::Guard"), m_numberOfElements(elementNumber) {
    // Shared memory initialization
    m_shmFd = shm_open(m_name.c_str(), O_CREAT | O_RDWR, 0666);
    if (m_shmFd == -1) {
        throw std::runtime_error("Failed to create or open shared memory");
    }

    size_t shmSize = elementNumber * sizeof(C_sharedObjectElement);
    if (ftruncate(m_shmFd, shmSize) == -1) {
        throw std::runtime_error("Failed to resize shared memory");
    }

    void* addr = mmap(nullptr, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_shmFd, 0);
    if (addr == MAP_FAILED) {
        throw std::runtime_error("Failed to map shared memory");
    }

    m_pSharedObject = static_cast<C_sharedObject*>(addr);
    memset(m_pSharedObject, 0, shmSize);

    m_guard = sem_open(m_guardName.c_str(), O_CREAT, 0666, 1);
    if (m_guard == SEM_FAILED) {
        throw std::runtime_error("Failed to create or open semaphore");
    }
}

C_BoostShmObj::~C_BoostShmObj() {
    if (m_pSharedObject) {
        munmap(m_pSharedObject, sizeof(C_sharedObject));
    }
    if (m_shmFd != -1) {
        close(m_shmFd);
    }
    if (m_guard) {
        sem_close(m_guard);
    }
}

int C_BoostShmObj::SetInt(const char* valueName, int newValue) {
    std::stringstream ss;
    ss << newValue;
    return SetValue(valueName, ss.str().c_str());
}

int C_BoostShmObj::SetFloat(const char* valueName, float newValue) {
    std::stringstream ss;
    ss << newValue;
    return SetValue(valueName, ss.str().c_str());
}

int C_BoostShmObj::GetInt(const char* valueName) {
    std::string value;
    if (GetValue(valueName, value) == SETVALUE_OK) {
        return std::stoi(value);
    }
    return 0;
}

float C_BoostShmObj::GetFloat(const char* valueName) {
    std::string value;
    if (GetValue(valueName, value) == SETVALUE_OK) {
        return std::stof(value);
    }
    return 0.0f;
}

int C_BoostShmObj::SetValue(const char* valueName, const char* newValue) {
    if (sem_wait(m_guard) == -1) {
        perror("sem_wait failed");
        return SETVALUE_FAIL;
    }

    for (int i = 0; i < m_numberOfElements; ++i) {
        if (m_pSharedObject->element[i].name[0] == '\0' || strcmp(m_pSharedObject->element[i].name, valueName) == 0) {
            strncpy(m_pSharedObject->element[i].name, valueName, MAXLEN_NAME);
            strncpy(m_pSharedObject->element[i].value, newValue, MAXLEN_VALUE);
            sem_post(m_guard);
            return SETVALUE_OK;
        }
    }

    sem_post(m_guard);
    return SETVALUE_FAIL;
}

int C_BoostShmObj::GetValue(const char* valueName, std::string& value) {
    if (sem_wait(m_guard) == -1) {
        perror("sem_wait failed");
        return SETVALUE_FAIL;
    }

    for (int i = 0; i < m_numberOfElements; ++i) {
        if (strcmp(m_pSharedObject->element[i].name, valueName) == 0) {
            value = m_pSharedObject->element[i].value;
            sem_post(m_guard);
            return SETVALUE_OK;
        }
    }

    sem_post(m_guard);
    value = "";
    return SETVALUE_FAIL;
}

void C_BoostShmObj::Remove() {
    if (sem_wait(m_guard) == -1) {
        perror("sem_wait failed");
        return;
    }

    shm_unlink(m_name.c_str());
    sem_post(m_guard);

    sem_unlink(m_guardName.c_str());
}
