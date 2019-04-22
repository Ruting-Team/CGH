//
//  TrNFAParser.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef TrNFAParser_hpp
#define TrNFAParser_hpp

#include "Parser.hpp"

using namespace std;

namespace cgh {
   
    /// \brief Parses TrNFA from file.
    template <class Character>
    class TrNFAParser : public Parser<Character> {
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef unordered_set<ID> IDs;
        typedef pair<Character, string> TrNFALabel;
        typedef tuple<ID, TrNFALabel, ID> TrNFATransition;
        typedef vector<TrNFATransition> TrNFATransitions;
    private:
        TrNFATransitions transitions;
        vector<TrNFAState<Character>* > stateVector;

    public:
        vector<TrNFAState<Character>*> getStateVector() {
            return stateVector;
        }
        /// \brief Defualt construction.
        TrNFAParser() { 
        }

        /// \brief Constructs TrNFA.
        TrNFA<Character>* parse(fstream& fin, unordered_map<string, DFT<Character>* >& dftMap) {
            IDs finalStates;
            ID stateNumber = this -> parseStateNumber(fin);
            ID initialState = this -> parseStateNumber(fin);
            this -> parseStates(fin, finalStates);
            this -> parseAlphabet(fin);
            TrNFA<Character>* result = new TrNFA<Character>(this -> alphabet, dftMap["Tid"]);
            parseTransitions(fin);
            // construct TrNFA
            // update state info
            for (ID pos = 0; pos < stateNumber; pos++) {
                if (pos == initialState) { 
                    stateVector.push_back(result -> mkInitialState()); 
                } else if (finalStates.find(pos) != finalStates.end()) {
                    stateVector.push_back(result -> mkFinalState());
                } else {
                    stateVector.push_back(result -> mkState());
                }
            }
            if (finalStates.count(initialState) > 0) {
                result -> addFinalState(result -> getInitialState());
            }
            // update transition info
            for (ID i = 0; i < transitions.size(); i++) {
                ID sourceState = get<0>(transitions[i]);
                ID targetState = get<2>(transitions[i]);
                TrNFALabel label = get<1>(transitions[i]);
                stateVector[sourceState] -> addTrans(label.first, dftMap[label.second], stateVector[targetState]);
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
            string line;
            int src;
            Character c;
            string t;
            int dst;
            getline(fin, line);
            while (getline(fin, line)) {
                stringstream stream;
                stream << line;
                if (line == "endTrNFA") {
                    break;
                } else {
                    stream >> src >> c >> t >> dst;
                    transitions.push_back(make_tuple(src, TrNFALabel(c, t), dst));
                }
            }
        }
    };
    
    
    
    
}




#endif /* TrNFAParser_hpp */
