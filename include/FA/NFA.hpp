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
#include "../PDS/PDS.hpp"
#include "RegExp.hpp"

namespace cgh {
    
    /// \brief A class of Nondeterministic Finite Automaton.
    template <class Character>
    class NFA : public FA<Character>
    {
        typedef typename Alias4Char<Character>::Word Word;
        typedef typename Alias4Char<Character>::Char2 Char2;
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4FA<Character>::NFAStates NFAStates;
        typedef typename Alias4FA<Character>::NFATransMap NFATransMap;
        typedef typename Alias4FA<Character>::NFAState2Map NFAState2Map;
        typedef typename Alias4FA<Character>::DFAState2NFAStateMap DFAState2NFAStateMap;
        typedef typename Alias4FA<Character>::NFAStates2DFAStateMap NFAStates2DFAStateMap;
        typedef typename Alias4FA<Character>::NFAState2NFAStatesMap NFAState2NFAStatesMap;
        typedef typename Alias4PDS<Character>::PDSState2NFAStateMap PDSState2NFAStateMap;
        typedef typename Alias4PDS<Character>::PDSStates PDSStates;
        typedef typename Alias4FA<Character>::StateChar StateChar;
        typedef typename Alias4FA<Character>::StateChar2 StateChar2;
        typedef typename Alias4FA<Character>::StateChars StateChars;
        typedef typename Alias4FA<Character>::StateChar2s StateChar2s;
        typedef typename Alias4FA<Character>::Char2StateCharsMap Char2StateCharsMap;
        typedef typename Alias4FA<Character>::Char2StateChar2sMap Char2StateChar2sMap;
        typedef typename Alias4FA<Character>::NeedMap NeedMap;
        typedef typename Alias4FA<Character>::Need2Map Need2Map;
        typedef typename Alias4FA<Character>::PostStarMap PostStarMap;

    protected:
        NFAState<Character>* initialState;      ///< The initial state for this NFA.
        NFAStates states;                       ///< The set of states for this NFA.
        NFAStates finalStates;                  ///< The set of final states for this NFA.

        void cpTransByDFA(DFAState<Character>* state, DFAState2NFAStateMap& state2Map) {
            FA<Character>::cpNFATransByDFA(this, state, state2Map);
        }

        void cpTransByNFA(NFAState<Character>* state, NFAState2Map& state2Map) {
            FA<Character>::cpNFATransByNFA(this, state, state2Map);
        }

        void getReachableStates(NFAStates& reachableStates, NFAStates& works) const {
            NFAStates targetStates, newWorks;
            for (NFAState<Character>* state : works) {
                targetStates.clear();
                state -> getTargetStates(targetStates);
                for (NFAState<Character>* targetState : targetStates) {
                    if (reachableStates.insert(targetState).second)
                        newWorks.insert(targetState);
                }
            }
            if (newWorks.size() == 0) return;
            getReachableStates(reachableStates, newWorks);
        }
        
        void getReverseMap(NFAState2NFAStatesMap& reverseMap) {
            for (NFAState<Character>* state : states) {
                for (auto& mapPair : state -> getTransMap()) {
                    for (NFAState<Character>* nfaState : mapPair.second) {
                        reverseMap[nfaState].insert(state);
                    }
                }
            }
        }
        
        void getLiveStates(const NFAState2NFAStatesMap& reverseMap, NFAStates& liveStates, NFAStates& works) {
            NFAStates newWorks;
            for (NFAState<Character>* state : works) {
                auto mapIt = reverseMap.find(state);
                if (mapIt != reverseMap.end()) {
                    for (NFAState<Character>* reverseState : mapIt -> second) {
                        if (liveStates.insert(reverseState).second) {
                            newWorks.insert(reverseState);
                        }
                    }
                }
            }
            if (works.size() == 0) return;
            getLiveStates(reverseMap, liveStates, newWorks);
        }

