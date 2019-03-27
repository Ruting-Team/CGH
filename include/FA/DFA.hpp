//
//  DFA.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef DFA_hpp
#define DFA_hpp

#include "FA.hpp"
#include "DFAState.hpp"

namespace cgh {

    /// \brief A class of Deterministic Finite Automaton.
    template <class Character>
    class DFA : public FA<Character>
    {
        typedef typename Alias4Char<Character>::Word Word;
        typedef typename Alias4Char<Character>::Characters Characters;

        typedef typename Alias4FA<Character>::DFAState2 DFAState2;
        typedef typename Alias4FA<Character>::DFAStates DFAStates;
        typedef typename Alias4FA<Character>::DFATransMap DFATransMap;
        typedef typename Alias4FA<Character>::DFAState2Map DFAState2Map;
        typedef typename Alias4FA<Character>::DFAState2DFAStatesMap DFAState2DFAStatesMap;
        
    protected:
        DFAState<Character>* initialState;     ///< The initial state for this DFA.
        DFAStates states;       ///< The set of states for this DFA.
        DFAStates finalStates;  ///< The set of final states for this DFA.

        void cpTrans(DFAState<Character>* state, DFAState2Map& state2map) {
            FA<Character>::cpDFATransByDFA(this, state, state2map);
        }

        void getReachableStates(DFAStates& reachableStates, DFAStates& works) {
            if (works.size() == 0) return;
            DFAStates newWorks, newReachables;
            for (DFAState<Character>* state : works) {
                newReachables.clear();
                state -> getTargetStates(newReachables);
                for (DFAState<Character>* newState : newReachables) {
                    if (reachableStates.insert(newState).second)
                        newWorks.insert(newState);
                }
            }
            getReachableStates(reachableStates, newWorks);
        }

        void getReverseMap(DFAState2DFAStatesMap& reverseMap) {
            for (DFAState<Character>* state : states) {
                DFATransMap &dfaTransMap = state -> getTransMap();
                for (auto& mapPair : dfaTransMap) {
                    reverseMap[mapPair.second].insert(state);
                }
            }
        }
        
        void getLiveStates(const DFAState2DFAStatesMap& reverseMap, DFAStates& liveStates, DFAStates& works) {
            if (works.size() == 0) return;
            DFAStates newWorks;
            for (DFAState<Character>* state : works) {
                auto mapIt = reverseMap.find(state);
                if (mapIt != reverseMap.end()) {
                    for (DFAState<Character>* newState : mapIt -> second) {
                        if (liveStates.insert(newState).second)
                            newWorks.insert(newState);
                    }
                }
            }
            getLiveStates(reverseMap, liveStates, newWorks);
        }

        DFAState<Character>* getTargetStateByWord(Word& word) {
            DFAState<Character>* state = getInitialState();
            for (Character& c : word) {
                if (state) {
                    state = state -> getTargetStateByChar(c);
                } else {
                    break;
                }
            }
            return state;
        }

        bool isEqual(const DFAState<Character>* s1, const DFAState<Character>* s2, DFAState2Map &stateMap) {
            const DFATransMap &transMap1 = s1  ->  getTransMap();
            const DFATransMap &transMap2 = s2  ->  getTransMap();
            if (transMap1.size() != transMap2.size()) return false;
            for (auto& mapPair : transMap1) {
                if (transMap2.count(mapPair.first) == 0) return false;
                else if (stateMap.at(mapPair.second) != stateMap.at(transMap2.at(mapPair.first)))
                    return false;
            }
            return true;
        }

