//
//  PDS.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/14.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef PDS_hpp
#define PDS_hpp

#include "PDSTransition.hpp"

namespace cgh
{
    template <class Character>
    class PDS
    {
        typedef Global<Character> Global;
        
        typedef typename Global::NFAStateSet NFAStateSet;
        typedef typename Global::PDSTransList PDSTransList;
        typedef typename Global::PushPDSTransList PushPDSTransList;
        typedef typename Global::ReplacePDSTransList ReplacePDSTransList;
    private:
        PDSStateSet stateSet;
        PDSStateSet controlStateSet;
        PDSTransList popTransList;
        PushPDSTransList pushTransList;
        ReplacePDSTransList replaceTransList;
    public:
        NFAStateSet& getControlStateSet() { return controlStateSet; }
        NFAStateSet& getStateSet() { return stateSet; }
        PDSTransList& getPopTransList() { return popTransList;}
        PushPDSTransList& getPushTransList() { return pushTransList; }
        ReplacePDSTransList& getReplaceTransList() { return replaceTransList; }
        
        const NFAStateSet& getControlStateSet() const { return controlStateSet; }
        const NFAStateSet& getStateSet() const { return stateSet; }
        const PDSTransList& getPopTransList() const { return popTransList;}
        const PushPDSTransList& getPushTransList() const { return pushTransList; }
        const ReplacePDSTransList& getReplaceTransList() const { return replaceTransList; }
    };
}

#endif /* PDS_hpp */