        void getTransMapByStates(const NFAStates& states, NFATransMap& nfaTransMap) const {
            for (NFAState<Character>* state : states) {
                NFAStates works;
                for (auto& mapPair : state -> getTransMap()) {
                    if (mapPair.first != FA<Character>::epsilon) {
                        works.clear();
                        state -> getTargetStatesByChar(works, mapPair.first);
                        if (works.size() > 0)
                            nfaTransMap[mapPair.first].insert(works.begin(), works.end());
                    }
                }
            }
        }
        
        void determinize(DFA<Character>* dfa) const {
            NFAStates epsilonClosure;
            initialState -> getEpsilonClosure(epsilonClosure);
            epsilonClosure.insert(initialState);
            NFAStates2DFAStateMap setMap;
            determinize(dfa, dfa -> mkInitialState(), epsilonClosure, setMap);
            dfa -> setReachableFlag(1);
            Manage::manage(dfa);
        }

        void determinize(DFA<Character>* dfa, DFAState<Character>* sourceState, const NFAStates& nfaStates, NFAStates2DFAStateMap &setMap) const {
            if (hasFinalState(nfaStates)) dfa -> addFinalState(sourceState);
            NFATransMap transMap;
            getTransMapByStates(nfaStates, transMap);
            for (auto& mapPair : transMap) {
                auto setMapIt = setMap.find(mapPair.second);
                DFAState<Character>* targetState = nullptr;
                if (setMapIt == setMap.end()) {
                    targetState = dfa -> mkState();
                    setMap[mapPair.second] = targetState;
                    determinize(dfa, targetState, mapPair.second, setMap);
                } else {
                    targetState = setMapIt -> second;
                }
                sourceState -> addTrans(mapPair.first, targetState);
            }
        }
        
        void addPreStarNeedMap(NFAState<Character>* sState, Character sc, NFAState<Character>* tState, Character tc, NeedMap &needMap, Need2Map& need2Map) {
            if (needMap[tState][tc].insert(StateChar(sState, sc)).second)
                addPreStarTrans(sState, sc, tState, tc, needMap, need2Map);
        }
        
        void addPreStarNeed2Map(NFAState<Character>* sState, Character sc, NFAState<Character>* tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map& need2Map) {
            NFAStates states;
            tState -> getTargetStatesByChar(states, tc1);
            for (NFAState<Character>* state : states)
                addPreStarNeedMap(sState, sc, state, tc2, needMap, need2Map);
        }
        
        void addPreStarTrans(NFAState<Character>* sState, Character sc, NFAState<Character>* tState, Character tc, NeedMap& needMap, Need2Map& need2Map) {
            NFAStates states;
            tState -> getTargetStatesByChar(states, tc);
            for (NFAState<Character>* state : states)
                if (sState -> addTrans(sc, state))
                    addPreStarTrans(sState, sc, state, needMap, need2Map);
        }

        void addPreStarTrans(NFAState<Character>* sState, Character c, NFAState<Character>* tState, NeedMap& needMap, Need2Map& need2Map) {
            auto& stateChars = needMap[sState][c];
            for (StateChar stateChar: stateChars) {
                NFAState<Character>* state = stateChar.first;
                Character character = stateChar.second;
                if (state -> addTrans(character, tState)) {
                    addPreStarTrans(state, character, tState, needMap, need2Map);
                }
            }
            auto& stateChar2s = need2Map[sState][c];
            for (StateChar2 stateChar2: stateChar2s) {
                addPreStarNeedMap(stateChar2.first, stateChar2.second.first, tState, stateChar2.second.second, needMap, need2Map);
            }
        }
        
        void addPostStarNeedMap(NFAState<Character>* sState, Character sc, NFAState<Character>* tState, Character tc, NeedMap &needMap, Need2Map& need2Map, PostStarMap& postStarMap) {
            if (needMap[sState][sc].insert(StateChar(tState, tc)).second)
                addPostStarTrans(sState, sc, tState, tc, needMap, need2Map, postStarMap);
        }
        
