//
// Created by lutfullah on 06.05.2022.
//

#ifndef ODTU_OS_ODEV_2_EMIRLER_H
#define ODTU_OS_ODEV_2_EMIRLER_H

#include <time.h>
#include <pthread.h>

struct Mintika;

timespec timespec_add(timespec ts1, timespec ts2);
timespec timespec_add_ms(timespec ts1, int ms);

class Emir {
public:
    timespec zaman, programBaslamaZamani;
    int zamanMs;
    Mintika &mintika;


    Emir(int zamanMs, timespec programBaslamaZamani, Mintika &mintika);

    void zamaniBekle();

    virtual void emret() = 0;
};

class MolaEmri : public Emir {
public:
    MolaEmri(int zamanMs, timespec programBaslamaZamani, Mintika &mintika);

    void emret();
};

class DevamEmri : public Emir {
public:
    DevamEmri(int zamanMs, timespec programBaslamaZamani, Mintika &mintika);

    void emret();
};

class DurEmri : public Emir {
public:
    DurEmri(int zamanMs, timespec programBaslamaZamani, Mintika &mintika);

    void emret();
};

#endif //ODTU_OS_ODEV_2_EMIRLER_H
