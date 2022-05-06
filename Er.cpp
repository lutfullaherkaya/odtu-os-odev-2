#include "Er.h"
#include "Mintika.h"

Er::Er(int gid, int toplamaSuresiMs, std::vector<Kapsam> &kapsamlar, Mintika &mintika)
        : gid(gid), toplamaSuresiMs(toplamaSuresiMs),
          kapsamlar(kapsamlar),
          mintika(mintika) {

}

Er::~Er() {

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
