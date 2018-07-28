//
//  NFA.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef NFA_hpp
#define NFA_hpp

#include "FA.hpp"
#include "NFAState.hpp"
#include "PDS.hpp"

namespace cgh {
    template <class Character>
    class NFA : public FA<Character>
    {
        typedef Global<Character> Global;
        typedef NFAState<Character> NFAState;
        typedef DFAState<Character> DFAState;
        typedef FA<Character> FA;
        typedef DFA<Character> DFA;
        typedef PDS<Character> PDS;
        typedef typename Global::Word Word;
        typedef typename Global::Char2 Char2;
        typedef typename Global::CharacterSet CharacterSet;
        typedef typename Global::CharacterSetIter CharacterSetIter;
        typedef typename Global::NFAStateSet NFAStateSet;
        typedef typename Global::DFAStateSet DFAStateSet;
        typedef typename Global::NFATransMap NFATransMap;
        typedef typename Global::DFATransMap DFATransMap;
        typedef typename Global::NFAMapIter NFAMapIter;
        typedef typename Global::NFAStateSetIter NFAStateSetIter;
        typedef typename Global::NFATransMapIter NFATransMapIter;
        typedef typename Global::DFATransMapIter DFATransMapIter;
        typedef typename Global::NFAStateSetConstIter NFAStateSetConstIter;
        typedef typename Global::NFATransMapConstIter NFATransMapConstIter;
        typedef typename Global::DFATransMapConstIter DFATransMapConstIter;
        
        typedef typename Global::NFAState2NFAStateSetMap NFAState2NFAStateSetMap;
        typedef typename Global::NFAState2Map NFAState2Map;
        typedef typename Global::DFAState2NFAStateMap DFAState2NFAStateMap;
        typedef typename Global::NFAState2DFAStateMap NFAState2DFAStateMap;
        typedef typename Global::PDSState2NFAStateMap PDSState2NFAStateMap;
        typedef typename Global::NFAStateSet2DFAStateMap NFAStateSet2DFAStateMap;
        
        typedef typename Global::NFAState2MapIter NFAState2MapIter;
        typedef typename Global::NFAStateSet2DFAStateMapIter NFAStateSet2DFAStateMapIter;
        typedef typename Global::DFAState2NFAStateMapIter DFAState2NFAStateMapIter;
        typedef typename Global::PDSState2NFAStateMapIter PDSState2NFAStateMapIter;
        typedef typename Global::NFAState2NFAStateSetMapIter NFAState2NFAStateSetMapIter;
        typedef typename Global::NFAState2NFAStateSetMapConstIter NFAState2NFAStateSetMapConstIter;
        
        typedef typename Global::PDSTransList PDSTransList;
        typedef typename Global::PushPDSTransList PushPDSTransList;
        typedef typename Global::ReplacePDSTransList ReplacePDSTransList;
        
        typedef typename Global::PDSTransListConstIter PDSTransListConstIter;
        typedef typename Global::PushPDSTransListConstIter PushPDSTransListConstIter;
        typedef typename Global::ReplacePDSTransListConstIter ReplacePDSTransListConstIter;
        
    private:
        typedef pair<NFAState*, Character> StateChar;
        typedef pair<StateChar, Character> StateChar2;
        typedef set<StateChar> StateCharSet;
        typedef set<StateChar2> StateChar2Set;
        typedef unordered_map<Character, StateCharSet> Char2StateCharSetMap;
        typedef unordered_map<Character, StateChar2Set> Char2StateChar2SetMap;
        typedef unordered_map<NFAState*, Char2StateCharSetMap> NeedMap;
        typedef unordered_map<NFAState*, Char2StateChar2SetMap> Need2Map;
        typedef typename StateCharSet::iterator StateCharSetIter;
        typedef typename StateChar2Set::iterator StateChar2SetIter;
        typedef typename NeedMap::iterator NeedMapIter;
        typedef typename Need2Map::iterator Need2MapIter;
        typedef typename Char2StateCharSetMap::iterator Char2StateCharSetMapIter;
        typedef typename Char2StateChar2SetMap::iterator Char2StateChar2SetMapIter;
        
