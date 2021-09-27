#include "Computer.hpp"

using namespace monitor;

Computer::Computer()
: ip_ {}
, cpu_(new Cpu)
, memory_(new Memory)
, network_(new Network)
, storage_(new Storage)
, system_(new System)
, Icon()
{
}

Computer::~Computer()
{
    delete cpu_;
    delete memory_;
    delete network_;
    delete storage_;
    delete system_;
}

const QString &Computer::ip() const
{
    return ip_;
}

Cpu* Computer::cpu() const
{
    return cpu_;
}

Memory* Computer::memory() const
{
    return memory_;
}

Network* Computer::network() const
{
    return network_;
}

Storage* Computer::storage() const
{
    return storage_;
}

System* Computer::system() const
{
    return system_;
}
