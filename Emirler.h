//
// Created by lutfullah on 06.05.2022.
//

#ifndef ODTU_OS_ODEV_2_EMIRLER_H
#define ODTU_OS_ODEV_2_EMIRLER_H

#include <time.h>

class Emir {
public:
    Emir(int zamanMs, timespec programBaslamaZamani);

    timespec zaman, programBaslamaZamani;
    int zamanMs;

    void zamaniBekle();

    virtual void emret() = 0;
};

class MolaEmri : public Emir {
public:
    MolaEmri(int zamanMs, timespec programBaslamaZamani);

    void emret();
};

class DevamEmri : public Emir {
public:
    DevamEmri(int zamanMs, timespec programBaslamaZamani);

    void emret();
};

class DurEmri : public Emir {
public:
    DurEmri(int zamanMs, timespec programBaslamaZamani);

    void emret();
};

#endif //ODTU_OS_ODEV_2_EMIRLER_H