    private:
        NFAState* initialState;
        NFAStateSet stateSet;
        NFAStateSet finalStateSet;
    private:
        void makeCopyTransByDFA(DFAState *state, DFAState2NFAStateMap &state2map)
        {
            DFATransMap& map = state->getDFATransMap();
            NFAState* preState = state2map[state];
            for(DFATransMapIter mapIt = map.begin(); mapIt != map.end(); mapIt++)
            {
                NFAState* postState;
                DFAState2NFAStateMapIter state2MapIt = state2map.find(mapIt->second);
                if(state2MapIt == state2map.end())
                {
                    if(mapIt->second->isFinal()) postState = mkNFAFinalState();
                    else postState = mkNFAState();
                    state2map[mapIt->second] = postState;
                    makeCopyTransByDFA(mapIt->second, state2map);
                }
                else postState = state2MapIt->second;
                preState->addNFATrans(mapIt->first, postState);
            }
        }
        void makeCopyTransByNFA(NFAState *state, NFAState2Map &state2map)
        {
            NFATransMap& map = state->getNFATransMap();
            NFAState* preState = state2map[state];
            for(NFATransMapIter mapIt = map.begin(); mapIt != map.end(); mapIt++)
            {
                NFAState* postState;
                NFAStateSet set = mapIt->second;
                for(NFAStateSetIter it = set.begin(); it != set.end(); it++)
                {
                    NFAState2MapIter state2MapIt = state2map.find(*it);
                    if(state2MapIt == state2map.end())
                    {
                        if((*it)->isFinal()) postState = mkNFAFinalState();
                        else postState = mkNFAState();
                        state2map[*it] = postState;
                        makeCopyTransByNFA(*it, state2map);
                    }
                    else postState = state2MapIt->second;
                    preState->addNFATrans(mapIt->first, postState);
                }
            }
        }
        void getReachableStateSet(NFAStateSet& reachableStateSet, NFAStateSet& workSet)
        {
            if(workSet.size() == 0) return;
            NFAStateSet tempSet;
            NFAStateSet set;
            for(NFAStateSetIter it = workSet.begin(); it != workSet.end(); it++)
            {
                tempSet.clear();
                (*it)->getTargetStateSet(tempSet);
                for(NFAStateSetIter sIt = tempSet.begin(); sIt != tempSet.end(); sIt++)
                    if(reachableStateSet.insert(*sIt).second)
                        set.insert(*sIt);
            }
            getReachableStateSet(reachableStateSet, set);
        }
        void getReverseMap(NFAState2NFAStateSetMap& reverseMap)
        {
            for(NFAStateSetIter it = stateSet.begin(); it != stateSet.end(); it++)
                reverseMap[*it] = NFAStateSet();
            for(NFAStateSetIter it = stateSet.begin(); it != stateSet.end(); it++)
            {
                NFATransMap &nfaTransMap = (*it)->getNFATransMap();
                for(NFATransMapIter mapIter = nfaTransMap.begin(); mapIter != nfaTransMap.end(); mapIter++)
                    for(NFAStateSetIter sIt = mapIter->second.begin(); sIt != mapIter->second.end(); sIt++)
                        reverseMap[*sIt].insert(*it);
            }
        }
        
        void getLiveStateSet(const NFAState2NFAStateSetMap& reverseMap, NFAStateSet& liveStateSet, NFAStateSet& workSet)
        {
            if(workSet.size() == 0) return;
            NFAStateSet set;
            for(NFAStateSetIter it = workSet.begin(); it != workSet.end(); it++)
            {
                NFAState2NFAStateSetMapConstIter mapConstIt = reverseMap.find(*it);
                if(mapConstIt != reverseMap.end())
                    for(NFAStateSetConstIter sIt = mapConstIt->second.begin(); sIt != mapConstIt->second.end(); sIt++)
                        if(liveStateSet.insert(*sIt).second)
                            set.insert(*sIt);
            }
            getLiveStateSet(reverseMap, liveStateSet, set);
        }
        void getTransMapByStateSet(const NFAStateSet& stateSet, NFATransMap& nfaTransMap)const
        {
            for(NFAStateSetConstIter it = stateSet.begin(); it != stateSet.end(); it++)
            {
                NFATransMap& transMap = (*it)->getNFATransMap();
                NFAStateSet set;
                for(NFATransMapIter mapIt = transMap.begin(); mapIt != transMap.end(); mapIt++)
                    if(mapIt->first != Global::epsilon)
                    {
                        set.clear();
                        (*it)->getTargetStateSetByChar(set, mapIt->first);
                        if(set.size() > 0) nfaTransMap[mapIt->first].insert(set.begin(), set.end());
                    }
            }
        }
        
