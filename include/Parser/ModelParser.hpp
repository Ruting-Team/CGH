//
//  ModelParser.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/3.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef MODELPARSER_HPP
#define MODELPARSER_HPP
#include "NFAParser.hpp"
#include "NFTParser.hpp"
#include "PDSParser.hpp"
#include "TrNFAParser.hpp"
#include "TrPDSParser.hpp"

using namespace std;

namespace cgh {
    enum LogLevel {
        ERROR,
        DEBUG_,
        WARN,
        INFO,
    };
    
    /// \brief Reports the error info.
    class ErrorReport {
    public:
         /// \brief Reports some info and displays in terminal
         /// \param info  [info string]
         /// \param level [report level, default INFO]
        void static report(string info, LogLevel level = INFO) {
            switch (level) {
                case ERROR:
                    cout << "ERROR: " << info << endl;
                    exit(-1);
                    break;
                case DEBUG_:
                    cout << "DEBUG: " << info << endl;
                    break;
                case WARN:
                    cout << "WARN: " << info << endl;
                    break;
                case INFO:
                    cout << "INFO: " << info << endl;
                    break;
                default:
                    break;
            }
        }
    };
    
    /// \brief Parses from file.
    class ModelParser {
        typedef Alias4TrNFA<char>::TrPDSState2TrNFAStateMap StateMap;
        typedef Alias4FT<char>::DFTPairMap DFTPairMap;
        typedef Alias4FT<char>::DFTLabel2DFTMap DFTLabel2DFTMap;
        typedef Alias4FT<char>::DFT2Map DFT2Map;
        typedef Alias4FT<char>::DFTs DFTs;
    protected:
        unordered_map<string, DFT<char>*> dftMap;
        unordered_map<string, DFA<char>*> dfaMap;
        unordered_map<string, PDS<char>*> pdsMap;
        unordered_map<string, TrNFA<char>*> trnfaMap;
        unordered_map<string, TrPDS<char>*> trpdsMap;
        unordered_map<string, vector<TrNFAState<char>*> > trnfaStateMap;
        unordered_map<string, vector<TrPDSState*>> trpdsStateMap;
        DFTs dfts;

    public:
        /// \brief Defualt construction.
        ModelParser() { 
        }

        void parse(const string& fileName) {
            fstream fin;
            fin.open(fileName, fstream::in);
            string info = fileName + " not found or open failed!";
            if (!fin.is_open()) {
                ErrorReport::report(info, ERROR);
                exit(-1);
            }
            string line;
            while (getline(fin, line)) {
                if (line.find("beginNFA") != string::npos) {
                    string name = Manage::split(line, " ")[1];
                    NFAParser<char> parser;
                    NFA<char>* nfa = parser.parse(fin);
                    DFA<char>* dfa = &(nfa -> minimize());
                    delete nfa;
                    dfaMap[name] = dfa;
                } else if (line.find("beginNFT") != string::npos) {
                    string name = Manage::split(line, " ")[1];
                    NFTParser<char> parser;
                    NFT<char>* nft = parser.parse(fin);
                    DFT<char>* dft = &(nft -> minimizeFT());
                    delete nft;
                    dftMap[name] = dft;
                    dfts.insert(dft);
                } else if (line.find("beginPDS") != string::npos) {
                    string name = Manage::split(line, " ")[1];
                    PDSParser<char> parser;
                    PDS<char>* pds = parser.parse(fin);
                    pdsMap[name] = pds;
                    Manage::manage(pds);
                } else if (line.find("beginTrNFA") != string::npos) {
                    string name = Manage::split(line, " ")[1];
                    TrNFAParser<char> parser;
                    TrNFA<char>* trnfa = parser.parse(fin, dftMap);
                    trnfaMap[name] = trnfa;
                    trnfaStateMap[name] = parser.getStateVector();
                    Manage::manage(trnfa);
                } else if (line.find("beginTrPDS") != string::npos) {
                    string name = Manage::split(line, " ")[1];
                    TrPDSParser<char> parser;
                    TrPDS<char>* trpds = parser.parse(fin, dftMap);
                    trpdsMap[name] = trpds;
                    trpdsStateMap[name] = parser.getStateVector();
                    Manage::manage(trpds);
                } else if (line.find("beginMap") != string::npos) {
                    string trpdsName = Manage::split(line, " ")[1];
                    string trnfaName = Manage::split(line, " ")[2];
                    string str;
                    StateMap stateMap;
                    while(getline(fin, str)) {
                        stringstream stream;
                        stream << str;
                        int a;
                        int b;
                        stream >> a >> b;
                        stateMap[trpdsStateMap[trpdsName][a]] = trnfaStateMap[trnfaName][b];
                    }
                    TrNFA<char>* trnfa = trnfaMap[trnfaName];
                    TrPDS<char>* trpds = trpdsMap[trpdsName];
                    DFTLabel2DFTMap leftQuotientMap;
                    DFTPairMap compositionMap;
                    DFT2Map inversionMap;
                    cout << FT<char>::getDFTClosure(dfts, trnfa -> getAlphabet(), leftQuotientMap, compositionMap, inversionMap).size() << endl;

                    trnfa -> postStar(*trpds, stateMap, compositionMap, leftQuotientMap, inversionMap).output();

                }
            }
        }
    };
}




#endif /* ModelParser_hpp */
