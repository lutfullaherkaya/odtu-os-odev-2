//
// Created by lutfullah on 04.05.2022.
//

#ifndef ODTU_OS_ODEV_2_ER_H
#define ODTU_OS_ODEV_2_ER_H

#include "hw2_output.h"
#include <iostream>
#include <utility>
#include <pthread.h>
#include <vector>


struct Mintika;
struct MintikaHucresi;
class Emir;
struct Er;

timespec timespec_add_ms(timespec ts1, int ms);


struct Kapsam {
    Kapsam(int strSayisi, int stnSayisi, const std::pair<int, int> &solUstKoordinat);
    MintikaHucresi & mintikaHucresiGetir(std::vector<std::vector<MintikaHucresi>> &mintika, int i, int j);

    int strSayisi;
    int stnSayisi;
    std::pair<int, int> solUstKoordinat;

    void temizlikcileriAyarla(Mintika &mintika, Er *erPtr);

    void iVeJYeKadarKilitAc(int i, int j, std::vector<std::vector<MintikaHucresi>> &mintika);
};

/**
 * Proper Private
 */
struct Er {
    int gid;
    int toplamaSuresiMs;
    std::vector<Kapsam> kapsamlar;
    Mintika &mintika;

    Er(int gid, int toplamaSuresiMs, std::vector<Kapsam> &kapsamlar, Mintika &mintika);
    ~Er();

    void kapsamRezerveEt(Kapsam &kapsam);

    bool izmaritTopla(Kapsam &kapsam);

    void rezervasyonuBitir(Kapsam &kapsam);

    bool molada;

    void emirVarsaUy();

    timespec toplamaZamaniHesapla();

    bool molaysaRezervasyonBitirGerekirseDur(Kapsam &kapsam);

    void moladaDegilseMolayaAlVeNotifyYap();

    void moladaysaMoladanCikarVeNotifyYap();
};

#endif //ODTU_OS_ODEV_2_ER_H
