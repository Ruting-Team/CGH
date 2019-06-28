#ifndef CGH_hpp
#define CGH_hpp
#include "FT/DFT.hpp"
#include "FT/NFT.hpp"
#include "TrNFA/TrNFA.hpp"
#include "TrPDS/TrPDS.hpp"

namespace cgh{
ID State::counter = 0;
unordered_set<Object*> Manage::buffer;
Manage manage;
}
#endif /* CGH_hpp */
