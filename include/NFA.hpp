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
#include "RegularExp.hpp"

namespace cgh {
    
    /// \brief A class of Nondeterministic Finite Automaton.
    template <class Character>
    class NFA : public FA<Character>
    {
        typedef FA<Character> FA;
        typedef DFA<Character> DFA;
        typedef PDS<Character> PDS;
        typedef Global<Character> Global;
        typedef NFAState<Character> NFAState;
        typedef DFAState<Character> DFAState;
        typedef PDSTrans<Character> PDSTrans;
        typedef PushPDSTrans<Character> PushPDSTrans;
        typedef ReplacePDSTrans<Character> ReplacePDSTrans;
        
        typedef typename Global::Word Word;
        typedef typename Global::Char2 Char2;
        typedef typename Global::CharacterSet CharacterSet;
        typedef typename Global::NFAStateSet NFAStateSet;
        typedef typename Global::DFAStateSet DFAStateSet;
        typedef typename Global::NFATransMap NFATransMap;
        typedef typename Global::DFATransMap DFATransMap;
        
        typedef typename Global::NFAState2Map NFAState2Map;
        typedef typename Global::DFAState2NFAStateMap DFAState2NFAStateMap;
        typedef typename Global::NFAState2DFAStateMap NFAState2DFAStateMap;
        typedef typename Global::PDSState2NFAStateMap PDSState2NFAStateMap;
        typedef typename Global::NFAStateSet2DFAStateMap NFAStateSet2DFAStateMap;
        typedef typename Global::NFAState2NFAStateSetMap NFAState2NFAStateSetMap;
        
        typedef typename Global::PDSTransList PDSTransList;
        typedef typename Global::PushPDSTransList PushPDSTransList;
        typedef typename Global::ReplacePDSTransList ReplacePDSTransList;
        
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
        NFAState* initialState;     ///< The initial state for this NFA.
        NFAStateSet stateSet;       ///< The set of states for this NFA.
        NFAStateSet finalStateSet;  ///< The set of final states for this NFA.
    private:
        void cpTransByDFA(DFAState *state, DFAState2NFAStateMap &state2map) {
            NFAState* sourceState = state2map[state];
            for (auto& mapPair : state -> getDFATransMap()) {
                NFAState* targetState = nullptr;
                auto state2MapIt = state2map.find(mapPair.second);
                if (state2MapIt == state2map.end()) {
                    if (mapPair.second -> isFinal()) {
                        targetState = mkNFAFinalState();
                    } else {
                        targetState = mkNFAState();
                    }
                    state2map[mapPair.second] = targetState;
                    cpTransByDFA(mapPair.second, state2map);
                } else {
                    targetState = state2MapIt -> second;
                }
                sourceState -> addNFATrans(mapPair.first, targetState);
            }
        }
        void cpTransByNFA(NFAState *state, NFAState2Map &state2map) {
            NFAState* sourceState = state2map[state];
            for (auto& mapPair : state -> getNFATransMap()) {
                NFAState* targetState = nullptr;
                for (NFAState* state : mapPair.second) {
                    auto state2MapIt = state2map.find(state);
                    if (state2MapIt == state2map.end()) {
                        if (state -> isFinal()) {
                            targetState = mkNFAFinalState();
                        } else {
                            targetState = mkNFAState();
                        }
                        state2map[state] = targetState;
                        cpTransByNFA(state, state2map);
                    } else {
                        targetState = state2MapIt -> second;
                    }
                    sourceState -> addNFATrans(mapPair.first, targetState);
                }
            }
        }

        void getReachableStateSet(NFAStateSet& reachableStateSet, NFAStateSet& workSet) {
            NFAStateSet targetStateSet, newWorkSet;
            for (NFAState* state : workSet) {
                targetStateSet.clear();
                state -> getTargetStateSet(targetStateSet);
                for (NFAState* targetState : targetStateSet) {
                    if (reachableStateSet.insert(targetState).second)
                        newWorkSet.insert(targetState);
                }
            }
            if (newWorkSet.size() == 0) return;
            getReachableStateSet(reachableStateSet, newWorkSet);
        }
        
