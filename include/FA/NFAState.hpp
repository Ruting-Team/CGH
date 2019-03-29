//
//  NFAState.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/2.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef NFAState_hpp
#define NFAState_hpp

#include "../State.hpp"
namespace cgh{

    /// \brief States in Nondeterministic Finite Automaton.
    ///
    /// Example:
    ///    NFAState<char>* nfaState0 = nfa -> mkNFAState();
    ///    NFAState<char>* nfaState1 = nfa -> mkNFAState();
    ///    nfaState0 -> addTrans('a', nfaState1);
    ///    nfaState0 -> addTrans('b', nfaState0);
    ///    nfaState1 -> addEpsilonTrans(nfaState1);
    ///    nfaState0 -> delNFATrans('a');
    ///    nfaState0 -> delNFATrans('b', nfaState0);
    ///    NFATransMap& nfaTransMap = nfaState0 -> getTransMap();
    ///    NFAStates nfaStates = nfaState0 -> getTargets();
    ///    NFAStates nfaStates = nfaState0 -> getTargets('a');
    ///    NFAStates nfaStates = nfaState1 -> getEpsilonClosure();
    ///    nfa -> delNFAState(nfaState0);
    template <class Character>
    class NFAState : public State {
    public:
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4FA<Character>::NFAStates NFAStates;
        typedef typename Alias4FA<Character>::NFATransMap NFATransMap;
        
    protected:
        NFATransMap nfaTransMap; ///< A transition map for this state, the key is character and the value is a set of states.
        
        void getTargetStates(NFAStates& states) {
            for(auto& mapPair : nfaTransMap)
                getTargetStatesByChar(states, mapPair.first);
        }
        
        void getTargetStatesByChar(NFAStates& states, Character character) {
            if (character == FA<Character>::epsilon) {
                getEpsilonClosure(states);
                return;
            }
            NFAStates epsilonClosure;
            getEpsilonClosure(epsilonClosure);
            epsilonClosure.insert(this);
            for (NFAState* nfaState : epsilonClosure) {
                NFATransMap& transMap = nfaState -> getTransMap();
                auto mapIt = transMap.find(character);
                if (mapIt != transMap.end()) {
                    for(NFAState* state : mapIt -> second) {
                        state -> getEpsilonClosure(states);
                        states.insert(state);
                    }
                }
            }
        }

        void getEpsilonClosure(NFAStates& epsilonClosure) {
            auto mapIt = nfaTransMap.find(FA<Character>::epsilon);
            if (mapIt != nfaTransMap.end()) { 
                NFAStates works;
                for (NFAState* state : mapIt -> second)
                    if (epsilonClosure.insert(state).second) works.insert(state);
                for (NFAState* state : works)
                    state -> getEpsilonClosure(epsilonClosure);
            }
        }

    public:
        /// \brief Gets the reference of transition map for this state.
        ///
        /// responsibility to use a reference to get this map, otherwise it will call copy construction.
        /// This reference map can be used to modify.
        /// \return A map reference. 
        NFATransMap& getTransMap() {return nfaTransMap;}

        /// \brief Gets a const transition map for this state.
        ///
        /// This map can not be used to modify.
        /// \return A const map. 
        const NFATransMap getTransMap() const {return nfaTransMap;}

        /// \brief Adds a transition which label is param character and target state is param target for this state.
        ///
        /// If this state has the same transition, then do nothing and return false.
        /// Otherwise add transition and return true;
        /// The target state must be created by the same NFA with this state.
        /// \param character The label in the transition, which is a template class.
        /// \param target The target state in the transition.
        /// \return A boolean representing whether add a transition to a state successfully.
        virtual bool addTrans(Character character, NFAState* target) {
            return nfaTransMap[character].insert(target).second;
        }

        /// \brief Adds a epsilon transition for this state.
        ///
        /// If this state has the same transition, then do nothing and return false.
        /// Otherwise add transition and return true;
        /// The target state must be created by the same NFA with this state.
        /// \param target The target state in the transition.
        /// \return A boolean representing whether add an epsilon transition to a state successfully.
        bool addEpsilonTrans(NFAState* target) {
            return addTrans(FA<Character>::epsilon, target);
        }
        
