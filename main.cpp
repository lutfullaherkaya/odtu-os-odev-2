#include "main.h"
#include "Er.h"
#include "TutunTutturucu.h"
#include "Mintika.h"
#include "hw2_output.h"
#include <iostream>
#include <vector>
#include <utility>
#include <pthread.h>
#include <unistd.h>
#include "HataAyiklama.h"

#include "Emirler.h"


using namespace std;


void *erThreadMaini(void *erPtr) {
    hw2_notify(GATHERER_CREATED, ((Er *) erPtr)->gid, 0, 0);
    Er &buEr = *(Er *) erPtr;
    Mintika &mintika = buEr.mintika;

    buEr.molaninBitmesiniBekleGerekirseDur();

    for (Kapsam &kapsam: buEr.kapsamlar) {
        buEr.kapsamRezerveEt(kapsam);
        while (!buEr.izmaritTopla(kapsam)) {
            buEr.molaninBitmesiniBekleGerekirseDur();
            buEr.kapsamRezerveEt(kapsam);
        }
        buEr.rezervasyonuBitir(kapsam);

    }
    hw2_notify(GATHERER_EXITED, buEr.gid, 0, 0);
    return nullptr;
}

void *amirThreadMaini(void *emirlerPtr) {
    vector<Emir *> emirler = *((vector<Emir *> *) emirlerPtr);
    for (Emir *emir: emirler) {
        emir->zamaniBekle();
        emir->emret();
    }
    return nullptr;
}

void *tutturucuThreadMaini(void *tutturucuPtr) {
    hw2_notify(SNEAKY_SMOKER_CREATED, ((TutunTutturucu *) tutturucuPtr)->sid, 0, 0);
    TutunTutturucu &buTutturucu = *(TutunTutturucu *) tutturucuPtr;
    Mintika &mintika = buTutturucu.mintika;

    buTutturucu.durulacaksaDurEmirKilitli(nullptr);

    for (TutturucuKonumu &konum : buTutturucu.konumlar) {
        buTutturucu.konumRezerveEt(konum);
        buTutturucu.tuttur(konum);
        buTutturucu.rezervasyonuBitir(konum);
    }
    hw2_notify(SNEAKY_SMOKER_EXITED, buTutturucu.sid, 0, 0);



    return nullptr;
}

int main() {
    hw2_init_notifier();
    timespec programBaslamaZamani;
    timespec_get(&programBaslamaZamani, TIME_UTC);
    pthread_mutex_init(&(HataAyiklama::ioKilidi), nullptr);


    int Gi, Gj;
    cin >> Gi >> Gj;
    std::vector<std::vector<MintikaHucresi>> mintikaVektoru;
    for (int i = 0; i < Gi; ++i) {
        mintikaVektoru.emplace_back();
        for (int j = 0; j < Gj; ++j) {
            int izmaritSayisi;
            cin >> izmaritSayisi;
            std::pair<int, int> kordinat = {i, j};
            mintikaVektoru[i].emplace_back(izmaritSayisi, false, kordinat);
        }
    }
    Mintika mintika(mintikaVektoru);

    std::vector<pthread_t> threadIdleri;

    std::vector<Er> erler;

    std::vector<Emir *> emirler;
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

    bool durEmriGeldi = false;
    int emirSayisi;
    cin >> emirSayisi;
    for (int i = 0; i < emirSayisi; ++i) {
        int ms;
        cin >> ms;

        string emirYazisi;
        cin >> emirYazisi;

        if (!durEmriGeldi) {
            if (emirYazisi == "break") {
                emirler.push_back(new MolaEmri(ms, programBaslamaZamani, mintika));
            } else if (emirYazisi == "continue") {
                emirler.push_back(new DevamEmri(ms, programBaslamaZamani, mintika));
            } else if (emirYazisi == "stop") {
                emirler.push_back(new DurEmri(ms, programBaslamaZamani, mintika));
                durEmriGeldi = true;
            }
        }

    }

    std::vector<TutunTutturucu> tutturuculer;
    int tutturucuSayisi;
    cin >> tutturucuSayisi;
    for (int i = 0; i < tutturucuSayisi; ++i) {
        int sid, tutturmeSuresiMs, konumSayisi;
        cin >> sid >> tutturmeSuresiMs >> konumSayisi;
        std::vector<TutturucuKonumu> konumlar;
        for (int j = 0; j < konumSayisi; ++j) {
            int icilecekSigaraSayisi;
            std::pair<int, int> konum = {i, j};
            cin >> konum.first >> konum.second >> icilecekSigaraSayisi;
            konumlar.emplace_back(icilecekSigaraSayisi, konum);
        }
        tutturuculer.emplace_back(sid, tutturmeSuresiMs, konumlar, mintika);
    }

    int i;
    for (i = 0; i < erler.size(); ++i) {
        threadIdleri.emplace_back();
        pthread_create(&(threadIdleri[i]), nullptr, erThreadMaini, &(erler[i]));
        /*pthread_detach(erThreadIdleri[i]);*/
    }
    threadIdleri.emplace_back();
    pthread_create(&(threadIdleri[i]), nullptr, amirThreadMaini, &emirler);
    i++;
    for (auto & tutturucu : tutturuculer) {
        threadIdleri.emplace_back();
        pthread_create(&(threadIdleri[i]), nullptr, tutturucuThreadMaini, &tutturucu);
        i++;
    }


    // todo: sil
    while (1) {
        HataAyiklama::ioKitle();
        mintika.yazdir();
        HataAyiklama::ioKilidiAc();
        sleep(1);
    }

    for (pthread_t threadId: threadIdleri) {
        pthread_join(threadId, nullptr);
    }

    // todo: delete emirler
    return 0;
}