        void getReverseMap(NFAState2NFAStateSetMap& reverseMap) {
            for (NFAState* state : stateSet) {
                for (auto& mapPair : state -> getNFATransMap()) {
                    for (NFAState* nfaState : mapPair.second) {
                        reverseMap[nfaState].insert(state);
                    }
                }
            }
        }
        
        void getLiveStateSet(const NFAState2NFAStateSetMap& reverseMap, NFAStateSet& liveStateSet, NFAStateSet& workSet) {
            NFAStateSet newWorkSet;
            for (NFAState* state : workSet) {
                auto mapIt = reverseMap.find(state);
                if (mapIt != reverseMap.end()) {
                    for (NFAState* reverseState : mapIt -> second) {
                        if (liveStateSet.insert(reverseState).second) {
                            newWorkSet.insert(reverseState);
                        }
                    }
                }
            }
            if (workSet.size() == 0) return;
            getLiveStateSet(reverseMap, liveStateSet, newWorkSet);
        }

        void getTransMapByStateSet(const NFAStateSet& stateSet, NFATransMap& nfaTransMap) const {
            for (NFAState* state : stateSet) {
                NFAStateSet workSet;
                for (auto& mapPair : state -> getNFATransMap()) {
                    if (mapPair.first != Global::epsilon) {
                        workSet.clear();
                        state -> getTargetStateSetByChar(workSet, mapPair.first);
                        if (workSet.size() > 0)
                            nfaTransMap[mapPair.first].insert(workSet.begin(), workSet.end());
                    }
                }
            }
        }
        
        void mkDFATrans(DFAState* sourceState, NFAStateSet2DFAStateMap &setMap, const NFATransMap &nfaTransMap, DFA* dfa) const {
            NFATransMap transMap;
            for (auto& mapPair : nfaTransMap) {
                auto setMapIt = setMap.find(mapPair.second);
                DFAState* targetState = nullptr;
                if (setMapIt == setMap.end()) {
                    transMap.clear();
                    getTransMapByStateSet(mapPair.second, transMap);
                    if (NFA::hasFinalState(mapPair.second)) {
                        targetState = dfa -> mkDFAFinalState();
                    } else {
                        targetState = dfa -> mkDFAState();
                    }
                    setMap[mapPair.second] = targetState;
                    mkDFATrans(targetState, setMap, transMap, dfa);
                } else {
                    targetState = setMap[mapPair.second];
                }
                sourceState -> addDFATrans(mapPair.first, targetState);
            }
        }
        
