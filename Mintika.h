//
// Created by lutfullah on 04.05.2022.
//

#ifndef ODTU_OS_ODEV_2_MINTIKA_H
#define ODTU_OS_ODEV_2_MINTIKA_H

#include "hw2_output.h"
#include <vector>
#include <iostream>
#include "Er.h"

class Mintika {
private:
public:
    std::vector<std::vector<int>> mintika;
public:
    explicit Mintika(std::vector<std::vector<int>> &mintika);

    void yazdir();



};

#endif //ODTU_OS_ODEV_2_MINTIKA_H
