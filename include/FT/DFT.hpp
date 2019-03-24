//
//  DFT.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef DFT_hpp
#define DFT_hpp

#include "../FA/DFA.hpp"
#include "DFTState.hpp"
#include "FT.hpp"

namespace cgh {
    
    template <class Character>
    class DFT : public DFA<Label<Character> >, public FT<Character> {
        typedef typename Alias4Char<Character>::Word Word;
        typedef typename Alias4Char<Character>::Words Words;
        typedef typename Alias4FT<Character>::Labels Labels;
        typedef typename Alias4FT<Character>::DFTStateSet DFTStateSet;
    private:
        void getTargetStateSetByUppers(Word& uppers, DFTStateSet& stateSet) {
            DFTStateSet workSet;
            DFTStateSet newWorkSet;
            workSet.insert((DFTState<Character>*)(this -> initialState));
            for (Character& upper : uppers) {
                newWorkSet.clear();
                for (DFTState<Character>* state : workSet) {
                    state -> getTargetStateSetByUpper(upper, newWorkSet);
                }
                if (newWorkSet.size() > 0) {
                    workSet.clear();
                    workSet.insert(newWorkSet.begin(), newWorkSet.end());
                }
            }
            if (workSet.size()) stateSet.insert(workSet.begin(), workSet.end());
        }

    public:

        DFT() : DFA<Label<Character> >() {
        }

        DFT(const Labels& labels) : DFA<Label<Character> >(labels) {
        }

        DFAState<Label<Character> >* mkState() {
            DFTState<Character>* dftState = new DFTState<Character>();
            this -> stateSet.insert(dftState);
            return dftState;
        }

        DFAState<Label<Character> >* mkInitialState() {
            DFAState<Label<Character> >* dftState = mkState();
            this -> initialState = dftState;
            return dftState;
        }

        DFTState<Character>* getInitialState() {
            return (DFTState<Character>*)(this -> initialState);
        }

        DFAState<Label<Character> >* mkFinalState() {
            DFAState<Label<Character> >* dftState = mkState();
            dftState -> setFinalFlag(1);
            this -> finalStateSet.insert(dftState);
            return dftState;
        }

        DFA<Label<Character> >& minimize() {
            if (this -> isMinimal()) return *this;
            DFT* dft = new DFT();
            this -> removeDeadState();
            this -> removeUnreachableState();
            if (this -> isNULL()) return FA<Label<Character> >::EmptyDFA();
            DFA<Label<Character> >::minimize(dft);
            return *dft;
        }

        DFA<Label<Character> >& minimize() const {
            return const_cast<DFT*>(this) -> minimize();
        }

        DFT<Character>& minimizeFT() {
            return (DFT<Character>&)minimize();
        }

        DFT<Character>& minimizeFT() const {
            return const_cast<DFT*>(this) -> minimizeFT();
        }

        DFT<Character>& operator & (const FT<Character>& ft) const {
            return FT<Character>::intersectFT(*this, ft);
        }
        
        DFT<Character>& operator | (const FT<Character>& ft) const {
            return FT<Character>::unionFT(*this, ft);
        }

        //DFT<Character>& leftQuotient(Word& word) {
        //    DFT<Character>& mdft = minimize();
        //    DFAState<Character>* state = mdft.getTargetStateByWord(word);
        //    if(!state) return FA<Character>::EmptyDFA();
        //    DFA<Character> dfa;
        //    dfa.flag = mdft.flag;
        //    dfa.setAlphabet(mdft.getAlphabet());
        //    DFAState2Map state2Map;
        //    state2Map[state] = dfa.mkInitialState();
        //    dfa.cpTrans(state, state2Map);
        //    dfa.setReachableFlag(1);
        //    return dfa.minimize();
        //}

    };
};

#endif /* DFT_hpp */

