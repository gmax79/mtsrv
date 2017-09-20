#pragma once

class TrafficQueue {
public:
    void push(const char* data, int len);
    bool pop(std::string& packet, std::chrono::milliseconds waittime);
private:
    std::queue<std::string> m_inputPackets;
    std::mutex m_inputPacketsMutex;
    std::condition_variable m_inputPacketsEvent;
};
