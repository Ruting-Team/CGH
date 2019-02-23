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
    template <class Character> class NFA;

    /// \brief A class of Deterministic Finite Automaton.
    template <class Character>
    class DFA : public FA<Character>
    {
        typedef FA<Character> FA;
        typedef NFA<Character> NFA;
        typedef Global<Character> Global;
        typedef DFAState<Character> DFAState;

        typedef typename Global::Word Word;
        typedef typename Global::DFAState2 DFAState2;
        typedef typename Global::DFAStateSet DFAStateSet;
        typedef typename Global::DFATransMap DFATransMap;
        typedef typename Global::DFAState2Map DFAState2Map;
        typedef typename Global::CharacterSet CharacterSet;
        typedef typename Global::DFAStateSetMap DFAStateSetMap;
        typedef typename Global::DFAState2DFAStateSetMap DFAState2DFAStateSetMap;
        
    private:
        DFAState* initialState;     ///< The initial state for this DFA.
        DFAStateSet stateSet;       ///< The set of states for this DFA.
        DFAStateSet finalStateSet;  ///< The set of final states for this DFA.

        void cpTrans(DFAState* state, DFAState2Map& state2map) {
            FA::cpDFATransByDFA(this, state, state2map);
        }

        void getReachableStateSet(DFAStateSet& reachableStateSet, DFAStateSet& workSet) {
            if (workSet.size() == 0) return;
            DFAStateSet newWorkSet, newReachableSet;
            for (DFAState* state : workSet) {
                newReachableSet.clear();
                state -> getTargetStateSet(newReachableSet);
                for (DFAState* newState : newReachableSet) {
                    if (reachableStateSet.insert(newState).second)
                        newWorkSet.insert(newState);
                }
            }
            getReachableStateSet(reachableStateSet, newWorkSet);
        }

        void getReverseMap(DFAState2DFAStateSetMap& reverseMap) {
            for (DFAState* state : stateSet) {
                DFATransMap &dfaTransMap = state -> getDFATransMap();
                for (auto& mapPair : dfaTransMap) {
                    reverseMap[mapPair.second].insert(state);
                }
            }
        }
        
        void getLiveStateSet(const DFAState2DFAStateSetMap& reverseMap, DFAStateSet& liveStateSet, DFAStateSet& workSet) {
            if (workSet.size() == 0) return;
            DFAStateSet newWorkSet;
            for (DFAState* state : workSet) {
                auto mapIt = reverseMap.find(state);
                if (mapIt != reverseMap.end()) {
                    for (DFAState* newState : mapIt -> second) {
                        if (liveStateSet.insert(newState).second)
                            newWorkSet.insert(newState);
                    }
                }
            }
            getLiveStateSet(reverseMap, liveStateSet, newWorkSet);
        }

    public:
        /// \brief Default construction function, sets initialState to nullptr.
        DFA() : FA(), initialState(nullptr) {
            this -> setDeterministicFlag(1);
        }

        /// \brief Construction function with alphabet.
        /// \param charSet The alphabet.
        DFA(const CharacterSet& charSet) : FA(charSet), initialState(nullptr) {
            this -> setDeterministicFlag(1);
        }
    
        /// \brief Copy construction function.
        /// \param nfa The copied DFA.
        DFA(const DFA& dfa) {
            if (dfa.initialState) {
                this -> flag = dfa.flag; 
                this -> setAlphabet(dfa.getAlphabet());
                DFAState* iniState = mkInitialState();
                DFAState2Map state2Map;
                state2Map[dfa.initialState] = iniState;
                cpTrans(dfa.initialState, state2Map);
                this -> setDeterministicFlag(1);
            }
        }

        /// \brief Desconstruction function.
        ///
        /// delete all pointers of states for this DFA.
        ~DFA() {
            initialState = NULL; 
            for(DFAState* state : stateSet)
                delete state;
        }

        /// \brief Copies self and return.
        /// \return A reference of FA.
        FA& copy() {
            return *(new DFA(*this));
        }
        
        
        
        /// \brief Sets initialState to param state.
        /// \param state The state set to be initialState.
        void setInitialState(DFAState* state) {
            initialState = state;
        }

        /// \brief Adds param state to finalStateSet.
        /// \param state The state need to be added in finalStateSet.
        void addFinalState(DFAState* state) {
            finalStateSet.insert(state); state -> setFinalFlag(1);
        }

        /// \brief Gets stateSet.
        /// \return The set reference of DFAState pointer for this DFA.
        DFAStateSet& getStateSet() {
            return stateSet;
        }

        /// \brief Gets finalStateSet.
        /// \return The set reference of DFAState pointer for this DFA.
        DFAStateSet& getFinalStateSet() {
            return finalStateSet;
        }

        /// \brief Gets initialState.
        /// \return The DFAstate pointer of initialState for this DFA.
        DFAState* getInitialState() {
            return initialState;
        }

        /// \brief Gets stateSet, a const function.
        /// \return The const set reference of DFAState pointer for this DFA.
        const DFAStateSet& getStateSet() const {
            return stateSet;
        }

        /// \brief Gets finalStateSet, a const function.
        /// \return The const set reference of DFAState pointer for this DFA.
        const DFAStateSet& getFinalStateSet() const {
            return finalStateSet;
        }

        /// \brief Gets initialState, a const function.
        /// \return The const DFAState pointer of initialState for this DFA.
        const DFAState* getInitialState() const {
            return initialState;
        }

        /// \brief Removes all state in the finalStateSet for this DFA.
        void clearFinalStateSet() {
            for (DFAState* state : finalStateSet) {
                state -> setFinalFlag(0);
            }
            finalStateSet.clear();
        }

        /// \brief Checks whether given param stateSet has finalState.
        /// \param stateSet The DFAStateSet for checking.
        /// \return True means param stateSet has finalState, otherwise not.
        static bool hasFinalState(const DFAStateSet& stateSet) {
            for (const DFAState* state : stateSet) {
                if (state -> isFinal()) return true;
            }
            return false;
        }

        /// \brief Checks whether all states in the given param stateSet are finalState.
        /// \param stateSet The DFAStateSet for checking.
        /// \return True means all states are finalState, otherwise not.
        static bool allFinalState(const DFAStateSet& stateSet) {
            for (const DFAState* state : stateSet) {
                if (!state -> isFinal()) return false;
            }
            return true;
        }

        /// \brief Makes a state in this DFA.
        /// \return A DFAState pointer made by this DFA.
        DFAState *mkState() {
            DFAState *dfaState = new DFAState();
            stateSet.insert(dfaState);
            return dfaState;
        }

        /// \brief Makes a initialState in this DFA.
        /// \return A DFAState pointer made by this DFA.
        DFAState *mkInitialState() {
            initialState = mkState();
            return initialState;
        }

        /// \brief Makes a finalState in this NFA.
        /// \return A NFAState pointer made by this NFA.
        DFAState *mkFinalState() {
            DFAState *dfaState = mkState();
            dfaState -> setFinalFlag(1);
            finalStateSet.insert(dfaState);
            return dfaState;
        }

        bool isNULL() const {
            if (!initialState) return true;
            if (finalStateSet.size() == 0) return true;
            return false;
        }
        
        DFA& determinize( void ) {
            return *this;
        }

        const DFA& determinize( void ) const {
            return *this;
        }
        
        NFA& nondeterminize( void ) {
            NFA* nfa = new NFA(*this);
            return *nfa;
        }

        const NFA& nondeterminize( void ) const {
            NFA* nfa = new NFA(*this);
            return *nfa;
        }
        
        static bool isEqual(const DFAState *s1, const DFAState *s2, DFAState2Map &stateMap) {
            const DFATransMap &transMap1 = s1  ->  getDFATransMap();
            const DFATransMap &transMap2 = s2  ->  getDFATransMap();
            if (transMap1.size() != transMap2.size()) return false;
            for (auto& mapPair : transMap1) {
                if (transMap2.count(mapPair.first) == 0) return false;
                else if (stateMap.at(mapPair.second) != stateMap.at(transMap2.at(mapPair.first)))
                    return false;
            }
            return true;
        }

        DFA& minimize() {
            DFA* dfa = new DFA();
            removeDeadState();
            removeUnreachableState();
            if (isNULL()) return *dfa;
            
            int lastSize = 0;
            DFAStateSet unFinalStateSet;
            DFAStateSet finalStatesSet;
            DFAState *unFinalState = dfa -> mkState();
            DFAState *finalState = dfa -> mkState();
            
            DFAState2Map stateMap;
            for (DFAState *state : stateSet) {
                if (state -> isFinal()) {
                    finalStatesSet.insert(state);
                    stateMap[state] = finalState;
                } else {
                    unFinalStateSet.insert(state);
                    stateMap[state] = unFinalState;
                }
            }
            
            queue<DFAStateSet> equiClass;
            if (unFinalStateSet.size() != 0) {
                equiClass.push(unFinalStateSet);
            } else {
                (dfa -> stateSet).erase(unFinalState);
                delete unFinalState;
            }
            if (finalStatesSet.size() != 0) {
                equiClass.push(finalStatesSet);
            } else {
                (dfa -> stateSet).erase(finalState);
                delete finalState;
            }
            size_t curSize = equiClass.size();
            
            while (curSize != lastSize) {
                for (int i = 0; i < curSize; ++i) {
                    DFAStateSet set = equiClass.front();
                    equiClass.pop();
                    if (set.size() == 0) {
                        continue;
                    }
                    
                    auto it = set.begin();
                    DFAState *lastDfaState = stateMap[*it];
                    
                    //对于一个等价类，重新划分等价类
                    while (set.size() != 0) {
                        it = set.begin();
                        auto nextIt = it;
                        ++nextIt;
                        DFAStateSet newEquiClass;
                        newEquiClass.insert(*it);
                        set.erase(it);
                        while (nextIt != set.end()) {
                            if (DFA::isEqual(*it, *nextIt, stateMap)) {
                                DFAState *nextState = *nextIt;
                                newEquiClass.insert(nextState);
                                ++nextIt;
                                set.erase(nextState);
                            }
                            else {
                                ++nextIt;
                            }
                        }
                        equiClass.push(newEquiClass);
                        DFAState *newMapState = dfa -> mkState();
                        for (DFAState *state : newEquiClass) {
                            stateMap[state] = newMapState;
                        }
                    }
                    (dfa -> stateSet).erase(lastDfaState);
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
                DFATransMap &firstTransMap = mapPair.first -> getDFATransMap();
                DFATransMap &secondTransMap = mapPair.second -> getDFATransMap();
                if (secondTransMap.size() == 0) {
                    for (auto& mapPair1 : firstTransMap) {
                        mapPair.second -> addDFATrans(mapPair1.first, stateMap[mapPair1.second]);
                    }
                }
            }
            dfa -> setReachableFlag(1);
            dfa -> setMinimalFlag(1);
            return *dfa;
        }
        
        FA &subset(const DFAState *iState, const DFAState *fState) {
            if (isNULL()) return FA::EmptyDFA();
            DFA *dfa = new DFA(this -> alphabet);
            DFAState* state = dfa -> mkInitialState();
            DFAState2Map state2Map;
            state2Map[const_cast<State*>(iState)] = state;
            dfa -> makeCopyTrans(const_cast<State*>(iState), state2Map);
            dfa -> clearFinalStateSet();
            DFAState* dfaState = state2Map[const_cast<State*>(fState)];
            dfa -> addFinalState(dfaState);
            dfa -> removeDeadState();
            return *dfa;
        }

        FA &rightQuotient(Character character) {
            DFA* dfa = new DFA(*this);
            DFAStateSet finSteteSet;
            for (DFAState* state : dfa -> stateSet) {
                State* targetState = state -> getTargetStateByChar(character);
                if (targetState && targetState -> isFinal())
                    finSteteSet.insert(state);
            }
            dfa -> clearFinalStateSet();
            for (DFAState* state : finSteteSet) {
                dfa -> addFinalState(state);
            }
            return *dfa;
        }
        
        FA& leftQuotient(Character character) {
            DFAState* state = initialState -> getTargetStateByChar(character);
            if(!state) return FA::EmptyDFA();
            DFA* dfa = new DFA(*this);
            dfa -> setInitialState(dfa -> initialState -> getTargetStateByChar(character));
            dfa -> removeUnreachableState();
            return *dfa;
        }
        
        void removeUnreachableState() {
            if (isNULL()) return;
            DFAStateSet reachableStateSet;
            DFAStateSet workSet;
            workSet.insert(initialState);
            reachableStateSet.insert(initialState);
            getReachableStateSet(reachableStateSet, workSet);
            if (!DFA::hasFinalState(reachableStateSet)) {
                initialState = NULL;
                return;
            }
            if (reachableStateSet.size() != this -> stateSet.size()) {
                DFAStateSet delSet;
                for(DFAState* state : stateSet ) {
                    if (reachableStateSet.count(state) == 0) {
                        DFAStateSet targetStateSet = state -> getTargetStateSet();
                        for (DFAState* targetState : targetStateSet) {
                            if (reachableStateSet.count(targetState) > 0)
                                state -> delDFATrans(targetState);
                        }
                        delSet.insert(state);
                    }
                }
                for (DFAState* state : delSet) {
                    stateSet.erase(state);
                    delete state;
                }
            }
            this -> setReachableFlag(1);
        }

         void removeDeadState() {
            if (isNULL()) return;
            DFAState2DFAStateSetMap reverseMap;
            getReverseMap(reverseMap);
            DFAStateSet liveStateSet(finalStateSet.begin(), finalStateSet.end());
            getLiveStateSet(reverseMap, liveStateSet, finalStateSet);
            if (liveStateSet.count(initialState) == 0) { 
                initialState = NULL;
                return;
            }
            DFAStateSet delSet;
            for (DFAState* state : stateSet) {
                if (liveStateSet.count(state) == 0) {
                    DFAStateSet sourceStateSet = reverseMap.find(state) -> second;
                    for (DFAState* sourceState : sourceStateSet) {
                        if (liveStateSet.count(sourceState) > 0)
                            sourceState -> delDFATrans(state);
                    }
                    delSet.insert(state);
                }
            }
            for(DFAState* state : delSet) {
                stateSet.erase(state);
                delete state;
            }
        }
        
        //Word getOneRun();

        bool isAccepted(const Word &word) {
            if (isNULL()) return false;
            DFAState* state = initialState;
            for (Character c : word) {
                state = state -> getTargetStateByChar(c) ;
                if (!state) return false;
            }
            if (state -> isFinal()) return true;
            return false;
        }

        bool isAccepted(Character character) {
            if (isNULL()) return false;
            DFAState* state = initialState;
            state = state -> getTargetStateByChar(character) ;
            if (!state) return false;
            if (state -> isFinal()) return true;
            return false;
        }
        
        bool isEmpty() {
            if (isNULL()) return true;
            if (!this -> isReachable()) removeUnreachableState();
            if (finalStateSet.size() == 0) return true;
            return false;
        }
        
        void output() const
        {
            if(isNULL()) return;
            cout<<initialState -> getID()<<endl;
            for(auto it = stateSet.begin(); it != stateSet.end(); it++)
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
            for (auto iter = finalStateSet.begin(); iter != finalStateSet.end(); iter++) {
                f << "Q" << (*iter) -> getID() << " [shape=doublecircle];\n";
            }
         
            
            // cout trisitions
            for(auto iter = stateSet.begin(); iter != stateSet.end(); iter++)
            {
                DFATransMap& transMap = (*iter) -> getDFATransMap();
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
        
        friend NFA;
    };

    template <class Character>
    class SmartDFA {
    typedef DFA<Character> DFA;
    private:
        DFA* dfa;
        bool del;
        bool confirm; 
    public:
        SmartDFA() : dfa(nullptr), del(0), confirm(0){}
        SmartDFA(const DFA* d, bool b, bool c = 0) : dfa(const_cast<DFA*>(d)), del(b) , confirm(c) {}
        SmartDFA(const SmartDFA& smartDFA) {
            dfa = smartDFA.dfa;
            del = smartDFA.del;
            confirm = 1;
        }
        ~SmartDFA() {
            if (del & confirm) delete dfa;
        }

        DFA* getDFA() {return dfa;}
    };
      
}
#endif /* DFA_hpp */
