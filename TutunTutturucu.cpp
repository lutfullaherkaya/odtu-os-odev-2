//
// Created by lutfullah on 07.05.2022.
//

#include "TutunTutturucu.h"

TutturucuKonumu::TutturucuKonumu(int icilecekSigaraSayisi, const std::pair<int, int> &konum) : icilecekSigaraSayisi(
        icilecekSigaraSayisi), konum(konum) {}

MintikaHucresi &TutturucuKonumu::mintikaHucresiGetir(Mintika &mintika, int i, int j) {
    return mintika.mintika[konum.first + i][konum.second + j];
}

void TutturucuKonumu::iVeJYeKadarKilitAc(int i, int j, TutunTutturucu &tutturucu, Mintika &mintika) {
    for (int ii = konum.first - 1; ii <= konum.first + 1; ++ii) {
        for (int jj = konum.second - 1; jj <= konum.second + 1; ++jj) {
            if (ii == i && jj == j) {
                return;
            }
            mintika.mintika[ii][jj].tutturucuTerketsin(tutturucu);
        }
    }
}

TutunTutturucu::TutunTutturucu(int sid, int tutturmeSuresiMs, const std::vector<TutturucuKonumu> &konumlar,
                               Mintika &mintika) : sid(sid), tutturmeSuresiMs(tutturmeSuresiMs), konumlar(konumlar),
                                                   mintika(mintika) {}

void TutunTutturucu::emirVarsaUy(TutturucuKonumu *konum, pthread_mutex_t *dururkenAcilacakKilit) {
    pthread_mutex_lock(&mintika.emirKilidi);
    durEmriyseDur(konum, dururkenAcilacakKilit);
    pthread_mutex_unlock(&mintika.emirKilidi);
}

void TutunTutturucu::durEmriyseDur(TutturucuKonumu *konum, pthread_mutex_t *dururkenAcilacakKilit) {
    if (mintika.durEmriGeldi) {
        hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
        pthread_mutex_unlock(&mintika.emirKilidi);
        if (konum) {
            rezervasyonuBitir(*konum);
        }
        if (dururkenAcilacakKilit) {
            pthread_mutex_unlock(dururkenAcilacakKilit);
        }
        pthread_exit(nullptr);
    }
}

void TutunTutturucu::konumRezerveEt(TutturucuKonumu &konum) {
    pthread_mutex_t *tutturucuVarKilidi = &mintika.mintika[konum.konum.first][konum.konum.second].tutturucuVarKilidi;
    pthread_mutex_lock(tutturucuVarKilidi);
    emirVarsaUy(nullptr, tutturucuVarKilidi);

    MintikaHucresi *erRezervliHucre;
    while ((erRezervliHucre = mintika.konumBossaKilitleDoluysaIlkDoluHucreyiDon(konum, *this))) {
        pthread_mutex_unlock(tutturucuVarKilidi);
        emirVarsaUy();
        erRezervliHucre->temizliginBitmesiniBekle();
        emirVarsaUy();
        pthread_mutex_lock(tutturucuVarKilidi);
        emirVarsaUy();
    }
    hw2_notify(SNEAKY_SMOKER_ARRIVED, sid, konum.konum.first, konum.konum.second);
}

void TutunTutturucu::rezervasyonuBitir(TutturucuKonumu &konum) {
    MintikaHucresi &tutturmeHucresi = mintika.mintika[konum.konum.first][konum.konum.second];
    pthread_mutex_unlock(&tutturmeHucresi.tutturucuVarKilidi);
    for (int i = konum.konum.first - 1; i <= konum.konum.first + 1; ++i) {
        for (int j = konum.konum.second - 1; j <= konum.konum.second + 1; ++j) {
            std::cerr << "rez kalkiyor[" + std::to_string(i) + "][" + std::to_string(j) + "]\n"; //todo: sil
            mintika.mintika[i][j].tutturucuTerketsin(*this);
        }
    }
}

void TutunTutturucu::tuttur(TutturucuKonumu &konum) {
    /**
     * 123
     * 804
     * 765
     */
    std::vector<std::pair<int, int>> icilecekKordinatlar = {
            {-1, -1},
            {-1, 0},
            {-1, 1},
            {0,  1},
            {1,  1},
            {1,  0},
            {1,  -1},
            {0,  -1}
    };

    while (konum.icilecekSigaraSayisi > 0) {
        for (auto &kordinat: icilecekKordinatlar) {
            MintikaHucresi &mintikaHucresi = konum.mintikaHucresiGetir(mintika, kordinat.first, kordinat.second);

            pthread_mutex_lock(&mintika.emirKilidi);
            timespec izmaritAtmaZamani = izmaritAtmaZamaniHesapla();
            // mola emri gelirse biraz daha beklemek icin
            while (pthread_cond_timedwait(&mintika.cond, &mintika.emirKilidi, &izmaritAtmaZamani) == 0) {
                // emir geldi
                durEmriyseDur(&konum);
                if (mintika.molada) {
                    pthread_mutex_lock(&mintikaHucresi.tutturuluyorKilidi);
                    pthread_cond_broadcast(&mintikaHucresi.tutturucuKalmadiVeyaMolaCond);
                    pthread_mutex_unlock(&mintikaHucresi.tutturuluyorKilidi);
                }
            }
            // izmarit atma zamani geldi
            pthread_mutex_unlock(&mintika.emirKilidi);
            mintikaHucresi.izmaritEkle();
            hw2_notify(SNEAKY_SMOKER_FLICKED, sid, mintikaHucresi.kordinat.first, mintikaHucresi.kordinat.second);
            if (--konum.icilecekSigaraSayisi <= 0) {
                break;
            }

        }
    }
    hw2_notify(SNEAKY_SMOKER_LEFT, sid, 0, 0);
}

timespec TutunTutturucu::izmaritAtmaZamaniHesapla() {
    timespec simdi;
    timespec_get(&simdi, TIME_UTC);
    return timespec_add_ms(simdi, tutturmeSuresiMs);
}


