#include "Er.h"
#include "Mintika.h"
#include <chrono>
#include <thread>

Er::Er(int gid, int toplamaSuresiMs, std::vector<Kapsam> &kapsamlar, Mintika &mintika)
        : gid(gid), toplamaSuresiMs(toplamaSuresiMs),
          kapsamlar(kapsamlar),
          mintika(mintika),
          molada(false) {

}

Er::~Er() {

}


void Er::kapsamRezerveEt(Kapsam &kapsam) {
    MintikaHucresi *doluHucre;
    while ((doluHucre = mintika.kapsamBossaKilitleDoluysaIlkDoluHucreyiDon(kapsam, *this))) {
        doluHucre->temizliginBitmesiniBekle();
        emirVarsaUy();
        doluHucre->tutturuculerinGitmesiniBekle(mintika);
        emirVarsaUy();
    }
    kapsam.temizlikcileriAyarla(mintika, this);
    hw2_notify(GATHERER_ARRIVED, gid, kapsam.solUstKoordinat.first, kapsam.solUstKoordinat.second);
}

/**
 * @param kapsam
 * @return molaya girerse false isini bitirdiyse true
 */
bool Er::izmaritTopla(Kapsam &kapsam) {
    for (int i = 0; i < kapsam.strSayisi; ++i) {
        for (int j = 0; j < kapsam.stnSayisi; ++j) {
            MintikaHucresi &mintikaHucresi = kapsam.mintikaHucresiGetir(mintika.mintika, i, j);
            while (mintikaHucresi.izmaritSayisi) {
                pthread_mutex_lock(&mintika.emirKilidi);
                if (molaysaRezervasyonBitirGerekirseDur(kapsam)) {
                    pthread_mutex_unlock(&mintika.emirKilidi);
                    return false;
                }

                timespec toplamaZamani = toplamaZamaniHesapla();
                if (pthread_cond_timedwait(&mintika.emirCond, &mintika.emirKilidi, &toplamaZamani) == 0) {
                    if (molaysaRezervasyonBitirGerekirseDur(kapsam)) {
                        pthread_mutex_unlock(&mintika.emirKilidi);
                        return false;
                    }
                } else {
                    mintikaHucresi.izmaritSayisi--;
                    hw2_notify(GATHERER_GATHERED, gid, kapsam.solUstKoordinat.first + i,
                               kapsam.solUstKoordinat.second + j);
                }
                pthread_mutex_unlock(&mintika.emirKilidi);

            }
        }
    }
    hw2_notify(GATHERER_CLEARED, gid, 0, 0);
    return true;
}

void Er::rezervasyonuBitir(Kapsam &kapsam) {
    for (int i = 0; i < kapsam.strSayisi; ++i) {
        for (int j = 0; j < kapsam.stnSayisi; ++j) {
            kapsam.mintikaHucresiGetir(mintika.mintika, i, j).temizligiBirak();
        }
    }
}

void Er::emirVarsaUy() {
    pthread_mutex_lock(&mintika.emirKilidi);
    while (mintika.molada || mintika.durEmriGeldi) {
        if (mintika.durEmriGeldi) {
            if (mintika.molada) {
                mintika.moladaErAzalt();
            } else {
                mintika.calisanErAzalt();
            }
            hw2_notify(GATHERER_STOPPED, gid, 0, 0);
            pthread_mutex_unlock(&mintika.emirKilidi);
            pthread_exit(nullptr);
        }
        if (mintika.molada) {
            moladaDegilseMolayaAlVeNotifyYap();
            pthread_cond_wait(&mintika.emirCond, &mintika.emirKilidi);
        }
    }
    // buraya gelirse mintika.molada falsedir.
    moladaysaMoladanCikarVeNotifyYap();
    pthread_mutex_unlock(&mintika.emirKilidi);
}

timespec Er::toplamaZamaniHesapla() {
    timespec simdi;
    timespec_get(&simdi, TIME_UTC);
    return timespec_add_ms(simdi, toplamaSuresiMs);
}

/**
 * mintika.emirKilidi mutexi korumasinda olmalidir. dur emrinde kilit acilir. bunu sonradan fark ettim ama cok guzel bir bugdu begendim.
 * @param kapsam
 * @return molaysa ve rezervasyon bitirdiyse true yoksa false
 */
bool Er::molaysaRezervasyonBitirGerekirseDur(Kapsam &kapsam) {
    if (mintika.durEmriGeldi) {
        if (mintika.molada) {
            mintika.moladaErAzalt();
        } else {
            mintika.calisanErAzalt();
        }
        hw2_notify(GATHERER_STOPPED, gid, 0, 0);
        rezervasyonuBitir(kapsam);
        pthread_mutex_unlock(&mintika.emirKilidi);
        pthread_exit(nullptr);
    }
    if (mintika.molada) {
        moladaDegilseMolayaAlVeNotifyYap();
        rezervasyonuBitir(kapsam);
        return true;
    }
    return false;

}

/**
 * mintika.emirKilidi mutexi korumasinda olmalidir.
 */
void Er::moladaDegilseMolayaAlVeNotifyYap() {
    if (!molada) {
        molada = true;
        mintika.calisanErAzalt();
        mintika.moladaErArttir();
        hw2_notify(GATHERER_TOOK_BREAK, gid, 0, 0);
    }
}

/**
 * mintika.emirKilidi mutexi korumasinda olmalidir.
 */
void Er::moladaysaMoladanCikarVeNotifyYap() {
    if (molada) {
        molada = false;
        mintika.moladaErAzalt();
        mintika.calisanErArttir();
        hw2_notify(GATHERER_CONTINUED, gid, 0, 0);
    }
}


Kapsam::Kapsam(int strSayisi, int stnSayisi,
               const std::pair<int, int> &solUstKoordinat)
        : strSayisi(strSayisi),
          stnSayisi(stnSayisi),
          solUstKoordinat(solUstKoordinat) {}

MintikaHucresi &Kapsam::mintikaHucresiGetir(std::vector<std::vector<MintikaHucresi>> &mintika, int i, int j) {
    return mintika[solUstKoordinat.first + i][solUstKoordinat.second + j];
}

void Kapsam::temizlikcileriAyarla(Mintika &mintika, Er *erPtr) {
    for (int i = 0; i < strSayisi; ++i) {
        for (int j = 0; j < stnSayisi; ++j) {
            mintikaHucresiGetir(mintika.mintika, i, j).temizlikci = erPtr;
        }
    }
}

void Kapsam::iVeJYeKadarKilitAc(int i, int j, std::vector<std::vector<MintikaHucresi>> &mintika) {
    for (int ii = 0; ii < strSayisi; ++ii) {
        for (int jj = 0; jj < stnSayisi; ++jj) {
            if (ii == i && jj == j) {
                return;
            }
            mintikaHucresiGetir(mintika, ii, jj).temizligiBirak();
        }
    }
}