        void makeDFATrans(DFAState* preState, NFAStateSet2DFAStateMap &setMapping, const NFATransMap &nfaTransMap, DFA* dfa)const
        {
            NFATransMap transMap;
            for(NFATransMapConstIter mapIt = nfaTransMap.begin(); mapIt != nfaTransMap.end(); mapIt++)
            {
                NFAStateSet2DFAStateMapIter setMapIt = setMapping.find(mapIt->second);
                DFAState* postState;
                if(setMapIt == setMapping.end())
                {
                    transMap.clear();
                    getTransMapByStateSet(mapIt->second, transMap);
                    if(NFA::hasFinalState(mapIt->second)) postState = dfa->mkDFAFinalState();
                    else postState = dfa->mkDFAState();
                    setMapping[mapIt->second] = postState;
                    makeDFATrans(postState, setMapping, transMap, dfa);
                }
                else postState = setMapping[mapIt->second];
                preState->addDFATrans(mapIt->first, postState);
            }
        }
        
        bool addNeedMap(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap &needMap, Need2Map& need2Map)
        {
            StateChar stateChar(sState, sc);
            StateCharSet stateCharSet;
            stateCharSet.insert(stateChar);
            NeedMapIter needmapIt = needMap.find(tState);
            if(needmapIt != needMap.end())
            {
                Char2StateCharSetMapIter mapIt = needmapIt->second.find(tc);
                if(mapIt != needmapIt->second.end())
                {
                    if(!mapIt->second.insert(stateChar).second)
                        return false;
                }
                else
                    needmapIt->second[tc] = stateCharSet;
            }
            else
            {
                Char2StateCharSetMap char2StateCharSetMap;
                char2StateCharSetMap[tc] = stateCharSet;
                needMap[tState] = char2StateCharSetMap;
            }
            return true;
        }
        bool addNeed2Map(NFAState *sState, Character sc, NFAState *tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map &need2Map)
        {
            StateChar stateChar(sState, sc);
            StateChar2 stateChar2(stateChar, tc2);
            StateChar2Set stateChar2Set;
            stateChar2Set.insert(stateChar2);
            Need2MapIter need2MapIt = need2Map.find(tState);
            if(need2MapIt != need2Map.end())
            {
                Char2StateChar2SetMapIter mapIt = need2MapIt->second.find(tc1);
                if(mapIt != need2MapIt->second.end())
                {
                    if(!mapIt->second.insert(stateChar2).second)
                        return false;
                }
                else
                    need2MapIt->second[tc1] = stateChar2Set;
            }
            else
            {
                Char2StateChar2SetMap char2StateChar2SetMap;
                char2StateChar2SetMap[tc1] = stateChar2Set;
                need2Map[tState] = char2StateChar2SetMap;
            }
            return true;
        }
        
        void addPreStarNeedMap(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap &needMap, Need2Map& need2Map)
        {
            if(addNeedMap(sState, sc, tState, tc, needMap, need2Map))
                addPreStarTrans(sState, sc, tState, tc, needMap, need2Map);
        }
        
        void addPreStarNeed2Map(NFAState* sState, Character sc, NFAState* tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map& need2Map)
        {
            if(addNeed2Map(sState, sc, tState, tc1, tc2, needMap, need2Map))
            {
                NFAStateSet stateset = tState->getTargetStateSetByChar(tc1);
                for(NFAStateSetIter it = stateset.begin(); it != stateset.end(); it++)
                    addPreStarNeedMap(sState, sc, *it, tc2, needMap, need2Map);
            }
        }
        
        void addPreStarTrans(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap& needMap, Need2Map& need2Map)
        {
            NFAStateSet stateset;
            tState->getTargetStateSetByChar(stateset, tc);
            for(NFAStateSetIter it = stateset.begin(); it != stateset.end(); it++)
                if(sState->addNFATrans(sc, *it))
                    addPreStarTrans(sState, sc, *it, needMap, need2Map);
        }
        void addPreStarTrans(NFAState *sState, Character c, NFAState *tState, NeedMap& needMap, Need2Map& need2Map)
        {
            Char2StateCharSetMap map = needMap[sState];
            Char2StateCharSetMapIter mapIt = map.find(c);
            if(mapIt != map.end())
            {
                StateCharSet stateCharSet = mapIt->second;
                for(StateCharSetIter sIt = stateCharSet.begin(); sIt != stateCharSet.end(); sIt++)
                    if(sIt->first->addNFATrans(sIt->second, tState))
                        addPreStarTrans(sIt->first, sIt->second, tState, needMap, need2Map);
            }
            Char2StateChar2SetMap map2 = need2Map[sState];
            Char2StateChar2SetMapIter map2It = map2.find(c);
            if(map2It != map2.end())
            {
                StateChar2Set stateChar2Set = map2It->second;
                for(StateChar2SetIter sIt = stateChar2Set.begin(); sIt != stateChar2Set.end(); sIt++)
                    addPreStarNeedMap(sIt->first.first, sIt->first.second, tState, sIt->second, needMap, need2Map);
            }
        }
        
        
        void addPostStarNeedMap(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap &needMap, Need2Map& need2Map)
        {
            if(addNeedMap(sState, sc, tState, tc, needMap, need2Map))
                addPostStarTrans(sState, sc, tState, tc, needMap, need2Map);
        }
        
