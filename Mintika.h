//
// Created by lutfullah on 04.05.2022.
//

#ifndef ODTU_OS_ODEV_2_MINTIKA_H
#define ODTU_OS_ODEV_2_MINTIKA_H

#include "hw2_output.h"
#include "Er.h"
#include "TutunTutturucu.h"
#include <vector>
#include <iostream>
#include "HataAyiklama.h"

#include <pthread.h>

struct TutturucuKonumu;
struct TutunTutturucu;

struct MintikaHucresi {
    int izmaritSayisi;

    pthread_mutex_t temizleniyorKilidi;
    pthread_mutex_t tutturucuVarKilidi;
    pthread_mutex_t tutturuluyorKilidi;
    pthread_cond_t tutturucuKalmadiVeyaMolaCond;
    pthread_cond_t temizlikBirakildiCond;
    pthread_mutex_t izmaritEklemeKilidi;
    Er *temizlikci;
    std::vector<TutunTutturucu *> tutturuculer;

    MintikaHucresi(int izmaritSayisi, bool temizleniyor, std::pair<int, int> &kordinat);

    int tutturucuSayisi;

    std::pair<int, int> kordinat;

    bool temizleniyor;

    virtual ~MintikaHucresi();

    bool tutturuluyor();

    void temizligiBirak();

    void tutturucuTerketsin(TutunTutturucu &tutturucu);

    void tutturucuGelsin(TutunTutturucu &tutturucu);

    void izmaritEkle();

    void temizliginBitmesiniBekle();

    void tutturuculerinGitmesiniBekle(Mintika &mintika);

    bool temizleniyordur();

    void setTemizleniyor(bool b);
};

struct Mintika {
    bool molada, durEmriGeldi;
    pthread_cond_t emirCond;
    pthread_mutex_t emirKilidi;
    pthread_cond_t erSayisiCond;
    pthread_mutex_t erSayisiKilidi;
    std::vector<std::vector<MintikaHucresi>> mintika;

    // erSayisiKilidi korur.
    int moladaErSayisi;
    // erSayisiKilidi korur.
    int calisanErSayisi;

    explicit Mintika(std::vector<std::vector<MintikaHucresi>> &mintika);

    void yazdir();

    MintikaHucresi *konumBossaKilitleDoluysaIlkDoluHucreyiDon(TutturucuKonumu &konum, TutunTutturucu &tutturucu);

    MintikaHucresi *kapsamBossaKilitleDoluysaIlkDoluHucreyiDon(Kapsam &kapsam, Er &er);

    ~Mintika();


    void calisanErAzalt();

    void moladaErAzalt();

    void moladaErArttir();

    void calisanErArttir();
};

#endif //ODTU_OS_ODEV_2_MINTIKA_H
