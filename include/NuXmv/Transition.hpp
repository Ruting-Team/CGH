//
//  Transition.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/6.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef Transition_hpp 
#define Transition_hpp 
#include "Condition.hpp"

namespace cgh {
    /// /biref Transition in NuXmv.
    class Transition {
    private:
        Condition* condition;       ///< the condition for this Transition.
        Value* targetValue;         ///< the target Value for this Transition.
        Var* targetVar;             ///< the target Var for this Transition.
    public:
        /// \brief Default construction function.
        Transition() : condition(nullptr), targetValue(nullptr), targetVar(nullptr){}

        /// \brief Construction function with params.
        /// \param c The condition for this Transition.
        /// \param v The value for this Transition.
        Transition(Condition* c, Value* v) : condition(c), targetValue(v), targetVar(nullptr){}

        /// \brief Construction function with params.
        /// \param c The condition for this Transition.
        /// \param v The var for this Transition.
        Transition(Condition* c, Var* v) : condition(c), targetValue(nullptr), targetVar(v) {}

        /// \brief Desconstruction function.
        ~Transition() {
        }

        /// \brief Gets the condition for this Transition.
        /// \return Condition pointer.
        Condition* getCondition() {return condition;}
        Condition* getCondition() const {return condition;}

        /// \brief Gets the stateVar for this Transition.
        /// \return Var pointer.
        Var* getVar() {return targetVar;}
        Var* getVar() const {return targetVar;}

        /// \brief Gets the value for this Transition.
        /// \return Value pointer.
        Value* getValue() {return targetValue;}
        Value* getValue() const {return targetValue;}

        string getStr() {
            string conditionStr = "TRUE";
            if (condition) {
                conditionStr = condition -> getStr();
            }
            if (targetVar) {
                return conditionStr + " : " + targetVar -> getName() + ";\n";
            } else {
                return conditionStr + " : " + targetValue -> getName() + ";\n";
            }
        }
    };
};
#endif /* Transition_hpp */