        void addPostStarNeed2Map(NFAState* sState, Character sc, NFAState* tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map& need2Map)
        {
            if(addNeed2Map(sState, sc, tState, tc1, tc2, needMap, need2Map))
            {
                NFAState* midState = mkNFAState();
                sState->addNFATrans(sc, midState);
                addPostStarTrans(sState, sc, midState, needMap, need2Map);
                addPostStarNeedMap(midState, tc2, tState, tc1, needMap, need2Map);
            }
        }
        
        void addPostStarTrans(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap& needMap, Need2Map& need2Map)
        {
            NFAStateSet stateset = tState->getTargetStateSetByChar(tc);
            for(NFAStateSetIter it = stateset.begin(); it != stateset.end(); it++)
                if(sState->addNFATrans(sc, *it))
                    addPostStarTrans(sState, sc, *it, needMap, need2Map);
        }
        
        void addPostStarTrans(NFAState *sState, Character c, NFAState *tState, NeedMap& needMap, Need2Map& need2Map)
        {
            Char2StateCharSetMap map = needMap[sState];
            Char2StateCharSetMapIter mapIt = map.find(c);
            if(mapIt != map.end())
            {
                StateCharSet stateCharSet = mapIt->second;
                for(StateCharSetIter sIt = stateCharSet.begin(); sIt != stateCharSet.end(); sIt++)
                    if(sIt->first->addNFATrans(sIt->second, tState))
                        addPostStarTrans(sIt->first, sIt->second, tState, needMap, need2Map);
            }
        }
    public:
        NFA() : FA(), initialState(NULL){}
        NFA(const NFA& nfa)
        {
            if(nfa.initialState)
            {
                this->flag = nfa.flag;
                this->setAlphabet(nfa.alphabet);
                NFAState* iniState = mkNFAInitialState();
                if(nfa.initialState->isFinal())
                    addFinalState(iniState);
                NFAState2Map state2Map;
                state2Map[nfa.initialState] = iniState;
                makeCopyTransByNFA(nfa.initialState, state2Map);
            }
        }
        NFA(const NFA& nfa, NFAState2Map& state2Map)
        {
            if(nfa.initialState)
            {
                this->flag = nfa.flag;
                this->setAlphabet(nfa.alphabet);
                NFAState* iniState = mkNFAInitialState();
                if(nfa.initialState->isFinal())
                    addFinalState(iniState);
                state2Map[nfa.initialState] = iniState;
                makeCopyTransByNFA(nfa.initialState, state2Map);
            }
        }
        NFA(const DFA& dfa)
        {
            if(dfa.initialState)
            {
                this->flag = dfa.flag;
                this->setAlphabet(dfa.alphabet);
                NFAState* iniState = mkNFAInitialState();
                if(dfa.initialState->isFinal())
                    addFinalState(iniState);
                DFAState2NFAStateMap state2Map;
                state2Map[dfa.initialState] = iniState;
                makeCopyTransByDFA(dfa.initialState, state2Map);
            }
        }
        NFA(const RawFaData& data) : FA()
        {
            RawFaDataWithInt* rawdata = dynamic_cast<RawFaDataWithInt*>(data.alphabetAndTransitions);
            this->setAlphabet(rawdata->alphabet);
            int stateNum = data.stateNumber;
            vector<NFAState*> stateVector;
            for(int pos = 0; pos < stateNum; pos++)
            {
                if(pos == data.initialState) stateVector.push_back(mkNFAInitialState());
                else if(data.finalStates.find(pos) != data.finalStates.end()) stateVector.push_back(mkNFAFinalState());
                else stateVector.push_back(mkNFAState());
            }
            if(data.finalStates.find(data.initialState) != data.finalStates.end())
            {
                initialState->setFinalFlag(1);
                finalStateSet.insert(initialState);
            }
            vector<tuple<int, int, int> > transVector = rawdata->transitions;
            for(int i = 0; i < transVector.size(); i++)
            {
                int sourceState = get<0>(transVector[i]);
                int targetState = get<2>(transVector[i]);
                int character = get<1>(transVector[i]);
                stateVector[sourceState]->addNFATrans(character, stateVector[targetState]);
            }
        }
        ~NFA()
        {
            initialState = NULL;
            for(NFAStateSetIter it = stateSet.begin(); it != stateSet.end(); it++)
                delete *it;
        }
        void setInitialState(NFAState* state) {initialState = state;}
        void addFinalState(NFAState* state) {finalStateSet.insert(state); state->setFinalFlag(1);}
        NFAStateSet& getStateSet() {return stateSet;}
        NFAStateSet& getFinalStateSet() {return finalStateSet;}
        NFAState* getInitialState() {return initialState;}
        const NFAStateSet& getStateSet() const {return stateSet;}
        const NFAStateSet& getFinalStateSet() const {return finalStateSet;}
        const NFAState* getInitialState() const {return initialState;}
        void clearFinalStateSet()
        {
            for(NFAStateSetIter it = finalStateSet.begin(); it != finalStateSet.end(); it++)
                (*it)->setFinalFlag(0);
            finalStateSet.clear();
        }
        static bool hasFinalState(const NFAStateSet& stateSet)
        {
            for(NFAStateSetConstIter it = stateSet.begin(); it != stateSet.end(); it++)
                if((*it)->isFinal()) return true;
            return false;
        }
        static bool allFinalState(const NFAStateSet& stateSet)
        {
            for(NFAStateSetConstIter it = stateSet.begin(); it != stateSet.end(); it++)
                if(!(*it)->isFinal()) return false;
            return true;
        }
        bool isNULL() const {if(!initialState) return true; return false;}
        
        
        
