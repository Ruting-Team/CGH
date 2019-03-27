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
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4Char<Label<Character> >::Characters Labels;
        typedef typename Alias4Char<Label<Character> >::Word LabelWord;
        typedef typename Alias4FA<Label<Character> >::DFAState2NFAStateMap DFAState2NFAStateMap;
        typedef typename Alias4FT<Character>::DFTStates DFTStates;
    private:
        void getTargetStatesByUppers(Word& uppers, DFTStates& states) {
            DFTStates works;
            DFTStates newWorks;
            works.insert((DFTState<Character>*)(this -> initialState));
            for (Character& upper : uppers) {
                newWorks.clear();
                for (DFTState<Character>* state : works) {
                    state -> getTargetStatesByUpper(upper, newWorks);
                }
                if (newWorks.size() > 0) {
                    works.clear();
                    works.insert(newWorks.begin(), newWorks.end());
                }
            }
            if (works.size()) states.insert(works.begin(), works.end());
        }

        ID getCode() {
            auto& transMap = (this -> initialState) -> getTransMap();
            ID code = hash<double>()(transMap.size() * 1234);
            for (auto& mapPair : transMap) {
                ID upperCode = hash<double>()(hash<Character>()(mapPair.first.upper) * 56.78);
                ID lowerCode = hash<double>()(hash<Character>()(mapPair.first.lower) * 0.9876);
                code ^= (upperCode + lowerCode);
            }
            return code;
        }

    public:

        DFT() : DFA<Label<Character> >() {
        }

        DFT(const Labels& labels) : DFA<Label<Character> >(labels) {
        }

        DFT(const Characters& chars) : DFA<Label<Character> >(), FT<Character>(chars) {
            mkAlphabet();
        }

        DFT(const Labels& labels, const Characters& chars) : DFA<Label<Character> >(labels), FT<Character>(chars) {
        }

        void mkAlphabet() {
            for (Character upper : this -> symbols) {
                for (Character lower : this -> symbols) {
                    this -> addAlphabet(Label<Character>(upper, lower));
                }
            }
        }

        DFAState<Label<Character> >* mkState() {
            DFTState<Character>* dftState = new DFTState<Character>();
            this -> states.insert(dftState);
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
            this -> finalStates.insert(dftState);
            return dftState;
        }

        DFA<Label<Character> >& minimize() {
            if (this -> isMinimal()) return *this;
            DFT* dft = new DFT(this -> symbols);
            this -> removeDeadState();
            this -> removeUnreachableState();
            if (this -> isNULL()) return FT<Character>::EmptyDFT();
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

        DFT<Character>& operator > (Label<Character> label) const {
            return FT<Character>::leftQuotientFT(*this, label);
        }

        DFT<Character>& operator > (LabelWord& lWord) const {
            return FT<Character>::leftQuotientFT(*this, lWord);
        }

        DFT<Character>& operator > (Character character) const {
            return leftQuotientByUppers(character);
        }

        DFT<Character>& operator > (Word& word) const {
            return leftQuotientByUppers(word);
        }

        DFT<Character>& leftQuotientByUppers(Character character) const  {
            Word uppers;
            uppers.push_back(character);
            return leftQuotientByUppers(uppers);
        }

        DFT<Character>& leftQuotientByUppers(Word& uppers) const {
            DFT<Character>& mdft = minimizeFT();
            DFTStates initialStates;
            mdft.getTargetStatesByUppers(uppers, initialStates);
            if(initialStates.size() == 0) return FT<Character>::EmptyDFT();
            DFAState2NFAStateMap state2Map;
            NFT<Character> nft(mdft.getSymbols());
            DFTState<Character>* dftIniState = mdft.getInitialState();
            state2Map[dftIniState] = nft.mkInitialState();
            nft.cpTransByDFA(dftIniState, state2Map);
            NFAState<Label<Character> >* iniState = nft.mkInitialState();
            for (DFTState<Character>* state : initialStates) {
                iniState -> addEpsilonTrans(state2Map[state]);
            }
            return nft.minimizeFT();
        }

    };
};

#endif /* DFT_hpp */

