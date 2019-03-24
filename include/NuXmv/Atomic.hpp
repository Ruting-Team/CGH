//
//  Atomic.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/6.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef Atomic_hpp 
#define Atomic_hpp 
#include "Var.hpp"
#include "Value.hpp"

namespace cgh {
    /// \brief The Atomic fomula in the NuXmv
    class Atomic {
    protected:
        Var* lhsVar;           ///< the left hand side Var for thie Atomic
        Var* rhsVar;           ///< the right hand side Var for this Atomic.
        Value* rhsValue;       ///< the right hand side Value for this Atomic.
        bool flag;             ///< the flag representing positive or not for this Atomic.
    public:
        /// \brief Default construction function.
        Atomic() : lhsVar(nullptr), rhsVar(nullptr), rhsValue(nullptr), flag(true) {
        }

        /// \brief construcion with params.
        /// \param lVar lhs Var for this Atomic.
        /// \param rValue rhs Value for this Atomic.
        /// \param f Flag for this Atomic. Default is true.
        Atomic(Var* lVar, Value* rValue, bool f = true) : lhsVar(lVar), rhsVar(nullptr), rhsValue(rValue), flag(f) {
        }

        /// \brief construcion with params.
        /// \param lVar lhs Var for this Atomic.
        /// \param rVar rhs Var for this Atomic.
        /// \param f Flag for this Atomic. Default is true.
        Atomic(Var* lVar, Var* rVar, bool f = true) : lhsVar(lVar), rhsVar(rVar), rhsValue(nullptr), flag(f) {
        }

        /// \brief Desconstruction function.
        virtual ~Atomic() {
        }

        /// \brief Gets the flag for this Atomic.
        /// \return Boolean.
        bool isPositive() {return flag;}
        bool isPositive() const {return flag;}

        /// \brief Gets the str for this Atomic.
        /// \return string.
        virtual string getStr() = 0;
    };

    /// \brief The Atomic fomula represents the equal relation.
    ///
    /// Examples: s1 = v1 or !(s2 = s3).
    class EquAtomic : public Atomic {
    public:
        EquAtomic() : Atomic() { 
        }

        EquAtomic(Var* lVar, Value* rValue, bool f = true) : Atomic(lVar, rValue, f) {
        }

        EquAtomic(Var* lVar, Var* rVar, bool f = true) : Atomic(lVar, rVar, f) { 
        }

        ~EquAtomic() {
        }

        string getStr() {
            string lhs = lhsVar -> getName();
            string rhs = "";
            if (rhsVar) rhs = rhsVar -> getName();
            if (rhsValue) rhs = rhsValue -> getName();
            if (isPositive()) {
                return Equal(lhs, rhs);
            } else {
                return Not(Equal(lhs, rhs));
            }
        }
    private:
        string Equal(const string& lhs, const string& rhs) {
            return lhs + " = " + rhs;
        }

        string Not(const string& str) {
            return "!(" + str + ")";
        }
    };
};
#endif /* Atomic_hpp*/
