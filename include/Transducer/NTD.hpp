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
#include "../FA/DFA.hpp"
#include "NTDState.hpp"
#include "DTDState.hpp"

namespace cgh {
    
    /// \brief A class of Nondeterministic Finite Automaton.
    template <class Character>
    class NTD : public NFA<Label<Character> > {
    };

    /// \brief A class of Nondeterministic Finite Automaton.
    template <class Character>
    class DTD : public DFA<Label<Character> > {
    public:
        typedef typename Alias4TD<Character>::Labels Labels;

        DTD(const Labels& labels) : DFA<Label<Character> >(labels) {
        }
        DTDState<Character>* mkState() {
            DTDState<Character>* dtdState = new DTDState<Character>();
            this -> stateSet.insert(dtdState);
            return dtdState;
        }

        DTDState<Character>* mkInitialState() {
            DTDState<Character>* dtdState = mkState();
            this -> initialState = dtdState;
            return dtdState;
        }

        DTDState<Character>* mkFinalState() {
            DTDState<Character>* dtdState = mkState();
            dtdState -> setFinalFlag(1);
            this -> finalStateSet.insert(dtdState);
            return dtdState;
        }
    };
};

#endif /* NTD_hpp */