        /*******************************************************************/
        /*                                                                 */
        /*  NFA::mkNFAState                                                */
        /*                                                                 */
        /*******************************************************************/
        NFAState *mkNFAState()
        {
            NFAState *nfaState = new NFAState();
            stateSet.insert(nfaState);
            return nfaState;
        }
        
        NFAState *mkNFAInitialState()
        {
            initialState = mkNFAState();
            return initialState;
        }
        
        NFAState *mkNFAFinalState()
        {
            NFAState *nfaState = mkNFAState();
            nfaState->setFinalFlag(1);
            finalStateSet.insert(nfaState);
            return nfaState;
        }
        
        FA &operator &(const FA &fa)
        {
            DFA &dfa1 = this->determine();
            DFA &dfa2 = const_cast<FA&>(fa).determine();
            FA &intersection = dfa1 & dfa2;
            if(!fa.isDeterminate()) delete &dfa2;
            delete &dfa1;
            return intersection;
        }
        FA &operator |(const FA &fa)
        {
            if(isNULL()) return const_cast<FA&>(fa);
            if(fa.isNULL()) return *this;
            NFA *nfa = new NFA();
            nfa->setAlphabet(this->alphabet);
            NFAState* iniState = nfa->mkNFAInitialState();
            NFAState* state1 = nfa->mkNFAState();
            NFAState* state2 = nfa->mkNFAState();
            NFAState2Map state2Map1;
            state2Map1[initialState] = state1;
            if(initialState->isFinal()) nfa->addFinalState(state1);
            nfa->makeCopyTransByNFA(initialState, state2Map1);
            if(fa.isDeterminate())
            {
                DFAState2NFAStateMap state2Map2;
                DFA& tempDFA = const_cast<FA&>(fa).determine();
                state2Map2[tempDFA.initialState] = state2;
                if(tempDFA.initialState->isFinal()) nfa->addFinalState(state2);
                nfa->makeCopyTransByDFA(tempDFA.initialState, state2Map2);
            }
            else
            {
                NFAState2Map state2Map2;
                NFA& tempNFA = const_cast<FA&>(fa).nondetermine();
                state2Map2[tempNFA.initialState] = state2;
                if(tempNFA.initialState->isFinal()) nfa->addFinalState(state2);
                nfa->makeCopyTransByNFA(tempNFA.initialState, state2Map2);
            }
            iniState->addEpsilonTrans(state1);
            iniState->addEpsilonTrans(state2);
            return *nfa;
        }
        DFA &operator !( void )
        {
            DFA &tempDFA = determine();
            DFA &complement = (!tempDFA);
            delete &tempDFA;
            return complement;
        }
        DFA &determine( void )
        {
            DFA *dfa = new DFA();
            if(isNULL()) return *dfa;
            dfa->setAlphabet(this->alphabet);
            NFAStateSet set;
            initialState->getEpsilonClosure(set);
            set.insert(initialState);
            NFAStateSet2DFAStateMap setMapping;
            NFATransMap nfaTransMap;
            getTransMapByStateSet(set, nfaTransMap);
            DFAState *iniState = dfa->mkDFAInitialState();
            if(NFA::hasFinalState(set)) dfa->addFinalState(iniState);
            setMapping[set] = iniState;
            makeDFATrans(iniState, setMapping, nfaTransMap, dfa);
            if(dfa->finalStateSet.size() == 0)
            {
                delete dfa;
                return FA::EmptyDFA();
            }
            dfa->setReachableFlag(1);
            return *dfa;
        }
        