        //bool addNeedMap(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap &needMap, Need2Map& need2Map)
        //{
        //    StateChar stateChar(sState, sc);
        //    StateCharSet stateCharSet;
        //    stateCharSet.insert(stateChar);
        //    NeedMapIter needmapIt = needMap.find(tState);
        //    if(needmapIt != needMap.end())
        //    {
        //        Char2StateCharSetMapIter mapIt = needmapIt -> second.find(tc);
        //        if(mapIt != needmapIt -> second.end())
        //        {
        //            if(!mapIt -> second.insert(stateChar).second)
        //                return false;
        //        }
        //        else
        //            needmapIt -> second[tc] = stateCharSet;
        //    }
        //    else
        //    {
        //        Char2StateCharSetMap char2StateCharSetMap;
        //        char2StateCharSetMap[tc] = stateCharSet;
        //        needMap[tState] = char2StateCharSetMap;
        //    }
        //    return true;
        //}
        //bool addNeed2Map(NFAState *sState, Character sc, NFAState *tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map &need2Map)
        //{
        //    StateChar stateChar(sState, sc);
        //    StateChar2 stateChar2(stateChar, tc2);
        //    StateChar2Set stateChar2Set;
        //    stateChar2Set.insert(stateChar2);
        //    Need2MapIter need2MapIt = need2Map.find(tState);
        //    if(need2MapIt != need2Map.end())
        //    {
        //        Char2StateChar2SetMapIter mapIt = need2MapIt -> second.find(tc1);
        //        if(mapIt != need2MapIt -> second.end())
        //        {
        //            if(!mapIt -> second.insert(stateChar2).second)
        //                return false;
        //        }
        //        else
        //            need2MapIt -> second[tc1] = stateChar2Set;
        //    }
        //    else
        //    {
        //        Char2StateChar2SetMap char2StateChar2SetMap;
        //        char2StateChar2SetMap[tc1] = stateChar2Set;
        //        need2Map[tState] = char2StateChar2SetMap;
        //    }
        //    return true;
        //}
        //
        //void addPreStarNeedMap(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap &needMap, Need2Map& need2Map)
        //{
        //    if(addNeedMap(sState, sc, tState, tc, needMap, need2Map))
        //        addPreStarTrans(sState, sc, tState, tc, needMap, need2Map);
        //}
        //
        //void addPreStarNeed2Map(NFAState* sState, Character sc, NFAState* tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map& need2Map)
        //{
        //    if(addNeed2Map(sState, sc, tState, tc1, tc2, needMap, need2Map))
        //    {
        //        NFAStateSet stateset = tState -> getTargetStateSetByChar(tc1);
        //        for(NFAState* state : stateset)
        //            addPreStarNeedMap(sState, sc, state, tc2, needMap, need2Map);
        //    }
        //}
        //
        //void addPreStarTrans(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap& needMap, Need2Map& need2Map)
        //{
        //    NFAStateSet stateset;
        //    tState -> getTargetStateSetByChar(stateset, tc);
        //    for(NFAState* state : stateset)
        //        if(sState -> addNFATrans(sc, state))
        //            addPreStarTrans(sState, sc, state, needMap, need2Map);
        //}
        //void addPreStarTrans(NFAState *sState, Character c, NFAState *tState, NeedMap& needMap, Need2Map& need2Map)
        //{
        //    Char2StateCharSetMap map = needMap[sState];
        //    Char2StateCharSetMapIter mapIt = map.find(c);
        //    if(mapIt != map.end())
        //    {
        //        StateCharSet stateCharSet = mapIt -> second;
        //        for(StateChar scpair : stateCharSet)
        //            if(scpair.first -> addNFATrans(scpair.second, tState))
        //                addPreStarTrans(scpair.first, scpair.second, tState, needMap, need2Map);
        //    }
        //    Char2StateChar2SetMap map2 = need2Map[sState];
        //    Char2StateChar2SetMapIter map2It = map2.find(c);
        //    if(map2It != map2.end())
        //    {
        //        StateChar2Set stateChar2Set = map2It -> second;
        //        for(StateChar2 sc2pair : stateChar2Set)
        //            addPreStarNeedMap(sc2pair.first.first, sc2pair.first.second, tState, sc2pair.second, needMap, need2Map);
        //    }
        //}
        //
        //
        //void addPostStarNeedMap(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap &needMap, Need2Map& need2Map)
        //{
        //    if(addNeedMap(sState, sc, tState, tc, needMap, need2Map))
        //        addPostStarTrans(sState, sc, tState, tc, needMap, need2Map);
        //}
        //
        //void addPostStarNeed2Map(NFAState* sState, Character sc, NFAState* tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map& need2Map)
        //{
        //    if(addNeed2Map(sState, sc, tState, tc1, tc2, needMap, need2Map))
        //    {
        //        NFAState* midState = mkNFAState();
        //        sState -> addNFATrans(sc, midState);
        //        addPostStarTrans(sState, sc, midState, needMap, need2Map);
        //        addPostStarNeedMap(midState, tc2, tState, tc1, needMap, need2Map);
        //    }
        //}
        //
        //void addPostStarTrans(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap& needMap, Need2Map& need2Map)
        //{
        //    NFAStateSet stateset = tState -> getTargetStateSetByChar(tc);
        //    for(NFAState* state : stateset)
        //        if(sState -> addNFATrans(sc, state))
        //            addPostStarTrans(sState, sc, state, needMap, need2Map);
        //}
        //
        //void addPostStarTrans(NFAState *sState, Character c, NFAState *tState, NeedMap& needMap, Need2Map& need2Map)
        //{
        //    Char2StateCharSetMap map = needMap[sState];
        //    Char2StateCharSetMapIter mapIt = map.find(c);
        //    if(mapIt != map.end())
        //    {
        //        StateCharSet stateCharSet = mapIt -> second;
        //        for(StateChar scpair : stateCharSet)
        //            if(scpair.first -> addNFATrans(scpair.second, tState))
        //                addPostStarTrans(scpair.first, scpair.second, tState, needMap, need2Map);
        //    }
        //}
    public:
        /// \brief Default construction function, sets initialState to nullptr.
        NFA() : FA(), initialState(nullptr){}

