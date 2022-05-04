#include "Mintika.h"


Mintika::Mintika(std::vector<std::vector<int>> &mintika) : mintika(mintika) {

}

void Mintika::yazdir() {
    std::cerr << "Mintika: " << std::endl;
    for (const std::vector<int>& satir: mintika) {
        for (int izmaritSayisi: satir) {
            std::cerr << izmaritSayisi << " ";
        }
        std::cerr << std::endl;
    }

}
