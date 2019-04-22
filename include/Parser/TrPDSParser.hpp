//
//  TrPDSParser.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef TrPDSParser_hpp
#define TrPDSParser_hpp

#include "Parser.hpp"

using namespace std;

namespace cgh {
   
    /// \brief Parses TrPDS from file.
    template <class Character>
    class TrPDSParser : public Parser<Character> {
        typedef typename Alias4Char<Character>::Characters Characters;
        typedef typename Alias4Char<Character>::Char2 Char2;
        typedef unordered_set<ID> IDs;
        typedef pair<Character, string> CharStr;
        typedef pair<CharStr, Character> CharStr2;
        typedef pair<CharStr, Char2> CharStr3;
        typedef tuple<ID, CharStr, ID> PopTrPDSTransition;
        typedef tuple<ID, CharStr2, ID> ReplaceTrPDSTransition;
        typedef tuple<ID, CharStr3, ID> PushTrPDSTransition;
        typedef vector<PopTrPDSTransition> PopTrPDSTransitions;
        typedef vector<ReplaceTrPDSTransition> ReplaceTrPDSTransitions;
        typedef vector<PushTrPDSTransition> PushTrPDSTransitions;
    private:
        PopTrPDSTransitions poptransitions;
        ReplaceTrPDSTransitions replacetransitions;
        PushTrPDSTransitions pushtransitions;
        vector<TrPDSState* > stateVector;

    public:
        vector<TrPDSState*>& getStateVector() {
            return stateVector;
        }

        /// \brief Defualt construction.
        TrPDSParser() { 
        }

        /// \brief Constructs TrPDS.
        TrPDS<Character>* parse(fstream& fin, unordered_map<string, DFT<Character>*>& dftMap) {
            IDs finalStates;
            ID stateNumber = this -> parseStateNumber(fin);
            this -> parseAlphabet(fin);
            TrPDS<Character>* result = new TrPDS<Character>(this -> alphabet);
            parsePopTransitions(fin);
            parseReplaceTransitions(fin);
            parsePushTransitions(fin);
            // construct TrPDS
            // update state info
            for (ID pos = 0; pos < stateNumber; pos++) {
                stateVector.push_back(result -> mkState());
            }
            // update transition info
            for (ID i = 0; i < poptransitions.size(); i++) {
                ID sourceState = get<0>(poptransitions[i]);
                ID targetState = get<2>(poptransitions[i]);
                CharStr label = get<1>(poptransitions[i]);
                result -> mkPopTrPDSTrans(stateVector[sourceState], stateVector[targetState], label.first, dftMap[label.second]);
            }
            for (ID i = 0; i < replacetransitions.size(); i++) {
                ID sourceState = get<0>(replacetransitions[i]);
                ID targetState = get<2>(replacetransitions[i]);
                CharStr2 charStr2 = get<1>(replacetransitions[i]);
                result -> mkReplaceTrPDSTrans(stateVector[sourceState], stateVector[targetState], charStr2.first.first, charStr2.second, dftMap[charStr2.first.second]);
            }
            for (ID i = 0; i < pushtransitions.size(); i++) {
                ID sourceState = get<0>(pushtransitions[i]);
                ID targetState = get<2>(pushtransitions[i]);
                CharStr3 charStr3 = get<1>(pushtransitions[i]);
                result -> mkPushTrPDSTrans(stateVector[sourceState], stateVector[targetState], charStr3.first.first, charStr3.second, dftMap[charStr3.first.second]);
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
            string t;
            int dst;
            while (getline(fin, line)) {
                stringstream stream;
                stream << line;
                if (line == "$") {
                    break;
                } else {
                    stream >> src >> sch >> t >> tch >> dst;
                    replacetransitions.push_back(make_tuple(src, CharStr2(CharStr(sch, t), tch), dst));
                }
            }
        }

        void parsePopTransitions(fstream& fin) {
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
                if (line == "$") {
                    break;
                } else {
                    stream >> src >> c >> t >> dst;
                    poptransitions.push_back(make_tuple(src, CharStr(c, t), dst));
                }
            }
        }
        void parsePushTransitions(fstream& fin) {
            string line;
            int src;
            Character sc;
            Character tc1;
            Character tc2;
            string t;
            int dst;
            while (getline(fin, line)) {
                stringstream stream;
                stream << line;
                if (line == "endTrPDS") {
                    break;
                } else {
                    stream >> src >> sc >> t >> tc1 >> tc2 >> dst;
                    pushtransitions.push_back(make_tuple(src, CharStr3(CharStr(sc, t), Char2(tc1, tc2)), dst));
                }
            }
        }
    };
    
    
    
    
}




#endif /* TrPDSParser_hpp */
