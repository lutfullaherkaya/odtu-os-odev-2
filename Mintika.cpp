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
            for (auto &tutturucu : hucre.tutturuculer) {
                std::cerr << "{" << tutturucu->sid << "}";
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

            pthread_mutex_lock(&mintikaHucresi.temizleniyorKilidi);
            if (mintikaHucresi.temizleniyor || mintikaHucresi.tutturuluyor()) {  // bu hucre dolu
                pthread_mutex_unlock(&mintikaHucresi.temizleniyorKilidi);
                // kapsamdaki diger kitlenen kilitleri ac
                for (int ii = 0; ii < kapsam.strSayisi; ++ii) {
                    for (int jj = 0; jj < kapsam.stnSayisi; ++jj) {
                        if (ii == i && jj == j) {
                            return &mintikaHucresi;
                        }

                        MintikaHucresi &kitliMintikaHucresi =
                                mintika[kapsam.solUstKoordinat.first + ii][kapsam.solUstKoordinat.second + jj];
                        kitliMintikaHucresi.temizligiBirak();
                    }
                }
            } else if (!mintikaHucresi.temizleniyor) {
                mintikaHucresi.temizleniyor = true;
                pthread_mutex_unlock(&mintikaHucresi.temizleniyorKilidi);
            }
        }

    return nullptr;
}

MintikaHucresi *Mintika::konumBossaKilitleDoluysaIlkDoluHucreyiDon(TutturucuKonumu &konum, TutunTutturucu &tutturucu) {
    for (int i = konum.konum.first - 1; i <= konum.konum.first + 1; ++i) {
        for (int j = konum.konum.second - 1; j <= konum.konum.second + 1; ++j) {
            MintikaHucresi &mintikaHucresi = mintika[i][j];

            pthread_mutex_lock(&mintikaHucresi.temizleniyorKilidi);
            if (mintikaHucresi.temizleniyor) {  // bu hucre dolu
                pthread_mutex_unlock(&mintikaHucresi.temizleniyorKilidi);

                // kapsamdaki diger kitlenen kilitleri ac
                for (int ii = konum.konum.first - 1; ii <= konum.konum.first + 1; ++ii) {
                    for (int jj = konum.konum.second - 1; jj <= konum.konum.second + 1; ++jj) {
                        if (!(ii == i && jj == j)) {
                            MintikaHucresi &kitliMintikaHucresi = mintika[ii][jj];
                            kitliMintikaHucresi.tutturucuTerketsin(tutturucu);
                        } else {
                            return &mintikaHucresi;
                        }
                    }
                }
            } else {
                mintikaHucresi.tutturucuGelsin(tutturucu);
                pthread_mutex_unlock(&mintikaHucresi.temizleniyorKilidi);
            }

        }
    }
    return nullptr;
}


MintikaHucresi::MintikaHucresi(int izmaritSayisi, bool temizleniyor, std::pair<int, int> &kordinat)
        : izmaritSayisi(izmaritSayisi),
          temizlikci(nullptr),
          tutturucuSayisi(0),
          kordinat(kordinat),
          temizleniyor(false) {
    pthread_mutex_init(&temizleniyorKilidi, nullptr);
    pthread_mutex_init(&tutturucuVarKilidi, nullptr);
    pthread_mutex_init(&tutturuluyorKilidi, nullptr);
    pthread_mutex_init(&hucreTutturucuSayisiKilidi, nullptr);
    pthread_mutex_init(&izmaritEklemeKilidi, nullptr);
    pthread_cond_init(&temizlikBirakildiCond, nullptr);
    pthread_cond_init(&tutturucuKalmadiVeyaMolaCond, nullptr);
}

MintikaHucresi::~MintikaHucresi() {
    pthread_mutex_destroy(&temizleniyorKilidi);
    pthread_mutex_destroy(&tutturucuVarKilidi);
    pthread_mutex_destroy(&tutturuluyorKilidi);
    pthread_mutex_destroy(&hucreTutturucuSayisiKilidi);
    pthread_mutex_destroy(&izmaritEklemeKilidi);
    pthread_cond_destroy(&temizlikBirakildiCond);
    pthread_cond_destroy(&tutturucuKalmadiVeyaMolaCond);
}

bool MintikaHucresi::tutturuluyor() {
    bool sonuc = false;

    pthread_mutex_lock(&tutturuluyorKilidi);
    sonuc = (tutturucuSayisi != 0);
    pthread_mutex_unlock(&tutturuluyorKilidi);

    return sonuc;
}

void MintikaHucresi::temizligiBirak() {
    pthread_mutex_lock(&temizleniyorKilidi);
    temizlikci = nullptr;
    temizleniyor = false;
    pthread_cond_signal(&temizlikBirakildiCond);
    pthread_mutex_unlock(&temizleniyorKilidi);
}

void MintikaHucresi::tutturucuTerketsin(TutunTutturucu &tutturucu) {
    pthread_mutex_lock(&tutturuluyorKilidi);
    for (auto it = begin(tutturuculer); it != end(tutturuculer); ++it) {
        if ((*it)->sid == tutturucu.sid) {
            tutturuculer.erase(it);
            break;
        }
    }
    if (--tutturucuSayisi == 0) {
        pthread_cond_signal(&tutturucuKalmadiVeyaMolaCond);
    }
    pthread_mutex_unlock(&tutturuluyorKilidi);
}

void MintikaHucresi::tutturucuGelsin(TutunTutturucu &tutturucu) {
    pthread_mutex_lock(&tutturuluyorKilidi);
    tutturuculer.push_back(&tutturucu);
    tutturucuSayisi++;
    pthread_mutex_unlock(&tutturuluyorKilidi);
}

void MintikaHucresi::izmaritEkle() {
    pthread_mutex_lock(&izmaritEklemeKilidi);
    izmaritSayisi++;
    pthread_mutex_unlock(&izmaritEklemeKilidi);

}

