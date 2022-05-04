
#include "Er.h"

Er::Er(int gid, int toplamaSuresiMs, int kapsamStrSayisi, int kapsamStnSayisi,
       const std::vector<std::pair<int, int>> &kapsamSolUstKordinatlari)
        : gid(gid), toplamaSuresiMs(toplamaSuresiMs),
          kapsamStrSayisi(kapsamStrSayisi),
          kapsamStnSayisi(kapsamStnSayisi),
          kapsamSolUstKordinatlari(kapsamSolUstKordinatlari) {

}
