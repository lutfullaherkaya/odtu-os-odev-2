//
// Created by lutfullah on 04.05.2022.
//

#ifndef ODTU_OS_ODEV_2_ER_H
#define ODTU_OS_ODEV_2_ER_H

#include "Mintika.h"
#include "hw2_output.h"
#include <iostream>
#include <utility>


class Er {
public:
    Er(int gid, int toplamaSuresiMs, int kapsamStrSayisi, int kapsamStnSayisi,
       const std::vector<std::pair<int, int>> &kapsamSolUstKordinatlari);

public:
    int gid;
    int toplamaSuresiMs;
    int kapsamStrSayisi;
    int kapsamStnSayisi;
    std::vector<std::pair<int, int>> kapsamSolUstKordinatlari;

public:
};

#endif //ODTU_OS_ODEV_2_ER_H
