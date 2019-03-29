//
//  TrNFA.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef TrNFA_hpp
#define TrNFA_hpp

#include "TrNFAState.hpp"

namespace cgh {
    template<class Character>
    class TrNFA {
        typedef typename Alias4Char<Character>::Word Word;
        typedef typename Alias4Char<Character>::Char2 Char2;
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4TrNFA<Character>::TrNFAStates TrNFAStates;
        typedef typename Alias4TrNFA<Character>::TrNFAState2Map TrNFAState2Map;
        typedef typename Alias4TrNFA<Character>::DFT2TrNFAStatesMap DFT2TrNFAStatesMap;
        typedef typename Alias4TrNFA<Character>::TrPDSState2TrNFAStateMap TrPDSState2TrNFAStateMap;

        typedef pair<Character, DFT<Character>*> TrNFALabel;
        typedef pair<Char2, TrNFALabel> TrNFALabel2;
        typedef pair<TrNFAState<Character>*, TrNFALabel> StateLabel;
        typedef pair<TrNFAState<Character>*, TrNFALabel2> StateLabel2;
        typedef set<StateLabel> StateLabels;
        typedef set<StateLabel2> StateLabel2s;
        typedef unordered_map<Character, StateLabels> Char2StateLabelsMap;
        typedef unordered_map<Character, StateLabel2s> Char2StateLabel2sMap;
        typedef unordered_map<TrNFAState<Character>*, Char2StateLabelsMap> NeedMap;
        typedef unordered_map<TrNFAState<Character>*, Char2StateLabel2sMap> Need2Map;
    private:
        TrNFAState<Character>* initialState;      ///< The initial state for this TrNFA.
        TrNFAStates states;                       ///< The set of states for this TRNFA.
        TrNFAStates finalStates;                  ///< The set of final states for this TrNFA.
        Characters alphabet;                      ///< A set of characters which in the label on the transitions.

        void cpTrans(TrNFAState<Character>* state, TrNFAState2Map& state2map) {
            TrNFAState<Character>* sourceState = state2map[state];
            if (state -> isFinal()) addFinalState(sourceState);
            for (auto& mapPair1 : state -> getTransMap()) {
                for (auto& mapPair2 : mapPair1.second) {
                    for (TrNFAState<Character>* state : mapPair2.second){
                        TrNFAState<Character>* targetState = nullptr;
                        auto state2MapIt = state2map.find(state);
                        if (state2MapIt == state2map.end()) {
                            targetState =  mkState();
                            state2map[state] = targetState;
                            cpTrans(state, state2map);
                        } else {
                            targetState = state2MapIt -> second;
                        }
                        sourceState -> addTrans(mapPair1.first, mapPair2.first, targetState);
                    }
                }
            }
        }

        

        bool addNeedMap(TrNFAState<Character>* sState, Character sc, TrNFAState<Character>* tState, Character tc, DFT<Character>* dft, NeedMap &needMap) {
            return needMap[tState][tc].insert(StateLabel(sState, TrTrNFALabel(sc, dft))).second;
        }

        bool addNeed2Map(TrNFAState<Character>* sState, Character sc, TrNFAState<Character>* tState, Character tc1, Character tc2, DFT<Character>* dft, Need2Map &need2Map) {
            return need2Map[tState][tc1].insert(StateLabel2(sState, TrTrNFALabel2(Char2(sc, tc2), dft))).second;
        }
        
        void addPreStarNeedMap(TrNFAState<Character>* sState, Character sc, TrNFAState<Character>* tState, Character tc, DFT<Character>* dft, NeedMap &needMap, Need2Map& need2Map) {
            if (addNeedMap(sState, sc, tState, tc, dft, needMap))
                addPreStarTrans(sState, sc, tState, tc, dft, needMap, need2Map);
        }
        
        void addPreStarNeed2Map(TrNFAState<Character>* sState, Character sc, TrNFAState<Character>* tState, Character tc1, Character tc2, DFT<Character>* dft, NeedMap& needMap, Need2Map& need2Map) {
            if (addNeed2Map(sState, sc, tState, tc1, tc2, dft, need2Map)) {
                TrNFAStates stateset = tState -> getTargetStatesByChar(tc1);
                for (TrNFAState<Character>* state : stateset)
                    addPreStarNeedMap(sState, sc, state, tc2, needMap, need2Map);
            }
        }
        
