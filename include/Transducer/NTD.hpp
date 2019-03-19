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
    public:
        typedef typename Alias4TD<Character>::Labels Labels;
        typedef typename Alias4Char<Character>::Word Word;
        typedef typename Alias4Char<Character>::Words Words;

        NTD() : NFA<Label<Character> >() {

        }
        NTD(const Labels& labels) : NFA<Label<Character> >(labels) {
        }

        NFAState<Label<Character> >* mkState() {
            NTDState<Character>* ntdState = new NTDState<Character>();
            this -> stateSet.insert(ntdState);
            return ntdState;
        }

        NFAState<Label<Character> >* mkInitialState() {
            NFAState<Label<Character> >* ntdState = mkState();
            this -> initialState = ntdState;
            return ntdState;
        }

        NFAState<Label<Character> >* mkFinalState() {
            NFAState<Label<Character> >* ntdState = mkState();
            ntdState -> setFinalFlag(1);
            this -> finalStateSet.insert(ntdState);
            return ntdState;
        }

        DFA<Label<Character> >& determinize( void ) {
            if (this -> isNULL()) return FA<Label<Character> >::EmptyDFA();
            DTD<Character>* dtd = new DTD<Character>(this -> alphabet);
            NFA<Label<Character> >::determinize(dtd);
            return *dtd;
        }

        DFA<Label<Character> >& determinize( void ) const {
            return const_cast<NTD*>(this) -> determinize();
        }

        DTD<Character>& minimizeTD() {
            return ((DTD<Character>&)(determinize())).minimizeTD();
        }

        DTD<Character>& minimizeTD() const{
            return const_cast<NTD*>(this) -> minimizeTD();
        }
    };
};

#endif /* NTD_hpp */

