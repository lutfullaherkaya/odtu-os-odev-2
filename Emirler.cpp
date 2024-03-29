//
// Created by lutfullah on 06.05.2022.
//

#include "Emirler.h"
#include "Mintika.h"
#include "hw2_output.h"



#define NSEC_PER_SEC 1000000000

timespec timespec_normalise(timespec ts) {
    while (ts.tv_nsec >= NSEC_PER_SEC) {
        ++(ts.tv_sec);
        ts.tv_nsec -= NSEC_PER_SEC;
    }
    while (ts.tv_nsec <= -NSEC_PER_SEC) {
        --(ts.tv_sec);
        ts.tv_nsec += NSEC_PER_SEC;
    }
    if (ts.tv_nsec < 0) {
        --(ts.tv_sec);
        ts.tv_nsec = (NSEC_PER_SEC + ts.tv_nsec);
    }
    return ts;
}

timespec timespec_add(timespec ts1, timespec ts2) {
    ts1 = timespec_normalise(ts1);
    ts2 = timespec_normalise(ts2);

    ts1.tv_sec += ts2.tv_sec;
    ts1.tv_nsec += ts2.tv_nsec;

    return timespec_normalise(ts1);
}

timespec timespec_add_ms(timespec ts1, int ms) {
    timespec eklenecekZaman = {ms / 1000, (ms % 1000)  * 1000000};
    return timespec_add(ts1, eklenecekZaman);
}

pthread_mutex_t fakeMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fakeCond = PTHREAD_COND_INITIALIZER;


Emir::Emir(int zamanMs, timespec programBaslamaZamani, Mintika &mintika)
        : programBaslamaZamani(programBaslamaZamani), zamanMs(zamanMs), mintika(mintika) {
    zaman = timespec_add_ms(programBaslamaZamani, zamanMs);
}

void Emir::zamaniBekle() {
    pthread_mutex_lock(&fakeMutex);
    pthread_cond_timedwait(&fakeCond, &fakeMutex, &zaman);
    pthread_mutex_unlock(&fakeMutex);
}




void MolaEmri::emret() {
    pthread_mutex_lock(&mintika.emirKilidi);
    if (mintika.molada) {
        hw2_notify(ORDER_BREAK, 0, 0, 0);
        pthread_mutex_unlock(&mintika.emirKilidi);
    } else {
        pthread_mutex_unlock(&mintika.emirKilidi);

        pthread_mutex_lock(&mintika.erSayisiKilidi);
        while (mintika.moladaErSayisi > 0) {
            pthread_cond_wait(&mintika.erSayisiCond, &mintika.erSayisiKilidi);
        }
        pthread_mutex_unlock(&mintika.erSayisiKilidi);

        pthread_mutex_lock(&mintika.emirKilidi);
        hw2_notify(ORDER_BREAK, 0, 0, 0);
        mintika.molada = true;
        pthread_cond_broadcast(&mintika.emirCond);
        pthread_mutex_unlock(&mintika.emirKilidi);
    }

}

MolaEmri::MolaEmri(int zamanMs, timespec programBaslamaZamani, Mintika &mintika)
        : Emir(zamanMs, programBaslamaZamani, mintika) {

}

DurEmri::DurEmri(int zamanMs, timespec programBaslamaZamani, Mintika &mintika)
        : Emir(zamanMs, programBaslamaZamani, mintika) {

}

void DurEmri::emret() {
    pthread_mutex_lock(&mintika.emirKilidi);
    hw2_notify(ORDER_STOP, 0, 0, 0);
    if (!mintika.durEmriGeldi) {
        mintika.durEmriGeldi = true;
        pthread_cond_broadcast(&mintika.emirCond);
    }
    pthread_mutex_unlock(&mintika.emirKilidi);
}

DevamEmri::DevamEmri(int zamanMs, timespec programBaslamaZamani, Mintika &mintika)
        : Emir(zamanMs, programBaslamaZamani, mintika) {

}

void DevamEmri::emret() {
    pthread_mutex_lock(&mintika.emirKilidi);
    if (mintika.molada) {
        pthread_mutex_unlock(&mintika.emirKilidi);

        pthread_mutex_lock(&mintika.erSayisiKilidi);
        while (mintika.calisanErSayisi > 0) {
            pthread_cond_wait(&mintika.erSayisiCond, &mintika.erSayisiKilidi);
        }
        pthread_mutex_unlock(&mintika.erSayisiKilidi);

        pthread_mutex_lock(&mintika.emirKilidi);
        hw2_notify(ORDER_CONTINUE, 0, 0, 0);
        mintika.molada = false;
        pthread_cond_broadcast(&mintika.emirCond);
        pthread_mutex_unlock(&mintika.emirKilidi);
    } else {
        hw2_notify(ORDER_CONTINUE, 0, 0, 0);
        pthread_mutex_unlock(&mintika.emirKilidi);
    }
}