        void addPostStarNeed2Map(NFAState<Character>* sState, Character sc, NFAState<Character>* tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map& need2Map, PostStarMap& postStarMap) {
            if (need2Map[sState][sc].insert(StateChar2(tState, Char2(tc1, tc2))).second) {
                NFAState<Character>* midState = postStarMap[StateChar(tState, tc1)];
                if (!midState) {
                    midState = mkState();
                    postStarMap[StateChar(tState, tc1)] = midState;
                }
                NFAStates states;
                sState -> getTargetStatesByChar(states, sc);
                if (states.size() > 0) {
                    for (NFAState<Character>* state : states) {
                        if (midState -> addTrans(tc2, state)) {
                            addPostStarTrans(midState, tc2, state, needMap, need2Map, postStarMap);
                        }
                    }
                    if (tState -> addTrans(tc1, midState)) {
                        addPostStarTrans(tState, tc1, midState, needMap, need2Map, postStarMap);
                    }
                }
            }
        }
        
        void addPostStarTrans(NFAState<Character>* sState, Character sc, NFAState<Character>* tState, Character tc, NeedMap& needMap, Need2Map& need2Map, PostStarMap& postStarMap) {
            NFAStates states;
            sState -> getTargetStatesByChar(states, sc);
            for (NFAState<Character>* state : states) {
                if (tState -> addTrans(tc, state)) {
                    addPostStarTrans(tState, tc, state, needMap, need2Map, postStarMap);
                }
            }
        }
        
        void addPostStarTrans(NFAState<Character>* sState, Character c, NFAState<Character>* tState, NeedMap& needMap, Need2Map& need2Map, PostStarMap& postStarMap) {
            NFATransMap workMap;
            if (c == FA<Character>::epsilon) {
                tState -> getTargetMap(workMap);
            } else { 
                NFAStates epsilonStates;
                tState -> getEpsilonClosure(epsilonStates);
                epsilonStates.insert(tState);
                workMap[c].insert(epsilonStates.begin(), epsilonStates.end());
            }
            for (auto& mapPair : workMap) {
                Character c = mapPair.first;
                if (c == FA<Character>::epsilon) continue;
                StateChars& stateChars = needMap[sState][c];
                for (NFAState<Character>* tState : mapPair.second) {
                    for (StateChar stateChar: stateChars) {
                        NFAState<Character>* state = stateChar.first;
                        Character character = stateChar.second;
                        if (state -> addTrans(character, tState)) {
                            addPostStarTrans(state, character, tState, needMap, need2Map, postStarMap);
                        }
                    }
                }
                StateChar2s& stateChar2s = need2Map[sState][c];
                for (NFAState<Character>* tState : mapPair.second) {
                    for (StateChar2 stateChar2: stateChar2s) {
                        NFAState<Character>* state = stateChar2.first;
                        Char2 char2 = stateChar2.second;
                        NFAState<Character>* midState = postStarMap[StateChar(state, char2.first)];
                        if (state -> addTrans(char2.first, midState)) {
                            addPostStarTrans(state, char2.first, midState, needMap, need2Map, postStarMap);
                        }
                        if (midState -> addTrans(char2.second, tState)) {
                            addPostStarTrans(midState, char2.second, tState, needMap, need2Map, postStarMap);
                        }
                    }
                }
            }
        }

        void mkPDSState2Map(NFA* nfa, const PDS<Character>& pds, NFAState2Map& copyMap, PDSState2NFAStateMap& state2Map) {
            PDSStates pdsStates;
            for (auto& mapPair : state2Map)
                pdsStates.insert(mapPair.first);
            for (PDSState* state : pds.getStates()) {
                if (pdsStates.find(state) == pdsStates.end()) {
                    state2Map[state] = nfa -> mkState();
                } else {
                    state2Map[state] = copyMap[state2Map[state]];
                }
            }
        }