        NFA &nondetermine( void )
        {
            return *this;
        }
        
        FA &concat(const FA &fa)
        {
            if(isNULL()) return const_cast<FA&>(fa);
            if(fa.isNULL()) return *this;
            NFA *nfa = new NFA(*this);
            NFAState* state = nfa->mkNFAState();
            for(NFAStateSetIter it = nfa->finalStateSet.begin(); it != nfa->finalStateSet.end(); it++)
                (*it)->addEpsilonTrans(state);
            nfa->clearFinalStateSet();
            if(fa.isDeterminate())
            {
                DFAState2NFAStateMap state2Map;
                DFA& tempDFA = const_cast<FA&>(fa).determine();
                state2Map[tempDFA.initialState] = state;
                nfa->makeCopyTransByDFA(tempDFA.initialState, state2Map);
                if(tempDFA.initialState->isFinal()) nfa->addFinalState(state);
            }
            else
            {
                NFAState2Map state2Map;
                NFA& tempNFA = const_cast<FA&>(fa).nondetermine();
                state2Map[tempNFA.initialState] = state;
                nfa->makeCopyTransByNFA(tempNFA.initialState, state2Map);
                if(tempNFA.initialState->isFinal()) nfa->addFinalState(state);
            }
            return *nfa;
        }
        FA &subset(const NFAState *iState, const NFAState *fState)
        {
            NFA *nfa = new NFA();
            if(isNULL()) return *nfa;
            nfa->setAlphabet(this->alphabet);
            NFAState* iniState = nfa->mkNFAInitialState();
            NFAState2Map state2Map;
            NFAStateSetIter iter;
            state2Map[const_cast<State*>(iState)] = iniState;
            nfa->makeCopyTransByNFA((const_cast<State*>(iState)), state2Map);
            nfa->clearFinalStateSet();
            NFAState* nfaState = (state2Map[const_cast<State*>(fState)]);
            nfa->addFinalState(nfaState);
            nfa->removeDeadState();
            return *nfa;
        }
        FA &rightQuotient(Character character)
        {
            NFA* nfa = new NFA(*this);
            NFAStateSet finSteteSet;
            NFAStateSet tempSet;
            for(NFAStateSetIter it = nfa->stateSet.begin(); it != nfa->stateSet.end(); it++)
            {
                tempSet.clear();
                (*it)->getTargetStateSetByChar(tempSet, character);
                if(NFA::hasFinalState(tempSet))
                    finSteteSet.insert(*it);
            }
            nfa->clearFinalStateSet();
            for(NFAStateSetIter it = finSteteSet.begin(); it != finSteteSet.end(); it++)
                nfa->addFinalState(*it);
            return *nfa;
        }
        
