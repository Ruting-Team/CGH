//
//  PDSParser.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef PDSParser_hpp
#define PDSParser_hpp

#include "../Parser.hpp"
#include "../State.hpp"

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
        PDS<Character>* parse(const string& fileName) {
            fstream fin;
            fin.open(fileName, fstream::in);
            string info = fileName + " not found or open failed!";
            if (!fin.is_open()) {
                ErrorReport::report(info, ERROR);
                exit(-1);
            }
            this -> parseAlphabet(fin);
            PDS<Character>* result = new PDS<Character>(this -> alphabet);
            ID stateNumber = this -> parseStateNumber(fin);
            this -> parseFinalStates(fin);
            parsePopTransitions(fin);
            parseReplaceTransitions(fin);
            parsePushTransitions(fin);
            fin.close();
            // construct PDS
            vector<PDSState* > stateVector;
            // update state info
            for (ID pos = 0; pos < stateNumber; pos++) {
                if (this -> finalStates.find(pos) != this -> finalStates.end()) {
                    stateVector.push_back(result -> mkControlState());
                } else {
                    stateVector.push_back(result -> mkState());
                }
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
            this -> parseComment(fin);
            int src;
            Character uc;
            Character lc;
            int dst;
            while(fin >> src >> uc >> lc >> dst) {
                replacetransitions.push_back(make_tuple(src, Char2(uc, lc), dst));
            }
        }
        void parsePopTransitions(fstream& fin) {
            this -> parseComment(fin);
            int src;
            Character c;
            int dst;
            while(fin >> src >> c  >> dst) {
                poptransitions.push_back(make_tuple(src, c, dst));
            }
        }
        void parsePushTransitions(fstream& fin) {
            this -> parseComment(fin);
            int src;
            Character sc;
            Character tc1;
            Character tc2;
            int dst;
            while(fin >> src >> sc >> tc1 >> tc2 >> dst) {
                replacetransitions.push_back(make_tuple(src, Char3(sc, Char2(tc1, tc2)), dst));
            }
        }
    };
    
    
    
    
}




#endif /* PDSParser_hpp */
