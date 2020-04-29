#include "SysTimer.h"
#include <cstring>
#include <unistd.h>

static constexpr uint64_t NS_IN_MS = 1000000;
static constexpr uint64_t NS_IN_S  = 1000000000;

static struct timespec operator+(const timespec& ts, const uint32_t ms)
{

    struct timespec ts_ret;
    uint64_t ns = (uint64_t)ts.tv_nsec + (uint64_t)ms * NS_IN_MS;
    ts_ret.tv_sec = ts.tv_sec + ns / NS_IN_S;
    ns %= NS_IN_S;
    ts_ret.tv_nsec= ns;
    return ts_ret;
}

bool operator>(struct timespec &lhs, struct timespec &rhs)
{
    if (lhs.tv_sec < rhs.tv_sec)
        return false;
    if (lhs.tv_sec > rhs.tv_sec)
        return true;
    if (lhs.tv_nsec <= rhs.tv_nsec)
        return false;

    return true;
}

bool operator>=(struct timespec &lhs, struct timespec &rhs)
{
    if (lhs.tv_sec < rhs.tv_sec)
        return false;
    if (lhs.tv_sec > rhs.tv_sec)
        return true;
    if (lhs.tv_nsec < rhs.tv_nsec)
        return false;

    return true;
}

static int getTsDiffInMs(const timespec& ts1, const timespec& ts2)
{
    int64_t delta_s = ts1.tv_sec - ts2.tv_sec;
    int64_t delta_ns = ts1.tv_nsec - ts2.tv_nsec;

    if (delta_ns < 0)
    {
        delta_ns += NS_IN_S;
        delta_s--;
    }

    if (delta_s < 0)
        return 0;

    int ms = delta_s * 1000;
    ms += delta_ns / NS_IN_MS;
    return ms;
}

SysTimer::SysTimer()
{
}

SysTimer::~SysTimer()
{
}

bool SysTimer::subscribe(ISysTimer &iface, uint32_t period, bool repeat)
{
    struct timespec ts_now;
    clock_gettime(CLOCK_MONOTONIC, &ts_now);

    /* Search for existing, update if found */
    for (auto it = mSubscribers.begin(); it != mSubscribers.end(); it++)
    {
        if (it->iface == &iface)
        {
            it->targettime = ts_now + period;
            it->repeat = repeat;
            if (repeat)
            {
                it->period = period;
            }
            return true;
        }
    }
    /* Subscribe new client */
    SysTimerSubscriber subscriber;
    subscriber.iface = &iface;
    subscriber.period = period;
    subscriber.targettime = ts_now + period;
    subscriber.repeat = repeat;
    mSubscribers.push_back(subscriber);

    return true;
}

void SysTimer::unsubscribe(ISysTimer &iface)
{
    /* Search for existing, update if found */
    for (auto it = mSubscribers.begin(); it != mSubscribers.end(); it++)
    {
        if (it->iface == &iface)
        {
            mSubscribers.erase(it);
            return;
        }
    }
}

void SysTimer::waitAndProcess(void)
{
    /* Get the current time */
    struct timespec ts_now;
    clock_gettime(CLOCK_MONOTONIC, &ts_now);
    struct timespec ts_waitfor = ts_now + 100;

    auto it = mSubscribers.begin();
    while (it != mSubscribers.end())
    {
        bool erased = false;
        if (it->iface != 0)
        {
            if (ts_now >= it->targettime)
            {
                ISysTimer * iface = it->iface;

                /* Clear iface for one-shot timers */
                if (it->repeat == false)
                {
                    it = mSubscribers.erase(it);
                    erased = true;
                }
                else
                {
                    it->targettime = ts_now + it->period;
                }

                iface->onTimer();
                /* iface could be deleted at onTimer */
                /* Search for it, do nothing if found */
                auto it2 = mSubscribers.begin();
                while (it2 != mSubscribers.end())
                {
                    if (it2->iface == iface)
                    {
                        break;
                    }
                    it2++;
                }
                /* Was not found */
                if (it2 == mSubscribers.end())
                {
                    erased = true;
                }
            }
        }
        if (!erased)
        {
            it++;
        }
    }
    for (auto it = mSubscribers.begin(); it != mSubscribers.end(); it++)
    {
        if (it->iface != 0)
        {
            if (ts_waitfor > it->targettime)
            {
                ts_waitfor = it->targettime;
            }
        }
    }
    //usleep(1000 * getTsDiffInMs(ts_waitfor, ts_now));
    usleep(1000);
}

