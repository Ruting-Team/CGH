//
//  DFAState.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef DFAState_hpp
#define DFAState_hpp

#include "../State.hpp"
namespace cgh{
    
    /// \brief States in Deterministic Finite Automaton.
    ///
    /// Example:
    ///    DFAState<char>* dfaState0 = dfa -> mkState();
    ///    DFAState<char>* dfaState1 = dfa -> mkState();
    ///    dfaState0 -> addTrans('a', dfaState1);
    ///    dfaState0 -> addTrans('b', dfaState0);
    ///    dfaState0 -> delDFATrans('a');
    ///    dfaState0 -> delDFATrans(dfaState1);
    ///    dfaState0 -> delDFATrans('b', dfaState0);
    ///    DFATransMap& dfaTransMap = dfaState0 -> getTransMap();
    ///    DFAStates dfaStates = dfaState0 -> getTargetStates();
    ///    DFAState* targetState = dfaState0 -> getTargetStateByChar('a');
    ///    dfa -> delDFAState(dfaState0);
    template <class Character>
    class DFAState : public State {
    public:
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4FA<Character>::DFAStates DFAStates;
        typedef typename Alias4FA<Character>::DFATransMap DFATransMap;
        
    protected:
        DFATransMap dfaTransMap; ///< A transition map for this state, the key is character and the value is a state.

        void getTargetStates(DFAStates& states) {
            for (auto& mapPair : dfaTransMap)
                states.insert(mapPair.second);
        }

    public:
        /// \brief Gets the reference of transition map for this state.
        /// \return A reference of map. 
        DFATransMap& getTransMap() {return dfaTransMap;}

        /// \brief Gets a const transition map for this state.
        /// \return A const reference of map. 
        const DFATransMap& getTransMap() const {return dfaTransMap;}

        /// \brief Adds a transition which from param character to param target.
        ///
        /// If this state has the a transition with the same label as pram character, then do nothing and return false.
        /// Otherwise add transition and return true;
        /// The target state must be created by the same DFA with this state.
        /// \param character The label in the transition.
        /// \param target The target state in the transition.
        /// \return A boolean representing whether add a transition to a state successfully.
        virtual bool addTrans(Character character, DFAState* target) {
            if (dfaTransMap.count(character) != 0) return false;
            dfaTransMap[character] = target;
            return true;
        }

        /// \brief Deletes a transition which form param character to param target.
        ///
        /// If this state has this transition, then delete it and return true;
        /// Otherwise do nothing and return false;
        /// \param character The label in the transition.
        /// \param target The target state in the transition.
        /// Returns a boolean representing whether the transition is deleted successfully.
        bool delDFATrans(Character character, DFAState* target) {
            auto mapIt = dfaTransMap.find(character);
            if (mapIt != dfaTransMap.end() && mapIt -> second == target) {
                dfaTransMap.erase(mapIt);
                return true;
            }
            return false;
        }

        /// \brief Deletes all transitions target to the param target for this state.
        /// 
        /// If the target state in the target states set of thie state, then delete it and return true;
        /// Otherwise do nothing and return false;
        /// \param target The target state in the transition.
        /// \return A boolean representing whether the target state is deleted successfully.
        bool delDFATrans(DFAState* target) {
            Characters chars;
            for (auto& mapPair : dfaTransMap)
                if (mapPair.second == target)
                    chars.insert(mapPair.first);
            if (chars.size() == 0) return false;
            for (Character character : chars)
                dfaTransMap.erase(character);
            return true;
        }

        /// \brief Deletes the transition which the label is param character for this state.
        /// 
        /// If this character in the keys of transition map, then delete it and return true;
        /// Otherwise do nothing and return false;
        /// \param character The label in the transition.
        /// \return A boolean representing whether delete all transitions with given character successfully.
        bool delDFATrans(Character character) {
            return dfaTransMap.erase(character);
        }

        /// \brief Gets a set of all the target states for this state.
        /// \return A const set of states in DFA.
        const DFAStates getTargetStates() {
            DFAStates states;
            getTargetStates(states);
            return states;
        }

        /// \brief Gets the target states which from the transition that label is param character.
        ///
        /// If this state has no transition with the label param character, then return a nullptr.
        /// \param character The label in a transition, which is a template class.
        /// \return A states in DFA. 
        DFAState* getTargetStateByChar(Character character) {
            auto mapIt = dfaTransMap.find(character);
            if (mapIt != dfaTransMap.end()) return mapIt -> second;
            DFAState* null = nullptr;
            return null;
        }

        void output(){
            for (auto iter = dfaTransMap.begin(); iter != dfaTransMap.end(); iter++) {
                cout<< getID()<<" "<< iter->first <<" "<<iter->second->getID()<<endl;
            }
        }
        friend DFA<Character>;
    };
    
}

#endif /* DFAState_hpp */