        /// \brief Construction function with alphabet.
        /// \param charSet The alphabet.
        NFA(const CharacterSet& charSet) : FA(charSet), initialState(NULL) {}

        /// \brief Copy construction function.
        /// \param nfa The copied NFA.
        NFA(const NFA& nfa) {
            if (!nfa.isNULL()) {
                this -> flag = nfa.flag;
                this -> setAlphabet(nfa.alphabet);
                NFAState* iniState = mkNFAInitialState();
                if (nfa.initialState -> isFinal())
                    addFinalState(iniState);
                NFAState2Map state2Map;
                state2Map[nfa.initialState] = iniState;
                cpTransByNFA(nfa.initialState, state2Map);
            }
        }

        //NFA(const NFA& nfa, NFAState2Map& state2Map) {
        //    if (!nfa.isNULL()) {
        //        this -> flag = nfa.flag;
        //        this -> setAlphabet(nfa.alphabet);
        //        NFAState* iniState = mkNFAInitialState();
        //        if (nfa.initialState -> isFinal())
        //            addFinalState(iniState);
        //        state2Map[nfa.initialState] = iniState;
        //        cpTransByNFA(nfa.initialState, state2Map);
        //    }
        //}

        /// \brief Copy construction function by DFA.
        /// \param dfa The copied DFA.
        NFA(const DFA& dfa) {
            if(!dfa.isNULL())
            {
                this -> flag = dfa.flag;
                this -> setAlphabet(dfa.alphabet);
                NFAState* iniState = mkNFAInitialState();
                if (dfa.initialState -> isFinal())
                    addFinalState(iniState);
                DFAState2NFAStateMap state2Map;
                state2Map[dfa.initialState] = iniState;
                cpTransByDFA(dfa.initialState, state2Map);
            }
        }

        /// \brief Copy construction function by regEx.
        /// \param regEx The regular expression.
        NFA(const string& regEx) {
            BasicRegEx<Character> basicRegEx(regEx);
            *this = *basicRegEx.mkNFA();
        }
        
        /// \brief Desconstruction function.
        ///
        /// delete all pointers of states for this NFA.
        ~NFA() {
            initialState = nullptr;
            for (NFAState* state : stateSet) {
                delete state;
            }
        }

        /// \brief Sets initialState to param state.
        /// \param state The state set to be initialState.
        void setInitialState(NFAState* state) {initialState = state;}

        /// \brief Adds param state to finalStateSet.
        /// \param state The state need to be added in finalStateSet.
        void addFinalState(NFAState* state) {
            finalStateSet.insert(state);
            state -> setFinalFlag(1);
        }

        /// \brief Gets stateSet.
        /// \return The set reference of NFAState pointer for this NFA.
        NFAStateSet& getStateSet() {return stateSet;}

        /// \brief Gets finalStateSet.
        /// \return The set reference of NFAState pointer for this NFA.
        NFAStateSet& getFinalStateSet() {return finalStateSet;}

        /// \brief Gets initialState.
        /// \return The NFAstate pointer of initialState for this NFA.
        NFAState* getInitialState() {return initialState;}

