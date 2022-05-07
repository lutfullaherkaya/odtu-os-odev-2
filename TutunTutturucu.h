//
// Created by lutfullah on 07.05.2022.
//

#ifndef ODTU_OS_ODEV_2_TUTUNTUTTURUCU_H
#define ODTU_OS_ODEV_2_TUTUNTUTTURUCU_H

#include <utility>
#include <vector>
#include <csignal>

struct Mintika;

struct TutturucuKonumu {
    TutturucuKonumu(int icilecekSigaraSayisi, const std::pair<int, int> &konum);

    int icilecekSigaraSayisi;
    std::pair<int, int> konum;
};

struct TutunTutturucu {
    TutunTutturucu(int sid, int tutturmeSuresiMs, const std::vector<TutturucuKonumu> &konumlar, Mintika &mintika);

    int sid;
    int tutturmeSuresiMs;
    std::vector<TutturucuKonumu> konumlar;
    Mintika &mintika;

    void durulacaksaDur(pthread_mutex_t *dururkenAcilacakKilit=nullptr);

    void konumRezerveEt(TutturucuKonumu &konum);

    void rezervasyonuBitir(TutturucuKonumu &konum);
};


#endif //ODTU_OS_ODEV_2_TUTUNTUTTURUCU_H
