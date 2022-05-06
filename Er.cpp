#include "Er.h"
#include "Mintika.h"
#include <chrono>
#include <thread>
Er::Er(int gid, int toplamaSuresiMs, std::vector<Kapsam> &kapsamlar, Mintika &mintika)
        : gid(gid), toplamaSuresiMs(toplamaSuresiMs),
          kapsamlar(kapsamlar),
          mintika(mintika) {

}

Er::~Er() {

}

void Er::kapsamRezerveEt(Kapsam &kapsam) {
    MintikaHucresi *doluHucre;
    while ((doluHucre = mintika.kapsamBossaKitleDoluysaIlkDoluHucreyiDon(kapsam, *this)) != nullptr) {
        /*HataAyiklama::ioKitle();
        std::cerr << "gid:" << buEr.gid << "kitlendi." << std::endl;
        HataAyiklama::ioKilidiAc();*/

        pthread_mutex_lock(&doluHucre->temizleniyorKilidi);
        // artik bu hucre serbest. diger hucreleri bir daha kontrol edip serbestse baslayabiliriz.
        pthread_mutex_unlock(&doluHucre->temizleniyorKilidi);

        /*HataAyiklama::ioKitle();
        std::cerr << "gid:" << buEr.gid << "kilit acildi." << std::endl;
        HataAyiklama::ioKilidiAc();*/
    }
    hw2_notify(GATHERER_ARRIVED, gid, kapsam.solUstKoordinat.first, kapsam.solUstKoordinat.second);

    for (int i = 0; i < kapsam.strSayisi; ++i) {
        for (int j = 0; j < kapsam.stnSayisi; ++j) {
            MintikaHucresi &mintikaHucresi = kapsam.mintikaHucresiGetir(mintika, i, j);
            mintikaHucresi.temizlikci = this;

        }
    }
}

void Er::izmaritTopla(Kapsam &kapsam) {
    for (int i = 0; i < kapsam.strSayisi; ++i) {
        for (int j = 0; j < kapsam.stnSayisi; ++j) {
            MintikaHucresi &mintikaHucresi = kapsam.mintikaHucresiGetir(mintika, i, j);
            while (mintikaHucresi.izmaritSayisi) {
                std::this_thread::sleep_for(std::chrono::milliseconds(toplamaSuresiMs));
                mintikaHucresi.izmaritSayisi--;
                hw2_notify(GATHERER_GATHERED, gid, i, j);
            }
        }
    }
    hw2_notify(GATHERER_CLEARED, gid, 0, 0);
}

void Er::rezervasyonuBitir(Kapsam &kapsam) {
    for (int i = 0; i < kapsam.strSayisi; ++i) {
        for (int j = 0; j < kapsam.stnSayisi; ++j) {
            MintikaHucresi &mintikaHucresi = kapsam.mintikaHucresiGetir(mintika, i, j);
            mintikaHucresi.temizlikci = nullptr;
            pthread_mutex_unlock(&mintikaHucresi.temizleniyorKilidi);
        }
    }
}


Kapsam::Kapsam(int strSayisi, int stnSayisi,
               const std::pair<int, int> &solUstKoordinat)
        : strSayisi(strSayisi),
          stnSayisi(stnSayisi),
          solUstKoordinat(
                  solUstKoordinat) {}

MintikaHucresi &Kapsam::mintikaHucresiGetir(Mintika &mintika, int i, int j) {
    return mintika.mintika[solUstKoordinat.first + i][solUstKoordinat.second + j];;
}
