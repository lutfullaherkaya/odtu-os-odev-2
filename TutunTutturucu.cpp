//
// Created by lutfullah on 07.05.2022.
//

#include "TutunTutturucu.h"
#include "Mintika.h"

TutturucuKonumu::TutturucuKonumu(int icilecekSigaraSayisi, const std::pair<int, int> &konum) : icilecekSigaraSayisi(
        icilecekSigaraSayisi), konum(konum) {}

TutunTutturucu::TutunTutturucu(int sid, int tutturmeSuresiMs, const std::vector<TutturucuKonumu> &konumlar,
                               Mintika &mintika) : sid(sid), tutturmeSuresiMs(tutturmeSuresiMs), konumlar(konumlar),
                                                   mintika(mintika) {}

void TutunTutturucu::durulacaksaDur(pthread_mutex_t *dururkenAcilacakKilit) {
    pthread_mutex_lock(&mintika.emirKilidi);
    if (mintika.durEmriGeldi) {
        hw2_notify(SNEAKY_SMOKER_STOPPED, sid, 0, 0);nu
        pthread_mutex_unlock(&mintika.emirKilidi);
        if (dururkenAcilacakKilit) {
            pthread_mutex_unlock(dururkenAcilacakKilit);
        }
        pthread_exit(nullptr);
    }
    pthread_mutex_unlock(&mintika.emirKilidi);
}

// todo: su anda tutturucu hucreleri birbirini engelliyor. bunun cozulmesi lazim.
void TutunTutturucu::konumRezerveEt(TutturucuKonumu &konum) {
    MintikaHucresi &tutturmeHucresi = mintika.mintika[konum.konum.first][konum.konum.second];

    pthread_mutex_lock(&tutturmeHucresi.tutturuluyorKilidi);
    // erle dolu hucre
    MintikaHucresi *doluHucre;
    while ((doluHucre = mintika.konumBossaKitleDoluysaIlkDoluHucreyiDon(konum, *this)) != nullptr) {
        // bunun sebebi dolu hucrenin bosalmasini beklerken tutturme hucresini mesgul etmeyelim.
        pthread_mutex_unlock(&tutturmeHucresi.tutturuluyorKilidi);
        durulacaksaDur(&tutturmeHucresi.tutturuluyorKilidi);

        pthread_mutex_lock(&doluHucre->temizleniyorKilidi);
        // artik bu hucre serbest. diger hucreleri bir daha kontrol edip kitleyip serbestse baslayabiliriz.
        pthread_mutex_unlock(&doluHucre->temizleniyorKilidi);

        durulacaksaDur(&tutturmeHucresi.tutturuluyorKilidi);


        pthread_mutex_lock(&tutturmeHucresi.tutturuluyorKilidi);
    }
    // bu noktada konum rezerve edilmistir.
    hw2_notify(SNEAKY_SMOKER_ARRIVED, sid, konum.konum.first, konum.konum.second);


}

void TutunTutturucu::rezervasyonuBitir(TutturucuKonumu &konum) {
    MintikaHucresi &tutturmeHucresi = mintika.mintika[konum.konum.first][konum.konum.second];
    pthread_mutex_unlock(&tutturmeHucresi.tutturuluyorKilidi);
    for (int i = konum.konum.first - 1; i <= konum.konum.first + 1; ++i) {
        for (int j = konum.konum.second - 1; j <= konum.konum.second + 1; ++j) {
            pthread_mutex_unlock(&mintika.mintika[i][j].temizleniyorKilidi);
        }
    }
}
