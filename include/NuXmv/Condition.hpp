//
//  Condition.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/6.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef Condition_hpp 
#define Condition_hpp 
#include "Atomic.hpp"

namespace cgh {
    /// \brief Conjunction of Atomics.
    class Conjunction {
    private:
        Atomics atomics;
    public:
        /// \brief Default construction function.
        Conjunction() {
        }

        /// \brief Desconstruction function.
        ///
        /// Deletes all Atomic pointer in atomics.
        ~Conjunction() {
            for (Atomic* atomic : atomics) {
                delete atomic;
            }
        }

        /// \brief Makes a EquAtomic for this Conjunction.
        /// \param lVar lhs Var for this Atomic.
        /// \param rValue rhs Value for this Atomic.
        /// \param f Flag for this Atomic. Default is true.
        EquAtomic* mkEquAtomic(Var* lVar, Value* rValue, bool f = true) {
            EquAtomic* atomic = new EquAtomic(lVar, rValue, f);
            atomics.push_back(atomic);
            return atomic;
        }

        /// \brief Makes a EquAtomic for this Conjunction.
        /// \param lVar lhs Var for this Atomic.
        /// \param rVar rhs Var for this Atomic.
        /// \param f Flag for this Atomic. Default is true.
        EquAtomic* mkEquAtomic(Var* lVar, Var* rVar, bool f = true) {
            EquAtomic* atomic = new EquAtomic(lVar, rVar, f);
            atomics.push_back(atomic);
            return atomic;
        }

        string getStr() {
            string res = "";
            for (Atomic* atomic: atomics) {
                res = And(res, atomic -> getStr());
            }
            return res; 
        }

    private:
        string And(const string& lhs, const string& rhs) {
            if (lhs.size() == 0) return rhs;
            if (rhs.size() == 0) return lhs;
            return lhs + " & " + rhs;
        }
    };


    /// \brief Condition in NuXmv.
    class Condition {
    private:
        DNF dnf;        ///< the DNF for this Condition.

    public:
        /// \brief Defualt construction function.
        Condition() {}

        /// \brief Desconstruction function.
        ///
        /// Deletes all Conjunction pointer in dnf.
        ~Condition() {
            for (Conjunction* conjunction: dnf) {
                delete conjunction;
            }
        }

        /// \brief Makes a Conjunction for this Condition.
        Conjunction* mkConjunction() {
            Conjunction* conjunction = new Conjunction();
            dnf.push_back(conjunction);
            return conjunction;
        }

        DNF& getDNF() {return dnf;}

        string getStr() {
            string res = "";
            for (Conjunction* conjunction: dnf) {
                res = Or(res, conjunction -> getStr());
            }
            return res; 
        }

    private:
        string Or(const string& lhs, const string& rhs) {
            if (lhs.size() == 0) return rhs;
            if (rhs.size() == 0) return lhs;
            return lhs + " | " + rhs;
        }
    };
};
#endif /* Condition_hpp*/
