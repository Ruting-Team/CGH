//
//  NFTState.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/2.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef NFTState_hpp
#define NFTState_hpp

#include "../State.hpp"
#include "Label.hpp"
namespace cgh{

    template <class Character>
    class NFTState : public NFAState<Label<Character> > {
        typedef typename Alias4FA<Character>::NFAStates NFAStates;
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4FT<Character>::Char2LabelsMap Char2LabelsMap;

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

        void getTargetStatesAndLowers(Character upper, Characters& lowers, NFAStates& states) {
            auto mapIt = upperMap.find(upper);
            if (mapIt != upperMap.end()) {
                for (auto& label : mapIt.second) {
                    lowers.insert(label.getLower());
                    NFAStates targetStates;
                    this -> getTargetStates(label, targetStates);
                    states.insert(targetStates);
                }
            }
        }

    public:
        /// \brief Gets the lowers and the target states from upper.
        /// \param upper The upper Character in label.
        /// \param staets The target states.
        /// \return The set of Character.
        Characters getTargetStatesAndLowers(Character upper, NFAStates& states) {
            Characters lowers;
            auto mapIt = upperMap.find(upper);
            if (mapIt != upperMap.end()) {
                for (auto& label : mapIt.second) {
                    lowers.insert(label.getLower());
                    NFAStates targetStates;
                    this -> getTargetStates(label, targetStates);
                    states.insert(targetStates);
                }
            }
            return lowers;
        }

        /// \brief Adds a transition which label is param character and target state is param target for this state.
        ///
        /// If this state has the same transition, then do nothing and return false.
        /// Otherwise add transition and return true;
        /// The target state must be created by the same NFA with this state.
        /// \param character The label in the transition, which is a template class.
        /// \param target The target state in the transition.
        /// \return A boolean representing whether add a transition to a state successfully.
        bool addTrans(Label<Character> label, NFAState<Label<Character> >* target) {
            upperMap[label.getUpper()].insert(label);
            lowerMap[label.getLower()].insert(label);
            return NFAState<Label<Character> >::addTrans(label, target);
        }
    };

}


#endif /* NFTState_hpp */
