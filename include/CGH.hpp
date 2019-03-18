#ifndef CGH_hpp
#define CGH_hpp
#include "Transducer/DTD.hpp"
#include "Transducer/NTD.hpp"

namespace cgh{
    ID State::counter = 0;
    vector<Object*> Manage::buffer;
    Manage manage;
}
#endif /* CGH_hpp */
