//
//  NTDState.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/2.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef NTDState_hpp
#define NTDState_hpp

#include "State.hpp"
namespace cgh{

    template <class Character>
    class NTDState : public NFAState<Label<Character> > {
        typedef typename Global<Character>::Char2LabelsMap Char2LabelsMap;

    private:
        Char2LabelsMap char2LabelsMap;      ///< the map from Character to Labels.

    public:

    };

}


#endif /* NTDState_hpp */