        void addPreStarTrans(TrNFAState<Character>* sState, Character sc, TrNFAState<Character>* tState, Character tc, NeedMap& needMap, Need2Map& need2Map) {
            TrNFAStates stateset;
            tState -> getTargetStatesByChar(stateset, tc);
            for (TrNFAState<Character>* state : stateset)
                if (sState -> addTrans(sc, state))
                    addPreStarTrans(sState, sc, state, needMap, need2Map);
        }

        void addPreStarTrans(TrNFAState<Character>* sState, Character c, TrNFAState<Character>* tState, NeedMap& needMap, Need2Map& need2Map) {
            Char2StateCharsMap map = needMap[sState];
            auto mapIt = map.find(c);
            if (mapIt != map.end()) {
                for (StateChar scpair : mapIt -> second)
                    if (scpair.first -> addTrans(scpair.second, tState))
                        addPreStarTrans(scpair.first, scpair.second, tState, needMap, need2Map);
            }
            Char2StateChar2sMap& map2 = need2Map[sState];
            auto map2It = map2.find(c);
            if (map2It != map2.end()) {
                for (StateChar2 sc2pair : map2It -> second) {
                    addPreStarNeedMap(sc2pair.first, sc2pair.second.first, tState, sc2pair.second.second, needMap, need2Map);
                }
            }
        }
        
        void addPostStarNeedMap(TrNFAState<Character>* sState, Character sc, TrNFAState<Character>* tState, Character tc, NeedMap &needMap, Need2Map& need2Map) {
            if (addNeedMap(sState, sc, tState, tc, needMap))
                addPostStarTrans(sState, sc, tState, tc, needMap, need2Map);
        }
        
        void addPostStarNeed2Map(TrNFAState<Character>* sState, Character sc, TrNFAState<Character>* tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map& need2Map) {
            if (addNeed2Map(sState, sc, tState, tc1, tc2, need2Map)) {
                TrNFAState<Character>* midState = mkState();
                sState -> addTrans(sc, midState);
                addPostStarTrans(sState, sc, midState, needMap, need2Map);
                addPostStarNeedMap(midState, tc2, tState, tc1, needMap, need2Map);
            }
        }
        
        void addPostStarTrans(TrNFAState<Character>* sState, Character sc, TrNFAState<Character>* tState, Character tc, NeedMap& needMap, Need2Map& need2Map) {
            TrNFAStates trnfaStates;
            tState -> getTargetStatesByChar(trnfaStates, tc);
            for (TrNFAState<Character>* state : trnfaStates) {
                if (sState -> addTrans(sc, state)) {
                    addPostStarTrans(sState, sc, state, needMap, need2Map);
                }
            }
        }
        
        void addPostStarTrans(TrNFAState<Character>* sState, Character c, TrNFAState<Character>* tState, NeedMap& needMap, Need2Map& need2Map) {
            Char2StateCharsMap& map = needMap[sState];
            auto mapIt = map.find(c);
            if (mapIt != map.end()) {
                for (StateChar scpair : mapIt -> second) {
                    if (scpair.first -> addTrans(scpair.second, tState)) {
                        addPostStarTrans(scpair.first, scpair.second, tState, needMap, need2Map);
                    }
                }
            }
        }

        void mkTrPDSState2Map(TrNFA* trnfa, const TrPDS<Character>& trpds, TrNFAState2Map& copyMap, TrPDSState2TrNFAStateMap& state2Map) {
            TrPDSStates trpdsStates;
            for (auto& mapPair : state2Map)
                trpdsStates.insert(mapPair.first);
            for (TrPDSState* state : trpds.getStates()) {
                if (trpdsStates.find(state) == trpdsStates.end()) {
                    state2Map[state] = trnfa -> mkState();
                } else {
                    state2Map[state] = copyMap[state2Map[state]];
                }
            }
        }

    public:
        /// \brief Copy construction function.
        /// \param trnfa The copied TrNFA.
        TrNFA(const TrNFA& trnfa) {
            setAlphabet(trnfa.alphabet);
            TrNFAState<Character>* iniState = mkInitialState();
            TrNFAState2Map state2Map;
            state2Map[trnfa.initialState] = iniState;
            cpTrans(trnfa.initialState, state2Map);
        }

