#include "ast.h"

extern bool syntaxCorrect;

AstNode *astRoot = nullptr;
SymbolTable AstNode::symTable;

string DICT[] = {
    "Program", "ExtDefList", "ExtDef",
    "Error", "ExtDecList", "Specifier",
    "StructSpecifier", "OptTag", "Empty",
    "Tag", "VarDec", "FunDec", "VarList",
    "ParamDec", "CompSt", "StmtList",
    "Stmt", "DefList", "Def", "DecList",
    "Dec", "Exp", "Args",

    "TYPE", "STRUCT", "IF", "ELSE",
    "WHILE", "RETURN", 
    "INT", "INT", "INT", "FLOAT",
    "ID", "SEMI", "COMMA", "ASSIGNOP",
    "RELOP", "PLUS", "MINUS", "STAR",
    "DIV", "AND", "OR", "DOT", "NOT",
    "LP", "RP", "LB", "RB", "LC", "RC"
};

AstNode::AstNode(enum Tag tag, int n, ...) {
    va_list valist;
    va_start(valist, n);

    this->tag = tag;
    first_child = nullptr;
    first_sibling = nullptr;

    if(n > 0) {
        AstNode *child = va_arg(valist, AstNode*);
        line_no = child->line_no;
        first_child = child;
        if (tag == TAG_EXP)
            str = child->str;
        for(int i = 1; i < n; ++i) {
            child->first_sibling = va_arg(valist, AstNode*);
            child = child->first_sibling;
            if (tag == TAG_EXP or tag == TAG_ARGS)
                str += child->str;
        }
    } else {
        str = va_arg(valist, char*);
        line_no = va_arg(valist, int);
        first_child = first_sibling = nullptr;
    }

    lval = false;
    va_end(valist);
}

void AstNode::extraInfo(enum Tag tag, char *yytext) {
    switch (tag) {
        case TAG_HEX:
            sscanf(yytext, "%x", &ival);
            break;
        case TAG_INT:
            sscanf(yytext, "%d", &ival);
            break;
        case TAG_OCT:
            sscanf(yytext, "%o", &ival);
            break;
        case TAG_FLOAT:
            sscanf(yytext, "%f", &fval);
            break;
        default:
            assert(false);
    }
}

void AstNode::travesalAst(int indent) {
	if(tag == TAG_EMPTY)
		return;
    for(int i = 0; i < 2 * indent; ++i)
        cout << " ";
    cout << DICT[tag];
    if(first_child == nullptr) {
        if (tag == TAG_ID || tag == TAG_TYPE)
            cout << ": " << str;
        else if (tag == TAG_INT)
            cout << ": " << ival;
        else if (tag == TAG_FLOAT)
            cout << ": " << fval;
        cout << endl;
        return;
    }
    cout << " (" << line_no << ")" << endl;;
    AstNode *child = first_child;
    while(child != nullptr) {
        child->travesalAst(indent + 1);
        child = child->first_sibling;
    }
}

void AstNode::syntaxParse() {
    assert(tag == TAG_PROGRAM);
    AstNode *extDefList = first_child, *extDef = extDefList->first_child;
    while (extDefList->tag != TAG_EMPTY) {
        extDef->parseExtDef();
        extDefList = extDef->first_sibling;
        extDef = extDefList->first_child;
    }
}

void AstNode::parseExtDef() {
    assert(tag == TAG_EXT_DEF);
    AstNode *specifier = first_child;
    auto type = specifier->parseSpecifier();
    if (attr == FUNC_DEC) {                 // ExtDef -> Specifier FunDec SEMI
        AstNode *funDec = specifier->first_sibling;
        auto func = Function(funDec, type, false);
        symTable.declareFunc(func);
    } else if (attr == FUNC_DEF) {          // ExtDef -> Specifier FunDec Compst
        AstNode *funDec = specifier->first_sibling,
                *compSt = funDec->first_sibling;
        auto func = Function(funDec, type, true);
        symTable.defineFunc(func);
        symTable.enterScope(func);
        compSt->parseCompSt(func.getRetType());
        symTable.leaveScope();
    } else if (attr == VOID_DEC) {          // ExtDef -> Specifier SEMI
                                            // do nothing
    } else if (attr == DEC_LIST) {          // ExtDef -> Specifier ExtDecList SEMI
        AstNode *extDecList = specifier->first_sibling;
        extDecList->parseExtDecList(type);
    } else {
        assert(false);
    }

    delete type;
    type = nullptr;
    return;
}

