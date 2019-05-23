//
//  PDSParser.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef PDSParser_hpp
#define PDSParser_hpp

#include "Parser.hpp"

using namespace std;

namespace cgh {
   
    /// \brief Parses PDS from file.
    template <class Character>
    class PDSParser : public Parser<Character> {
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4Char<Character>::Char2 Char2;
        typedef unordered_set<ID> IDs;
        typedef pair<Character, Char2> Char3;
        typedef tuple<ID, Character, ID> PopPDSTransition;
        typedef tuple<ID, Char2, ID> ReplacePDSTransition;
        typedef tuple<ID, Char3, ID> PushPDSTransition;
        typedef vector<PopPDSTransition> PopPDSTransitions;
        typedef vector<ReplacePDSTransition> ReplacePDSTransitions;
        typedef vector<PushPDSTransition> PushPDSTransitions;
    private:
        PopPDSTransitions poptransitions;
        ReplacePDSTransitions replacetransitions;
        PushPDSTransitions pushtransitions;

    public:
        /// \brief Defualt construction.
        PDSParser() { 
        }

        /// \brief Constructs PDS.
        PDS<Character>* parse(fstream& fin) {
            IDs finalStates;
            ID stateNumber = this -> parseStateNumber(fin);
            this -> parseAlphabet(fin);
            PDS<Character>* result = new PDS<Character>(this -> alphabet);
            parsePopTransitions(fin);
            parseReplaceTransitions(fin);
            parsePushTransitions(fin);
            // construct PDS
            vector<PDSState* > stateVector;
            // update state info
            for (ID pos = 0; pos < stateNumber; pos++) {
                stateVector.push_back(result -> mkState());
            }
            // update transition info
            for (ID i = 0; i < poptransitions.size(); i++) {
                ID sourceState = get<0>(poptransitions[i]);
                ID targetState = get<2>(poptransitions[i]);
                Character c = get<1>(poptransitions[i]);
                result -> mkPopPDSTrans(stateVector[sourceState], stateVector[targetState], c);
            }
            for (ID i = 0; i < replacetransitions.size(); i++) {
                ID sourceState = get<0>(replacetransitions[i]);
                ID targetState = get<2>(replacetransitions[i]);
                Char2 char2 = get<1>(replacetransitions[i]);
                result -> mkReplacePDSTrans(stateVector[sourceState], stateVector[targetState], char2.first, char2.second);
            }
            for (ID i = 0; i < pushtransitions.size(); i++) {
                ID sourceState = get<0>(pushtransitions[i]);
                ID targetState = get<2>(pushtransitions[i]);
                Char3 char3 = get<1>(pushtransitions[i]);
                result -> mkPushPDSTrans(stateVector[sourceState], stateVector[targetState], char3.first, char3.second);
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
        void parseReplaceTransitions(fstream& fin) {
            string line;
            int src;
            Character sch;
            Character tch;
            int dst;
            while (getline(fin, line)) {
                stringstream stream;
                stream << line;
                if (line == "$") {
                    break;
                } else {
                    stream >> src >> sch >> tch >> dst;
                    replacetransitions.push_back(make_tuple(src, Char2(sch, tch), dst));
                }
            }
        }

        void parsePopTransitions(fstream& fin) {
            this -> parseComment(fin);
            string line;
            int src;
            Character c;
            int dst;
            getline(fin, line);
            while (getline(fin, line)) {
                stringstream stream;
                stream << line;
                if (line == "$") {
                    break;
                } else {
                    stream >> src >> c >> dst;
                    poptransitions.push_back(make_tuple(src, c, dst));
                }
            }
        }
        void parsePushTransitions(fstream& fin) {
            string line;
            int src;
            Character sc;
            Character tc1;
            Character tc2;
            int dst;
            while (getline(fin, line)) {
                stringstream stream;
                stream << line;
                if (line == "endPDS") {
                    break;
                } else {
                    stream >> src >> sc >> tc1 >> tc2 >> dst;
                    pushtransitions.push_back(make_tuple(src, Char3(sc, Char2(tc1, tc2)), dst));
                }
            }
        }
    };
    
    
    
    
}




#endif /* PDSParser_hpp */
