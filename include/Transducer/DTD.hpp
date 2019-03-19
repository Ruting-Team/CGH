//
//  DTD.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef DTD_hpp
#define DTD_hpp

#include "../FA/DFA.hpp"
#include "DTDState.hpp"
#include "Transducer.hpp"

namespace cgh {
    
    template <class Character>
    class DTD : public DFA<Label<Character> >, public Transducer<Character> {
    private:

    public:
        typedef typename Alias4TD<Character>::Labels Labels;
        typedef typename Alias4Char<Character>::Word Word;
        typedef typename Alias4Char<Character>::Words Words;

        DTD() : DFA<Label<Character> >() {
        }

        DTD(const Labels& labels) : DFA<Label<Character> >(labels) {
        }

        DFAState<Label<Character> >* mkState() {
            DTDState<Character>* dtdState = new DTDState<Character>();
            this -> stateSet.insert(dtdState);
            return dtdState;
        }

        DFAState<Label<Character> >* mkInitialState() {
            DFAState<Label<Character> >* dtdState = mkState();
            this -> initialState = dtdState;
            return dtdState;
        }

        DTDState<Character>* getInitialState() {
            return (DTDState<Character>*)(this -> initialState);
        }

        DFAState<Label<Character> >* mkFinalState() {
            DFAState<Label<Character> >* dtdState = mkState();
            dtdState -> setFinalFlag(1);
            this -> finalStateSet.insert(dtdState);
            return dtdState;
        }

        DFA<Label<Character> >& minimize() {
            if (this -> isMinimal()) return *this;
            DTD* dtd = new DTD();
            this -> removeDeadState();
            this -> removeUnreachableState();
            if (this -> isNULL()) return FA<Label<Character> >::EmptyDFA();
            DFA<Label<Character> >::minimize(dtd);
            return *dtd;
        }

        DFA<Label<Character> >& minimize() const {
            return const_cast<DTD*>(this) -> minimize();
        }

        DTD<Character>& minimizeTD() {
            return (DTD<Character>&)minimize();
        }

        DTD<Character>& minimizeTD() const {
            return const_cast<DTD*>(this) -> minimizeTD();
        }
    };
};

#endif /* DTD_hpp */