Type *AstNode::parseSpecifier() {
    assert(tag == TAG_SPECIFIER);
    AstNode *child = first_child;
    if (child->tag == TAG_STRUCT_SPECIFIER and child->attr == STRUCT_DEC) {
        AstNode *optTag = child->first_child->first_sibling;
        string name = optTag->first_child->str;
        auto ptr = symTable.findStruct(name);
        if (ptr == nullptr) {
            string msg = "Undefined structure ";        // Error: use undefined structure to
            msg += "\"" + name + "\".";                 // define variable
            reportError(17, msg, line_no);              // return empty Type;
            return new Type();
        } else {
            return new Type(*ptr);
        }
    } else {
        auto type = new Type(this);                     // else branch reached if: 
        if (not type->isBasic() and                     // 1. Specifier -> TYPE 
            type->getStructName() != "anonymous")       // 2. Specifier -> StructSpecifier
            symTable.defineStruct(*type);               //              -> STRUCT OptTag LC DefList RC
        return type;
    }
}

void AstNode::parseExtDecList(Type *type) {
    assert(tag == TAG_EXT_DEC_LIST);
    AstNode *extDecList = this, *varDec = first_child;
    while (varDec->first_sibling != nullptr) {
        symTable.defineSymbol(Symbol(varDec, type));
        extDecList = varDec->first_sibling->first_sibling;
        varDec = extDecList->first_child;
    }
}

vector<Symbol> AstNode::parseDefList(bool assign) {
    assert(tag == TAG_DEF_LIST or tag == TAG_EMPTY);
    vector<Symbol> result;
    if (tag == TAG_EMPTY)
        return result;
    AstNode *defList = this, *def = first_child;
    while (defList->tag != TAG_EMPTY) {
        AstNode *specifier = def->first_child, 
                *decList = specifier->first_sibling;

        auto type = specifier->parseSpecifier();
        decList->parseDecList(result, type, assign);
        delete type;
        type = nullptr;

        defList = def->first_sibling;
        def = defList->first_child;
    }
    return result;
}

void AstNode::parseDecList(vector<Symbol> &symbols, Type *type, bool assign) {
    assert(tag == TAG_DEC_LIST);
    AstNode *decList = this, *dec = first_child;
    while (true) {
        assert(dec->attr == EMPTY_DEC or dec->attr == ASSIGN_DEC);
        AstNode *varDec = dec->first_child;
        auto symbol = Symbol(varDec, type);

        if (assign) {
            symbols.emplace_back(symbol);
            if (dec->attr == ASSIGN_DEC) {
                AstNode *exp = varDec->first_sibling->first_sibling;
                auto temp = exp->parseExp();
                if (not (*temp == *type)) {
                    string msg = "Type mismatched for assignment";
                    reportError(5, msg, varDec->line_no);
                }
                delete temp;
                temp = nullptr;
            }

        } else {
            bool conflict = false;
            for (auto &ele : symbols) {
                if (ele.getName() == symbol.getName()) {
                    conflict = true;
                    break;
                }
            }

            if (not conflict)
                symbols.emplace_back(symbol);
            else {
                string msg = "Redefined field";
                msg += "\"" + symbol.getName() + "\".";
                reportError(15, msg, symbol.getLineNo());
            }

            if (dec->attr == ASSIGN_DEC) {
                string msg = "Initialize field ";
                msg += "\"" + symbol.getName() + "\" in definition.";
                reportError(15, msg, symbol.getLineNo());
            }
        }

        if (dec->first_sibling == nullptr)
            break;
        decList = dec->first_sibling->first_sibling;
        dec = decList->first_child;
    }
}

vector<Symbol> AstNode::parseVarList() {
    assert(tag == TAG_VAR_LIST);
    AstNode *param = first_child;
    vector<Symbol> result;
    while (true) {
        result.emplace_back(Symbol(param));
        if (param->first_sibling == nullptr)
            break;
        param = param->first_sibling->first_child;
    }
    return result;
}