        /// \brief Gets stateSet, a const function.
        /// \return The const set reference of NFAState pointer for this NFA.
        const NFAStateSet& getStateSet() const {return stateSet;}

        /// \brief Gets finalStateSet, a const function.
        /// \return The const set reference of NFAState pointer for this NFA.
        const NFAStateSet& getFinalStateSet() const {return finalStateSet;}

        /// \brief Gets initialState, a const function.
        /// \return The const NFAState pointer of initialState for this NFA.
        const NFAState* getInitialState() const {return initialState;}

        /// \brief Removes all state in the finalStateSet for this NFA.
        void clearFinalStateSet()
        {
            for (NFAState* state : finalStateSet) {
                state -> setFinalFlag(0);
            }
            finalStateSet.clear();
        }

        /// \brief Checks whether given param stateSet has finalState.
        /// \param stateSet The NFAStateSet for checking.
        /// \return True means param stateSet has finalState, otherwise not.
        static bool hasFinalState(const NFAStateSet& stateSet)
        {
            for (NFAState* state : stateSet) {
                if (state -> isFinal()) return true;
            }
            return false;
        }

        /// \brief Checks whether all states in the given param stateSet are finalState.
        /// \param stateSet The NFAStateSet for checking.
        /// \return True means all states are finalState, otherwise not.
        static bool allFinalState(const NFAStateSet& stateSet)
        {
            for (NFAState* state : stateSet) {
                if (!state -> isFinal()) return false;
            }
            return true;
        }

        bool isNULL() const {
            if (!initialState) return true;
            if (finalStateSet.size() == 0) return true;
            return false;
        }

        /// \brief Makes a state in this NFA.
        /// \return A NFAState pointer made by this NFA.
        NFAState* mkNFAState() {
            NFAState *state = new NFAState();
            stateSet.insert(state);
            return state;
        }
        
        /// \brief Makes a initialState in this NFA.
        /// \return A NFAState pointer made by this NFA.
        NFAState* mkNFAInitialState() {
            initialState = mkNFAState();
            return initialState;
        }
        
        /// \brief Makes a finalState in this NFA.
        /// \return A NFAState pointer made by this NFA.
        NFAState* mkNFAFinalState() {
            NFAState *state = mkNFAState();
            state -> setFinalFlag(1);
            finalStateSet.insert(state);
            return state;
        }
        
        FA& operator & (const FA& fa) {
            if (isNULL() || fa.isNULL()) return FA::EmptyDFA();
            DFA& dfa1 = this -> determinize();
            const DFA& dfa2 = fa.determinize();
            FA& intersection = dfa1 & dfa2;
            if (!fa.isDeterministic()) delete &dfa2;
            delete &dfa1;
            return intersection;
        }

        FA& operator | (const FA& fa) {
            if (isNULL()) return const_cast<FA&>(fa);
            if (fa.isNULL()) return *this;
            NFA *nfa = new NFA();
            nfa -> setAlphabet(this -> alphabet);
            NFAState* iniState = nfa -> mkNFAInitialState();
            NFAState* state1 = nfa -> mkNFAState();
            NFAState* state2 = nfa -> mkNFAState();
            NFAState2Map state2Map1;
            state2Map1[initialState] = state1;
            if (initialState -> isFinal()) 
                nfa -> addFinalState(state1);
            nfa -> cpTransByNFA(initialState, state2Map1);
            if (fa.isDeterministic()) {
                DFAState2NFAStateMap state2Map2;
                DFA& tempDFA = const_cast<FA&>(fa).determinize();
                state2Map2[tempDFA.initialState] = state2;
                if (tempDFA.initialState -> isFinal()) 
                    nfa -> addFinalState(state2);
                nfa -> cpTransByDFA(tempDFA.initialState, state2Map2);
            } else {
                NFAState2Map state2Map2;
                NFA& tempNFA = const_cast<FA&>(fa).nondeterminize();
                state2Map2[tempNFA.initialState] = state2;
                if(tempNFA.initialState -> isFinal()) nfa -> addFinalState(state2);
                nfa -> cpTransByNFA(tempNFA.initialState, state2Map2);
            }
            iniState -> addEpsilonTrans(state1);
            iniState -> addEpsilonTrans(state2);
            return *nfa;
        }

