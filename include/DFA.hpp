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
    template <class Character>
    class DFA : public FA<Character>
    {
        typedef Global<Character> Global;
        typedef DFAState<Character> DFAState;
        typedef FA<Character> FA;
        typedef NFA<Character> NFA;
        typedef typename Global::Word Word;
        typedef typename Global::CharacterSet CharacterSet;
        typedef typename Global::CharacterSetIter CharacterSetIter;
        typedef typename Global::DFAStateSet DFAStateSet;
        typedef typename Global::DFATransMap DFATransMap;
        typedef typename Global::DFAStateSetIter DFAStateSetIter;
        typedef typename Global::DFATransMapIter DFATransMapIter;
        typedef typename Global::DFAStateSetConstIter DFAStateSetConstIter;
        typedef typename Global::DFATransMapConstIter DFATransMapConstIter;
        
        typedef typename Global::DFAState2 DFAState2;
        typedef typename Global::DFAState2Map DFAState2Map;
        typedef typename Global::DFAState2DFAStateSetMap DFAState2DFAStateSetMap;
        typedef typename Global::Char2DFAState2Map Char2DFAState2Map;
        typedef typename Global::DFAStatePairMap DFAStatePairMap;
        typedef typename Global::DFAStateSetMap DFAStateSetMap;
        
        typedef typename Global::DFAState2MapIter DFAState2MapIter;
        typedef typename Global::DFAStatePairMapIter DFAStatePairMapIter;
        typedef typename Global::DFAStateSetMapIter DFAStateSetMapIter;
        typedef typename Global::Char2DFAState2MapIter Char2DFAState2MapIter;
        typedef typename Global::DFAState2DFAStateSetMapIter DFAState2DFAStateSetMapIter;
        
        typedef typename Global::DFAState2MapConstIter DFAState2MapConstIter;
        typedef typename Global::DFAStatePairMapConstIter DFAStatePairMapConstIter;
        typedef typename Global::DFAStateSetMapConstIter DFAStateSetMapConstIter;
        typedef typename Global::Char2DFAState2MapConstIter Char2DFAState2MapConstIter;
        typedef typename Global::DFAState2DFAStateSetMapConstIter DFAState2DFAStateSetMapConstIter;
        
    private:
        DFAState* initialState;
        DFAStateSet stateSet;
        DFAStateSet finalStateSet;
    private:
        void makeCopyTrans(const DFAState* state, DFAState2Map& state2map)
        {
            const DFATransMap& map = state -> getDFATransMap();
            DFAState* sourceState = state2map.at(const_cast<DFAState*>(state));
            for(DFATransMapConstIter mapIt = map.begin(); mapIt != map.end(); mapIt++)
            {
                DFAState* targetState;
                DFAState2MapIter state2MapIt = state2map.find(mapIt -> second);
                if(state2MapIt == state2map.end())
                {
                    if(mapIt -> second -> isFinal()) targetState = mkDFAFinalState();
                    else targetState = mkDFAState();
                    state2map[mapIt -> second] = targetState;
                    makeCopyTrans(mapIt -> second, state2map);
                }
                else targetState = state2MapIt -> second;
                sourceState -> addDFATrans(mapIt -> first, targetState);
            }
        }
        void getTransMapByStatePair(const DFAState2 &statePair, Char2DFAState2Map& char2DFAState2Map)
        {
            DFATransMap &map1 = statePair.first -> getDFATransMap();
            DFATransMap &map2 = statePair.second -> getDFATransMap();
            for(DFATransMapIter mapIt1 = map1.begin(); mapIt1 != map1.end(); mapIt1++)
            {
                DFATransMapIter mapIt2 = map2.find(mapIt1 -> first);
                if(mapIt2 != map2.end())
                    char2DFAState2Map[mapIt1 -> first] = DFAState2(mapIt1 -> second, mapIt2 -> second);
            }
        }
        
        void makeDFAIntersectionTrans(DFAState *sourceState, DFAStatePairMap &dfaStatePairMap, const Char2DFAState2Map &char2DFAState2Map, DFA *dfa)
        {
            Char2DFAState2Map map;
            for(Char2DFAState2MapConstIter mapIt = char2DFAState2Map.begin(); mapIt!= char2DFAState2Map.end(); mapIt++)
            {
                DFAStatePairMapIter pairMapIt = dfaStatePairMap.find(mapIt -> second);
                DFAState* targetState;
                if(pairMapIt == dfaStatePairMap.end())
                {
                    map.clear();
                    getTransMapByStatePair(mapIt -> second, map);
                    if(mapIt -> second.first -> isFinal() && mapIt -> second.second -> isFinal())
                        targetState = dfa -> mkDFAFinalState();
                    else targetState = dfa -> mkDFAState();
                    dfaStatePairMap[mapIt -> second] = targetState;
                    makeDFAIntersectionTrans(targetState, dfaStatePairMap, map, dfa);
                }
                else targetState = dfaStatePairMap[mapIt -> second];
                sourceState -> addDFATrans(mapIt -> first, targetState);
            }
        }
        void makeDFAComplementTrans(const DFAState *state, DFAState* trapState, DFAState2Map &state2map, DFA *dfa)
        {
            CharacterSet charSet;
            const DFATransMap& map = state -> getDFATransMap();
            DFAState* sourceState = state2map.at(const_cast<DFAState*>(state));
            for(DFATransMapConstIter mapIt = map.begin(); mapIt != map.end(); mapIt++)
            {
                DFAState* targetState;
                DFAState2MapIter state2MapIter = state2map.find(mapIt -> second);
                if(state2MapIter == state2map.end())
                {
                    if(mapIt -> second -> isFinal()) targetState = dfa -> mkDFAState();
                    else targetState = dfa -> mkDFAFinalState();
                    state2map[mapIt -> second] = targetState;
                    makeDFAComplementTrans(mapIt -> second, trapState, state2map, dfa);
                }
                else targetState = state2MapIter -> second;
                sourceState -> addDFATrans(mapIt -> first, targetState);
                charSet.insert(mapIt -> first);
            }
            for(CharacterSetIter it = this -> alphabet.begin(); it != this -> alphabet.end(); it++)
                if(charSet.find(*it) == charSet.end())
                    sourceState -> addDFATrans(*it, trapState);
        }
        void getReachableStateSet(DFAStateSet& reachableStateSet, DFAStateSet& workSet)
        {
            if(workSet.size() == 0) return;
            DFAStateSet tempSet;
            DFAStateSet set;
            for(DFAStateSetIter it = workSet.begin(); it != workSet.end(); it++)
            {
                tempSet.clear();
                (*it) -> getTargetStateSet(tempSet);
                for(DFAStateSetIter sIt = tempSet.begin(); sIt != tempSet.end(); sIt++)
                    if(reachableStateSet.insert(*sIt).second)
                        set.insert(*sIt);
            }
            getReachableStateSet(reachableStateSet, set);
        }
        void getReverseMap(DFAState2DFAStateSetMap& reverseMap)
        {
            for(DFAStateSetIter it = stateSet.begin(); it != stateSet.end(); it++)
                reverseMap[*it] = DFAStateSet();
            for(DFAStateSetIter it = stateSet.begin(); it != stateSet.end(); it++)
            {
                DFATransMap &dfaTransMap = (*it) -> getDFATransMap();
                for(DFATransMapIter mapIt = dfaTransMap.begin(); mapIt != dfaTransMap.end(); mapIt++)
                    reverseMap[mapIt -> second].insert(*it);
            }
        }
        
        void getLiveStateSet(const DFAState2DFAStateSetMap& reverseMap, DFAStateSet& liveStateSet, DFAStateSet& workSet)
        {
            if(workSet.size() == 0) return;
            DFAStateSet set;
            for(DFAStateSetIter it = workSet.begin(); it != workSet.end(); it++)
            {
                DFAState2DFAStateSetMapConstIter mapConstIt = reverseMap.find(*it);
                if(mapConstIt != reverseMap.end())
                    for(DFAStateSetConstIter it = mapConstIt -> second.begin(); it != mapConstIt -> second.end(); it++)
                        if(liveStateSet.insert(*it).second)
                            set.insert(*it);
            }
            getLiveStateSet(reverseMap, liveStateSet, set);
        }
    public:
        
        DFA() : FA(), initialState(NULL) {this -> setDeterminateFlag(1);}
    
        DFA(const DFA& dfa)
        {
            if(dfa.initialState)
            {
                this -> flag = dfa.flag; 
                this -> setAlphabet(dfa.getAlphabet());
                DFAState* iniState = mkDFAInitialState();
                if(dfa.initialState -> isFinal())
                    addFinalState(iniState);
                DFAState2Map state2Map;
                state2Map[dfa.initialState] = iniState;
                makeCopyTrans(dfa.initialState, state2Map);
                this -> setDeterminateFlag(1);
            }
        }
        ~DFA()
        {
            initialState = NULL; 
            for(DFAState* state : stateSet)
                delete state;
        }
        
        DFAState *mkDFAState()
        {
            DFAState *dfaState = new DFAState();
            stateSet.insert(dfaState);
            return dfaState;
        }
        DFAState *mkDFAInitialState()
        {
            initialState = mkDFAState();
            return initialState;
        }
        DFAState *mkDFAFinalState()
        {
            DFAState *dfaState = mkDFAState();
            dfaState -> setFinalFlag(1);
            finalStateSet.insert(dfaState);
            return dfaState;
        }
        
        void setInitialState(DFAState* state) {initialState = state;}
        void addFinalState(DFAState* state) {finalStateSet.insert(state); state -> setFinalFlag(1);}
        DFAStateSet& getStateSet() {return stateSet;}
        DFAStateSet& getFinalStateSet() {return finalStateSet;}
        DFAState* getInitialState() {return initialState;}
        const DFAStateSet& getStateSet() const {return stateSet;}
        const DFAStateSet& getFinalStateSet() const {return finalStateSet;}
        const DFAState* getInitialState() const {return initialState;}
        void clearFinalStateSet()
        {
            for(DFAState* state : finalStateSet)
                state -> setFinalFlag(0);
            finalStateSet.clear();
        }
        static bool hasFinalState(const DFAStateSet& stateSet)
        {
            for(const DFAState* state : stateSet)
                if(state -> isFinal()) return true;
            return false;
        }
        static bool allFinalState(const DFAStateSet& stateSet)
        {
            for(const DFAState* state : stateSet)
                if(!state -> isFinal()) return false;
            return true;
        }
        bool isNULL() const {if(!initialState) return true; return false;}
        FA &operator &(const FA &fa)
        {
            DFA* dfa = new DFA(); 
            if(isNULL() || fa.isNULL()) return *dfa;
            DFA &tempDFA = const_cast<FA&>(fa).determine();
            dfa -> setAlphabet(this -> alphabet);
            DFAState2 statePair(initialState, tempDFA.initialState);
            DFAState* iniState = dfa -> mkDFAInitialState();
            if(initialState -> isFinal() && tempDFA.initialState -> isFinal()) dfa -> addFinalState(iniState);
            DFAStatePairMap pairMapping;
            pairMapping[statePair] = iniState;
            Char2DFAState2Map char2DFAState2Map;
            getTransMapByStatePair(statePair, char2DFAState2Map);
            makeDFAIntersectionTrans(iniState, pairMapping, char2DFAState2Map, dfa);
            if(!fa.isDeterminate()) delete &tempDFA;
            if(dfa -> finalStateSet.size() == 0)
            {
                delete dfa;
                return FA::EmptyDFA();
            }
            dfa -> setReachableFlag(1);
            return *dfa;
        }
        FA &operator |(const FA &fa)
        {
            if(isNULL()) return const_cast<FA&>(fa);
            if(fa.isNULL()) return *this;
            NFA nfa(*this);
            return (nfa | fa);
        }
        DFA &operator !( void )
        {
            if(isNULL()) return FA::CompleteFA(this -> getAlphabet());
            DFA *dfa = new DFA();
            dfa -> setAlphabet(this -> getAlphabet());
            DFAState2Map state2Map;
            DFAState* iniState = dfa -> mkDFAInitialState();
            DFAState* trapState = dfa -> mkDFAFinalState();
            for(CharacterSetIter it = this -> alphabet.begin(); it != this -> alphabet.end(); it++)
                trapState -> addDFATrans(*it, trapState);
            if(!initialState -> isFinal()) iniState -> setFinalFlag(1);
            state2Map[initialState] = iniState;
            makeDFAComplementTrans(initialState, trapState, state2Map, dfa);
            return *dfa;
        }
        
        DFA &determine( void )
        {
            return *this;
        }
        
        NFA &nondetermine( void )
        {
            NFA* nfa = new NFA(*this);
            return *nfa;
        }
        
        static bool isEqual(const DFAState *s1, const DFAState *s2, DFAState2Map &stateMap)
        {
            const DFATransMap &transMap1 = s1  ->  getDFATransMap();
            const DFATransMap &transMap2 = s2  ->  getDFATransMap();
            if (transMap1.size() != transMap2.size()) return false;
            for (DFATransMapConstIter it = transMap1.begin(); it != transMap1.end(); ++it)
            {
                if (transMap2.count(it  ->  first) == 0) return false;
                else if (stateMap.at(it  ->  second) != stateMap.at(transMap2.at(it  ->  first)))
                    return false;
            }
            return true;
        }
        DFA& minimize()
        {
            DFA* dfa = new DFA();
            removeDeadState();
            removeUnreachableState();
            if (isNULL()) return *dfa;
            
            int lastSize = 0;
            DFAStateSet unFinalStateSet;
            DFAStateSet finalStatesSet;
            DFAState *unFinalState = dfa -> mkDFAState();
            DFAState *finalState = dfa -> mkDFAState();
            
            DFAState2Map stateMap;
            for (DFAState *s : stateSet) {
                if (s -> isFinal()) {
                    finalStatesSet.insert(s);
                    stateMap[s] = finalState;
                } else {
                    unFinalStateSet.insert(s);
                    stateMap[s] = unFinalState;
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
                    
                    DFAStateSetIter it = set.begin();
                    DFAState *lastDfaState = stateMap[*it];
                    
                    //对于一个等价类，重新划分等价类
                    while (set.size() != 0) {
                        it = set.begin();
                        DFAStateSetIter nextIt = it;
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
                        DFAState *newMapState = dfa -> mkDFAState();
                        for (DFAState *s : newEquiClass) {
                            stateMap[s] = newMapState;
                        }
                    }
                    (dfa -> stateSet).erase(lastDfaState);
                    delete lastDfaState;
                }
                lastSize = curSize;
                curSize = equiClass.size();
            }
            //构造新自动机
            for (DFAState2MapIter it = stateMap.begin(); it != stateMap.end(); ++it) {
                if ((it -> first) == initialState) {
                    dfa -> setInitialState(it -> second);
                }
                if ((it -> first) -> isFinal()) {
                    dfa -> addFinalState(it -> second);
                }
                DFATransMap &firstTransMap = (it -> first) -> getDFATransMap();
                DFATransMap &secondTransMap = (it -> second) -> getDFATransMap();
                if (secondTransMap.size() == 0) {
                    for (DFATransMapIter it1 = firstTransMap.begin(); it1 != firstTransMap.end(); ++it1) {
                        (it -> second) -> addDFATrans(it1 -> first, (stateMap[it1 -> second]));
                    }
                }
            }
            dfa -> setReachableFlag(1);
            return *dfa;
        }
        
        FA &concat(const FA &fa)//concatination
        {
            if(isNULL()) return const_cast<FA&>(fa);
            if(fa.isNULL()) return *this;
            NFA nfa(*this);
            return nfa.concat(fa);
        }
        FA &subset(const DFAState *iState, const DFAState *fState)
        {
            DFA *dfa = new DFA();
            if(isNULL()) return *dfa;
            dfa -> setAlphabet(this -> alphabet);
            DFAState* state = dfa -> mkDFAInitialState();
            DFAState2Map state2Map;
            state2Map[const_cast<State*>(iState)] = state;
            dfa -> makeCopyTrans(const_cast<State*>(iState), state2Map);
            dfa -> clearFinalStateSet();
            DFAState* dfaState = state2Map[const_cast<State*>(fState)];
            dfa -> addFinalState(dfaState);
            dfa -> removeDeadState();
            return *dfa;
        }
        FA &rightQuotient(Character character)
        {
            DFA* dfa = new DFA(*this);
            DFAStateSet finSteteSet;
            for(DFAStateSetIter it = dfa -> stateSet.begin(); it != dfa -> stateSet.end(); it++)
            {
                State* state = (*it) -> getTargetStateByChar(character);
                if(state && state -> isFinal())
                    finSteteSet.insert(*it);
            }
            dfa -> clearFinalStateSet();
            for(DFAStateSetIter it = finSteteSet.begin(); it != finSteteSet.end(); it++)
                dfa -> addFinalState(*it);
            return *dfa;
        }
        
        FA& leftQuotient(Character character)
        {
            DFAState* state = initialState -> getTargetStateByChar(character);
            if(!state) return FA::EmptyDFA();
            DFA* dfa = new DFA(*this);
            dfa -> setInitialState(dfa -> initialState -> getTargetStateByChar(character));
            dfa -> removeUnreachableState();
            return *dfa;
        }
        
        void removeUnreachableState()
        {
            if(isNULL()) return;
            DFAStateSet reachableStateSet;
            DFAStateSet workSet;
            workSet.insert(initialState);
            reachableStateSet.insert(initialState);
            getReachableStateSet(reachableStateSet, workSet);
            if(!DFA::hasFinalState(reachableStateSet))
            {
                initialState = NULL;
                return;
            }
            if(reachableStateSet.size() != this -> stateSet.size())
            {
                DFAStateSet set;
                for(DFAStateSetIter it = stateSet.begin(); it != stateSet.end(); it++)
                    if(reachableStateSet.find(*it) == reachableStateSet.end())
                    {
                        DFAStateSet targetStateSet = (*it) -> getTargetStateSet();
                        for(DFAStateSetIter sIt = targetStateSet.begin(); sIt != targetStateSet.end(); sIt++)
                            if(reachableStateSet.find(*sIt) != reachableStateSet.end())
                                (*it) -> delDFATrans(*sIt);
                        set.insert(*it);
                    }
                for(DFAStateSetIter it = set.begin(); it != set.end(); it++)
                {
                    stateSet.erase(*it);
                    delete *it;
                }
            }
            this -> setReachableFlag(1);
        }
         void removeDeadState()
        {
            if(isNULL()) return;
            DFAState2DFAStateSetMap reverseMap;
            getReverseMap(reverseMap);
            DFAStateSet liveStateSet(finalStateSet.begin(), finalStateSet.end());
            getLiveStateSet(reverseMap, liveStateSet, finalStateSet);
            if(liveStateSet.find(initialState) == liveStateSet.end())
            {
                initialState = NULL;
                return;
            }
            DFAStateSet set;
            for(DFAStateSetIter it = stateSet.begin(); it != stateSet.end(); it++)
                if(liveStateSet.find(*it) == liveStateSet.end())
                {
                    DFAStateSet sourceStateSet = reverseMap.find(*it) -> second;
                    for(DFAStateSetIter sIt = sourceStateSet.begin(); sIt != sourceStateSet.end(); sIt++)
                        if(liveStateSet.find(*sIt) != liveStateSet.end())
                            (*sIt) -> delDFATrans(*it);
                    set.insert(*it);
                }
            for(DFAStateSetIter it = set.begin(); it != set.end(); it++)
            {
                stateSet.erase(*it);
                delete *it;
            }
        }
        
         Word getOneRun();
        bool isAccepted(const Word &word)//accepted
        {
            if(isNULL()) return false;
            DFAState* state = initialState;
            for(int i = 0; i < word.size(); i++)
            {
                state = state -> getTargetStateByChar(word[i]) ;
                if(!state) return false;
            }
            if(state -> isFinal()) return true;
            return false;
        }
        bool isAccepted(Character character)
        {
            if(isNULL()) return false;
            DFAState* state = initialState;
            state = state -> getTargetStateByChar(character) ;
            if(!state) return false;
            if(state -> isFinal()) return true;
            return false;
        }
        
        bool isEmpty()
        {
            if(isNULL()) return true;
            if(!this -> isReachable()) removeUnreachableState();
            if(finalStateSet.size() == 0) return true;
            return false;
        }
        
        void output() const
        {
            if(isNULL()) return;
            cout<<initialState -> getID()<<endl;
            for(DFAStateSetConstIter it = stateSet.begin(); it != stateSet.end(); it++)
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
            for (DFAStateSetConstIter iter = finalStateSet.begin(); iter != finalStateSet.end(); iter++) {
                f << "Q" << (*iter) -> getID() << " [shape=doublecircle];\n";
            }
         
            
            // cout trisitions
            for(DFAStateSetConstIter iter = stateSet.begin(); iter != stateSet.end(); iter++)
            {
                DFATransMap& transMap = (*iter) -> getDFATransMap();
                ID id = (*iter) -> getID();
                for (DFATransMapIter iter = transMap.begin(); iter != transMap.end(); iter++)
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
}
#endif /* DFA_hpp */
