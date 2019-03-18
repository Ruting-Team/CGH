//
//  DTDState.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/2.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef DTDState_hpp
#define DTDState_hpp

#include "../State.hpp"
namespace cgh{

    template <class Character>
    class DTDState : public DFAState<Label<Character> > {
        typedef typename Alias4FA<Character>::DFAStateSet DFAStateSet;
        typedef typename Alias4FA<Character>::DFATransMap DFATransMap;
        typedef typename Alias4Char<Character>::CharacterSet CharacterSet;
        typedef typename Alias4TD<Character>::Char2LabelsMap Char2LabelsMap;

    private:
        Char2LabelsMap char2LabelsMap;      ///< the map from Character to Labels.

        //void mkChar2LabelsMap() {
        //    if (char2LabelsMap.size() == 0) {
        //        for (auto& mapPair : this -> nfaTansMap) {
        //            Label<Character>& label = mapPair.first;
        //            char2LabelsMap[label.getUpper()] = label;
        //        }
        //    }
        //}

        void getTargetStateSetAndLowers(Character upper, DFATransMap& map) {
            auto mapIt = char2LabelsMap.find(upper);
            if (mapIt != char2LabelsMap.end()) {
                for (auto& label : mapIt.second) {
                    map[label.getLower()] = this -> getTargetStateByChar(label);
                }
            }
        }

    public:
        /// \brief Gets the lowers and the target stateSet from upper.
        /// \param upper The upper Character in label.
        /// \param staetSet The target stateSet.
        /// \return The set of Character.
        DFATransMap getTargetStateSetAndLowers(Character upper) {
            DFATransMap map;
            auto mapIt = char2LabelsMap.find(upper);
            if (mapIt != char2LabelsMap.end()) {
                for (auto& label : mapIt.second) {
                    map[label.getLower()] = this -> getTargetStateByChar(label);
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
        bool addDTDTrans(Label<Character> label, DTDState* target) {
            char2LabelsMap[label.getUpper()].insert(label);
            return this -> addDFATrans(label, target);
        }

    };

}


#endif /* DTDState_hpp */
