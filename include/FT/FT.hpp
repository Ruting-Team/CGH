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
        typedef typename Alias4Char<Character>::Words Words;
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4Char<Label<Character> >::Word LabelWord;
        typedef typename Alias4FA<Label<Character> >::DFATransMap DFATransMap;
        typedef typename Alias4FA<Label<Character> >::DFAState2Map DFTState2Map;
        typedef typename Alias4FT<Character>::FTs FTs;
        typedef typename Alias4FT<Character>::DFTs DFTs;
        typedef typename Alias4FT<Character>::DFTState2 DFTState2;
        typedef typename Alias4FT<Character>::Char2DFTStateMap Char2DFTStateMap;
        typedef typename Alias4FT<Character>::DFTStatePairMap DFTStatePairMap;
        typedef typename Alias4FT<Character>::DFT2 DFT2;
        typedef typename Alias4FT<Character>::DFTLabelPair DFTLabelPair;
        typedef typename Alias4FT<Character>::DFTPairMap DFTPairMap;
        typedef typename Alias4FT<Character>::DFTLabel2DFTMap DFTLabel2DFTMap;
        typedef typename Alias4FT<Character>::ID2DFTsMap ID2DFTsMap;
        typedef typename Alias4FT<Character>::DFT2Map DFT2Map;

    protected:
        Characters symbols;       ///< the symbol set which the item in label.

        virtual DFT<Character>& minimizeFT() = 0;
        virtual DFT<Character>& minimizeFT() const = 0;
        virtual ID getCode() = 0;
        virtual void mkAlphabet() = 0;
        //virtual DFT<Character>& determinizeFT() = 0;
        
        static void cpRTrans(DFT<Character>* dft, DFAState<Label<Character> >* state, DFTState2Map& state2Map)
        {
            DFAState<Label<Character> >* sourceState = state2Map[state];
            if (state -> isFinal()) dft -> addFinalState(sourceState);
            for (auto& mapPair : state -> getTransMap()) {
                DFAState<Label<Character> >* targetState = nullptr;
                auto state2MapIt = state2Map.find(mapPair.second);
                if (state2MapIt == state2Map.end()) {
                    targetState = dft -> mkState();
                    state2Map[mapPair.second] = targetState;
                    cpRTrans(dft, mapPair.second, state2Map);
                } else {
                    targetState = state2MapIt -> second;
                }
                Label<Character> label(mapPair.first.getLower(), mapPair.first.getUpper());
                sourceState -> addTrans(label, targetState);
            }
        }
        static void composeFT(NFT<Character>& nft, DFT<Character>& lhsDFT, DFT<Character>& rhsDFT) {
            nft.setSymbols(Manage::unionSet(lhsDFT.getSymbols(), rhsDFT.getSymbols()));
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
                Char2DFTStateMap map;
                rhsDFTState -> getTargetStatesAndLowers(lhsLower, map);
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

        static bool isNewDFT(ID2DFTsMap& codeMap, DFT<Character>*& newDFT) {
            DFTs& dfts = codeMap[newDFT -> getCode()];
            for (auto dft : dfts) {
                if (*dft == *newDFT) {
                    Manage::del(newDFT);
                    newDFT = dft;
                    return false;
                }
            }
            dfts.insert(newDFT);
            return true;
        }

        static void composeDFTs(const DFTs& lhsDFTs, const DFTs& rhsDFTs, DFTs& dftClosure, DFTs& newDFTs, DFTPairMap& compositionMap, ID2DFTsMap& codeMap) {
            for (DFT<Character>* lhs : lhsDFTs) {
                for (DFT<Character>* rhs : rhsDFTs) {
                    if (compositionMap.count(DFT2(lhs, rhs)) > 0)
                        continue;
                    DFT<Character>* dft = &((*lhs) * (*rhs));
                    if (dft -> isEmpty()) {
                        Manage::del(dft);
                        continue;
                    }
                    if (isNewDFT(codeMap, dft)) {
                        dftClosure.insert(dft);
                        newDFTs.insert(dft);
                    }
                    compositionMap[DFT2(lhs, rhs)] = dft;
                }
            }
        }
        static void unionDFTs(const DFTs& lhsDFTs, const DFTs& rhsDFTs, DFTs& dftClosure, DFTs& newDFTs, DFTPairMap& unionMap, ID2DFTsMap& codeMap) {
            for (DFT<Character>* lhs : lhsDFTs) {
                for (DFT<Character>* rhs : rhsDFTs) {
                    if (unionMap.count(DFT2(lhs, rhs)) > 0)
                        continue;
                    DFT<Character>* dft = &((*lhs) | (*rhs));
                    if (isNewDFT(codeMap, dft)) {
                        dft -> output();
                        cout << endl;
                        dftClosure.insert(dft);
                        newDFTs.insert(dft);
                    }
                    unionMap[DFT2(lhs, rhs)] = dft;
                }
            }
        }

        static void mkCodeMap(DFTs& dfts, ID2DFTsMap& codeMap) {
            for (DFT<Character>* dft : dfts) {
                codeMap[dft -> getCode()].insert(dft);
            }
        }
    public:

        /// \brief Default construction without arguments.
        FT() {
        }

        /// \brief Construct FT with symbol set.
        /// \param symbols The symbols.
        FT(const Characters& symbols) :  symbols(symbols.begin(), symbols.end()) {
        }

        /// \brief Sets symbols given a set of Characters.
        ///
        /// Clears current symbols and copy param chars to symbols.
        /// \param chars a const reference set of Character. 
        void setSymbols(const Characters &chars) {
            symbols.clear();
            symbols.insert(chars.begin(),chars.end());
            mkAlphabet();
        }

        /// \brief Sets symbols given a set of Characters.
        ///
        /// Clears current symbols and copy param chars to symbols.
        /// \param chars a const reference ordered_set of Character. 
        void setSymbols(const set<Character> &chars) {
            symbols.clear();
            symbols.insert(chars.begin(),chars.end());
            mkAlphabet();
        }

        /// \brief Gets the symbols.
        /// \return A reference set of Characters.
        Characters& getSymbols() {return symbols;}

        /// \brief Gets the symbols.
        /// \return A const reference set of Characters.
        const Characters& getSymbols() const {return symbols;}
        
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

        /// \brief Gets a FT which is the left quotient by param word of this FT.
        /// \param word A Word.
        /// \return A reference of FT.
        static DFT<Character>& leftQuotientFT(const FT& ft, LabelWord& lWord) {
            DFT<Character>& mdft = ft.minimizeFT();
            DFT<Character>* dft = new DFT<Character>(ft.getSymbols());
            FA<Label<Character> >::leftQuotient(*dft, mdft, lWord);
            return dft -> minimizeFT();
        }

        /// \brief Gets a FT which is the left quotient by param character of this FT.
        /// \param character A Character.
        /// \return A reference of FT.
        static DFT<Character>& leftQuotientFT(const FT& ft, Label<Character> label) {
            LabelWord lWord;
            lWord.push_back(label);
            return leftQuotientFT(ft, lWord);
        }

        /// \brief Gets a FT which is the inverse FT of param ft.
        /// \param ft A FT.
        /// \return A reference of FT.
        static DFT<Character>& inverseFT(const FT& ft) {
            DFT<Character>& mdft = ft.minimizeFT();
            DFT<Character>* dft = new DFT<Character>(ft.getSymbols());
            dft -> setMinimalFlag(1);
            DFTState2Map state2Map;
            DFAState<Label<Character> >* dftIniState = mdft.getInitialState();
            state2Map[dftIniState] = dft -> mkInitialState();
            cpRTrans(dft, dftIniState, state2Map);
            return dft -> minimizeFT();
        }
        
        /// \brief Translates a input param character by param ft.
        /// \param character input.
        /// \return Character.
        static Word translate(const FT& ft, Character character) {
            DFT<Character>& mdft = ft.minimizeFT();
            Word word;
            mdft.translate(character, word);
            return word;
        }

        /// \brief Translates a input param character by param ft.
        /// \param character input.
        /// \return Character.
        static Words translate(const FT& ft, Word& word) {
            DFT<Character>& mdft = ft.minimizeFT();
            Words words;
            mdft.translate(word, words);
            return words;
        }

        /// \brief Makes a empty DFT.
        /// \return reference of DFT.
        static DFT<Character>& EmptyDFT() {
            DFT<Character>* dft = new DFT<Character>();
            Manage::manage(dft);
            dft -> mkInitialState();
            return *dft;
        }

        /// \brief Makes a empty NFT.
        /// \return reference of NFT.
        static NFT<Character>& EmptyNFT() {
            NFT<Character>* nft = new NFT<Character>();
            Manage::manage(nft);
            nft -> mkInitialState();
            return *nft;
        }

        /// \brief Makes a id DFT.
        /// \return reference of DFT.
        static DFT<Character>& IDDFT(const Characters& chars) {
            DFT<Character>* dft = new DFT<Character>();
            Manage::manage(dft);
            DFAState<Label<Character> >* state = dft -> mkInitialState();
            dft -> addFinalState(state);
            for (Character c : chars) {
                Label<Character> l(c, c);
                state -> addTrans(l, state);
            }
            return *dft;
        }

        /// \brief Gets the FT closure in inverse.
        /// \param characters A reference of Characters.
        static void getDFTClosureInInverse(DFTs& dfts, DFTs& dftClosure, DFTs& newWorks, DFT2Map& inversionMap, ID2DFTsMap& codeMap) {
            DFTs works;
            works.insert(dfts.begin(), dfts.end());
            for (DFT<Character>* dft : works) {
                if (inversionMap.count(dft) == 0) {
                    DFT<Character>* rDFT = &inverseFT(*dft);
                    if (isNewDFT(codeMap, rDFT)) {
                        newWorks.insert(rDFT);
                        dftClosure.insert(rDFT);
                    }
                    inversionMap[dft] = rDFT;
                    inversionMap[rDFT] = dft;
                }
            }
        }

        /// \brief Gets the FT closure from param fts in left quotient in param characters.
        /// \param fts A reference of FTs.
        /// \param characters A reference of Characters.
        static void getDFTClosureInLeftQuotient(DFTs& dfts, Characters& characters, DFTs& dftClosure, DFTs& newWorks, DFTLabel2DFTMap& leftQuotientMap, ID2DFTsMap& codeMap) {
            DFTs works;
            DFTs newerWorks;
            works.insert(dfts.begin(), dfts.end());
            while (works.size() > 0) {
                newerWorks.clear();
                for (DFT<Character>* dft : works) {
                    for (Character upper : characters) {
                        for (Character lower : characters) {
                            Label<Character> label(upper, lower);
                            DFT<Character>* newDFT = &(*dft > label);
                            if (newDFT -> isEmpty()) {
                                Manage::del(newDFT);
                                continue;
                            }
                            if (isNewDFT(codeMap, newDFT)) {
                                newerWorks.insert(newDFT);
                                dftClosure.insert(newDFT);
                                newWorks.insert(newDFT);
                            }
                            leftQuotientMap[DFTLabelPair(dft, label)] = newDFT;
                        }
                    }
                }
                works.clear();
                if (newerWorks.size() > 0) {
                    works.insert(newerWorks.begin(), newerWorks.end());
                }
            }
        }

        static void getDFTClosureInComposition(DFTs& dfts, DFTs& dftClosure, DFTPairMap& compositionMap) {
            DFTs works;
            DFTs newWorks;
            DFTs newerWorks;
            works.insert(dfts.begin(), dfts.end());
            composeDFTs(works, works, dftClosure, newWorks, compositionMap);
            while (newWorks.size() > 0) {
                newerWorks.clear();
                composeDFTs(works, newWorks, dftClosure, newerWorks, compositionMap);
                composeDFTs(newWorks, works, dftClosure, newerWorks, compositionMap);
                newWorks.clear();
                if (newerWorks.size() > 0) {
                    works.insert(newerWorks.begin(), newerWorks.end());
                    newWorks.insert(newerWorks.begin(), newerWorks.end());
                }
            }
        }

        /// \brief Gets the FT closure from param fts in left quotient in param characters and composition. 
        /// \param fss A reference of FTs.
        /// \param characters A reference of Characters.
        /// \return FTs.
        static DFTs getDFTClosure(DFTs& dfts, Characters& characters, DFTLabel2DFTMap& leftQuotientMap, DFTPairMap& compositionMap, DFT2Map& inversionMap) {
            DFTs dftClosure;
            ID2DFTsMap codeMap;
            DFTs works;
            DFTs newWorks;
            DFTs newerWorks;
            dftClosure.insert(dfts.begin(), dfts.end());
            mkCodeMap(dftClosure, codeMap);
            getDFTClosureInLeftQuotient(dfts, characters, dftClosure, newWorks, leftQuotientMap, codeMap);
            getDFTClosureInInverse(dftClosure, dftClosure, newWorks, inversionMap, codeMap);
            newWorks.clear();
            works.insert(dftClosure.begin(), dftClosure.end());
            composeDFTs(works, works, dftClosure, newWorks, compositionMap, codeMap);
            do {
                getDFTClosureInLeftQuotient(newWorks, characters, dftClosure, newerWorks, leftQuotientMap, codeMap);
                if (newerWorks.size() > 0) {
                    newWorks.insert(newerWorks.begin(), newerWorks.end());
                }
                getDFTClosureInInverse(newWorks, dftClosure, newerWorks, inversionMap, codeMap);
                if (newerWorks.size() > 0) {
                    newWorks.insert(newerWorks.begin(), newerWorks.end());
                }
                works.insert(newWorks.begin(), newWorks.end());
                newerWorks.clear();
                composeDFTs(works, newWorks, dftClosure, newerWorks, compositionMap, codeMap);
                composeDFTs(newWorks, works, dftClosure, newerWorks, compositionMap, codeMap);
                //unionDFTs(newWorks, works, dftClosure, newerWorks, unionMap, codeMap);
                newWorks.clear();
            } while (newerWorks.size() > 0);
            for (auto dft : dftClosure) {
                dft -> output();
                cout << "code: " << dft -> getCode() << endl;
                cout << endl;
            }
            return dftClosure;
        }

        /// \brief Gets a FT which is the composition of this and param ft.
        /// \param ft A const reference FT.
        /// \return A reference of DFT.
        DFT<Character>& operator * (const FT& ft) {
            return composeFT(*this, ft);
        }

        Word operator [] (Character character) {
            return translate(*this, character);
        }

        Words operator [] (Word word) {
            return translate(*this ,word);
        }
    };
};

#endif /* FT_hpp */

