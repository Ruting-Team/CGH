//
//  DFTState.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/2.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef DFTState_hpp
#define DFTState_hpp

#include "../State.hpp"
namespace cgh{

    template <class Character>
    class DFTState : public DFAState<Label<Character> > {
        typedef typename Alias4FA<Character>::DFAStates DFAStates;
        typedef typename Alias4FT<Character>::DFTStates DFTStates;
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4Char<Character>::Word Word;
        typedef typename Alias4Char<Character>::Words Words;
        typedef typename Alias4FT<Character>::Char2LabelsMap Char2LabelsMap;
        typedef typename Alias4FT<Character>::Char2DFTStateMap Char2DFTStateMap;
        typedef typename Alias4FT<Character>::DFTState2WordsMap DFTState2WordsMap;

    private:
        Char2LabelsMap upperMap;      ///< the map from upper to Labels.
        Char2LabelsMap lowerMap;      ///< the map from lower to Labels.

        void translate(Character upper, Words& words,  DFTState2WordsMap& map) {
            auto mapIt = upperMap.find(upper);
            if (mapIt != upperMap.end()) {
                for (auto& label : mapIt -> second) {
                    Character lower = label.getLower();
                    DFTState* state = (DFTState*)(this -> getTargetStateByChar(label));
                    if (state) {
                        for (Word word : words) {
                            word.push_back(lower);
                            map[state].push_back(word);
                        }
                    }
                }
            }
        }

        void getTargetStatesAndLowers(Character upper, Char2DFTStateMap& map) {
            auto mapIt = upperMap.find(upper);
            if (mapIt != upperMap.end()) {
                for (auto& label : mapIt -> second) {
                    map[label.getLower()] = (DFTState*)this -> getTargetStateByChar(label);
                }
            }
        }

        void getTargetStatesByUpper(Character upper, DFTStates& states) {
            auto mapIt = upperMap.find(upper);
            if (mapIt != upperMap.end()) {
                for (auto& label : mapIt -> second) {
                    states.insert((DFTState*)(this -> getTargetStateByChar(label)));
                }
            }
        }

        void getTargetStatesByLower(Character lower, DFTStates& states) {
            auto mapIt = lowerMap.find(lower);
            if (mapIt != lowerMap.end()) {
                for (auto& label : mapIt -> second) {
                    states.insert((DFTState*)(this -> getTargetStateByChar(label)));
                }
            }
        }

    public:
        /// \brief Gets the lowers and the target states from upper.
        /// \param upper The upper Character in label.
        /// \param staets The target states.
        /// \return The set of Character.
        Char2DFTStateMap getTargetStatesAndLowers(Character upper) {
            Char2DFTStateMap map;
            auto mapIt = upperMap.find(upper);
            if (mapIt != upperMap.end()) {
                for (auto& label : mapIt.second) {
                    map[label.getLower()] = this -> getTargetStateByChar(label);
                }
            }
            return map;
        }

        Char2DFTStateMap getTargetStatesAndUppers(Character lower) {
            Char2DFTStateMap map;
            auto mapIt = lowerMap.find(lower);
            if (mapIt != lowerMap.end()) {
                for (auto& label : mapIt.second) {
                    map[label.getUpper()] = this -> getTargetStateByChar(label);
                }
            }
            return map;
        }

        /// \brief Adds a transition which label is param character and target state is param target for this state.
        ///
        /// If this state has the a transition with the same label as pram character, then do nothing and return false.
        /// Otherwise add transition and return true;
        /// The target state must be created by the same DFA with this state.
        /// \param character The label in the transition, which is a template class.
        /// \param target The target state in the transition.
        /// \return A boolean representing whether add a transition to a state successfully.
        bool addTrans(Label<Character> label, DFAState<Label<Character> >* target) {
            upperMap[label.getUpper()].insert(label);
            lowerMap[label.getLower()].insert(label);
            return DFAState<Label<Character> >::addTrans(label, target);
        }

        friend FT<Character>;
        friend DFT<Character>;
    };
}


#endif /* DFTState_hpp */
