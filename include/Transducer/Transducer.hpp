//
//  Transducer.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef Transducer_hpp
#define Transducer_hpp

#include "../FA/FA.hpp"

namespace cgh {
    
    /// \brief A class of Nondeterministic Finite Automaton.
    template <class Character>
    class Transducer {
        typedef typename Alias4Char<Character>::Word Word;
        typedef typename Alias4FA<Label<Character> >::DFATransMap DFATransMap;
        typedef typename Alias4TD<Character>::DTDTransMap DTDTransMap;
        typedef typename Alias4TD<Character>::DTDState2 DTDState2;
        typedef typename Alias4TD<Character>::DTDStatePairMap DTDStatePairMap;

    protected:
        virtual DTD<Character>& minimizeTD() = 0;
        virtual DTD<Character>& minimizeTD() const = 0;
        //virtual DTD<Character>& determinizeTD() = 0;
        
        static void composeTD(NTD<Character>& ntd, DTD<Character>& lhsDTD, DTD<Character>& rhsDTD) {
            //dfa.setAlphabet(intersectSet(lhsDFA.getAlphabet(), rhsDFA.getAlphabet()));
            DTDStatePairMap pairMap;
            composeTD(&ntd, (NTDState<Character>*)(ntd.mkInitialState()), DTDState2(lhsDTD.getInitialState(), rhsDTD.getInitialState()), pairMap);
        }
        static void composeTD(NTD<Character>* ntd, NTDState<Character>* sourceState, const DTDState2& statePair, DTDStatePairMap& dtdStatePairMap) {
            DTDState<Character>* lhsDTDState = statePair.first;
            DTDState<Character>* rhsDTDState = statePair.second;
            if (lhsDTDState -> isFinal() && rhsDTDState -> isFinal())
                ntd -> addFinalState(sourceState);
            dtdStatePairMap[statePair] = sourceState;
            DFATransMap& lhsTransMap = lhsDTDState -> getTransMap();
            for (auto& lhsPair : lhsTransMap) {
                Label<Character> lhsLabel = lhsPair.first;
                DTDState<Character>* newLhsDTDState = (DTDState<Character>*)(lhsPair.second);
                Character lhsUpper = lhsLabel.getUpper();
                Character lhsLower = lhsLabel.getLower();
                DTDTransMap map;
                rhsDTDState -> getTargetStateSetAndLowers(lhsLower, map);
                for (auto& rhsPair: map) {
                    Character rhsLower = rhsPair.first;
                    DTDState<Character>* newRhsDTDState = (DTDState<Character>*)(rhsPair.second);
                    NTDState<Character>* targetState = nullptr;
                    DTDState2 newStatePair(newLhsDTDState, newRhsDTDState);
                    auto pairMapIt = dtdStatePairMap.find(newStatePair);
                    if (pairMapIt == dtdStatePairMap.end()) {
                        targetState = (NTDState<Character>*)(ntd -> mkState());
                        composeTD(ntd, targetState, newStatePair, dtdStatePairMap);
                    } else {
                        targetState = pairMapIt -> second;
                    }
                    sourceState -> addTrans(Label<Character>(lhsUpper, rhsLower), targetState);
                }
            }
        }
    public:
        /// \brief Gets a Transducer which is the intersection of param lhsfa and param rhsfa.
        ///
        /// A static function.
        /// \param lhsfa A const reference Transducer.
        /// \param rhsfa A const reference Transducer.
        /// \return A reference of Transducer.
        static DTD<Character>& intersectTD(const Transducer& lhsTD, const Transducer& rhsTD) {
            DTD<Character>& lhsDTD = lhsTD.minimizeTD();
            DTD<Character>& rhsDTD = rhsTD.minimizeTD();
            DTD<Character> dtd;
            FA<Label<Character> >::intersectFA(dtd, lhsDTD, rhsDTD);
            return dtd.minimizeTD();
        }

        /// \brief Gets a Transducer which is the union of param lhsTD and param rhsTD.
        ///
        /// A static function.
        /// \param lhsTD A const reference Transducer.
        /// \param rhsTD A const reference Transducer.
        /// \return A reference of Transducer.
        static DTD<Character>& unionTD(const Transducer& lhsTD, const Transducer& rhsTD) {
            DTD<Character>& lhsDTD = lhsTD.minimizeTD();
            DTD<Character>& rhsDTD = rhsTD.minimizeTD();
            NTD<Character> ntd;
            FA<Label<Character>>::unionFA(ntd, lhsDTD, rhsDTD);
            return ntd.minimizeTD();
        }

        /// \brief Gets a Transducer which is the concatenation of param lhsTD and param rhsTD.
        ///
        /// A static function.
        /// \param lhsTD A const reference Transducer.
        /// \param rhsTD A const reference Transducer.
        /// \return A reference of Transducer.
        static DTD<Character>& concatenateTD(const Transducer& lhsTD, const Transducer& rhsTD) {
            DTD<Character>& lhsDTD = lhsTD.minimizeTD();
            DTD<Character>& rhsDTD = rhsTD.minimizeTD();
            NTD<Character> ntd;
            FA<Label<Character> >::concatenateFA(ntd, lhsDTD, rhsDTD);
            return ntd.minimizeTD();
        }

        /// \brief Gets a Transducer which is the composition of param lhstd and param rhstd.
        ///
        /// A static function.
        /// \param lhstd A const reference Transducer.
        /// \param rhstd A const reference Transducer.
        /// \return A reference of Transducer.
        static DTD<Character>& composeTD(Transducer& lhsTD, Transducer& rhsTD) {
            DTD<Character>& lhsDTD = lhsTD.minimizeTD();
            DTD<Character>& rhsDTD = rhsTD.minimizeTD();
            NTD<Character> ntd;
            composeTD(ntd, lhsDTD, rhsDTD);
            return ntd.minimizeTD();
        }

        Transducer& operator * (const Transducer& td) {
            return composition(*this, td);
        }
        //virtual Transducer& leftQuotientByChar(Character character) = 0;
        //virtual Transducer& leftQuotientByWord(Word& word) = 0;
        //virtual Transducer& rightQuotientByChar(Character character) = 0;
        //virtual Transducer& rightQuotientByWord(Word& word) = 0;

    };
};

#endif /* Transducer_hpp */

