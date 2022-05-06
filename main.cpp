#include "main.h"
#include "Er.h"
#include "Mintika.h"
#include "hw2_output.h"
#include <iostream>
#include <vector>
#include <utility>
#include <pthread.h>
#include <unistd.h>
#include "HataAyiklama.h"
#include <chrono>
#include <thread>

using namespace std;


void *erThreadMaini(void *erPtr) {
    hw2_notify(GATHERER_CREATED, ((Er *) erPtr)->gid, 0, 0);
    Er &buEr = *(Er *) erPtr;
    Mintika &mintika = buEr.mintika;

    for (Kapsam &kapsam: buEr.kapsamlar) {

        MintikaHucresi *doluHucre;
        while ((doluHucre = mintika.kapsamBossaKitleDoluysaIlkDoluHucreyiDon(kapsam, buEr)) != nullptr) {
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

        hw2_notify(GATHERER_ARRIVED, buEr.gid, kapsam.solUstKoordinat.first, kapsam.solUstKoordinat.second);

        for (int i = 0; i < kapsam.strSayisi; ++i) {
            for (int j = 0; j < kapsam.stnSayisi; ++j) {
                MintikaHucresi &mintikaHucresi = kapsam.mintikaHucresiGetir(mintika, i, j);
                mintikaHucresi.temizlikci = &buEr;

            }
        }

        for (int i = 0; i < kapsam.strSayisi; ++i) {
            for (int j = 0; j < kapsam.stnSayisi; ++j) {
                MintikaHucresi &mintikaHucresi = kapsam.mintikaHucresiGetir(mintika, i, j);
                while (mintikaHucresi.izmaritSayisi) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(buEr.toplamaSuresiMs));
                    mintikaHucresi.izmaritSayisi--;
                    hw2_notify(GATHERER_GATHERED, buEr.gid, i, j);
                }


            }
        }
        hw2_notify(GATHERER_CLEARED, buEr.gid, 0, 0);

        for (int i = 0; i < kapsam.strSayisi; ++i) {
            for (int j = 0; j < kapsam.stnSayisi; ++j) {
                MintikaHucresi &mintikaHucresi = kapsam.mintikaHucresiGetir(mintika, i, j);
                mintikaHucresi.temizlikci = nullptr; // todo:
                pthread_mutex_unlock(&mintikaHucresi.temizleniyorKilidi);
            }
        }
    }
    hw2_notify(GATHERER_EXITED, buEr.gid, 0, 0);
    return nullptr;
}

int main() {
    hw2_init_notifier();

    pthread_mutex_init(&(HataAyiklama::ioKilidi), nullptr);
    int Gi, Gj;
    cin >> Gi >> Gj;
    std::vector<std::vector<MintikaHucresi>> mintikaVektoru;
    for (int i = 0; i < Gi; ++i) {
        mintikaVektoru.emplace_back();
        for (int j = 0; j < Gj; ++j) {
            int izmaritSayisi;
            cin >> izmaritSayisi;
            mintikaVektoru[i].emplace_back(izmaritSayisi, false);
        }
    }
    Mintika mintika(mintikaVektoru);

    std::vector<pthread_t> erThreadIdleri;

    std::vector<Er> erler;

    int erSayisi;
    cin >> erSayisi;
    for (int i = 0; i < erSayisi; ++i) {
        int gid, si, sj, tg, ng;
        cin >> gid >> si >> sj >> tg >> ng;
        std::vector<Kapsam> kapsamlar;
        for (int j = 0; j < ng; ++j) {
            int ik, jk;
            cin >> ik >> jk;
            kapsamlar.emplace_back(si, sj, std::pair<int, int>(ik, jk));
        }
        erler.emplace_back(gid, tg, kapsamlar, mintika);


    }
    for (int i = 0; i < erler.size(); ++i) {
        erThreadIdleri.emplace_back();
        pthread_create(&(erThreadIdleri[i]), nullptr, erThreadMaini, &(erler[i]));
        pthread_detach(erThreadIdleri[i]);
    }


    while (1) {
        HataAyiklama::ioKitle();
        mintika.yazdir();
        HataAyiklama::ioKilidiAc();
        sleep(1);
    }


    return 0;
}