        void minimize(DFA* dfa) {
            ID lastSize = 0;
            DFAStates unFinalStates;
            DFAStates finalStatess;
            DFAState<Character>* unFinalState = dfa -> mkState();
            DFAState<Character>* finalState = dfa -> mkState();
            
            DFAState2Map stateMap;
            for (DFAState<Character>* state : states) {
                if (state -> isFinal()) {
                    finalStatess.insert(state);
                    stateMap[state] = finalState;
                } else {
                    unFinalStates.insert(state);
                    stateMap[state] = unFinalState;
                }
            }
            
            queue<DFAStates> equiClass;
            if (unFinalStates.size() != 0) {
                equiClass.push(unFinalStates);
            } else {
                (dfa -> states).erase(unFinalState);
                delete unFinalState;
            }
            if (finalStatess.size() != 0) {
                equiClass.push(finalStatess);
            } else {
                (dfa -> states).erase(finalState);
                delete finalState;
            }
            ID curSize = equiClass.size();
            
            while (curSize != lastSize) {
                for (ID i = 0; i < curSize; ++i) {
                    DFAStates set = equiClass.front();
                    equiClass.pop();
                    if (set.size() == 0) {
                        continue;
                    }
                    
                    auto it = set.begin();
                    DFAState<Character>* lastDfaState = stateMap[*it];
                    
                    //对于一个等价类，重新划分等价类
                    while (set.size() != 0) {
                        it = set.begin();
                        auto nextIt = it;
                        ++nextIt;
                        DFAStates newEquiClass;
                        newEquiClass.insert(*it);
                        set.erase(it);
                        while (nextIt != set.end()) {
                            if (DFA::isEqual(*it, *nextIt, stateMap)) {
                                DFAState<Character>* nextState = *nextIt;
                                newEquiClass.insert(nextState);
                                ++nextIt;
                                set.erase(nextState);
                            }
                            else {
                                ++nextIt;
                            }
                        }
                        equiClass.push(newEquiClass);
                        DFAState<Character>* newMapState = dfa -> mkState();
                        for (DFAState<Character>* state : newEquiClass) {
                            stateMap[state] = newMapState;
                        }
                    }
                    (dfa -> states).erase(lastDfaState);
                    delete lastDfaState;
                }
                lastSize = curSize;
                curSize = equiClass.size();
            }
            //构造新自动机
            for (auto& mapPair : stateMap) {
                if (mapPair.first == initialState) {
                    dfa -> setInitialState(mapPair.second);
                }
                if (mapPair.first -> isFinal()) {
                    dfa -> addFinalState(mapPair.second);
                }
                DFATransMap &firstTransMap = mapPair.first -> getTransMap();
                DFATransMap &secondTransMap = mapPair.second -> getTransMap();
                if (secondTransMap.size() == 0) {
                    for (auto& mapPair1 : firstTransMap) {
                        mapPair.second -> addTrans(mapPair1.first, stateMap[mapPair1.second]);
                    }
                }
            }
            dfa -> setReachableFlag(1);
            dfa -> setMinimalFlag(1);
            Manage::manage(dfa);
        }

    public:
        /// \brief Default construction function, sets initialState to nullptr.
        DFA() : FA<Character>(), initialState(nullptr) {
            this -> setDeterministicFlag(1);
        }

        /// \brief Construction function with alphabet.
        /// \param chars The alphabet.
        DFA(const Characters& chars) : FA<Character>(chars), initialState(nullptr) {
            this -> setDeterministicFlag(1);
        }
    
        /// \brief Copy construction function.
        /// \param nfa The copied DFA.
        DFA(const DFA& dfa) {
            if (dfa.initialState) {
                this -> flag = dfa.flag; 
                this -> setAlphabet(dfa.getAlphabet());
                DFAState2Map state2Map;
                state2Map[dfa.initialState] = mkInitialState();
                cpTrans(dfa.initialState, state2Map);
                this -> setReachableFlag(1);
            }
        }

        /// \brief Desconstruction function.
        ///
        /// delete all pointers of states for this DFA.
        ~DFA() {
            initialState = NULL; 
            for(DFAState<Character>* state : states)
                delete state;
        }

        /// \brief Copies self and return.
        /// \return A reference of FA.
        FA<Character>& copy() {
            return *(new DFA(*this));
        }
        
        /// \brief Sets initialState to param state.
        /// \param state The state set to be initialState.
        void setInitialState(DFAState<Character>* state) {
            initialState = state;
        }

        /// \brief Adds param state to finalStates.
        /// \param state The state need to be added in finalStates.
        void addFinalState(DFAState<Character>* state) {
            finalStates.insert(state); state -> setFinalFlag(1);
        }

