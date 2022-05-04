#include "main.h"
#include "Er.h"
#include "Mintika.h"
#include "hw2_output.h"
#include <iostream>
#include <vector>
#include <utility>

using namespace std;

int main() {
    int Gi, Gj;
    cin >> Gi >> Gj;
    std::vector<std::vector<int>> mintikaVektoru;
    for (int i = 0; i < Gi; ++i) {
        mintikaVektoru.emplace_back();
        for (int j = 0; j < Gj; ++j) {
            int izmaritSayisi;
            cin >> izmaritSayisi;
            mintikaVektoru[i].push_back(izmaritSayisi);
        }
    }
    Mintika mintika(mintikaVektoru);
    std::vector<Er> erler;

    int erSayisi;
    cin >> erSayisi;
    for (int i = 0; i < erSayisi; ++i) {
        int gid, si, sj, tg, ng;
        cin >> gid >> si >> sj >> tg >> ng;
        std::vector<std::pair<int, int>> kordinatlar;
        for (int j = 0; j < ng; ++j) {
            int ik, jk;
            cin >> ik >> jk;
            kordinatlar.emplace_back(ik, jk);
        }
        erler.emplace_back(gid, tg, si, sj, kordinatlar);
    }
    int j = 2 + 2;
    return 0;
}