        void postStar(NFA& nfa, const PDS<Character>& pds, PDSState2NFAStateMap& state2Map, PostStarMap& postStarMap) {
            NeedMap needMap;
            Need2Map need2Map;
            for (PushPDSTrans<Character>* trans : pds.getPushTransList()) {
                NFAState<Character>* sourceState = state2Map[trans -> getSourceState()];
                NFAState<Character>* targetState = state2Map[trans -> getTargetState()];
                Character character = trans -> getChar();
                Char2& stack = trans -> getStack();
                nfa.addPostStarNeed2Map(sourceState, character, targetState, stack.first, stack.second, needMap, need2Map, postStarMap);
            }

            for (PopPDSTrans<Character>* trans : pds.getPopTransList()) {
                NFAState<Character>* sourceState = state2Map[trans -> getSourceState()];
                NFAState<Character>* targetState = state2Map[trans -> getTargetState()];
                Character character = trans -> getChar();
                nfa.addPostStarNeedMap(sourceState, character, sourceState, FA<Character>::epsilon, needMap, need2Map, postStarMap);
            }

            for (ReplacePDSTrans<Character>* trans : pds.getReplaceTransList()) {
                NFAState<Character>* sourceState = state2Map[trans -> getSourceState()];
                NFAState<Character>* targetState = state2Map[trans -> getTargetState()];
                Character character = trans -> getChar();
                Character stack = trans -> getStack();
                nfa.addPostStarNeedMap(sourceState, character, targetState, stack, needMap, need2Map, postStarMap);
            }
        }

    public:
        /// \brief Default construction function, sets initialState to nullptr.
        NFA() : FA<Character>(), initialState(nullptr) {
        }

        /// \brief Construction function with alphabet.
        /// \param chars The alphabet.
        NFA(const Characters& chars) : FA<Character>(chars), initialState(nullptr) {
        }

        /// \brief Copy construction function.
        /// \param nfa The copied NFA.
        NFA(const NFA& nfa) {
            mkInitialState();
            if (!nfa.isEmpty()) {
                this -> flag = nfa.flag;
                this -> setAlphabet(nfa.alphabet);
                NFAState2Map state2Map;
                state2Map[nfa.initialState] = initialState;
                cpTransByNFA(nfa.initialState, state2Map);
            }
        }

        /// \brief Copy construction function with state2Map.
        /// \param nfa The copied NFA.
        /// \param state2Map Records the map for outside.
        NFA(const NFA& nfa, NFAState2Map& state2Map) {
            mkInitialState();
            if (!nfa.isEmpty()) {
                this -> flag = nfa.flag;
                this -> setAlphabet(nfa.alphabet);
                state2Map[nfa.initialState] = initialState;
                cpTransByNFA(nfa.initialState, state2Map);
            }
        }

        /// \brief Copy construction function by DFA.
        /// \param dfa The copied DFA.
        NFA(const DFA<Character>& dfa) {
            mkInitialState();
            if(!dfa.isEmpty()) {
                this -> flag = dfa.flag;
                this -> setAlphabet(dfa.alphabet);
                DFAState2NFAStateMap state2Map;
                state2Map[dfa.initialState] = initialState;
                cpTransByDFA(dfa.initialState, state2Map);
            }
        }

        /// \brief Construction function by regEx.
        /// \param regEx The regular expression.
        NFA(const string& regEx) {
            RegExp<Character> regExp(regEx);
            *this = *regExp.mkNFA();
        }

        /// \brief Desconstruction function.
        ///
        /// delete all pointers of states for this NFA.
        ~NFA() {
            initialState = nullptr;
            for (NFAState<Character>* state : states) {
                delete state;
            }
        }

        /// \brief Copies self and return.
        /// \return A reference of FA.
        FA<Character>& copy() {
            return *(new NFA(*this));
        }

        /// \brief Sets initialState to param state.
        /// \param state The state set to be initialState.
        void setInitialState(NFAState<Character>* state) {
            initialState = state;
        }

