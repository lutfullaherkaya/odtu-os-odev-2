//
// Created by lutfullah on 04.05.2022.
//

#ifndef ODTU_OS_ODEV_2_MINTIKA_H
#define ODTU_OS_ODEV_2_MINTIKA_H

#include "hw2_output.h"
#include "Er.h"
#include <vector>
#include <iostream>
#include "HataAyiklama.h"
#include "TutunTutturucu.h"
#include <pthread.h>

struct MintikaHucresi {
    int izmaritSayisi;

    pthread_mutex_t temizleniyorKilidi;
    pthread_mutex_t tutturuluyorKilidi;
    Er *temizlikci;

    MintikaHucresi(int izmaritSayisi, bool temizleniyor);

    virtual ~MintikaHucresi();
};

struct Mintika {
    bool molada, durEmriGeldi;
    pthread_cond_t cond;
    pthread_mutex_t emirKilidi;
    std::vector<std::vector<MintikaHucresi>> mintika;

    explicit Mintika(std::vector<std::vector<MintikaHucresi>> &mintika);

    void yazdir();

    ~Mintika();

    MintikaHucresi *kapsamBossaKilitleDoluysaIlkDoluHucreyiDon(Kapsam &kapsam, Er &er);

    MintikaHucresi *konumBossaKitleDoluysaIlkDoluHucreyiDon(TutturucuKonumu &konum, TutunTutturucu &tutturucu);
};

#endif //ODTU_OS_ODEV_2_MINTIKA_H
