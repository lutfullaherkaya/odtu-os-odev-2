#include "Mintika.h"

Mintika::Mintika(std::vector<std::vector<MintikaHucresi>> &mintika) : mintika(mintika) {
    pthread_mutex_init(&temizleniyorMuKilidi, nullptr);
}

void Mintika::yazdir() {
    std::cerr << "Mintika: " << std::endl;
    for (const std::vector<MintikaHucresi> &satir: mintika) {
        for (const MintikaHucresi &hucre: satir) {
            std::cerr << hucre.izmaritSayisi;
            if (hucre.temizlikci) {
                std::cerr << "[" << hucre.temizlikci->gid << "]";
            }
            std::cerr << "\t";
        }
        std::cerr << std::endl;
    }

}

Mintika::~Mintika() {
    pthread_mutex_destroy(&temizleniyorMuKilidi);
}

TemizlenmekteKapsam *Mintika::temizleniyor(Kapsam kapsam) {
    for (int i = 0; i < kapsam.strSayisi; ++i) {
        for (int j = 0; j < kapsam.stnSayisi; ++j) {
            MintikaHucresi &mintikaHucresi =
                    mintika[kapsam.solUstKoordinat.first + i][kapsam.solUstKoordinat.second + j];
            if (mintikaHucresi.temizleniyor()) {
                return mintikaHucresi.temizlenmekteKapsam;
            }
        }
    }

    return nullptr;
}

MintikaHucresi *Mintika::kapsamBossaKitleDoluysaIlkDoluHucreyiDon(Kapsam &kapsam, Er &er) {
    for (int i = 0; i < kapsam.strSayisi; ++i)
        for (int j = 0; j < kapsam.stnSayisi; ++j) {
            MintikaHucresi &mintikaHucresi =
                    mintika[kapsam.solUstKoordinat.first + i][kapsam.solUstKoordinat.second + j];

            /*HataAyiklama::ioKitle();
            std::cerr << "gid:" << er.gid << " trylock[" << kapsam.solUstKoordinat.first + i << "]["
                      << kapsam.solUstKoordinat.second + j << "]" << std::endl;
            HataAyiklama::ioKilidiAc();*/

            if (pthread_mutex_trylock(&mintikaHucresi.temizleniyorKilidi) != 0) { // bu hucre dolu
                /*HataAyiklama::ioKitle();
                std::cerr << "gid:" << er.gid << "FAIL trylock[" << i << "][" << j << "]" << std::endl;
                HataAyiklama::ioKilidiAc();*/

                // kapsamdaki diger kitlenen kilitleri ac
                for (int ii = 0; ii < kapsam.strSayisi; ++ii) {
                    for (int jj = 0; jj < kapsam.stnSayisi; ++jj) {
                        if (ii == i && jj == j) {
                            return &mintikaHucresi;
                        }
                        /*HataAyiklama::ioKitle();
                        std::cerr << "gid:" << er.gid << " unlock[" << kapsam.solUstKoordinat.first + ii
                                  << "][" << kapsam.solUstKoordinat.second + jj << "]" << std::endl;
                        HataAyiklama::ioKilidiAc();*/

                        MintikaHucresi &kitliMintikaHucresi =
                                mintika[kapsam.solUstKoordinat.first + ii][kapsam.solUstKoordinat.second + jj];
                        pthread_mutex_unlock(&kitliMintikaHucresi.temizleniyorKilidi);
                    }
                }
            }
        }

    return nullptr;
}

MintikaHucresi::MintikaHucresi(int izmaritSayisi, bool temizleniyor)
        : izmaritSayisi(izmaritSayisi),
          temizlenmekteKapsam(nullptr),
          temizlikci(nullptr) {
    pthread_mutex_init(&temizleniyorKilidi, nullptr);
}

bool MintikaHucresi::temizleniyor() {
    return temizlenmekteKapsam == nullptr;
}

MintikaHucresi::~MintikaHucresi() {
    pthread_mutex_destroy(&temizleniyorKilidi);
}

TemizlenmekteKapsam::TemizlenmekteKapsam(const Kapsam &kapsam) : kapsam(kapsam) {
    pthread_cond_init(&cond, nullptr);
    pthread_mutex_init(&kilit, nullptr);
}

TemizlenmekteKapsam::~TemizlenmekteKapsam() {
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&kilit);
}
