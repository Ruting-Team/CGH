//
//  FT.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef FT_hpp
#define FT_hpp

#include "../FA/FA.hpp"

namespace cgh {
    
    /// \brief A class of Finite Transducer.
    template <class Character>
    class FT {
        typedef typename Alias4Char<Character>::Word Word;
        typedef typename Alias4FA<Label<Character> >::DFATransMap DFATransMap;
        typedef typename Alias4FT<Character>::DFTTransMap DFTTransMap;
        typedef typename Alias4FT<Character>::DFTState2 DFTState2;
        typedef typename Alias4FT<Character>::DFTStatePairMap DFTStatePairMap;

    protected:
        virtual DFT<Character>& minimizeFT() = 0;
        virtual DFT<Character>& minimizeFT() const = 0;
        //virtual DFT<Character>& determinizeFT() = 0;
        
        static void composeFT(NFT<Character>& nft, DFT<Character>& lhsDFT, DFT<Character>& rhsDFT) {
            DFTStatePairMap pairMap;
            composeFT(&nft, (NFTState<Character>*)(nft.mkInitialState()), DFTState2(lhsDFT.getInitialState(), rhsDFT.getInitialState()), pairMap);
        }
        static void composeFT(NFT<Character>* nft, NFTState<Character>* sourceState, const DFTState2& statePair, DFTStatePairMap& dftStatePairMap) {
            DFTState<Character>* lhsDFTState = statePair.first;
            DFTState<Character>* rhsDFTState = statePair.second;
            if (lhsDFTState -> isFinal() && rhsDFTState -> isFinal())
                nft -> addFinalState(sourceState);
            dftStatePairMap[statePair] = sourceState;
            DFATransMap& lhsTransMap = lhsDFTState -> getTransMap();
            for (auto& lhsPair : lhsTransMap) {
                Label<Character> lhsLabel = lhsPair.first;
                DFTState<Character>* newLhsDFTState = (DFTState<Character>*)(lhsPair.second);
                Character lhsUpper = lhsLabel.getUpper();
                Character lhsLower = lhsLabel.getLower();
                DFTTransMap map;
                rhsDFTState -> getTargetStateSetAndLowers(lhsLower, map);
                for (auto& rhsPair: map) {
                    Character rhsLower = rhsPair.first;
                    DFTState<Character>* newRhsDFTState = (DFTState<Character>*)(rhsPair.second);
                    NFTState<Character>* targetState = nullptr;
                    DFTState2 newStatePair(newLhsDFTState, newRhsDFTState);
                    auto pairMapIt = dftStatePairMap.find(newStatePair);
                    if (pairMapIt == dftStatePairMap.end()) {
                        targetState = (NFTState<Character>*)(nft -> mkState());
                        composeFT(nft, targetState, newStatePair, dftStatePairMap);
                    } else {
                        targetState = pairMapIt -> second;
                    }
                    Label<Character> label(lhsUpper,rhsLower);
                    sourceState -> addTrans(label, targetState);
                    nft -> addAlphabet(label);
                }
            }
        }
    public:
        /// \brief Gets a FT which is the intersection of param lhsfa and param rhsfa.
        ///
        /// A static function.
        /// \param lhsfa A const reference FT.
        /// \param rhsfa A const reference FT.
        /// \return A reference of FT.
        static DFT<Character>& intersectFT(const FT& lhsFT, const FT& rhsFT) {
            DFT<Character>& lhsDFT = lhsFT.minimizeFT();
            DFT<Character>& rhsDFT = rhsFT.minimizeFT();
            DFT<Character> dft;
            FA<Label<Character> >::intersectFA(dft, lhsDFT, rhsDFT);
            return dft.minimizeFT();
        }

        /// \brief Gets a FT which is the union of param lhsFT and param rhsFT.
        ///
        /// A static function.
        /// \param lhsFT A const reference FT.
        /// \param rhsFT A const reference FT.
        /// \return A reference of FT.
        static DFT<Character>& unionFT(const FT& lhsFT, const FT& rhsFT) {
            DFT<Character>& lhsDFT = lhsFT.minimizeFT();
            DFT<Character>& rhsDFT = rhsFT.minimizeFT();
            NFT<Character> nft;
            FA<Label<Character>>::unionFA(nft, lhsDFT, rhsDFT);
            return nft.minimizeFT();
        }

        /// \brief Gets a FT which is the concatenation of param lhsFT and param rhsFT.
        ///
        /// A static function.
        /// \param lhsFT A const reference FT.
        /// \param rhsFT A const reference FT.
        /// \return A reference of FT.
        static DFT<Character>& concatenateFT(const FT& lhsFT, const FT& rhsFT) {
            DFT<Character>& lhsDFT = lhsFT.minimizeFT();
            DFT<Character>& rhsDFT = rhsFT.minimizeFT();
            NFT<Character> nft;
            FA<Label<Character> >::concatenateFA(nft, lhsDFT, rhsDFT);
            return nft.minimizeFT();
        }

        /// \brief Gets a FT which is the composition of param lhsft and param rhsft.
        ///
        /// A static function.
        /// \param lhsft A const reference FT.
        /// \param rhsft A const reference FT.
        /// \return A reference of FT.
        static DFT<Character>& composeFT(const FT& lhsFT, const FT& rhsFT) {
            DFT<Character>& lhsDFT = lhsFT.minimizeFT();
            DFT<Character>& rhsDFT = rhsFT.minimizeFT();
            NFT<Character> nft;
            composeFT(nft, lhsDFT, rhsDFT);
            return nft.minimizeFT();
        }

        DFT<Character>& operator * (const FT& ft) {
            return composeFT(*this, ft);
        }
        //virtual DFT<Character>& leftQuotient(Character character) = 0;
        //virtual DFT<Character>& leftQuotient(Word& word) = 0;
        //virtual DFT<Character>& rightQuotient(Character character) = 0;
        //virtual DFT<Character>& rightQuotient(Word& word) = 0;

    };
};

#endif /* FT_hpp */

