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
    class NFT : public NFA<Label<Character> > {
    public:
        typedef typename Alias4FT<Character>::Labels Labels;
        typedef typename Alias4Char<Character>::Word Word;
        typedef typename Alias4Char<Character>::Words Words;

        NFT() : NFA<Label<Character> >() {

        }
        NFT(const Labels& labels) : NFA<Label<Character> >(labels) {
        }

        NFAState<Label<Character> >* mkState() {
            NFTState<Character>* nftState = new NFTState<Character>();
            this -> stateSet.insert(nftState);
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
            this -> finalStateSet.insert(nftState);
            return nftState;
        }

        DFA<Label<Character> >& determinize( void ) {
            if (this -> isNULL()) return FA<Label<Character> >::EmptyDFA();
            DFT<Character>* dft = new DFT<Character>(this -> alphabet);
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
    };
};

#endif /* NFT_hpp */