        FA& leftQuotient(Character character)
        {
            if(initialState->getNFATransMap().count(character) == 0) return FA::EmptyNFA();
            NFA* nfa = new NFA(*this);
            NFAStateSet set;
            nfa->initialState->getTargetStateSetByChar(set, character);
            if(set.size() == 0) return FA::EmptyNFA();
            NFAState* state = nfa->mkNFAInitialState();
            for(NFAStateSetIter it = set.begin(); it != set.end(); it++)
                state->addEpsilonTrans(*it);
            nfa->removeUnreachableState();
            return *nfa;
        }
        
        
        void removeUnreachableState()
        {
            if(isNULL()) return;
            NFAStateSet reachableStateSet;
            NFAStateSet workSet;
            workSet.insert(initialState);
            reachableStateSet.insert(initialState);
            getReachableStateSet(reachableStateSet, workSet);
            if(!NFA::hasFinalState(reachableStateSet))
            {
                initialState = NULL;
                return;
            }
            if(reachableStateSet.size() != this->stateSet.size())
            {
                NFAStateSet set;
                for(NFAStateSetIter it = stateSet.begin(); it != stateSet.end(); it++)
                    if(reachableStateSet.find(*it) == reachableStateSet.end())
                    {
                        NFAStateSet postStateSet = (*it)->getTargetStateSet();
                        for(NFAStateSetIter sIt = postStateSet.begin(); sIt != postStateSet.end(); sIt++)
                            if(reachableStateSet.find(*sIt) != reachableStateSet.end())
                                dynamic_cast<NFAState*>(*it)->delNFATrans(*sIt);
                        set.insert(*it);
                    }
                for(NFAStateSetIter it = set.begin(); it != set.end(); it++)
                {
                    stateSet.erase(*it);
                    delete *it;
                }
            }
            this->setReachableFlag(1);
        }
        void removeDeadState()
        {
            if(isNULL()) return;
            NFAState2NFAStateSetMap reverseMap;
            getReverseMap(reverseMap);
            NFAStateSet liveStateSet(finalStateSet.begin(), finalStateSet.end());
            getLiveStateSet(reverseMap, liveStateSet, finalStateSet);
            if(liveStateSet.find(initialState) == liveStateSet.end())
            {
                initialState = NULL;
                return;
            }
            NFAStateSet set;
            for(NFAStateSetIter it = stateSet.begin(); it != stateSet.end(); it++)
                if(liveStateSet.find(*it) == liveStateSet.end())
                {
                    NFAStateSet preStateSet = reverseMap.find(*it)->second;
                    for(NFAStateSetIter sIt = preStateSet.begin(); sIt != preStateSet.end(); sIt++)
                        if(liveStateSet.find(*sIt) != liveStateSet.end())
                            dynamic_cast<NFAState*>(*sIt)->delNFATrans(*it);
                    set.insert(*it);
                }
            for(NFAStateSetIter it = set.begin(); it != set.end(); it++)
            {
                stateSet.erase(*it);
                delete *it;
            }
        }
        //
        //        Word getOneRun();
        bool isAccepted(const Word &word)//accepted
        {
            if(isNULL()) return false;
            NFAStateSet workSet;
            workSet.insert(initialState);
            for(int i = 0; i < word.size(); i++)
            {
                if(workSet.size() > 0)
                {
                    NFAStateSet set;
                    for(NFAStateSetIter it = workSet.begin(); it != workSet.end(); it++)
                        (*it)->getTargetStateSetByChar(set, word[i]);
                    workSet.clear();
                    if(set.size() > 0) workSet.insert(set.begin(), set.end());
                    else return false;
                }
            }
            if(workSet.size() == 0) return false;
            if(NFA::hasFinalState(workSet)) return true;
            return false;
        }
        bool isAccepted(Character character)
        {
            if(isNULL()) return false;
            NFAStateSet set;
            initialState->getTargetStateSetByChar(set, character);
            if(set.size() == 0) return false;
            if(NFA::hasFinalState(set)) return true;
            return false;
        }
        
        bool isEmpty()
        {
            if(isNULL()) return true;
            if(!this->isReachable()) removeUnreachableState();
            if(finalStateSet.size() == 0) return true;
            return false;
        }
        
        NFA& preStar(const PDS& pds, PDSState2NFAStateMap& state2Map)
        {
            NFAState2Map copyMap;
            NFA* nfa = new NFA(*this, copyMap);
            NeedMap needMap;
            Need2Map need2Map;
            PDSStateSet tempSet;
            for(PDSState2NFAStateMapIter it = state2Map.begin(); it != state2Map.end(); it++)
                tempSet.insert(it->first);
            for(PDSStateSetConstIter it = pds.getStateSet().begin(); it != pds.getStateSet().end(); it++)
            {
                if(tempSet.find(*it) == tempSet.end())
                    state2Map[*it] = nfa->mkNFAState();
                else
                    state2Map[*it] = copyMap[state2Map[*it]];
            }
            
            for(PDSTransListConstIter iter = pds.pdsPopTransList.begin(); iter != pds.pdsPopTransList.end(); iter++)
            {
                NFAState* sourceState = state2Map[(*iter)->getSourceState()];
                NFAState* targetState = state2Map[(*iter)->getTargetState()];
                Character character = (*iter)->getChar();
                sourceState->addNFATrans(character, targetState);
            }
            for(ReplacePDSTransListConstIter iter = pds.pdsReplaceTransList.begin(); iter != pds.pdsReplaceTransList.end();iter++)
            {
                NFAState* sourceState = state2Map[(*iter)->getSourceState()];
                NFAState* targetState = state2Map[(*iter)->getTargetState()];
                Character character = (*iter)->getChar();
                Character stack = (*iter)->getStack();
                nfa->addPreStarNeedMap(sourceState, character, targetState, stack, needMap, need2Map);
            }
            for(PushPDSTransListConstIter iter = pds.pdsPushTransList.begin(); iter != pds.pdsPushTransList.end();iter++)
            {
                NFAState* sourceState = state2Map[(*iter)->getSourceState()];
                NFAState* targetState = state2Map[(*iter)->getTargetState()];
                Character character = (*iter)->getChar();
                Char2 stack = (*iter)->getStack();
                nfa->addPreStarNeed2Map(sourceState, character, targetState, stack.first, stack.second, needMap, need2Map);
            }
            nfa->removeUnreachableState();
            nfa->removeDeadState();
            return *nfa;
        }
        
