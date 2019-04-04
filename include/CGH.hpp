#ifndef CGH_hpp
#define CGH_hpp
#include "FT/DFT.hpp"
#include "FT/NFT.hpp"
#include "TrNFA/TrNFA.hpp"
#include "NuXmv/NuXmv.hpp"

namespace cgh{
    ID State::counter = 0;
    ID Var::counter = 0;
    ID Value::counter = 0;
    unordered_set<Object*> Manage::buffer;
    Manage manage;
    //template<> Label<class Character> FA<Label<class Character> >::epsilon = Label<Character>(FA<Character>::epsilon, FA<Character>::epsilon);
}
#endif /* CGH_hpp */
