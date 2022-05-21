#include "Mintika.h"

Mintika::Mintika(std::vector<std::vector<MintikaHucresi>> &mintika) : molada(false), durEmriGeldi(false),
                                                                      mintika(mintika) {
    pthread_mutex_init(&emirKilidi, nullptr);
    pthread_cond_init(&cond, nullptr);
}

void Mintika::yazdir() {
    std::cerr << "Mintika: " << std::endl;
    for (const std::vector<MintikaHucresi> &satir: mintika) {
        for (const MintikaHucresi &hucre: satir) {
            std::string hucreStr;
            hucreStr += std::to_string(hucre.izmaritSayisi);
            if (hucre.temizlikci) {
                hucreStr += "[" + std::to_string(hucre.temizlikci->gid) + "]";
            }
            hucreStr += "("+ std::to_string(hucre.tutturucuSayisi) +")";
            for (auto &tutturucu: hucre.tutturuculer) {
                hucreStr += "{" + std::to_string(tutturucu->sid) + "}";
            }
            int hucreStrLen = hucreStr.size();
            for (int i = 0; i < 16 - hucreStrLen; ++i) {
                hucreStr += " ";
            }
            std::cerr << hucreStr;

        }
        std::cerr << std::endl;
    }

}

Mintika::~Mintika() {
    pthread_mutex_destroy(&emirKilidi);
    pthread_cond_destroy(&cond);
}

MintikaHucresi *Mintika::kapsamBossaKilitleDoluysaIlkDoluHucreyiDon(Kapsam &kapsam, Er &er) {
    for (int i = 0; i < kapsam.strSayisi; ++i) {
        for (int j = 0; j < kapsam.stnSayisi; ++j) {
            MintikaHucresi &mintikaHucresi = kapsam.mintikaHucresiGetir(mintika, i, j);
            if (mintikaHucresi.temizleniyordur() || mintikaHucresi.tutturuluyor()) {  // bu hucre dolu
                kapsam.iVeJYeKadarKilitAc(i, j, mintika);
                return &mintikaHucresi;
            } else if (!mintikaHucresi.temizleniyordur()) {
                mintikaHucresi.setTemizleniyor(true);
            }
        }
    }
    return nullptr;
}

MintikaHucresi *Mintika::konumBossaKilitleDoluysaIlkDoluHucreyiDon(TutturucuKonumu &konum, TutunTutturucu &tutturucu) {
    for (int i = konum.konum.first - 1; i <= konum.konum.first + 1; ++i) {
        for (int j = konum.konum.second - 1; j <= konum.konum.second + 1; ++j) {
            if (mintika[i][j].temizleniyordur()) {
                konum.iVeJYeKadarKilitAc(i, j, tutturucu, *this);
                return &mintika[i][j];
            } else {
                mintika[i][j].tutturucuGelsin(tutturucu);
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
    pthread_mutex_init(&izmaritEklemeKilidi, nullptr);
    pthread_cond_init(&temizlikBirakildiCond, nullptr);
    pthread_cond_init(&tutturucuKalmadiVeyaMolaCond, nullptr);
}

MintikaHucresi::~MintikaHucresi() {
    pthread_mutex_destroy(&temizleniyorKilidi);
    pthread_mutex_destroy(&tutturucuVarKilidi);
    pthread_mutex_destroy(&tutturuluyorKilidi);
    pthread_mutex_destroy(&izmaritEklemeKilidi);
    pthread_cond_destroy(&temizlikBirakildiCond);
    pthread_cond_destroy(&tutturucuKalmadiVeyaMolaCond);
}

bool MintikaHucresi::tutturuluyor() {
    bool sonuc;

    pthread_mutex_lock(&tutturuluyorKilidi);
    sonuc = (tutturucuSayisi != 0);
    pthread_mutex_unlock(&tutturuluyorKilidi);

    return sonuc;
}

void MintikaHucresi::temizligiBirak() {
    pthread_mutex_lock(&temizleniyorKilidi);
    temizlikci = nullptr;
    temizleniyor = false;
    pthread_cond_broadcast(&temizlikBirakildiCond);
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
    tutturucuSayisi--;
    if (tutturucuSayisi == 0) {
        pthread_cond_broadcast(&tutturucuKalmadiVeyaMolaCond);
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

void MintikaHucresi::temizliginBitmesiniBekle() {
    pthread_mutex_lock(&temizleniyorKilidi);
    while (temizleniyor) {
        pthread_cond_wait(&temizlikBirakildiCond, &temizleniyorKilidi);
    }
    pthread_mutex_unlock(&temizleniyorKilidi);
}

void MintikaHucresi::tutturuculerinGitmesiniBekle(Mintika &mintika) {
    pthread_mutex_lock(&tutturuluyorKilidi);
    bool emirGelmis = false;
    while (!emirGelmis && tutturucuSayisi != 0) {
        pthread_cond_wait(&tutturucuKalmadiVeyaMolaCond, &tutturuluyorKilidi);
        pthread_mutex_lock(&mintika.emirKilidi);
        if (mintika.molada || mintika.durEmriGeldi) {
            emirGelmis = true;
        }
        pthread_mutex_unlock(&mintika.emirKilidi);
    }
    pthread_mutex_unlock(&tutturuluyorKilidi);
}

bool MintikaHucresi::temizleniyordur() {
    bool sonuc;
    pthread_mutex_lock(&temizleniyorKilidi);
    sonuc = temizleniyor;
    pthread_mutex_unlock(&temizleniyorKilidi);
    return sonuc;
}

void MintikaHucresi::setTemizleniyor(bool b) {
    pthread_mutex_lock(&temizleniyorKilidi);
    temizleniyor = b;
    pthread_mutex_unlock(&temizleniyorKilidi);
}

