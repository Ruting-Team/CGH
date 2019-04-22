//
//  PDS.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/14.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef PDS_hpp
#define PDS_hpp

#include "PDSTransition.hpp"
#include "../Object.hpp"

namespace cgh {

    template <class Character>
    class PDS :public Object {
        typedef typename Alias4Char<Character>::Char2 Char2;
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4FA<Character>::NFAStates NFAStates;
        typedef typename Alias4PDS<Character>::PDSTransList PDSTransList;
        typedef typename Alias4PDS<Character>::PopPDSTransList PopPDSTransList;
        typedef typename Alias4PDS<Character>::PushPDSTransList PushPDSTransList;
        typedef typename Alias4PDS<Character>::ReplacePDSTransList ReplacePDSTransList;
        typedef typename Alias4PDS<Character>::PDSStates PDSStates;
    private:
        PDSStates states;                       ///< The set of states for this PDS.
        PDSStates controlStates;                ///< The set of control states for this PDS.
        PopPDSTransList popTransList;           ///< The list of pop transitions for this PDS.
        PushPDSTransList pushTransList;         ///< The list of push transitions for this PDS.
        ReplacePDSTransList replaceTransList;   ///< The list of replace transitions for this PDS.
        Characters alphabet;                    ///< The Alphabet fot this PDS.
    public:
        /// \brief Default construction function.
        PDS() {
        }

        /// brief Construction function with param characters.
        /// \param characters The alphabet.
        PDS(const Characters& characters) : alphabet(characters.begin(), characters.end()){
        }

        /// \brief Desconstruction function.
        ///
        /// delete all states in the states and all transtions.
        ~PDS() {
            for (PDSState* state : states) {
                delete state;
            }
            for (PopPDSTrans<Character>* trans : popTransList) {
                delete trans;
            }
            for (PushPDSTrans<Character>* trans : pushTransList) {
                delete trans;
            }
            for (ReplacePDSTrans<Character>* trans : replaceTransList) {
                delete trans;
            }
        }

        PDSStates& getControlStates() { return controlStates; }
        PDSStates& getStates() { return states; }
        PopPDSTransList& getPopTransList() { return popTransList;}
        PushPDSTransList& getPushTransList() { return pushTransList; }
        ReplacePDSTransList& getReplaceTransList() { return replaceTransList; }
        
        const PDSStates& getControlStates() const { return controlStates; }
        const PDSStates& getStates() const { return states; }
        const PopPDSTransList& getPopTransList() const { return popTransList;}
        const PushPDSTransList& getPushTransList() const { return pushTransList; }
        const ReplacePDSTransList& getReplaceTransList() const { return replaceTransList; }

        /// \brief Make a state for this PDS.
        ///
        /// \return A PDSState pointer.
        PDSState* mkState() {
            PDSState* state = new PDSState();
            states.insert(state);
            return state;
        }

        /// \brief Make a control state for this PDS.
        ///
        /// \return A PDSState pointer.
        PDSState* mkControlState() {
            PDSState* state = mkState();
            controlStates.insert(state);
            return state;
        }

        ///// \brief Make a state with param id for this PDS.
        ///// \param id The id for this state.
        ///// \return A PDSState pointer.
        //PDSState* mkState(ID id) {
        //    PDSState* state = new PDSState(id);
        //    states.insert(state);
        //    return state;
        //}

        ///// \brief Make a control state for this PDS.
        ///// \param id The id for this state.
        ///// \return A PDSState pointer.
        //PDSState* mkControlState(ID id) {
        //    PDSState state = mkState(id);
        //    controlStates.insert(state);
        //    return state;
        //}

        /// \brief Make a popPDSTrans for this PDS.
        /// \param souceState The source state in a popPDSTrans.
        /// \param targetState The target state in a popPDSTrans.
        /// \param character The source character in a popPDSTrans.
        /// \return A PopPDSTrans pointer.
        PopPDSTrans<Character>* mkPopPDSTrans(PDSState* sourceState, PDSState* targetState, Character character) {
            PopPDSTrans<Character>* pdsTrans = new PopPDSTrans<Character>(sourceState, targetState, character);
            popTransList.push_back(pdsTrans);
            return pdsTrans;
        }

        /// \brief Make a pushPDSTrans for this PDS.
        /// \param souceState The source state in a pushPDSTrans.
        /// \param targetState The target state in a pushPDSTrans.
        /// \param character The source character in a pushPDSTrans.
        /// \return A PushPDSTrans pointer.
        PushPDSTrans<Character>* mkPushPDSTrans(PDSState* sourceState, PDSState* targetState, Character character, const Char2& stack) {
            PushPDSTrans<Character>* pdsTrans = new PushPDSTrans<Character>(sourceState, targetState, character, stack);
            pushTransList.push_back(pdsTrans);
            return pdsTrans;
        }

        /// \brief Make a replacePDSTrans for this PDS.
        /// \param souceState The source state in a replacePDSTrans.
        /// \param targetState The target state in a replacePDSTrans.
        /// \param character The source character in a replacePDSTrans.
        /// \return A ReplacePDSTrans pointer.
        ReplacePDSTrans<Character>* mkReplacePDSTrans(PDSState* sourceState, PDSState* targetState, Character character, Character stack) {
            ReplacePDSTrans<Character>* pdsTrans = new ReplacePDSTrans<Character>(sourceState, targetState, character, stack);
            replaceTransList.push_back(pdsTrans);
            return pdsTrans;
        }

        void output() {
            for (auto transition : popTransList) {
                transition -> output();
            }
            for (auto transition : replaceTransList) {
                transition -> output();
            }
            for (auto transition : pushTransList) {
                transition -> output();
            }
        }
    };
}

#endif /* PDS_hpp */
