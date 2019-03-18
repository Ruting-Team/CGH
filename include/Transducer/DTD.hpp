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
        bool isMinimalTD() {
            return this -> isMinimal();
        }
    public:
        typedef typename Alias4TD<Character>::Labels Labels;
        typedef typename Alias4Char<Character>::Word Word;
        typedef typename Alias4Char<Character>::Words Words;

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
    template <class Character>
    class SmartDTD {
    private:
        DTD<Character>* dtd;
        bool del;
        bool confirm; 
    public:
        SmartDTD() : dtd(nullptr), del(0), confirm(0){}
        SmartDTD(const DTD<Character>* d, bool b, bool c = 0) : dtd(const_cast<DTD<Character>*>(d)), del(b) , confirm(c) {}
        SmartDTD(const SmartDTD& smartDTD) {
            dtd = smartDTD.dtd;
            del = smartDTD.del;
            confirm = 1;
        }
        ~SmartDTD() {
            if (del & confirm) delete dtd;
        }

        DTD<Character>* getDTD() {return dtd;}
    };
};

#endif /* DTD_hpp */