        DFA& operator ! ( void ) {
            DFA& tempDFA = determinize();
            DFA& complement = (!tempDFA);
            delete &tempDFA;
            return complement;
        }

        DFA& determinize( void ) {
            if (isNULL()) return FA::EmptyDFA();
            DFA *dfa = new DFA(this -> alphabet);
            NFAStateSet epsilonClosure;
            initialState -> getEpsilonClosure(epsilonClosure);
            epsilonClosure.insert(initialState);
            NFAStateSet2DFAStateMap setMap;
            NFATransMap nfaTransMap;
            getTransMapByStateSet(epsilonClosure, nfaTransMap);
            DFAState *iniState = dfa -> mkDFAInitialState();
            if (NFA::hasFinalState(epsilonClosure)) dfa -> addFinalState(iniState);
            setMap[epsilonClosure] = iniState;
            mkDFATrans(iniState, setMap, nfaTransMap, dfa);
            dfa -> setReachableFlag(1);
            return *dfa;
        }

        const DFA& determinize( void ) const {
            return determinize();
        }
        
        NFA& nondeterminize( void ) {
            return *this;
        }

        const NFA& nondeterminize( void ) const {
            return *this;
        }
        
        FA& concat(const FA& fa) {
            if (isNULL()) return const_cast<FA&>(fa);
            if (fa.isNULL()) return *this;
            NFA *nfa = new NFA(*this);
            NFAState* state = nfa -> mkNFAState();
            for (NFAState* state : finalStateSet) {
                state -> addEpsilonTrans(state);
            }
            nfa -> clearFinalStateSet();
            if(fa.isDeterministic())
            {
                DFAState2NFAStateMap state2Map;
                DFA& tempDFA = const_cast<FA&>(fa).determinize();
                state2Map[tempDFA.initialState] = state;
                nfa -> cpTransByDFA(tempDFA.initialState, state2Map);
                if(tempDFA.initialState -> isFinal()) nfa -> addFinalState(state);
            }
            else
            {
                NFAState2Map state2Map;
                NFA& tempNFA = const_cast<FA&>(fa).nondeterminize();
                state2Map[tempNFA.initialState] = state;
                nfa -> cpTransByNFA(tempNFA.initialState, state2Map);
                if(tempNFA.initialState -> isFinal()) nfa -> addFinalState(state);
            }
            return *nfa;
        }
        FA& subset(const NFAState *iState, const NFAState *fState)
        {
            NFA *nfa = new NFA();
            if(isNULL()) return *nfa;
            nfa -> setAlphabet(this -> alphabet);
            NFAState* iniState = nfa -> mkNFAInitialState();
            NFAState2Map state2Map;
            state2Map[const_cast<State*>(iState)] = iniState;
            nfa -> makeCopyTransByNFA((const_cast<State*>(iState)), state2Map);
            nfa -> clearFinalStateSet();
            NFAState* nfaState = (state2Map[const_cast<State*>(fState)]);
            nfa -> addFinalState(nfaState);
            nfa -> removeDeadState();
            return *nfa;
        }
        FA& rightQuotient(Character character)
        {
            NFA* nfa = new NFA(*this);
            NFAStateSet finSteteSet;
            NFAStateSet tempSet;
            for (NFAState* state : nfa -> stateSet) {
                tempSet.clear();
                state -> getTargetStateSetByChar(tempSet, character);
                if(NFA::hasFinalState(tempSet))
                    finSteteSet.insert(state);
            }
            nfa -> clearFinalStateSet();
            for(NFAState* state : finSteteSet)
                nfa -> addFinalState(state);
            return *nfa;
        }
        
