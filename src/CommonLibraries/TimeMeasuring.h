/*
 * Gate.hpp
 *
 *  Created on: Jul 2, 2022
 *      Author: rohan
 */

#ifndef TIMEMEASURING_H_
#define TIMEMEASURING_H_

#include <string>
#include <sys/time.h>

class CTimeMeasuring {
  public:
    CTimeMeasuring() : m_startingTime(0), m_stoppingTime(0), m_startingTimeSecond(0), m_stoppingTimeSecond(0) {}
    ~CTimeMeasuring() {}
    inline void FixStartingTime() { m_startingTime = GetCurrentMicroSecond(); }
    inline void FixStoppingTime() { m_stoppingTime = GetCurrentMicroSecond(); }
    inline bool IsStarted() const { return m_startingTime != 0; }
    inline void ResetCounters() {
        m_startingTime = 0;
        m_stoppingTime = 0;
    }
    long GetTimeDifference() { return m_stoppingTime - m_startingTime; }
    int GetTimeDifferenceSecond() { return m_stoppingTimeSecond - m_startingTimeSecond; }
    inline void FixStartingTimeSecond() { m_startingTimeSecond = GetCurrentSecond(); }
    inline void FixStoppingTimeSecond() { m_stoppingTimeSecond = GetCurrentSecond(); }

  private:
    inline long GetCurrentMicroSecond() {
        struct timeval aTime;
        gettimeofday(&aTime, 0);
        return aTime.tv_sec * 1000000 + aTime.tv_usec; // overflow at 71min. In 1 hour is safe
    }
    inline int GetCurrentSecond() {
        struct timeval aTime;
        gettimeofday(&aTime, 0);
        return aTime.tv_sec;
    }

  private:
    unsigned long m_startingTime;
    unsigned long m_stoppingTime;
    unsigned int m_startingTimeSecond;
    unsigned int m_stoppingTimeSecond;
};
#endif /* TIMEMEASURING_H_ */
