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
        typedef typename Alias4FA<Character>::DFAStateSet DFAStateSet;
        typedef typename Alias4FT<Character>::DFTStateSet DFTStateSet;
        typedef typename Alias4Char<Character>::CharacterSet CharacterSet;
        typedef typename Alias4FT<Character>::Char2LabelsMap Char2LabelsMap;
        typedef typename Alias4FT<Character>::DFTTransMap DFTTransMap;

    private:
        Char2LabelsMap upperMap;      ///< the map from upper to Labels.
        Char2LabelsMap lowerMap;      ///< the map from lower to Labels.

        //void mkChar2LabelsMap() {
        //    if (char2LabelsMap.size() == 0) {
        //        for (auto& mapPair : this -> nfaTansMap) {
        //            Label<Character>& label = mapPair.first;
        //            char2LabelsMap[label.getUpper()] = label;
        //        }
        //    }
        //}

        void getTargetStateSetAndLowers(Character upper, DFTTransMap& map) {
            auto mapIt = upperMap.find(upper);
            if (mapIt != upperMap.end()) {
                for (auto& label : mapIt -> second) {
                    map[label.getLower()] = (DFTState*)(this -> getTargetStateByChar(label));
                }
            }
        }

        void getTargetStateSetByUpper(Character upper, DFTStateSet& stateSet) {
            auto mapIt = upperMap.find(upper);
            if (mapIt != upperMap.end()) {
                for (auto& label : mapIt -> second) {
                    stateSet.insert((DFTState*)(this -> getTargetStateByChar(label)));
                }
            }
        }

        void getTargetStateSetAndUppers(Character upper, DFTTransMap& map) {
            auto mapIt = upperMap.find(upper);
            if (mapIt != upperMap.end()) {
                for (auto& label : mapIt -> second) {
                    map[label.getLower()] = this -> getTargetStateByChar(label);
                }
            }
        }

        void getTargetStateSetByLower(Character lower, DFTStateSet& stateSet) {
            auto mapIt = lowerMap.find(lower);
            if (mapIt != lowerMap.end()) {
                for (auto& label : mapIt -> second) {
                    stateSet.insert((DFTState*)(this -> getTargetStateByChar(label)));
                }
            }
        }

    public:
        /// \brief Gets the lowers and the target stateSet from upper.
        /// \param upper The upper Character in label.
        /// \param staetSet The target stateSet.
        /// \return The set of Character.
        DFTTransMap getTargetStateSetAndLowers(Character upper) {
            DFTTransMap map;
            auto mapIt = upperMap.find(upper);
            if (mapIt != upperMap.end()) {
                for (auto& label : mapIt.second) {
                    map[label.getLower()] = this -> getTargetStateByChar(label);
                }
            }
            return map;
        }

        DFTTransMap getTargetStateSetAndUppers(Character lower) {
            DFTTransMap map;
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

    };

}


#endif /* DFTState_hpp */
