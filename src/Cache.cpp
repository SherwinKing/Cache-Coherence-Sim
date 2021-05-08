#include "Cache.h"

Cache::Cache(int id, int s, int E, int b) : id(id), s(s), E(E), b(b) {
    this->id = id;
    this->s = s;
    this->E = E;
    this->b = b;

    for (long setID = 0; setID < 1 << s; setID++) {
        cacheSets.push_back(CacheSet(setID, this->E));
    }
    assert(cacheSets[0].getSetID() == 0);
}
