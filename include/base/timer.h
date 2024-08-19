#pragma  once

#include <thread>
#include <chrono>
#include <atomic>
#include <memory>

namespace bcf
{
class Timer : public std::enable_shared_from_this<Timer>
{
    std::atomic<bool> active{ true };

public:

    template<typename Function>
    void setTimeout(Function &&function, int milldelay)
    {
        active = true;
        auto self = shared_from_this();
        std::thread t([self, function, milldelay]() {
            if (!self->active.load()) return;
            std::this_thread::sleep_for(std::chrono::milliseconds(milldelay));
            if (!self->active.load()) return;
            function();
        });
        t.detach();
    }

    template<typename Function>
    void setInterval(Function &&function, int millinterval)
    {
        active = true;
        auto self = shared_from_this();
        std::thread t([self, function, millinterval]() {
            while (self->active.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(millinterval));
                if (!self->active.load()) return;
                function();
            }
        });
        t.detach();
    }

    void stop();

};

inline void Timer::stop()
{
    active = false;
}
}//namespace bcf
