//
//  NTD.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef NTD_hpp
#define NTD_hpp

#include "../FA/NFA.hpp"
#include "NTDState.hpp"

namespace cgh {
    
    /// \brief A class of Nondeterministic Finite Automaton.
    template <class Character>
    class NTD : public NFA<Label<Character> > {
    };
};

#endif /* NTD_hpp */