        /// \brief Copy construction function.
        /// \param trnfa The copied TrNFA.
        TrNFA(const TrNFA& trnfa, TrNFAState2Map& state2Map) {
            setAlphabet(trnfa.alphabet);
            TrNFAState<Character>* iniState = mkInitialState();
            state2Map[trnfa.initialState] = iniState;
            cpTrans(trnfa.initialState, state2Map);
        }

        /// \brief Sets alphabet given a set of Characters.
        ///
        /// Clears current alphabet and copy param chars to alphabet.
        /// \param chars a const reference set of Character. 
        void setAlphabet(const Characters &chars) {
            alphabet.clear();
            alphabet.insert(chars.begin(),chars.end());
        }

        /// \brief Sets alphabet given a ordered_set of Character.
        ///
        /// Clears current alphabet and copy param chars to alphabet.
        /// \param chars a const reference ordered_set of Character. 
        void setAlphabet(const set<Character> &chars) {
            alphabet.clear();
            alphabet.insert(chars.begin(),chars.end());
        }

        /// \brief Sets initialState to param state.
        /// \param state The state set to be initialState.
        void setInitialState(TrNFAState<Character>* state) {
            initialState = state;
        }

        /// \brief Adds param state to finalStates.
        /// \param state The state to be added in finalStates.
        void addFinalState(TrNFAState<Character>* state) {
            finalStates.insert(state);
            state -> setFinalFlag(1);
        }

        /// \brief Deletes param state from finalStates.
        /// \param state The state to be deleted from finalStates.
        void delFinalState(TrNFAState<Character>* state) {
            state -> setFinalFlag(0);
            finalStates.erase(state);
        }

        /// \brief Gets states.
        /// \return The set reference of TrNFAState pointer for this TrNFA.
        TrNFAStates& getStates() {
            return states;
        }

        /// \brief Gets finalStates.
        /// \return The set reference of TrNFAState pointer for this TrNFA.
        TrNFAStates& getFinalStates() {
            return finalStates;
        }

        /// \brief Gets initialState.
        /// \return The TrNFAstate pointer of initialState for this TrNFA.
        TrNFAState<Character>* getInitialState() {
            return initialState;
        }

        /// \brief Gets states, a const function.
        /// \return The const set reference of TrNFAState pointer for this TrNFA.
        const TrNFAStates& getStates() const {
            return states;
        }

        /// \brief Gets finalStates, a const function.
        /// \return The const set reference of TrNFAState pointer for this TrNFA.
        const TrNFAStates& getFinalStates() const {
            return finalStates;
        }

        /// \brief Gets initialState, a const function.
        /// \return The const TrNFAState pointer of initialState for this TrNFA.
        const TrNFAState<Character>* getInitialState() const {
            return initialState;
        }

        /// \brief Removes all state in the finalStates for this TrNFA.
        void clearFinalStates() {
            for (TrNFAState<Character>* state : finalStates) {
                state -> setFinalFlag(0);
            }
            finalStates.clear();
        }

        /// \brief Checks whether given param states has finalState.
        /// \param states The TrNFAStates for checking.
        /// \return True means param states has finalState, otherwise not.
        static bool hasFinalState(const TrNFAStates& states) {
            for (TrNFAState<Character>* state : states) {
                if (state -> isFinal()) return true;
            }
            return false;
        }

        /// \brief Checks whether all states in the given param states are finalState.
        /// \param states The TrNFAStates for checking.
        /// \return True means all states are finalState, otherwise not.
        static bool allFinalState(const TrNFAStates& states) {
            for (TrNFAState<Character>* state : states) {
                if (!state -> isFinal()) return false;
            }
            return true;
        }

        /// \brief Makes a state in this TrNFA.
        /// \return A TrNFAState pointer made by this TrNFA.
        TrNFAState<Character>* mkState() {
            TrNFAState<Character>* state = new TrNFAState<Character>();
            states.insert(state);
            return state;
        }

        /// \brief Deletes a state in this TrNFA.
        /// \param state The state to be deleted in this TrNFA.
        void delState(TrNFAState<Character>* state) {
            states.erase(state);
            finalStates.erase(state);
            if (initialState == state) initialState = nullptr;
            delete state;
        }
        
        /// \brief Makes a initialState in this TrNFA.
        /// \return A TrNFAState pointer made by this TrNFA.
        TrNFAState<Character>* mkInitialState() {
            initialState = mkState();
            return initialState;
        }
        