void AstNode::parseCompSt(Type *retType) {
    assert(tag == TAG_COMPST);
    AstNode *defList = first_child->first_sibling,
            *stmtList = defList->first_sibling,
            *stmt = stmtList->first_child;
    vector<Symbol> symbols = defList->parseDefList(true);
    for (auto ele: symbols)
        symTable.defineSymbol(ele);

    while(stmtList->tag != TAG_EMPTY) {
        stmt->parseStmt(retType);
        stmtList = stmt->first_sibling;
        stmt = stmtList->first_child;
    }
}

void AstNode::parseStmt(Type *retType) {
    assert(tag == TAG_STMT);
    if (attr == EXP_STMT) {
        auto type = first_child->parseExp();
        delete type;
        type = nullptr;
    } else if (attr == COMPST_STMT) {
        symTable.enterScope();
        first_child->parseCompSt(retType);
        symTable.leaveScope();
    } else if (attr == RETURN_STMT) {
        AstNode *exp = first_child->first_sibling;
        auto type = exp->parseExp();
        if (not (*type == *retType)) {
            string msg = "Type mismatched for return";
            reportError(8, msg, exp->line_no);
        }
        delete type;
        type = nullptr;
    } else if (attr == IF_STMT or attr == WHILE_STMT) {
        AstNode *exp = first_child->first_sibling->first_sibling,
                *stmt = exp->first_sibling->first_sibling;
        auto type = exp->parseExp();
        delete type;
        type = nullptr;
        stmt->parseStmt(retType);
    } else if (attr == IF_ELSE_STMT) {
        AstNode *exp = first_child->first_sibling->first_sibling,
                *stmt1 = exp->first_sibling->first_sibling,
                *stmt2 = stmt1->first_sibling->first_sibling;
        auto type = exp->parseExp();
        delete type;
        type = nullptr;
        stmt1->parseStmt(retType);
        stmt2->parseStmt(retType);
    } else {
        assert(false);
    }
}