        /// \brief Gets states.
        /// \return The set reference of DFAState pointer for this DFA.
        DFAStates& getStates() {
            return states;
        }

        /// \brief Gets finalStates.
        /// \return The set reference of DFAState pointer for this DFA.
        DFAStates& getFinalStates() {
            return finalStates;
        }

        /// \brief Gets initialState.
        /// \return The DFAstate pointer of initialState for this DFA.
        DFAState<Character>* getInitialState() {
            return initialState;
        }

        /// \brief Gets states, a const function.
        /// \return The const set reference of DFAState pointer for this DFA.
        const DFAStates& getStates() const {
            return states;
        }

        /// \brief Gets finalStates, a const function.
        /// \return The const set reference of DFAState pointer for this DFA.
        const DFAStates& getFinalStates() const {
            return finalStates;
        }

        /// \brief Gets initialState, a const function.
        /// \return The const DFAState pointer of initialState for this DFA.
        const DFAState<Character>* getInitialState() const {
            return initialState;
        }

        /// \brief Removes all state in the finalStates for this DFA.
        void clearFinalStates() {
            for (DFAState<Character>* state : finalStates) {
                state -> setFinalFlag(0);
            }
            finalStates.clear();
        }

        /// \brief Checks whether given param states has finalState.
        /// \param states The DFAStates for checking.
        /// \return True means param states has finalState, otherwise not.
        static bool hasFinalState(const DFAStates& states) {
            for (const DFAState<Character>* state : states) {
                if (state -> isFinal()) return true;
            }
            return false;
        }

        /// \brief Checks whether all states in the given param states are finalState.
        /// \param states The DFAStates for checking.
        /// \return True means all states are finalState, otherwise not.
        static bool allFinalState(const DFAStates& states) {
            for (const DFAState<Character>* state : states) {
                if (!state -> isFinal()) return false;
            }
            return true;
        }

        /// \brief Makes a state in this DFA.
        /// \return A DFAState pointer made by this DFA.
        virtual DFAState<Character>* mkState() {
            DFAState<Character>* dfaState = new DFAState<Character>();
            states.insert(dfaState);
            return dfaState;
        }

        /// \brief Makes a initialState in this DFA.
        /// \return A DFAState pointer made by this DFA.
        virtual DFAState<Character>* mkInitialState() {
            initialState = mkState();
            return initialState;
        }

        /// \brief Makes a finalState in this NFA.
        /// \return A NFAState pointer made by this NFA.
        virtual DFAState<Character>* mkFinalState() {
            DFAState<Character>* dfaState = mkState();
            dfaState -> setFinalFlag(1);
            finalStates.insert(dfaState);
            return dfaState;
        }

        bool isNULL() {
            if (!this -> isReachable()) removeUnreachableState();
            if (initialState -> getTransMap().size() == 0) return true;
            if (finalStates.size() == 0) return true;
            return false;
        }

        bool isNULL() const {
            return const_cast<DFA*>(this) -> isNULL();
        }
        
        DFA& determinize( void ) {
            return *this;
        }

        DFA& determinize( void ) const {
            return const_cast<DFA&>(*this);
        }
        
        virtual DFA& minimize(void) {
            if (this -> isMinimal()) return *this;
            DFA* dfa = new DFA(this -> alphabet);
            removeDeadState();
            removeUnreachableState();
            if (isNULL()) return FA<Character>::EmptyDFA();
            minimize(dfa);
            return *dfa;
        }

        virtual DFA& minimize( void ) const {
            return const_cast<DFA*>(this) -> minimize();
        }
        
        FA<Character>& subset(const DFAState<Character>* iState, const DFAState<Character>* fState) {
            if (isNULL()) return FA<Character>::EmptyDFA();
            DFA* dfa = new DFA(this -> alphabet);
            DFAState<Character>* state = dfa -> mkInitialState();
            DFAState2Map state2Map;
            state2Map[const_cast<State*>(iState)] = state;
            dfa -> makeCopyTrans(const_cast<State*>(iState), state2Map);
            dfa -> clearFinalStates();
            DFAState<Character>* dfaState = state2Map[const_cast<State*>(fState)];
            dfa -> addFinalState(dfaState);
            dfa -> removeDeadState();
            return *dfa;
        }
        
