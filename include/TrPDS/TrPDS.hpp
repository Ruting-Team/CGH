//
//  TrPDS.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/14.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef TrPDS_hpp
#define TrPDS_hpp

#include "TrPDSTransition.hpp"
#include "../Object.hpp"

namespace cgh {

    template <class Character>
    class TrPDS : public Object {
        typedef typename Alias4Char<Character>::Char2 Char2;
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4FA<Character>::NFAStates NFAStates;
        typedef typename Alias4TrPDS<Character>::TrPDSTransList TrPDSTransList;
        typedef typename Alias4TrPDS<Character>::PopTrPDSTransList PopTrPDSTransList;
        typedef typename Alias4TrPDS<Character>::PushTrPDSTransList PushTrPDSTransList;
        typedef typename Alias4TrPDS<Character>::ReplaceTrPDSTransList ReplaceTrPDSTransList;
        typedef typename Alias4TrPDS<Character>::TrPDSStates TrPDSStates;
        typedef typename Alias4FT<Character>::DFTs DFTs;
    private:
        TrPDSStates states;                       ///< The set of states for this TrPDS.
        TrPDSStates controlStates;                ///< The set of control states for this TrPDS.
        PopTrPDSTransList popTransList;           ///< The list of pop transitions for this TrPDS.
        PushTrPDSTransList pushTransList;         ///< The list of push transitions for this TrPDS.
        ReplaceTrPDSTransList replaceTransList;   ///< The list of replace transitions for this TrPDS.
        Characters alphabet;                      ///< A set of characters which in the label on the transitions.
        DFTs dfts;                                ///> The set of DFT in this TrPDS.

    public:
        /// \brief Default construction function.
        TrPDS() {}

        TrPDS(const Characters& chars) : alphabet(chars.begin(), chars.end()) {
        }

        /// \brief Desconstruction function.
        ///
        /// delete all states in the states and all transtions.
        ~TrPDS() {
            for (TrPDSState* state : states) {
                delete state;
            }
            for (PopTrPDSTrans<Character>* trans : popTransList) {
                delete trans;
            }
            for (PushTrPDSTrans<Character>* trans : pushTransList) {
                delete trans;
            }
            for (ReplaceTrPDSTrans<Character>* trans : replaceTransList) {
                delete trans;
            }
        }

        TrPDSStates& getControlStates() { return controlStates; }
        TrPDSStates& getStates() { return states; }
        PopTrPDSTransList& getPopTransList() { return popTransList;}
        PushTrPDSTransList& getPushTransList() { return pushTransList; }
        ReplaceTrPDSTransList& getReplaceTransList() { return replaceTransList; }
        
        const TrPDSStates& getControlStates() const { return controlStates; }
        const TrPDSStates& getStates() const { return states; }
        const PopTrPDSTransList& getPopTransList() const { return popTransList;}
        const PushTrPDSTransList& getPushTransList() const { return pushTransList; }
        const ReplaceTrPDSTransList& getReplaceTransList() const { return replaceTransList; }

        /// \brief Make a state for this TrPDS.
        ///
        /// \return A TrPDSState pointer.
        TrPDSState* mkState() {
            TrPDSState* state = new TrPDSState();
            states.insert(state);
            return state;
        }

        /// \brief Make a control state for this TrPDS.
        ///
        /// \return A TrPDSState pointer.
        TrPDSState* mkControlState() {
            TrPDSState* state = mkState();
            controlStates.insert(state);
            return state;
        }

        ///// \brief Make a state with param id for this TrPDS.
        ///// \param id The id for this state.
        ///// \return A TrPDSState pointer.
        //TrPDSState* mkState(ID id) {
        //    TrPDSState* state = new TrPDSState(id);
        //    states.insert(state);
        //    return state;
        //}

        ///// \brief Make a control state for this TrPDS.
        ///// \param id The id for this state.
        ///// \return A TrPDSState pointer.
        //TrPDSState* mkControlState(ID id) {
        //    TrPDSState state = mkState(id);
        //    controlStates.insert(state);
        //    return state;
        //}

        /// \brief Make a popTrPDSTrans for this TrPDS.
        /// \param souceState The source state in a popTrPDSTrans.
        /// \param targetState The target state in a popTrPDSTrans.
        /// \param character The source character in a popTrPDSTrans.
        /// \return A PopTrPDSTrans pointer.
        PopTrPDSTrans<Character>* mkPopTrPDSTrans(TrPDSState* sourceState, TrPDSState* targetState, Character character, DFT<Character>* dft) {
            PopTrPDSTrans<Character>* pdsTrans = new PopTrPDSTrans<Character>(sourceState, targetState, character, dft);
            popTransList.push_back(pdsTrans);
            dfts.insert(dft);
            return pdsTrans;
        }

        /// \brief Make a pushTrPDSTrans for this TrPDS.
        /// \param souceState The source state in a pushTrPDSTrans.
        /// \param targetState The target state in a pushTrPDSTrans.
        /// \param character The source character in a pushTrPDSTrans.
        /// \return A PushTrPDSTrans pointer.
        PushTrPDSTrans<Character>* mkPushTrPDSTrans(TrPDSState* sourceState, TrPDSState* targetState, Character character, const Char2& stack, DFT<Character>* dft) {
            PushTrPDSTrans<Character>* pdsTrans = new PushTrPDSTrans<Character>(sourceState, targetState, character, stack, dft);
            pushTransList.push_back(pdsTrans);
            dfts.insert(dft);
            return pdsTrans;
        }

        /// \brief Make a replaceTrPDSTrans for this TrPDS.
        /// \param souceState The source state in a replaceTrPDSTrans.
        /// \param targetState The target state in a replaceTrPDSTrans.
        /// \param character The source character in a replaceTrPDSTrans.
        /// \return A ReplaceTrPDSTrans pointer.
        ReplaceTrPDSTrans<Character>* mkReplaceTrPDSTrans(TrPDSState* sourceState, TrPDSState* targetState, Character character, Character stack, DFT<Character>* dft) {
            ReplaceTrPDSTrans<Character>* pdsTrans = new ReplaceTrPDSTrans<Character>(sourceState, targetState, character, stack, dft);
            replaceTransList.push_back(pdsTrans);
            dfts.insert(dft);
            return pdsTrans;
        }
    };
}

#endif /* TrPDS_hpp */
