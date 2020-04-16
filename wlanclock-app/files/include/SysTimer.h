#ifndef SYSTIMER_H
#define SYSTIMER_H

#include <time.h>
#include <cstdint>
#include <vector>

class ISysTimer
{
    public:
        virtual ~ISysTimer() {};
        virtual void onTimer(void) = 0;
};

class SysTimer
{
    public:
        static SysTimer& getInstance()
        {
            // Guaranteed to be destroyed.
            // Instantiated on first use.
            static SysTimer instance;

            return instance;
        }

        virtual ~SysTimer();

        /// @brief Subscribe new timer client or update existing
        bool subscribe(ISysTimer &iface, uint32_t period, bool repeat);
        /// @brief Unsubscribe timer client
        void unsubscribe(ISysTimer &iface);
        /// @brief Wait until the next event, then process it.
        void waitAndProcess(void);

        SysTimer(SysTimer const&)  = delete;
        void operator=(SysTimer const&) = delete;
    private:
        SysTimer();

        static constexpr uint32_t MAX_WAIT_MS = 100;

        struct SysTimerSubscriber{
            ISysTimer *iface;
            uint32_t period;
            struct timespec targettime;
            bool repeat;
        };

        std::vector<SysTimerSubscriber> mSubscribers;
};

#endif /* SYSTIMER_H */
