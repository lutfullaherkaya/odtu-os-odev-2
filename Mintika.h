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
#include <pthread.h>

struct TemizlenmekteKapsam {
    Kapsam kapsam;
    pthread_mutex_t kilit;
    pthread_cond_t cond;

    TemizlenmekteKapsam(const Kapsam &kapsam);

    ~TemizlenmekteKapsam();
};

struct MintikaHucresi {
    int izmaritSayisi;
    TemizlenmekteKapsam *temizlenmekteKapsam;

    pthread_mutex_t temizleniyorKilidi;
    Er *temizlikci;


    MintikaHucresi(int izmaritSayisi, bool temizleniyor);

    bool temizleniyor();


    virtual ~MintikaHucresi();
};

struct Mintika {
    bool molada, durEmriGeldi;
    pthread_cond_t cond;
    pthread_mutex_t emirKilidi;
    std::vector<std::vector<MintikaHucresi>> mintika;
    std::vector<TemizlenmekteKapsam> temizlenmekteKapsamlar;

    explicit Mintika(std::vector<std::vector<MintikaHucresi>> &mintika);

    void yazdir();

    ~Mintika();

    MintikaHucresi *kapsamBossaKitleDoluysaIlkDoluHucreyiDon(Kapsam &kapsam, Er &er);
};

#endif //ODTU_OS_ODEV_2_MINTIKA_H
