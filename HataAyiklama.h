//
// Created by lutfullah on 06.05.2022.
//

#ifndef ODTU_OS_ODEV_2_HATAAYIKLAMA_H
#define ODTU_OS_ODEV_2_HATAAYIKLAMA_H

#include <pthread.h>

class HataAyiklama {
public:
    static pthread_mutex_t ioKilidi;

    static void ioKitle() {
        pthread_mutex_lock(&ioKilidi);
    };
    static void ioKilidiAc() {
        pthread_mutex_unlock(&ioKilidi);
    };
};

#endif //ODTU_OS_ODEV_2_HATAAYIKLAMA_H
