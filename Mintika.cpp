#include "Mintika.h"

Mintika::Mintika(std::vector<std::vector<MintikaHucresi>> &mintika) : mintika(mintika), molada(false),
                                                                      durEmriGeldi(false) {
    pthread_mutex_init(&emirKilidi, nullptr);
    pthread_cond_init(&cond, nullptr);
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
    pthread_mutex_destroy(&emirKilidi);
    pthread_cond_destroy(&cond);
}

MintikaHucresi *Mintika::kapsamBossaKilitleDoluysaIlkDoluHucreyiDon(Kapsam &kapsam, Er &er) {
    for (int i = 0; i < kapsam.strSayisi; ++i)
        for (int j = 0; j < kapsam.stnSayisi; ++j) {
            MintikaHucresi &mintikaHucresi =
                    mintika[kapsam.solUstKoordinat.first + i][kapsam.solUstKoordinat.second + j];

            if (pthread_mutex_trylock(&mintikaHucresi.temizleniyorKilidi) != 0) { // bu hucre dolu
                // kapsamdaki diger kitlenen kilitleri ac
                for (int ii = 0; ii < kapsam.strSayisi; ++ii) {
                    for (int jj = 0; jj < kapsam.stnSayisi; ++jj) {
                        if (ii == i && jj == j) {
                            return &mintikaHucresi;
                        }

                        MintikaHucresi &kitliMintikaHucresi =
                                mintika[kapsam.solUstKoordinat.first + ii][kapsam.solUstKoordinat.second + jj];
                        pthread_mutex_unlock(&kitliMintikaHucresi.temizleniyorKilidi);
                    }
                }
            }
        }

    return nullptr;
}

MintikaHucresi *Mintika::konumBossaKitleDoluysaIlkDoluHucreyiDon(TutturucuKonumu &konum, TutunTutturucu &tutturucu) {
    for (int i = konum.konum.first - 1; i <= konum.konum.first + 1; ++i) {
        for (int j = konum.konum.second - 1; j <= konum.konum.second + 1; ++j) {
            MintikaHucresi &mintikaHucresi = mintika[i][j];
            if (pthread_mutex_trylock(&mintikaHucresi.temizleniyorKilidi) != 0) { // bu hucre dolu
                // kapsamdaki diger kitlenen kilitleri ac
                for (int ii = konum.konum.first - 1; ii <= konum.konum.first + 1; ++ii) {
                    for (int jj = konum.konum.second - 1; jj <= konum.konum.second + 1; ++jj) {
                        if (ii == i && jj == j) {
                            return &mintikaHucresi;
                        }
                        MintikaHucresi &kitliMintikaHucresi = mintika[ii][jj];
                        pthread_mutex_unlock(&kitliMintikaHucresi.temizleniyorKilidi);
                    }
                }
            }
        }
    }
    return nullptr;
}


MintikaHucresi::MintikaHucresi(int izmaritSayisi, bool temizleniyor)
        : izmaritSayisi(izmaritSayisi),
          temizlikci(nullptr) {
    pthread_mutex_init(&temizleniyorKilidi, nullptr);
    pthread_mutex_init(&tutturuluyorKilidi, nullptr);
}

MintikaHucresi::~MintikaHucresi() {
    pthread_mutex_destroy(&temizleniyorKilidi);
    pthread_mutex_destroy(&tutturuluyorKilidi);
}

