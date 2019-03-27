//
//  NFT.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef NFT_hpp
#define NFT_hpp

#include "../FA/NFA.hpp"
#include "NFTState.hpp"

namespace cgh {
    
    /// \brief A class of Nondeterministic Finite Automaton.
    template <class Character>
    class NFT : public NFA<Label<Character> >, FT<Character> {
    public:
        typedef typename Alias4Char<Character>::Word Word;
        typedef typename Alias4Char<Character>::Words Words;
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4Char<Label<Character> >::Word LabelWord;
        typedef typename Alias4Char<Label<Character> >::Characters Labels;

        NFT() : NFA<Label<Character> >() {

        }

        NFT(const Labels& labels) : NFA<Label<Character> >(labels) {
        }

        NFT(const Characters& chars) : NFA<Label<Character> >(), FT<Character>(chars) {
            mkAlphabet();
        }

        NFT(const Labels& labels, const Characters& chars) : DFA<Label<Character> >(labels), FT<Character>(chars) {
        }

        ID getCode() {
            return 0;
        }

        void mkAlphabet() {
            for (Character upper : this -> symbols) {
                for (Character lower : this -> symbols) {
                    this -> addAlphabet(Label<Character>(upper, lower));
                }
            }
        }


        NFAState<Label<Character> >* mkState() {
            NFTState<Character>* nftState = new NFTState<Character>();
            this -> states.insert(nftState);
            return nftState;
        }

        NFAState<Label<Character> >* mkInitialState() {
            NFAState<Label<Character> >* nftState = mkState();
            this -> initialState = nftState;
            return nftState;
        }

        NFAState<Label<Character> >* mkFinalState() {
            NFAState<Label<Character> >* nftState = mkState();
            nftState -> setFinalFlag(1);
            this -> finalStates.insert(nftState);
            return nftState;
        }

        DFA<Label<Character> >& determinize( void ) {
            if (this -> isNULL()) return FT<Character>::EmptyDFT();
            DFT<Character>* dft = new DFT<Character>(this -> symbols);
            NFA<Label<Character> >::determinize(dft);
            return *dft;
        }

        DFA<Label<Character> >& determinize( void ) const {
            return const_cast<NFT*>(this) -> determinize();
        }

        DFT<Character>& minimizeFT() {
            return ((DFT<Character>&)(determinize())).minimizeFT();
        }

        DFT<Character>& minimizeFT() const{
            return const_cast<NFT*>(this) -> minimizeFT();
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
            return ((DFT<Character>&)(determinize())).leftQuotientByUppers(character);
        }

        DFT<Character>& operator > (Word& word) const {
            return ((DFT<Character>&)(determinize())).leftQuotientByUppers(word);
        }


        friend DFT<Character>;
        friend FT<Character>;
    };
};

#endif /* NFT_hpp */