Type* AstNode::parseExp() {
    assert(tag == TAG_EXP);
    if (attr == INT_EXP) {
        lval = false;
        return new Type(true);

    } else if (attr == FLOAT_EXP) {
        lval = false;
        return new Type(false);

    } else if (attr == ID_EXP) {
        lval = true;
        AstNode *id = first_child;
        assert(id->tag == TAG_ID);
        auto ptr = symTable.findGlobalSymbol(id->str);
        if (ptr == nullptr) {
            string msg = "Undefined variable ";
            msg += "\"" + id->str + "\"";
            reportError(1, msg, id->line_no);
            return nullptr;
        } else
            return ptr->getType();

    } else if (attr == FUNC_EMPTY_EXP or attr == FUNC_ARGS_EXP) {
        lval = false;
        AstNode *id = first_child;
        assert(id->tag == TAG_ID);
        auto ptr = symTable.findFunc(id->str);
        if (ptr == nullptr) {
            if (symTable.findGlobalSymbol(id->str) == nullptr) {
                string msg = "Undefined function ";
                msg += "\"" + id->str + "\"";
                reportError(2, msg, id->line_no);

            } else {
                string msg = "\"";
                msg += id->str + "\" is not a function";
                reportError(11, msg, id->line_no); 
            }
            return nullptr;

        } else {
            vector<Type> types1 = ptr->getArgsType(), types2;
            if (attr == FUNC_ARGS_EXP) {
                AstNode *args = id->first_sibling->first_sibling;
                types2 = args->parseArgs();
            }
            bool applicable = true;
            if (types1.size() != types2.size())
                applicable = false;
            else {
                for (int i = 0; i < types1.size(); ++i) {
                    if (not (types1[i] == types2[i])) {
                        applicable = false;
                        break;
                    }
                }
            }

            if (not applicable) {
                string msg = "Function ";
                msg += "\"" + ptr->getName() + ptr->getArgsName() + "\"";
                msg += " is not applicable for arguments " + transferArgsToName(types2);
                reportError(9, msg, id->line_no);
                return nullptr;
            }    
        }
        return ptr->getRetType();

    } else if (attr == STRUCT_EXP) {
        lval = true;
        AstNode *exp = first_child, *id = exp->first_sibling->first_sibling;
        auto type = exp->parseExp();
        if (type == nullptr)        // already encountered error
            return nullptr;         // simply return

        if (not type->isStruct()) {
            string msg = "Illegal use of \".\"";
            reportError(13, msg, exp->line_no);
            delete type;
            type = nullptr;
            return nullptr;
        }

        auto field = type->findField(id->str);
        if (field == nullptr) {
            string msg = "Non-existent field ";
            msg += "\"" + id->str + "\"";
            reportError(14, msg, exp->line_no);
            delete type;
            type = nullptr;
            return nullptr;
        }
        
        auto fieldType = field->getType();
        delete type;
        type = nullptr;
        return fieldType;

    } else if (attr == ARRAY_EXP) {
        lval = true;
        AstNode *exp1 = first_child, *exp2 = exp1->first_sibling->first_sibling;
        auto type1 = exp1->parseExp(), type2 = exp2->parseExp();
        if (type1 == nullptr or type2 == nullptr) {
            delete type1, type2;
            return nullptr;
        }

        if (not type1->isArray()) {
            string msg = "\"";
            msg += exp1->str + "\" is not a array";
            reportError(10, msg, exp1->line_no);
            delete type1, type2;
            return nullptr; 
        }

        if (not type2->isInt()) {
            string msg = "\"";
            msg += exp2->str + "\" is not a integer";
            reportError(12, msg, exp2->line_no);
            delete type1, type2;
            return nullptr; 
        }

        return type1->arrayElemType();

    } else if (attr == NEST_EXP) {
        AstNode *exp = first_child->first_sibling;
        lval = exp->lval;
        auto type = exp->parseExp();
        if (type == nullptr)
            return nullptr;
        else
            return type;

    } else if (attr == NEG_EXP or attr == NOT_EXP) {
        lval = false;
        AstNode *exp = first_child->first_sibling;
        auto type = exp->parseExp();
        if (type == nullptr)
            return nullptr;
        bool error1 = (attr == NOT_EXP) and (not type->isInt()),
            error2 = (attr == NEG_EXP) and (not type->isBasic());
        if (error1 or error2) {
            string msg = "Type mismatched for operands";
            reportError(7, msg, exp->line_no);
            delete type;
            type = nullptr;
            return nullptr;
        }
        return type;
    } else if (attr == ASSIGN_EXP) {
        lval = false;
        AstNode *exp1 = first_child, 
                *exp2 = exp1->first_sibling->first_sibling;

        auto type1 = exp1->parseExp(),
            type2 = exp2->parseExp();

        if (not exp1->lval) {
            string msg = "The left-hand side of an assignment must be a variable";
            reportError(6, msg, exp1->line_no);
            return nullptr;
        }

        if (type1 == nullptr or type2 == nullptr) {
            delete type1, type2;
            type1 = type2 = nullptr;
            return nullptr;
        }

        if (not (*type1 == *type2)) {
            string msg = "Type mismatched for assignment";
            reportError(5, msg, exp1->line_no);
            delete type1, type2;
            type1 = type2 = nullptr;
            return nullptr;
        }

        delete type2;
        type2 = nullptr;
        return type1;
    } else {
        lval = false;
        AstNode *exp1 = first_child, 
                *exp2 = exp1->first_sibling->first_sibling;
        auto type1 = exp1->parseExp(),
            type2 = exp2->parseExp();
        if (type1 == nullptr or type2 == nullptr) {
            delete type1, type2;
            type1 = type2 = nullptr;
            return nullptr;
        }

        bool error;
        if (attr == AND_EXP or attr == OR_EXP or attr == REL_EXP)
            error = not (type1->isInt() and type2->isInt());
        else 
            error = not ((type1->isInt() and type2->isInt()) or 
                        (type1->isFloat() and type2->isFloat()));

        if (error) {
            string msg = "Type mismatched for operands";
            reportError(7, msg, exp1->line_no);
            delete type1, type2;
            type1 = type2 = nullptr;
            return nullptr;
        }
        delete type2;
        type2 = nullptr;
        return type1;
    }
}

vector<Type> AstNode::parseArgs() {
    assert(tag == TAG_ARGS);
    vector<Type> types;
    AstNode *args = this, *exp = first_child;
    while (true) {
        auto temp = exp->parseExp();
        types.emplace_back(*temp);
        delete temp;
        temp = nullptr;

        if (exp->first_sibling == nullptr)
            break;
        args = exp->first_sibling->first_sibling;
        exp = args->first_child;
    }
    return types;
}