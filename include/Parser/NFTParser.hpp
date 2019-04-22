//
//  NFTParser.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef NFTParser_hpp
#define NFTParser_hpp

#include "Parser.hpp"

using namespace std;

namespace cgh {
   
    /// \brief Parses NFT from file.
    template <class Character>
    class NFTParser : public Parser<Character> {
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4Char<Character>::Char2 Char2;
        typedef unordered_set<ID> IDs;
        typedef tuple<ID, Char2, ID> NFTTransition;
        typedef vector<NFTTransition> NFTTransitions;
    private:
        NFTTransitions transitions;

    public:
        /// \brief Defualt construction.
        NFTParser() { 
        }

        /// \brief Constructs NFT.
        NFT<Character>* parse(fstream& fin) {
            IDs finalStates;
            ID stateNumber = this -> parseStateNumber(fin);
            ID initialState = this -> parseStateNumber(fin);
            this -> parseStates(fin, finalStates);
            this -> parseAlphabet(fin);
            NFT<Character>* result = new NFT<Character>(this -> alphabet);
            parseTransitions(fin);
            // construct NFT
            vector<NFAState<Label<Character> >* > stateVector;
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
                Char2 char2 = get<1>(transitions[i]);
                stateVector[sourceState] -> addTrans(Label<Character>(char2.first, char2.second), stateVector[targetState]);
            }
            
            return result;
        }

    protected:
        /// \brief parse left lines
        ///
        /// Example : transitions lines is: 0 1 2 1
        /// Assumes transition line is: $src_state $charater $dst_state
        /// \param fin input stream
        /// \param transitions transitions
        void parseTransitions(fstream& fin) {
            this -> parseComment(fin);
            string line;
            int src;
            Character u;
            Character l;
            int dst;
            getline(fin, line);
            while (getline(fin, line)) {
                stringstream stream;
                stream << line;
                if (line == "endNFT") {
                    break;
                } else {
                    stream >> src >> u >> l >> dst;
                    transitions.push_back(make_tuple(src, Char2(u, l), dst));
                }
            }
        }
    };
    
    
}




#endif /* NFTParser_hpp */