        NFA& postStar(const PDS& pds, PDSState2NFAStateMap& state2Map)//todo
        {
            NFAState2Map copyMap;
            NFA* nfa = new NFA(*this, copyMap);
            NeedMap needMap;
            Need2Map need2Map;
            PDSStateSet tempSet;
            for(PDSState2NFAStateMapIter it = state2Map.begin(); it != state2Map.end(); it++)
                tempSet.insert(it->first);
            for(PDSStateSetConstIter it = pds.getStateSet().begin(); it != pds.getStateSet().end(); it++)
            {
                if(tempSet.find(*it) == tempSet.end())
                    state2Map[*it] = nfa->mkNFAState();
                else
                    state2Map[*it] = copyMap[state2Map[*it]];
            }
            
            for(PDSTransListConstIter iter = pds.pdsPopTransList.begin(); iter != pds.pdsPopTransList.end(); iter++)
            {
                NFAState* sourceState = state2Map[(*iter)->getSourceState()];
                NFAState* targetState = state2Map[(*iter)->getTargetState()];
                Character character = (*iter)->getChar();
                nfa->addPostStarNeedMap(targetState, Global::epsilon, sourceState, character, needMap, need2Map);
                
            }
            for(ReplacePDSTransListConstIter iter = pds.pdsReplaceTransList.begin(); iter != pds.pdsReplaceTransList.end();iter++)
            {
                NFAState* sourceState = state2Map[(*iter)->getSourceState()];
                NFAState* targetState = state2Map[(*iter)->getTargetState()];
                Character character = (*iter)->getChar();
                Character stack = (*iter)->getStack();
                nfa->addPostStarNeedMap(targetState, stack, sourceState, character, needMap, need2Map);
            }
            for(PDSTransListConstIter iter = pds.pdsPushTransList.begin(); iter != pds.pdsPushTransList.end();iter++)
            {
                NFAState* sourceState = state2Map[(*iter)->getSourceState()];
                NFAState* targetState = state2Map[(*iter)->getTargetState()];
                Character character = (*iter)->getChar();
                Char2 stack = (*iter)->getStack();
                nfa->addPostStarNeed2Map(targetState, stack.first, sourceState, character, stack.second, needMap, need2Map);
            }
            nfa->removeUnreachableState();
            nfa->removeDeadState();
            return *nfa;
        }
        
        
        
        
        void output()const{
            if(!initialState) return;
            cout<<initialState->getID()<<endl;
            for(NFAStateSetConstIter it = stateSet.begin(); it != stateSet.end(); it++)
            {
                if((*it)->isFinal()) cout<<"$"<<(*it)->getID()<<endl;;
                dynamic_cast<NFAState*>((*it))->output();
            }
        }
        void print(string filename)const
        {
            ofstream f;
            f.open(filename);
            f << "digraph {\n";
            f << "rankdir=LR;\n";
            
            
            // cout initial
            f << "Q"<<initialState->getID() << "[color=blue];\n";
            f << "node [shape=doublecircle];\n";
            
            
            
            // cout final states
            for (NFAStateSetConstIter iter = finalStateSet.begin(); iter != finalStateSet.end(); iter++) {
                f << "Q" << (*iter)->getID() << " ";
            }
            f << ";\n";
            
            
            
            
            f << "node [shape=circle];\n";
            
            
            
            // cout trisitions
            for(NFAStateSetConstIter iter = stateSet.begin(); iter != stateSet.end(); iter++)
            {
                NFATransMap& transMap = (*iter)->getNFATransMap();
                ID id = (*iter)->getID();
                for (NFATransMapIter iter = transMap.begin(); iter != transMap.end(); iter++) {
                    for (NFAStateSetIter iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++) {
                        f << "Q" << id <<  " -> " << "Q" << (*iter1)->getID() << "[label=\"" << iter->first <<"\"];\n";
                    }
                }
            }
            f <<"}\n";
            f.close();
        }
        friend FA;
        
    };
}

#endif /* NFA_hpp */