        /// \brief Adds param state to finalStates.
        /// \param state The state to be added in finalStates.
        void addFinalState(NFAState<Character>* state) {
            finalStates.insert(state);
            state -> setFinalFlag(1);
        }

        /// \brief Deletes param state from finalStates.
        /// \param state The state to be deleted from finalStates.
        void delFinalState(NFAState<Character>* state) {
            state -> setFinalFlag(0);
            finalStates.erase(state);
        }

        /// \brief Gets states.
        /// \return The set reference of NFAState pointer for this NFA.
        NFAStates& getStates() {
            return states;
        }

        /// \brief Gets finalStates.
        /// \return The set reference of NFAState pointer for this NFA.
        NFAStates& getFinalStates() {
            return finalStates;
        }

        /// \brief Gets initialState.
        /// \return The NFAstate pointer of initialState for this NFA.
        NFAState<Character>* getInitialState() {
            return initialState;
        }

        /// \brief Gets states, a const function.
        /// \return The const set reference of NFAState pointer for this NFA.
        const NFAStates& getStates() const {
            return states;
        }

        /// \brief Gets finalStates, a const function.
        /// \return The const set reference of NFAState pointer for this NFA.
        const NFAStates& getFinalStates() const {
            return finalStates;
        }

        /// \brief Gets initialState, a const function.
        /// \return The const NFAState pointer of initialState for this NFA.
        NFAState<Character>* getInitialState() const {
            return initialState;
        }

        /// \brief Removes all state in the finalStates for this NFA.
        void clearFinalStates() {
            for (NFAState<Character>* state : finalStates) {
                state -> setFinalFlag(0);
            }
            finalStates.clear();
        }

        /// \brief Checks whether given param states has finalState.
        /// \param states The NFAStates for checking.
        /// \return True means param states has finalState, otherwise not.
        static bool hasFinalState(const NFAStates& states) {
            for (NFAState<Character>* state : states) {
                if (state -> isFinal()) return true;
            }
            return false;
        }

        /// \brief Checks whether all states in the given param states are finalState.
        /// \param states The NFAStates for checking.
        /// \return True means all states are finalState, otherwise not.
        static bool allFinalState(const NFAStates& states) {
            for (NFAState<Character>* state : states) {
                if (!state -> isFinal()) return false;
            }
            return true;
        }

        /// \brief Makes a state in this NFA.
        /// \return A NFAState pointer made by this NFA.
        virtual NFAState<Character>* mkState() {
            NFAState<Character>* state = new NFAState<Character>();
            states.insert(state);
            return state;
        }

        /// \brief Deletes a state in this DFA.
        /// \return boolean.
        virtual void delState(NFAState<Character>* state) {
            states.erase(state);
            finalStates.erase(state);
            if (initialState == state) mkInitialState();
            delete state;
        }
        
        /// \brief Makes a initialState in this NFA.
        /// \return A NFAState pointer made by this NFA.
        virtual NFAState<Character>* mkInitialState() {
            initialState = mkState();
            return initialState;
        }
        
        /// \brief Makes a finalState in this NFA.
        /// \return A NFAState pointer made by this NFA.
        virtual NFAState<Character>* mkFinalState() {
            NFAState<Character>* state = mkState();
            state -> setFinalFlag(1);
            finalStates.insert(state);
            return state;
        }

        //DFA<Character>& minimize( void ) {
        //    return determinize().minimize();
        //}

        DFA<Character>& minimize( void ) const {
            return determinize().minimize();
        }

        //virtual DFA<Character>& determinize( void ) {
        //    if (isEmpty()) return FA<Character>::EmptyDFA();
        //    DFA<Character>* dfa = new DFA<Character>(this -> alphabet);
        //    determinize(dfa);
        //    return *dfa;
        //}

        virtual DFA<Character>& determinize( void ) const {
            if (isEmpty()) return FA<Character>::EmptyDFA();
            DFA<Character>* dfa = new DFA<Character>(this -> alphabet);
            determinize(dfa);
            return *dfa;
        }
        
