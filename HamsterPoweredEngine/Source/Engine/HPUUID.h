#pragma once

#include <xhash>


class HPUUID
{
public:
    HPUUID();
    HPUUID(uint64_t uuid);
    HPUUID(const HPUUID&) = default;

    operator uint64_t() const { return m_UUID; }

private:
    uint64_t m_UUID;
};



namespace std
{
    template<>
    struct hash<HPUUID>
    {
        std::size_t operator()(const HPUUID& uuid) const
        {
            return hash<uint64_t>()((uint64_t)uuid);
        }
    };
}
