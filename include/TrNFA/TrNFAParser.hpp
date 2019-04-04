//
//  TrNFAParser.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef TrNFAParser_hpp
#define TrNFAParser_hpp

#include "../Parser.hpp"
#include "../State.hpp"

using namespace std;

namespace cgh {
   
    /// \brief Parses TrNFA from file.
    template <class Character>
    class TrNFAParser : public Parser<Character> {
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef unordered_set<ID> IDs;
        typedef pair<Character, ID> TrNFALabel;
        typedef tuple<ID, TrNFALabel, ID> TrNFATransition;
        typedef vector<TrNFATransition> TrNFATransitions;
    private:
        TrNFATransitions transitions;

    public:
        /// \brief Defualt construction.
        TrNFAParser() { 
        }

        /// \brief Constructs TrNFA.
        TrNFA<Character>* parse(const string& fileName) {
            fstream fin;
            fin.open(fileName, fstream::in);
            string info = fileName + " not found or open failed!";
            if (!fin.is_open()) {
                ErrorReport::report(info, ERROR);
                exit(-1);
            }
            this -> parseAlphabet(fin);
            TrNFA<Character>* result = new TrNFA<Character>(this -> alphabet);
            ID stateNumber = this -> parseStateNumber(fin);
            this -> parseInitialState(fin);
            this -> parseFinalStates(fin);
            parseTransitions(fin);
            fin.close();
            // construct TrNFA
            vector<TrNFAState<Character>* > stateVector;
            // update state info
            for (ID pos = 0; pos < stateNumber; pos++) {
                if (pos == this -> initialState) { 
                    stateVector.push_back(result -> mkInitialState()); 
                } else if (this -> finalStates.find(pos) != this -> finalStates.end()) {
                    stateVector.push_back(result -> mkFinalState());
                } else {
                    stateVector.push_back(result -> mkState());
                }
            }
            if (this -> finalStates.count(this -> initialState) > 0) {
                result -> addFinalState(result -> getInitialState());
            }
            // update transition info
            for (ID i = 0; i < transitions.size(); i++) {
                ID sourceState = get<0>(transitions[i]);
                ID targetState = get<2>(transitions[i]);
                Character character = get<1>(transitions[i]);
                stateVector[sourceState] -> addTrans(character, stateVector[targetState]);
            }
            
            return result;
        }

    protected:
        /// \brief parse left lines
        ///
        /// Example : transitions lines is: 0 1 1
        /// Assumes transition line is: $src_state $charater $dst_state
        /// \param fin input stream
        /// \param transitions transitions
        void parseTransitions(fstream& fin) {
            this -> parseComment(fin);
            int src;
            Character ch;
            int dst;
            while(fin >> src >> ch >> dst) {
                transitions.push_back(make_tuple(src, ch, dst));
            }
        }
    };
    
    
    
    
}




#endif /* TrNFAParser_hpp */