        /// \brief Makes a finalState in this TrNFA.
        /// \return A TrNFAState pointer made by this TrNFA.
        TrNFAState<Character>* mkFinalState() {
            TrNFAState<Character>* state = mkState();
            state -> setFinalFlag(1);
            finalStates.insert(state);
            return state;
        }

        /// \brief Gets the pre* TrNFA from this configuration TrNFA by givin param trpds.
        /// \param trpds Provides the rules for pre*.
        /// \param state2Map Records the relation between trpds and trnfa.
        /// \return A reference of TrNFA.
        TrNFA& preStar(const TrPDS<Character>& trpds, TrPDSState2TrNFAStateMap& state2Map) {
            TrNFAState2Map copyMap;
            TrNFA* trnfa = new TrNFA(*this, copyMap);
            NeedMap needMap;
            Need2Map need2Map;
            mkTrPDSState2Map(this, trpds, copyMap, state2Map);
            
            for (PopTrPDSTrans<Character>* trans : trpds.getPopTransList()) {
                TrNFAState<Character>* sourceState = state2Map[trans -> getSourceState()];
                TrNFAState<Character>* targetState = state2Map[trans -> getTargetState()];
                Character character = trans -> getChar();
                sourceState -> addTrans(character, targetState);
            }

            for (ReplaceTrPDSTrans<Character>* trans : trpds.getReplaceTransList()) {
                TrNFAState<Character>* sourceState = state2Map[trans -> getSourceState()];
                TrNFAState<Character>* targetState = state2Map[trans -> getTargetState()];
                Character character = trans -> getChar();
                Character stack = trans -> getStack();
                trnfa -> addPreStarNeedMap(sourceState, character, targetState, stack, needMap, need2Map);
            }

            for (PushTrPDSTrans<Character>* trans : trpds.getPushTransList()) {
                TrNFAState<Character>* sourceState = state2Map[trans -> getSourceState()];
                TrNFAState<Character>* targetState = state2Map[trans -> getTargetState()];
                Character character = trans -> getChar();
                Char2 stack = trans -> getStack();
                trnfa -> addPreStarNeed2Map(sourceState, character, targetState, stack.first, stack.second, needMap, need2Map);
            }
            trnfa -> removeUnreachableState();
            trnfa -> removeDeadState();
            return *trnfa;
        }
        
        /// \brief Gets the post* TrNFA from this configuration TrNFA by givin param pds.
        /// \param pds Provides the rules for post*.
        /// \param state2Map Records the relation between pds and nfa.
        /// \return A reference of TrNFA.
        TrNFA& postStar(const TrPDS<Character>& pds, TrPDSState2TrNFAStateMap& state2Map) {
            TrNFAState2Map copyMap;
            TrNFA* nfa = new TrNFA(*this, copyMap);
            NeedMap needMap;
            Need2Map need2Map;
            mkTrPDSState2Map(this, pds, copyMap, state2Map);
            
            for (PopTrPDSTrans<Character>* trans : pds.getPopTransList()) {
                TrNFAState<Character>* sourceState = state2Map[trans -> getSourceState()];
                TrNFAState<Character>* targetState = state2Map[trans -> getTargetState()];
                Character character = trans -> getChar();
                nfa -> addPostStarNeedMap(targetState, FA<Character>::epsilon, sourceState, character, needMap, need2Map);
            }

            for (ReplaceTrPDSTrans<Character>* trans : pds.getReplaceTransList()) {
                TrNFAState<Character>* sourceState = state2Map[trans -> getSourceState()];
                TrNFAState<Character>* targetState = state2Map[trans -> getTargetState()];
                Character character = trans -> getChar();
                Character stack = trans -> getStack();
                nfa -> addPostStarNeedMap(targetState, stack, sourceState, character, needMap, need2Map);
            }

            for (PushTrPDSTrans<Character>* trans : pds.getPushTransList()) {
                TrNFAState<Character>* sourceState = state2Map[trans -> getSourceState()];
                TrNFAState<Character>* targetState = state2Map[trans -> getTargetState()];
                Character character = trans -> getChar();
                Char2& stack = trans -> getStack();
                nfa -> addPostStarNeed2Map(targetState, stack.first, sourceState, character, stack.second, needMap, need2Map);
            }
            nfa -> removeUnreachableState();
            nfa -> removeDeadState();
            return *nfa;
        }
    };
};

#endif /* TrNFA_hpp */
