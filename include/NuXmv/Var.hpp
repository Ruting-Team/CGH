//
//  Var.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/7/6.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef Var_hpp 
#define Var_hpp 
#include "../Object.hpp"
#include "Transition.hpp"

using namespace std;

namespace cgh {
    /// \brief Variables in NuXmv.
    class Var {
    protected:
        ID id;                      ///< the identity for this Var.
        string name;                ///< the name for this Var.
        Values values;              ///< the available values for this Var.
        Value* initValue;           ///< the initial value for this Var.
        Transitions transitions;    ///< the transitions for this Var.
    public:
        static ID counter;          ///< the static counter for class Var.

        /// \brief Default construction function.
        Var() : id(counter++), name("s" + to_string(id)), initValue(nullptr) {
        }

        /// \brief Constrction with param.
        /// \param vs The available values for this Var.
        Var(const Values& vs) : id(counter++), name("s" + to_string(id)), values(vs.begin(), vs.end()), initValue(nullptr) {
        }

        /// \brief Constrction with param.
        /// \param vs The available values for this Var.
        Var(const Values& vs, Value* v) : id(counter++), name("s" + to_string(id)), values(vs.begin(), vs.end()), initValue(v) {
        }

        /// \brief Constrction with param.
        /// \param i The id for this Var.
        /// \param vs The available values for this Var.
        Var(ID i, const Values& vs) : id(i), name("s" + to_string(id)), values(vs.begin(), vs.end()), initValue(nullptr) {
            counter = id + 1;
        }

        /// \brief Constrction with param.
        /// \param str The name for this Var.
        /// \param vs The available values for this Var.
        Var(const string& str, const Values& vs) : id(counter++), name(str), values(vs.begin(), vs.end()), initValue(nullptr) {
        }

        /// \brief Constrction with param.
        /// \param i The id for this Var.
        /// \param vs The available values for this Var.
        /// \param v The initial value for this Var.
        Var(ID i, const Values& vs, Value* v) : id(i), name("s" + to_string(id)), values(vs.begin(), vs.end()), initValue(v) {
            counter = id + 1;
        }

        /// \brief Constrction with param.
        /// \param str The name for this Var.
        /// \param vs The available values for this Var.
        /// \param v The initial value for this Var.
        Var(const string& str, const Values& vs, Value* v) : id(counter++), name(str), values(vs.begin(), vs.end()), initValue(v) {
        }

        /// \brief Desconstruction function.
        virtual ~Var() {
            for (Transition* transition : transitions) {
                delete transition;
           }
        }

        /// \brief Gets Id for this Var.
        /// \return ID.
        ID getID() {return id;}
        ID getID() const {return id;}

        /// \brief Gets Id for this Var.
        /// \return ID.
        string& getName() {return name;}
        const string& getName() const {return name;}

        /// \brief Gets the values for this Var.
        /// \return reference of Value pointer vector.
        Values& getValues() {return values;}
        const Values& getValues() const {return values;}

        /// \brief Gets the initial Value for this StateVar.
        /// \return Value pointer.
        Value* getInitialValue() {return initValue;}
        Value* getInitialValue() const {return initValue;}

        /// \brief Gets the transitions for this StateVar.
        /// \return reference of Transition pointer vector.
        Transitions& getTransitions() {return transitions;}
        const Transitions& getTransitions() const {return transitions;}

        /// \brief Makes a transition in the transitions for this Var.
        /// \param condition The condition in the transition.
        /// \param var The var in the transition.
        /// \return Transition pointer
        Transition* mkTransition(Condition* condition, Var* var) {
            Transition* transition = new Transition(condition, var);
            transitions.push_back(transition);
            return transition;
        }

        /// \brief Makes and adds a transition in the transitions for this Var.
        /// \param condition The condition in the transition.
        /// \param value The value in the transition.
        /// \return Transition pointer
        Transition* mkTransition(Condition* condition, Value* value) {
            Transition* transition = new Transition(condition, value);
            transitions.push_back(transition);
            return transition;
        }

        /// \brief Gets the VAR string for this Var.
        /// \return string.
        virtual string getVAR() = 0;

        /// \brief Gets the ASSIGN_INIT string for this Var.
        /// \return string.
        virtual string getASSIGN_INIT() = 0;

        /// \brief Gets the ASSIGN_NEXT string for this Var.
        /// \return string.
        virtual string getASSIGN_NEXT() = 0;
    };

    /// \brief Enum Variables in NuXmv.
    class EnumVar : public Var {
    public:
        EnumVar() : Var() {
        }

        EnumVar(const Values& vs) : Var(vs) {
        }

        EnumVar(const Values& vs, Value* v) : Var(vs, v) {
        }

        EnumVar(ID i, const Values& vs) : Var(i, vs) {
        }

        EnumVar(const string& str, const Values& vs) : Var(str, vs) {
        }

        EnumVar(ID i, const Values& vs, Value* v) : Var(i, vs, v) {
        }

        EnumVar(const string& str, const Values& vs, Value* v) : Var(str, vs, v) {
        }

        ~EnumVar() {
        }

        string getVAR() {
            return name + " : " + getValuesStr();
        }

        string getASSIGN_INIT() {
            if (initValue) {
                return Init(name) + ";\n";
            }
            return "";
        }

        string getASSIGN_NEXT() {
            string res = Next(name);
            for (Transition* transition : transitions) {
                res += transition -> getStr();
            }
            return res + "esac;\n";
        }
    private:
        string List(const string& lhs, const string& rhs) {
            if (lhs.size() == 0) return rhs;
            if (rhs.size() == 0) return lhs;
            return lhs + " , " + rhs;
        }

        string Init(const string& str) {
            return "Init(" + str + ") := ";
        }

        string Next(const string& str) {
            return "next(" + str + ") := case\n";
        }

        string getValuesStr() {
            string res = "";
            for (Value* value : values) {
                res = List(res, value -> getName());
            }
            return "{ " + res + " };";
        }
    };

    /// \brief Integer Variables in NuXmv.
    class IntVar : public Var {
    };

    /// \brief Boolean Variables in NuXmv.
    class BoolVar : public Var {
    };
};

#endif /* NuXmvSolver_hpp */