        void removeUnreachableState() {
            if (finalStates.size() == 0) return;
            if (this -> isReachable()) return;
            DFAStates reachableStates;
            DFAStates works;
            works.insert(initialState);
            reachableStates.insert(initialState);
            getReachableStates(reachableStates, works);
            if (!DFA::hasFinalState(reachableStates)) {
                clearFinalStates();
                return;
            }
            if (reachableStates.size() != this -> states.size()) {
                DFAStates dels;
                for(DFAState<Character>* state : states ) {
                    if (reachableStates.count(state) == 0) {
                        DFAStates targetStates = state -> getTargetStates();
                        for (DFAState<Character>* targetState : targetStates) {
                            if (reachableStates.count(targetState) > 0)
                                state -> delDFATrans(targetState);
                        }
                        dels.insert(state);
                    }
                }
                for (DFAState<Character>* state : dels) {
                    states.erase(state);
                    delete state;
                }
            }
            this -> setReachableFlag(1);
        }

         void removeDeadState() {
            if (isNULL()) return;
            DFAState2DFAStatesMap reverseMap;
            getReverseMap(reverseMap);
            DFAStates liveStates(finalStates.begin(), finalStates.end());
            getLiveStates(reverseMap, liveStates, finalStates);
            if (liveStates.count(initialState) == 0) { 
                clearFinalStates();
                return;
            }
            DFAStates dels;
            for (DFAState<Character>* state : states) {
                if (liveStates.count(state) == 0) {
                    DFAStates sourceStates = reverseMap.find(state) -> second;
                    for (DFAState<Character>* sourceState : sourceStates) {
                        if (liveStates.count(sourceState) > 0)
                            sourceState -> delDFATrans(state);
                    }
                    dels.insert(state);
                }
            }
            for(DFAState<Character>* state : dels) {
                states.erase(state);
                delete state;
            }
        }
        
        //Word getOneRun();

        bool isAccepted(const Word &word) {
            if (isNULL()) return false;
            DFAState<Character>* state = initialState;
            for (Character c : word) {
                state = state -> getTargetStateByChar(c) ;
                if (!state) return false;
            }
            if (state -> isFinal()) return true;
            return false;
        }

        bool isAccepted(Character character) {
            if (isNULL()) return false;
            DFAState<Character>* state = initialState;
            state = state -> getTargetStateByChar(character) ;
            if (!state) return false;
            if (state -> isFinal()) return true;
            return false;
        }
        
        bool isEmpty() {
            if (isNULL()) return true;
            if (!this -> isReachable()) removeUnreachableState();
            if (finalStates.size() == 0) return true;
            return false;
        }
        
        void output() const
        {
            if(isNULL()) return;
            cout<<initialState -> getID()<<endl;
            for(auto it = states.begin(); it != states.end(); it++)
            {
                if((*it) -> isFinal()) cout<<"$"<<(*it) -> getID()<<endl;;
                (*it) -> output();
            }
        }
        void print(string filename) const
        {
            if(isNULL()) return;
            ofstream f;
            f.open(filename);
            f << "digraph {\n";
            f << "rankdir=LR;\n";
            
            
            // cout initial
            f << "Q"<<initialState -> getID() << "[color=blue];\n";

            // cout final states
            for (auto iter = finalStates.begin(); iter != finalStates.end(); iter++) {
                f << "Q" << (*iter) -> getID() << " [shape=doublecircle];\n";
            }
         
            
            // cout trisitions
            for(auto iter = states.begin(); iter != states.end(); iter++)
            {
                DFATransMap& transMap = (*iter) -> getTransMap();
                ID id = (*iter) -> getID();
                for (auto iter = transMap.begin(); iter != transMap.end(); iter++)
                {
                    f << "Q" << id <<  "  ->  " << "Q" << (iter -> second) -> getID() << "[label=\"" << iter -> first <<"\"];\n";
                }
            }
            f <<"}\n";
            f.close();
            system("dot -Tpng -o res.png res.dot");
        }
        
        friend NFA<Character>;
        friend FA<Character>;
    };
}
#endif /* DFA_hpp */
