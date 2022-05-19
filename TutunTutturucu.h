//
// Created by lutfullah on 07.05.2022.
//

#ifndef ODTU_OS_ODEV_2_TUTUNTUTTURUCU_H
#define ODTU_OS_ODEV_2_TUTUNTUTTURUCU_H

#include <utility>
#include <vector>
#include <csignal>
#include "Mintika.h"

struct Mintika;
struct TutunTutturucu;

struct TutturucuKonumu {
    TutturucuKonumu(int icilecekSigaraSayisi, const std::pair<int, int> &konum);

    int icilecekSigaraSayisi;
    std::pair<int, int> konum;

    /**
     * @param i konum merkezine gore satir indeksi -1, 0 veya 1 olabilir.
     * @param j konum merkezine gore sutun indeksi -1, 0 veya 1 olabilir.
     */
    MintikaHucresi &mintikaHucresiGetir(Mintika &mintika, int i, int j);

    void iVeJYeKadarKilitAc(int i, int j, TutunTutturucu &tutturucu, Mintika &mintika);
};

struct TutunTutturucu {
    TutunTutturucu(int sid, int tutturmeSuresiMs, const std::vector<TutturucuKonumu> &konumlar, Mintika &mintika);

    int sid;
    int tutturmeSuresiMs;
    std::vector<TutturucuKonumu> konumlar;
    Mintika &mintika;

    void emirVarsaUy(TutturucuKonumu *konum = nullptr, pthread_mutex_t *dururkenAcilacakKilit = nullptr);

    void konumRezerveEt(TutturucuKonumu &konum);

    void rezervasyonuBitir(TutturucuKonumu &konum);

    void tuttur(TutturucuKonumu &konum);

    timespec izmaritAtmaZamaniHesapla();

    void durEmriyseDur(TutturucuKonumu *konum=nullptr, pthread_mutex_t *dururkenAcilacakKilit = nullptr);
};


#endif //ODTU_OS_ODEV_2_TUTUNTUTTURUCU_H
