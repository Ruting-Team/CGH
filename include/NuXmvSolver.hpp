//
//  NuXmvSolver.hpp
//  CGH-T
//
//  Created by 何锦龙 on 2018/7/6.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef NuXmvSolver_hpp
#define NuXmvSolver_hpp

#include <vector>
#include <string>
#include <stdio.h>
using namespace std;
class Var;
class Value;
class Atomic;
class Conjunction;
class Condition;
class Transition;
class NuXmvSolver;

typedef size_t ID;
typedef vector<Value*> Values;
typedef vector<Var*> Vars;
typedef vector<Atomic*> Atomics;
typedef vector<Conjunction*> DNF;
typedef vector<Transition*> Transitions;
typedef vector<Transition*> Transitions;
typedef vector<Condition*> Conditions;



/// \brief Values in NuXmv.
class Value {
protected:
    ID id;              ///< the identity for this Value.
    string name;        ///< the name for this Value.
public:
    static ID counter;          ///< the static counter for class Value.

    /// \brief Default construction function.
    Value() : id(counter++), name("") {
    }

    /// \brief Constrction with param.
    /// \param i Id for this Value.
    Value(ID i) : id(i), name("") {
        counter = id + 1;
    }

    /// \brief Gets Id for this Value.
    /// \return ID.
    ID getID() {return id;}
    ID getID() const {return id;}

    /// \brief Gets name for this Value.
    /// \return string.
    string& getName() {return name;}
    const string& getName() const {return name;}
};

/// \brief Enum Value in NuXmv.
class EnumValue : public Value {
public:
    EnumValue() : Value() {
        name = "s" + to_string(id);
    }

    EnumValue(ID i) : Value(i) {
        name = "s" + to_string(id);
    }

    EnumValue(const string& str) : Value() {
        name = str;
    }
};

/// \brief Integer Value in NuXmv.
class IntValue : public Value {
};

/// \brief Boolean Value in NuXmv.
class BoolValue : public Value {
};

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

class NuXmvSolver {
protected:
    Vars vars;                      ///< the Vars for this NuXmvSolver.
    Values values;                  ///< the whole values for this NuXmvSolver.
    Condition* targetConfig;        ///< the vefication configuration for this NuXmvSolver.
    Conditions conditions;          ///< the Condition vector used to store Condition made in this NuXmvSolver.

public:
    /// \brief Default construction function.
    NuXmvSolver() : targetConfig(nullptr) {
    }

    /// \brief Desconstruction function.
    ~NuXmvSolver() {
        for (Var* var: vars) {
            delete var;
        }
        for (Value* value : values) {
            delete value;
        }
        for (Condition* condition : conditions) {
            delete condition;
        }
    }

    /// \brief Makes a EnumVar for this NuXmvSolver.
    /// \param vs The available values for this Var.
    EnumVar* mkEnumVar(const Values& vs) {
        EnumVar* var = new EnumVar(vs);
        vars.push_back(var);
        return var;
    }

    /// \brief Makes a EnumVar for this NuXmvSolver.
    /// \param vs The available values for this Var.
    /// \param v The initial value for this Var.
    EnumVar* mkEnumVar(const Values& vs, Value* v) {
        EnumVar* var = new EnumVar(vs, v);
        vars.push_back(var);
        return var;
    }

    /// \brief Makes a EnumVar for this NuXmvSolver.
    /// \param i The id for this Var.
    /// \param vs The available values for this Var.
    EnumVar* mkEnumVar(ID i, const Values& vs) {
        EnumVar* var = new EnumVar(i, vs);
        vars.push_back(var);
        return var;
    }

    /// \brief Makes a EnumVar for this NuXmvSolver.
    /// \param str The name for this Var.
    /// \param vs The available values for this Var.
    EnumVar* mkEnumVar(const string& str, const Values& vs) {
        EnumVar* var = new EnumVar(str, vs);
        vars.push_back(var);
        return var;
    }