        FA<Character>& subset(NFAState<Character>* iState, NFAState<Character>* fState) {
            if (isEmpty()) return FA<Character>::EmptyNFA();
            NFA* nfa = new NFA(this -> alphabet);
            NFAState<Character>* iniState = nfa -> mkInitialState();
            NFAState2Map state2Map;
            state2Map[iState] = iniState;
            nfa -> cpTransByNFA(iState, state2Map);
            nfa -> clearFinalStates();
            NFAState<Character>* nfaState = (state2Map[fState]);
            nfa -> addFinalState(nfaState);
            nfa -> removeDeadState();
            return *nfa;
        }

        void removeUnreachableState() {
            if (finalStates.size() == 0) return;
            if (this -> isReachable()) return;
            NFAStates reachableStates, works;
            works.insert(initialState);
            reachableStates.insert(initialState);
            getReachableStates(reachableStates, works);
            if (!NFA::hasFinalState(reachableStates)) {
                clearFinalStates();
                return;
            }
            if (reachableStates.size() != this -> states.size()) {
                NFAStates dels;
                for (NFAState<Character>* state : states) {
                    if (reachableStates.count(state) == 0) {
                        NFAStates targetStates = state -> getTargetStates();
                        for (NFAState<Character>* tState : targetStates) {
                            if (reachableStates.count(tState) > 0)
                                state -> delNFATrans(tState);
                        }
                        dels.insert(state);
                    }
                }
                for (NFAState<Character>* state : dels) {
                    delState(state);
                }
            }
            this -> setReachableFlag(1);
        }

        void removeDeadState() {
            if (isEmpty()) return;
            NFAState2NFAStatesMap reverseMap;
            getReverseMap(reverseMap);
            NFAStates liveStates(finalStates.begin(), finalStates.end());
            getLiveStates(reverseMap, liveStates, finalStates);
            if (liveStates.count(initialState) == 0) {
                clearFinalStates();
                return;
            }
            NFAStates dels;
            for(NFAState<Character>* state : states) {
                if (liveStates.count(state) == 0) {
                    NFAStates sourceStates = reverseMap.find(state) -> second;
                    for (NFAState<Character>* sState : sourceStates) {
                        if (liveStates.count(sState) > 0)
                            sState -> delNFATrans(state);
                    }
                    dels.insert(state);
                }
            }
            for (NFAState<Character>* state : dels) {
                delState(state);
            }
        }

        //        Word getOneRun();
        
        bool isAccepted(const Word &word) {
            if (isEmpty()) return false;
            NFAStates works;
            works.insert(initialState);
            for (Character c : word) {
                if (works.size() > 0) {
                    NFAStates newWorks;
                    for(NFAState<Character>* state : works)
                        state -> getTargetStatesByChar(newWorks, c);
                    works.clear();
                    if (newWorks.size() > 0) {
                        works.insert(newWorks.begin(), newWorks.end());
                    } else {
                        return false;
                    }
                }
            }
            if (works.size() == 0) return false;
            if (NFA::hasFinalState(works)) return true;
            return false;
        }

        bool isAccepted(Character character) {
            if (isEmpty()) return false;
            NFAStates works;
            initialState -> getTargetStatesByChar(works, character);
            if (works.size() == 0) return false;
            if (NFA::hasFinalState(works)) return true;
            return false;
        }
        
        //bool isEmpty() {
        //    if (this -> getAlphabet().size() == 0) return false;
        //    if (!this -> isReachable()) removeUnreachableState();
        //    if (finalStates.size() == 0) return true;
        //    return false;
        //}

        bool isEmpty() const {
            if (this -> getAlphabet().size() == 0) return true;
            if (getFinalStates().size() == 0) return true;
            if (!this -> isReachable()) {
                NFAStates states;
                NFAStates works;
                works.insert(initialState);
                getReachableStates(states, works);
                if (!hasFinalState(states)) return true;
            }
            return false;
        }
        
