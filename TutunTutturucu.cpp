//
// Created by lutfullah on 07.05.2022.
//

#include "TutunTutturucu.h"

TutturucuKonumu::TutturucuKonumu(int icilecekSigaraSayisi, const std::pair<int, int> &konum) : icilecekSigaraSayisi(
        icilecekSigaraSayisi), konum(konum) {}

MintikaHucresi &TutturucuKonumu::mintikaHucresiGetir(Mintika &mintika, int i, int j) {
    return mintika.mintika[konum.first + i][konum.second + j];
}

TutunTutturucu::TutunTutturucu(int sid, int tutturmeSuresiMs, const std::vector<TutturucuKonumu> &konumlar,
                               Mintika &mintika) : sid(sid), tutturmeSuresiMs(tutturmeSuresiMs), konumlar(konumlar),
                                                   mintika(mintika) {}

void TutunTutturucu::durulacaksaDurEmirKilitli(TutturucuKonumu *konum, pthread_mutex_t *dururkenAcilacakKilit) {
    pthread_mutex_lock(&mintika.emirKilidi);
    durEmriyseDur(konum, dururkenAcilacakKilit);
    pthread_mutex_unlock(&mintika.emirKilidi);
}

void TutunTutturucu::durEmriyseDur(TutturucuKonumu* konum, pthread_mutex_t *dururkenAcilacakKilit) {
    if (mintika.durEmriGeldi) {
        hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);
        rezervasyonuBitir(*konum);
        pthread_mutex_unlock(&mintika.emirKilidi);
        if (dururkenAcilacakKilit) {
            pthread_mutex_unlock(dururkenAcilacakKilit);
        }
        pthread_exit(nullptr);
    }
}

// todo: su anda tutturucu hucreleri birbirini engelliyor. bunun cozulmesi lazim.
void TutunTutturucu::konumRezerveEt(TutturucuKonumu &konum) {
    MintikaHucresi &tutturmeHucresi = mintika.mintika[konum.konum.first][konum.konum.second];

    pthread_mutex_lock(&tutturmeHucresi.tutturucuVarKilidi);

    MintikaHucresi *erRezervliHucre;
    while ((erRezervliHucre = mintika.konumBossaKilitleDoluysaIlkDoluHucreyiDon(konum, *this)) != nullptr) {
        // bunun sebebi dolu hucrenin bosalmasini beklerken tutturme hucresini mesgul etmeyelim.
        pthread_mutex_unlock(&tutturmeHucresi.tutturucuVarKilidi);

        durulacaksaDurEmirKilitli(nullptr, &tutturmeHucresi.tutturucuVarKilidi);


        pthread_mutex_lock(&erRezervliHucre->temizleniyorKilidi);
        while (erRezervliHucre->temizleniyor) {
            pthread_cond_wait(&erRezervliHucre->temizlikBirakildiCond, &erRezervliHucre->temizleniyorKilidi);
        }
        pthread_mutex_unlock(&erRezervliHucre->temizleniyorKilidi);
        // artik bu hucre serbest. diger hucreleri bir daha kontrol edip kitleyip serbestse baslayabiliriz.

        durulacaksaDurEmirKilitli(nullptr, &tutturmeHucresi.tutturucuVarKilidi);

        pthread_mutex_lock(&tutturmeHucresi.tutturucuVarKilidi);
    }

    // bu noktada konum rezerve edilmistir.
    hw2_notify(SNEAKY_SMOKER_ARRIVED, sid, konum.konum.first, konum.konum.second);


}

void TutunTutturucu::rezervasyonuBitir(TutturucuKonumu &konum) {
    MintikaHucresi &tutturmeHucresi = mintika.mintika[konum.konum.first][konum.konum.second];
    pthread_mutex_unlock(&tutturmeHucresi.tutturucuVarKilidi);
    for (int i = konum.konum.first - 1; i <= konum.konum.first + 1; ++i) {
        for (int j = konum.konum.second - 1; j <= konum.konum.second + 1; ++j) {
            mintika.mintika[i][j].tutturucuTerketsin(*this);
        }
    }
}

void TutunTutturucu::tuttur(TutturucuKonumu &konum) {
    /**
     * 111
     * 101
     * 111
     */
    std::vector<std::pair<int, int>> icilecekKordinatlar = {
            {-1, -1},
            {-1, 0},
            {-1, 1},
            {0,  -1},
            {0,  1},
            {1,  -1},
            {1,  0},
            {1,  1}
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
                    pthread_cond_signal(&mintikaHucresi.tutturucuKalmadiVeyaMolaCond);
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