    /// \brief Makes a EnumVar for this NuXmvSolver.
    /// \param i The id for this Var.
    /// \param vs The available values for this Var.
    /// \param v The initial value for this Var.
    EnumVar* mkEnumVar(ID i, const Values& vs, Value* v) {
        EnumVar* var = new EnumVar(i, vs, v);
        vars.push_back(var);
        return var;
    }

    /// \brief Makes a EnumVar for this NuXmvSolver.
    /// \param str The name for this Var.
    /// \param vs The available values for this Var.
    /// \param v The initial value for this Var.
    EnumVar* mkEnumVar(const string& str, const Values& vs, Value* v) {
        EnumVar* var = new EnumVar(str, vs, v);
        vars.push_back(var);
        return var;
    }

    /// \brief Makes a Value for this NuXmvSolver.
    /// \return Value pointer.
    EnumValue* mkValue() {
        EnumValue* value = new EnumValue();
        values.push_back(value);
        return value;
    }

    /// \brief Makes a Value for this NuXmvSolver.
    /// \param id Id for this Value.
    /// \return Value pointer.
    EnumValue* mkValue(ID id) {
        EnumValue* value = new EnumValue(id);
        values.push_back(value);
        return value;
    }

    /// \brief Makes a Value for this NuXmvSolver.
    /// \param str Name for this Value.
    /// \return Value pointer.
    EnumValue* mkValue(const string& str) {
        EnumValue* value = new EnumValue(str);
        values.push_back(value);
        return value;
    }

    Condition* mkTargetConfig() {
        Condition* condition = new Condition();
        conditions.push_back(condition);
        targetConfig = condition;
        return targetConfig;
    }

    string getVAR() {
        string res = "VAR\n";
        for (Var* var: vars) {
            res += var -> getVAR();
        }
        return res;
    }

    string getASSIGN_INIT() {
        string res = "ASSIGN\n";
        for (Var* var: vars) {
            res += var -> getASSIGN_INIT();
        }
        return res;
    }

    string getASSIGN_NEXT() {
        string res = "";
        for (StateVar* stateVar : stateVars) {
            string transitionStr = Next(stateVar) + " := case\n";
            Value* trapValue = stateVar -> getTrapValue();
            for (Transition* transition : stateVar -> getTransitions()) {
                string conditionStr = getStr(transition -> getCondition());
                StateVar* targetStateVar = transition -> getStateVar();
                Value* targetValue = transition -> getValue();
                if (targetStateVar) {
                    transitionStr += conditionStr + " : " + StateVarItem(targetStateVar) + ";\n";
                } else {
                    transitionStr += conditionStr + " : " + ValueItem(targetValue) + ";\n";
                }
            }
            if (trapValue) {
                transitionStr += "TRUE : " + ValueItem(trapValue) + ";\n";
            } else {
                transitionStr += "TRUE : " + StateVarItem(stateVar) + ";\n";
            }
            transitionStr += "esac;\n";
            res += transitionStr;
        }
        return res;
    }

    string getINVARSPEC() {
        //cout << "INV" << endl;
        string res = "INVARSPEC\n!(";
        string configurationStr = "";
        for (StateAtomics& stateAtomics: configuration) {
            string stateVarConditionStr = "";
            for (StateAtomic* stateAtomic : stateAtomics) {
                stateVarConditionStr = And(stateVarConditionStr, StateAtomicItem(stateAtomic));
            }
            configurationStr = Or(configurationStr, stateVarConditionStr);
        }
        res += configurationStr + ");";
        return res;
    }

    string getPreSMV() {
        return getVAR() + getASSIGN_INIT() + getASSIGN_NEXT();
    }

    string getSMV() {
        return getVAR() + getASSIGN_INIT() + getASSIGN_NEXT() + getINVARSPEC();
    }
};


#endif /* NuXmvSolver_hpp */
