#ifndef DISKCONTROLLER_HPP
#define DISKCONTROLLER_HPP

#include <functional>
#include <vector>

#include "disk.hpp"

class DiskController
{
    DiskController(const DiskController&) = delete;
    DiskController& operator=(const DiskController&) = delete;
    bool operator==(const DiskController&) = delete;
public:
    DiskController();

    std::vector<Disk> listDisks();

};

#endif // DISKCONTROLLER_HPP
