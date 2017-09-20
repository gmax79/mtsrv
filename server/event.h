#pragma once

class SyncEvent {
public:
    void wait() {
        std::unique_lock<std::mutex> lk(mutex);
        cond.wait(lk);
        lk.unlock();
    }
    void notify() {
       std::lock_guard<std::mutex> lk(mutex);
       cond.notify_all();
    }
private:
    std::mutex mutex;
    std::condition_variable cond;
};