#pragma once

#include <QString>
#include <QPixmap>

#include "Cpu/Cpu.hpp"
#include "Memory/Memory.hpp"
#include "Network/Network.hpp"
#include "Storage/Storage.hpp"
#include "System/System.hpp"

#include "../Icon/Icon.hpp"

namespace monitor
{
class Computer : public Icon
{
public:
    Computer();
    ~Computer();
    const QString &ip() const;
    Cpu* cpu() const;
    Memory* memory() const;
    Network* network() const;
    Storage* storage() const;
    System* system() const;

private:
    QString ip_;

    Cpu* cpu_;
    Memory* memory_;
    Network* network_;
    Storage* storage_;
    System* system_;
};
}