        FA& leftQuotient(Character character)
        {
            if(initialState -> getNFATransMap().count(character) == 0) return FA::EmptyNFA();
            NFA* nfa = new NFA(*this);
            NFAStateSet set;
            nfa -> initialState -> getTargetStateSetByChar(set, character);
            if(set.size() == 0) return FA::EmptyNFA();
            NFAState* iniState = nfa -> mkNFAInitialState();
            for(NFAState* state : set)
                iniState -> addEpsilonTrans(state);
            nfa -> removeUnreachableState();
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
            if(reachableStateSet.size() != this -> stateSet.size())
            {
                NFAStateSet set;
                for(NFAState* state : stateSet)
                    if(reachableStateSet.find(state) == reachableStateSet.end())
                    {
                        NFAStateSet targetStateSet = state -> getTargetStateSet();
                        for(NFAState* tState : targetStateSet)
                            if(reachableStateSet.find(tState) != reachableStateSet.end())
                                (state)->delNFATrans(tState);
                        set.insert(state);
                    }
                for(NFAState* state : set)
                {
                    stateSet.erase(state);
                    delete state;
                }
            }
            this -> setReachableFlag(1);
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
            for(NFAState* state : stateSet)
                if(liveStateSet.find(state) == liveStateSet.end())
                {
                    NFAStateSet sourceStateSet = reverseMap.find(state) -> second;
                    for(NFAState* sState : sourceStateSet)
                        if(liveStateSet.find(sState) != liveStateSet.end())
                            (sState)->delNFATrans(state);
                    set.insert(state);
                }
            for(NFAState* state : set)
            {
                stateSet.erase(state);
                delete state;
            }
        }
        //
        //        Word getOneRun();
        bool isAccepted(const Word &word)//accepted
        {
            if(isNULL()) return false;
            NFAStateSet workSet;
            workSet.insert(initialState);
            for(Character c : word)
            {
                if(workSet.size() > 0)
                {
                    NFAStateSet set;
                    for(NFAState* state : workSet)
                        state -> getTargetStateSetByChar(set, c);
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
            initialState -> getTargetStateSetByChar(set, character);
            if(set.size() == 0) return false;
            if(NFA::hasFinalState(set)) return true;
            return false;
        }
        
        bool isEmpty()
        {
            if(isNULL()) return true;
            if(!this -> isReachable()) removeUnreachableState();
            if(finalStateSet.size() == 0) return true;
            return false;
        }
        
        //NFA& preStar(const PDS& pds, PDSState2NFAStateMap& state2Map)
        //{
        //    NFAState2Map copyMap;
        //    NFA* nfa = new NFA(*this, copyMap);
        //    NeedMap needMap;
        //    Need2Map need2Map;
        //    PDSStateSet tempSet;
        //    for(PDSState2NFAStateMapIter it = state2Map.begin(); it != state2Map.end(); it++)
        //        tempSet.insert(it -> first);
        //    for(PDSState* state : pds.getStateSet())
        //    {
        //        if(tempSet.find(state) == tempSet.end())
        //            state2Map[state] = nfa -> mkNFAState();
        //        else
        //            state2Map[state] = copyMap[state2Map[state]];
        //    }
        //    
        //    for(PDSTrans* trans : pds.pdsPopTransList)
        //    {
        //        NFAState* sourceState = state2Map[trans -> getSourceState()];
        //        NFAState* targetState = state2Map[trans -> getTargetState()];
        //        Character character = trans -> getChar();
        //        sourceState -> addNFATrans(character, targetState);
        //    }
        //    for(ReplacePDSTrans* trans : pds.pdsReplaceTransList)
        //    {
        //        NFAState* sourceState = state2Map[trans -> getSourceState()];
        //        NFAState* targetState = state2Map[trans -> getTargetState()];
        //        Character character = trans -> getChar();
        //        Character stack = trans -> getStack();
        //        nfa -> addPreStarNeedMap(sourceState, character, targetState, stack, needMap, need2Map);
        //    }
        //    for(PushPDSTrans* trans : pds.pdsPushTransList)
        //    {
        //        NFAState* sourceState = state2Map[trans -> getSourceState()];
        //        NFAState* targetState = state2Map[trans -> getTargetState()];
        //        Character character = trans -> getChar();
        //        Char2 stack = trans -> getStack();
        //        nfa -> addPreStarNeed2Map(sourceState, character, targetState, stack.first, stack.second, needMap, need2Map);
        //    }
        //    nfa -> removeUnreachableState();
        //    nfa -> removeDeadState();
        //    return *nfa;
        //}
        //
        //NFA& postStar(const PDS& pds, PDSState2NFAStateMap& state2Map)//todo
        //{
        //    NFAState2Map copyMap;
        //    NFA* nfa = new NFA(*this, copyMap);
        //    NeedMap needMap;
        //    Need2Map need2Map;
        //    PDSStateSet tempSet;
        //    for(PDSState2NFAStateMapIter it = state2Map.begin(); it != state2Map.end(); it++)
        //        tempSet.insert(it -> first);
        //    for(PDSState* state : pds.getStateSet())
        //    {
        //        if(tempSet.find(state) == tempSet.end())
        //            state2Map[state] = nfa -> mkNFAState();
        //        else
        //            state2Map[state] = copyMap[state2Map[state]];
        //    }
        //    
        //    for(PDSTrans* trans : pds.pdsPopTransList)
        //    {
        //        NFAState* sourceState = state2Map[trans -> getSourceState()];
        //        NFAState* targetState = state2Map[trans -> getTargetState()];
        //        Character character = trans -> getChar();
        //        nfa -> addPostStarNeedMap(targetState, Global::epsilon, sourceState, character, needMap, need2Map);
        //        
        //    }
        //    for(ReplacePDSTrans* trans : pds.pdsReplaceTransList)
        //    {
        //        NFAState* sourceState = state2Map[trans -> getSourceState()];
        //        NFAState* targetState = state2Map[trans -> getTargetState()];
        //        Character character = trans -> getChar();
        //        Character stack = trans -> getStack();
        //        nfa -> addPostStarNeedMap(targetState, stack, sourceState, character, needMap, need2Map);
        //    }
        //    for(PushPDSTrans* trans : pds.pdsPushTransList)
        //    {
        //        NFAState* sourceState = state2Map[trans -> getSourceState()];
        //        NFAState* targetState = state2Map[trans -> getTargetState()];
        //        Character character = trans -> getChar();
        //        Char2 stack = trans -> getStack();
        //        nfa -> addPostStarNeed2Map(targetState, stack.first, sourceState, character, stack.second, needMap, need2Map);
        //    }
        //    nfa -> removeUnreachableState();
        //    nfa -> removeDeadState();
        //    return *nfa;
        //}
        
        
    
        void output()const{
            if(!initialState) return;
            cout<<initialState -> getID()<<endl;
            for(auto it = stateSet.begin(); it != stateSet.end(); it++)
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
            f << "Q"<<initialState -> getID() << "[color=blue];\n";
            
            
            // cout final states
            for (auto iter = finalStateSet.begin(); iter != finalStateSet.end(); iter++) {
                f << "Q" << (*iter) -> getID() << " [shape=doublecircle];\n";
            }
            f << ";\n";
            
            
            // cout trisitions
            for(auto iter = stateSet.begin(); iter != stateSet.end(); iter++)
            {
                NFATransMap& transMap = (*iter)->getNFATransMap();
                ID id = (*iter)->getID();
                for (auto iter = transMap.begin(); iter != transMap.end(); iter++) {
                    for (auto iter1 = iter -> second.begin(); iter1 != iter -> second.end(); iter1++) {
                        f << "Q" << id <<  " -> " << "Q" << (*iter1)->getID() << "[label=\"" << iter -> first <<"\"];\n";
                    }
                }
            }
            f <<"}\n";
            f.close();
        }
        friend FA;
        friend BasicRegEx<Character>;
        
    };
}

#endif /* NFA_hpp */

