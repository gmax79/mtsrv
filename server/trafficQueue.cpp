#include "libs.h"
#include "trafficQueue.h"

void TrafficQueue::push(const char* data, int len)
{
    if (!data || len < 0) {
        assert(false);
        return;
    }
    if (len == 0)
        return;
    std::lock_guard<std::mutex> lk(m_inputPacketsMutex);
    std::string packet(data, len);
    m_inputPackets.push(packet);
    m_inputPacketsEvent.notify_one();
}

bool TrafficQueue::pop(std::string& packet, std::chrono::milliseconds waittime)
{
    std::unique_lock<std::mutex> lk(m_inputPacketsMutex);
    if (m_inputPacketsEvent.wait_for(lk, waittime) == std::cv_status::timeout)
        return false;
    packet.assign(m_inputPackets.front());
    m_inputPackets.pop();
    return true;
}