        /// \brief Deletes a transition which label is param character and target state is param target for this state.
        ///
        /// If this state has this transition, then delete it and return true;
        /// Otherwise do nothing and return false;
        /// \param character The label in the transition, which is a template class.
        /// \param target The target state in the transition.
        /// Returns a boolean representing whether the transition is deleted successfully.
        bool delNFATrans(Character character, const NFAState* target) {
            auto mapIt = nfaTransMap.find(character);
            if (mapIt == nfaTransMap.end()) {
                return false;
            } else {
                NFAStates& states = mapIt -> second;
                auto sIt = states.find(target);
                if (sIt == states.end()) return false;
                if (states.size() == 1) {
                    nfaTransMap.erase(mapIt);
                } else {
                    states.erase(sIt);
                }
                return true;
            }
        }
        
        /// \brief Deletes all transitions target to the param target for this state.
        /// 
        /// If the target state in the target states set of thie state, then delete it and return true;
        /// Otherwise do nothing and return false;
        /// \param target The target state in the transition.
        /// \return A boolean representing whether the target state is deleted successfully.
        bool delNFATrans(const NFAState* target) {
            int count = 0;
            Characters chars;
            for (auto& mapPair : nfaTransMap) {
                NFAStates& states = mapPair.second;
                auto sIt = states.find(const_cast<NFAState*>(target));
                if (sIt != states.end()) {
                    count++;
                    if (states.size() == 1) {
                        chars.insert(mapPair.first);
                    } else {
                        states.erase(sIt);
                    }
                }
            }
            if (count == 0) return false;
            for(Character character : chars)
                nfaTransMap.erase(character);
            return true;
        }

        /// \brief Deletes all transitions which the label is param character for this state.
        /// 
        /// If this character in the keys of transition map, then delete it and return true;
        /// Otherwise do nothing and return false;
        /// \param character The label in the transition, which is a template class.
        /// \return A boolean representing whether delete all transitions with given character successfully.
        bool delNFATrans(Character character) {
            return nfaTransMap.erase(character);
        }

        /// \brief Gets a set of all the target states for this state.
        /// \return A const set of states in NFA.
        const NFAStates getTargetStates() {
            NFAStates states;
            for (auto& mapPair : nfaTransMap)
                getTargetStatesByChar(states, mapPair.first);
            return states;
        }

        /// \brief Gets a set of all the target states which from the transition that label is param character.
        ///
        /// If this state has no transition with the label param character, then return a empty set.
        /// \param character The label in a transition, which is a template class.
        /// \return A const set of states in NFA.
        const NFAStates getTargetStatesByChar(Character character) {
            NFAStates epsilonClosure;
            getEpsilonClosure(epsilonClosure);
            if (character == FA<Character>::epsilon) return epsilonClosure;
            epsilonClosure.insert(this);
            NFAStates states;
            for (NFAState* nfaState : epsilonClosure) {
                NFATransMap& transMap = nfaState -> getTransMap();
                auto mapIt = transMap.find(character);
                if (mapIt != transMap.end()) {
                    for(NFAState* state : mapIt -> second) {
                        state -> getEpsilonClosure(states);
                        states.insert(state);
                    }
                }
            }
            return states;
        }

        /// \brief Gets a set of all the states reached by epsilon from this state.
        ///
        /// It is a closure.
        /// \return A const set of states in NFA.
        const NFAStates getEpsilonClosure() {
            NFAStates epsilonClosure;
            auto mapIt = nfaTransMap.find(FA<Character>::epsilon);
            if (mapIt == nfaTransMap.end()) return epsilonClosure;
            NFAStates works;
            for (NFAState* state : mapIt -> second)
                if (epsilonClosure.insert(state).second) works.insert(state);
            for (NFAState* state : works)
                state -> getEpsilonClosure(epsilonClosure);
            return epsilonClosure;
        }

        void output() {
            for (auto& mapPair : nfaTransMap)
                for (NFAState* state : mapPair.second)
                    cout << getID() << " " << mapPair.first << " " << state -> getID() << endl;
            }

        friend NFA<Character>;
    };
}


#endif /* NFAState_hpp */
