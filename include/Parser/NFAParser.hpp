//
//  NFAParser.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef NFAParser_hpp
#define NFAParser_hpp

#include "Parser.hpp"

using namespace std;

namespace cgh {
   
    /// \brief Parses NFA from file.
    template <class Character>
    class NFAParser : public Parser<Character> {
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef unordered_set<ID> IDs;
        typedef tuple<ID, Character, ID> NFATransition;
        typedef vector<NFATransition> NFATransitions;
    private:
        NFATransitions transitions;

    public:
        /// \brief Defualt construction.
        NFAParser() { 
        }

        /// \brief Constructs NFA.
        NFA<Character>* parse(fstream& fin) {
            IDs finalStates;
            ID stateNumber = this -> parseStateNumber(fin);
            ID initialState = this -> parseStateNumber(fin);
            this -> parseStates(fin, finalStates);
            this -> parseAlphabet(fin);
            NFA<Character>* result = new NFA<Character>(this -> alphabet);
            parseTransitions(fin);
            // construct NFA
            vector<NFAState<Character>* > stateVector;
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
            string line;
            int src;
            Character c;
            int dst;
            getline(fin, line);
            while (getline(fin, line)) {
                stringstream stream;
                stream << line;
                if (line == "endNFA") {
                    break;
                } else {
                    stream >> src >> c >> dst;
                    transitions.push_back(make_tuple(src, c, dst));
                }
            }
        }
    };
    
}




#endif /* NFAParser_hpp */