        /// \brief Gets the pre* NFA from this configuration NFA by givin param pds.
        /// \param pds Provides the rules for pre*.
        /// \param state2Map Records the relation between pds and nfa.
        /// \return A reference of NFA.
        NFA& preStar(const PDS<Character>& pds, PDSState2NFAStateMap& state2Map) {
            NFAState2Map copyMap;
            NFA* nfa = new NFA(*this, copyMap);
            NeedMap needMap;
            Need2Map need2Map;
            mkPDSState2Map(nfa, pds, copyMap, state2Map);
            
            for (PopPDSTrans<Character>* trans : pds.getPopTransList()) {
                NFAState<Character>* sourceState = state2Map[trans -> getSourceState()];
                NFAState<Character>* targetState = state2Map[trans -> getTargetState()];
                Character character = trans -> getChar();
                sourceState -> addTrans(character, targetState);
            }

            for (ReplacePDSTrans<Character>* trans : pds.getReplaceTransList()) {
                NFAState<Character>* sourceState = state2Map[trans -> getSourceState()];
                NFAState<Character>* targetState = state2Map[trans -> getTargetState()];
                Character character = trans -> getChar();
                Character stack = trans -> getStack();
                nfa -> addPreStarNeedMap(sourceState, character, targetState, stack, needMap, need2Map);
            }

            for (PushPDSTrans<Character>* trans : pds.getPushTransList()) {
                NFAState<Character>* sourceState = state2Map[trans -> getSourceState()];
                NFAState<Character>* targetState = state2Map[trans -> getTargetState()];
                Character character = trans -> getChar();
                Char2 stack = trans -> getStack();
                nfa -> addPreStarNeed2Map(sourceState, character, targetState, stack.first, stack.second, needMap, need2Map);
            }
            Manage::manage(nfa);
            return *nfa;
        }
        
        /// \brief Gets the post* NFA from this configuration NFA by givin param pds.
        /// \param pds Provides the rules for post*.
        /// \param state2Map Records the relation between pds and nfa.
        /// \return A reference of NFA.
        void postStar(const PDS<Character>& pds, PDSState2NFAStateMap& state2Map, PostStarMap& postStarMap) {
            postStar(*this, pds, state2Map, postStarMap);
        }

        NFA& postStar(const PDS<Character>& pds, PDSState2NFAStateMap& state2Map) {
            NFAState2Map copyMap;
            NFA* nfa = new NFA(*this, copyMap);
            PostStarMap postStarMap;
            mkPDSState2Map(nfa, pds, copyMap, state2Map);
            postStar(nfa, pds, state2Map, postStarMap);
            Manage::manage(nfa);
            return *nfa;
        }
    
        void output()const{
            if(!initialState) return;
            cout<<initialState -> getID()<<endl;
            for(auto it = states.begin(); it != states.end(); it++)
            {
                if((*it)->isFinal()) cout<<"$"<<(*it)->getID()<<endl;;
                dynamic_cast<NFAState<Character>*>((*it))->output();
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
            for (auto iter = finalStates.begin(); iter != finalStates.end(); iter++) {
                f << "Q" << (*iter) -> getID() << " [shape=doublecircle];\n";
            }
            f << ";\n";
            
            
            // cout trisitions
            for(auto iter = states.begin(); iter != states.end(); iter++)
            {
                NFATransMap& transMap = (*iter)->getTransMap();
                ID id = (*iter)->getID();
                for (auto iter = transMap.begin(); iter != transMap.end(); iter++) {
                    for (auto iter1 = iter -> second.begin(); iter1 != iter -> second.end(); iter1++) {
                        f << "Q" << id <<  " -> " << "Q" << (*iter1)->getID() << "[label=\"" << iter -> first <<"\"];\n";
                    }
                }
            }
            f <<"}\n";
            f.close();
            system("dot -Tpng -o res.png res.dot");
        }
        friend FA<Character>;
        //friend BasicRegEx<Character>;
        
    };
}

#endif /* NFA_hpp */

