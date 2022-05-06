//
// Created by lutfullah on 06.05.2022.
//

#include "Emirler.h"

#include "Er.h"

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

pthread_mutex_t fakeMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fakeCond = PTHREAD_COND_INITIALIZER;


Emir::Emir(int zamanMs, timespec programBaslamaZamani) : programBaslamaZamani(programBaslamaZamani), zamanMs(zamanMs) {
    timespec eklenecekZaman = {zamanMs / 1000, zamanMs * 1000000};
    zaman = timespec_add(programBaslamaZamani, eklenecekZaman);
}

void Emir::zamaniBekle() {
    pthread_mutex_lock(&fakeMutex);
    pthread_cond_timedwait(&fakeCond, &fakeMutex, &zaman);
    pthread_mutex_unlock(&fakeMutex);
}


void MolaEmri::emret() {
    hw2_notify(ORDER_BREAK, 0, 0, 0);
}

MolaEmri::MolaEmri(int zamanMs, timespec programBaslamaZamani) : Emir(zamanMs, programBaslamaZamani) {

}

DurEmri::DurEmri(int zamanMs, timespec programBaslamaZamani) : Emir(zamanMs, programBaslamaZamani) {

}

void DurEmri::emret() {
    hw2_notify(ORDER_STOP, 0, 0, 0);
}

DevamEmri::DevamEmri(int zamanMs, timespec programBaslamaZamani) : Emir(zamanMs, programBaslamaZamani) {

}

void DevamEmri::emret() {
    hw2_notify(ORDER_CONTINUE, 0, 0, 0);
}
