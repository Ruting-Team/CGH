//
//  TrNFAState.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef TrNFAState_hpp
#define TrNFAState_hpp

#include "../State.hpp"
namespace cgh {

    template<class Character>
    class TrNFAState : public State {
        typedef typename Alias4Char<Character>::Word Word;
        typedef typename Alias4Char<Label<Character> >::Characters Labels;
        typedef typename Alias4FT<Character>::DFT2 DFT2;
        typedef typename Alias4TrNFA<Character>::TrNFATransMap TrNFATransMap;
        typedef typename Alias4TrNFA<Character>::TrNFAStates TrNFAStates;
        typedef typename Alias4TrNFA<Character>::DFT2TrNFAStatesMap DFT2TrNFAStatesMap;
        typedef typename Alias4FT<Character>::DFTLabelPair DFTLabelPair;
        typedef typename Alias4FT<Character>::DFTPairMap DFTPairMap;
        typedef typename Alias4FT<Character>::DFTLabel2DFTMap DFTLabel2DFTMap;

    private: 
        TrNFATransMap trnfaTransMap;

        void getTargetStatesAndTranslate(Word& word, TrNFAStates& targetStates, Word& targetWord) {

        }

    public:
        void getTargetMap(TrNFATransMap& targetMap, DFTPairMap& compositionMap, DFTLabel2DFTMap& leftQuotientMap, DFT<Character>* tid) {
            DFT2TrNFAStatesMap epsilonClosure;
            getEpsilonClosure(epsilonClosure, compositionMap, tid);
            epsilonClosure[tid].insert(this);
            for (auto& epsMapPair : epsilonClosure) {
                DFT<Character>* lhsDFT = epsMapPair.first;
                TrNFAStates& epsilonStates = epsMapPair.second;
                Labels labels = lhsDFT -> getInitialLabels();
                for (Label<Character> label : labels) {
                    Character upper = label.getUpper();
                    Character lower = label.getLower();
                    DFT<Character>* qtDFT = leftQuotientMap[DFTLabelPair(lhsDFT, label)];
                    for (TrNFAState<Character>* epsState : epsilonStates) {
                        auto mapIt = epsState -> trnfaTransMap.find(lower);
                        if (mapIt != trnfaTransMap.end()) { 
                            auto& map = mapIt -> second;
                            for (auto& mapPair : map) {
                                DFT<Character>* rhsDFT = mapPair.first;
                                DFT<Character>* dft = compositionMap[DFT2(qtDFT, rhsDFT)];
                                if (!dft) continue;
                                TrNFAStates& states = mapPair.second;
                                TrNFAStates& targetStates = targetMap[upper][dft];
                                for (TrNFAState<Character>* state : states) {
                                    targetMap[upper][dft].insert(state);
                                    //if (targetStates.insert(state).second) {
                                    //    state -> getEpsilonClosure(targetMap[upper], compositionMap, dft);
                                    //}
                                }
                            }
                        }
                    }
                }
            }
        }
        void getDFT2TrNFAStatesMapByChar(DFT2TrNFAStatesMap& targetMap, Character character, DFTPairMap& compositionMap, DFTLabel2DFTMap& leftQuotientMap, DFT<Character>* dftid) {
            if (character == FA<Character>::epsilon) {
                getEpsilonClosure(targetMap, compositionMap, dftid);
            } else {
                DFT2TrNFAStatesMap epsilonClosure;
                getEpsilonClosure(epsilonClosure, compositionMap, dftid);
                epsilonClosure[dftid].insert(this);
                for (auto& epsMapPair : epsilonClosure) {
                    DFT<Character>* lhsDFT = epsMapPair.first;
                    TrNFAStates& epsilonStates = epsMapPair.second;
                    Word word = (*lhsDFT)[character];
                    for (Character c : word) {
                        Label<Character> label(character, c);
                        DFT<Character>* qtDFT = leftQuotientMap[DFTLabelPair(lhsDFT, label)];
                        for (TrNFAState<Character>* epsState : epsilonStates) {
                            auto mapIt = epsState -> trnfaTransMap.find(c);
                            if (mapIt != trnfaTransMap.end()) { 
                                auto& map = mapIt -> second;
                                for (auto& mapPair : map) {
                                    DFT<Character>* rhsDFT = mapPair.first;
                                    DFT<Character>* dft = compositionMap[DFT2(qtDFT, rhsDFT)];
                                    if (!dft) continue;
                                    TrNFAStates& states = mapPair.second;
                                    TrNFAStates& targetStates = targetMap[dft];
                                    for (TrNFAState<Character>* state : states) {
                                        if (targetStates.insert(state).second) {
                                            state -> getEpsilonClosure(targetMap, compositionMap, dft);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        void getEpsilonClosure(DFT2TrNFAStatesMap& epsilonClosure, DFTPairMap& compositionMap, DFT<Character>* lhsDFT) {
            auto mapIt = trnfaTransMap.find(FA<Character>::epsilon);
            if (mapIt != trnfaTransMap.end()) { 
                auto& map = mapIt -> second;
                for (auto& mapPair : map) {
                    DFT<Character>* rhsDFT = mapPair.first;
                    DFT<Character>* dft = compositionMap[DFT2(lhsDFT, rhsDFT)];
                    TrNFAStates& states = mapPair.second;
                    if (dft) {
                        TrNFAStates& epsilonStates = epsilonClosure[dft];
                        for (TrNFAState<Character>* state : states) {
                            if (epsilonStates.insert(state).second) {
                                state -> getEpsilonClosure(epsilonClosure, compositionMap, dft);
                            }
                        }
                    }
                }
            }
        }
    public:
        /// \brief Gets the reference of transition map for this state.
        ///
        /// This reference map can be used to modify.
        /// \return A map reference. 
        TrNFATransMap& getTransMap() {return trnfaTransMap;}

        /// \brief Gets a const transition map for this state.
        ///
        /// This map can not be used to modify.
        /// \return A const map. 
        const TrNFATransMap& getTransMap() const {return trnfaTransMap;}

        /// \brief Adds a transition which label is pair of param character and param dft, and target state is param target for this state.
        ///
        /// If this state has the same transition, then do nothing and return false.
        /// Otherwise add transition and return true;
        /// The target state must be created by the same TrNFA with this state.
        /// \param character The character of label in the transition, which is a template class.
        /// \param dft The DFT of label in the transition.
        /// \param target The target state in the transition.
        /// \return A boolean representing whether add a transition to a state successfully.
        bool addTrans(Character character, DFT<Character>* dft, TrNFAState* target) {
            auto& states = trnfaTransMap[character][dft];
            return states.insert(target).second;
        }

        void output() {
            for (auto& mapPair : trnfaTransMap) {
                for (auto& mapPair1 : mapPair.second) {
                    for (auto state : mapPair1.second) {
                        cout << getID() << " " << mapPair.first << " " << mapPair1.first << " " << state -> getID() << endl;
                    }
                }
            }
        }
    };
};

#endif /* TrNFAState_hpp */
