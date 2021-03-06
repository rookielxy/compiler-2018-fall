#include "ast.h"

void AstNode::syntaxParse() {
    assert(tag == TAG_PROGRAM);
    AstNode *extDefList = first_child, *extDef = extDefList->first_child;
    while (extDefList->tag != TAG_EMPTY) {
        extDef->parseExtDef();
        extDefList = extDef->first_sibling;
        extDef = extDefList->first_child;
    }
    symTable.checkFunc();
}

void AstNode::parseExtDef() {
    assert(tag == TAG_EXT_DEF);
    AstNode *specifier = first_child;
    auto type = specifier->parseSpecifier();
    switch (attr) {
        case FUNC_DEC: {
            AstNode *funDec = specifier->first_sibling;
            auto func = Function(funDec, type, false);
            symTable.declareFunc(func);
            break;
        }
        case FUNC_DEF: {
            AstNode *funDec = specifier->first_sibling,
                    *compSt = funDec->first_sibling;
            auto func = Function(funDec, type, true);
            symTable.defineFunc(func);
            symTable.enterScope(func);
            auto type = func.getRetType();
            compSt->parseCompSt(type);
            symTable.leaveScope();
            break;
        }
        case VOID_DEC: break;
        case DEC_LIST: {
            AstNode *extDecList = specifier->first_sibling;
            extDecList->parseExtDecList(type);
            break;
        }
        default: assert(false);    
    }
}

Type AstNode::parseSpecifier() {
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
            return Type();
        } else
            return Type(*ptr);
        
    } else {
        auto type = Type(this);                     // else branch reached if: 
        if (not type.isBasic() and                     // 1. Specifier -> TYPE 
            type.getStructName() != "anonymous")       // 2. Specifier -> StructSpecifier
            symTable.defineStruct(type);               //              -> STRUCT OptTag LC DefList RC
        return type;
    }
}

void AstNode::parseExtDecList(const Type &type) {
    assert(tag == TAG_EXT_DEC_LIST);
    AstNode *extDecList = this, *varDec = first_child;
    while (true) {
        auto symbol = Symbol(varDec, type);
        symTable.defineSymbol(symbol);
        varDec->type = new Type(symbol.getType());
        varDec->str = symbol.getName();
        if (varDec->first_sibling == nullptr)
            break;
        extDecList = varDec->first_sibling->first_sibling;
        varDec = extDecList->first_child;
    }
}

vector<Symbol> AstNode::parseDefList(bool outer) {
    assert(tag == TAG_DEF_LIST or tag == TAG_EMPTY);
    vector<Symbol> result;
    if (tag == TAG_EMPTY)
        return result;
    AstNode *defList = this, *def = first_child;
    while (defList->tag != TAG_EMPTY) {
        AstNode *specifier = def->first_child, 
                *decList = specifier->first_sibling;

        auto type = specifier->parseSpecifier();
        decList->parseDecList(result, type, outer);

        defList = def->first_sibling;
        def = defList->first_child;
    }
    return result;
}

void AstNode::parseDecList(vector<Symbol> &symbols, const Type &type, bool outer) {
    assert(tag == TAG_DEC_LIST);
    AstNode *decList = this, *dec = first_child;
    while (true) {
        assert(dec->attr == EMPTY_DEC or dec->attr == ASSIGN_DEC);
        AstNode *varDec = dec->first_child;
        auto symbol = Symbol(varDec, type);
        varDec->type = new Type(symbol.getType());
        varDec->str = symbol.getName();

        if (outer) {
            symTable.defineSymbol(symbol);
            if (dec->attr == ASSIGN_DEC) {
                AstNode *expr = varDec->first_sibling->first_sibling;
                expr->parseExp();
                if (expr->type != nullptr and (not (*expr->type == type))) {
                    string msg = "Type mismatched for assignment";
                    reportError(5, msg, varDec->line_no);
                }
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
        auto symbol = Symbol(param);
        result.emplace_back(symbol);
        param->type = new Type(symbol.getType());
        param->str = symbol.getName();
        if (param->first_sibling == nullptr)
            break;
        param = param->first_sibling->first_sibling->first_child;
    }
    return result;
}

void AstNode::parseCompSt(const Type &retType) {
    assert(tag == TAG_COMPST);
    AstNode *defList = first_child->first_sibling,
            *stmtList = defList->first_sibling,
            *stmt = stmtList->first_child;
    defList->parseDefList(true);

    while(stmtList->tag != TAG_EMPTY) {
        stmt->parseStmt(retType);
        stmtList = stmt->first_sibling;
        stmt = stmtList->first_child;
    }
}

void AstNode::parseStmt(const Type &retType) {
    assert(tag == TAG_STMT);
    switch (attr) {
        case EXP_STMT:
            first_child->parseExp();
            break;
        case COMPST_STMT:
            symTable.enterScope();
            first_child->parseCompSt(retType);
            symTable.leaveScope();
            break;
        case RETURN_STMT: {
            AstNode *expr = first_child->first_sibling;
            expr->parseExp();
            if (expr->type == nullptr)
                break;
            if (not (*expr->type == retType)) {
                string msg = "Type mismatched for return";
                reportError(8, msg, expr->line_no);
            }
            break;
        }
        case IF_STMT: case WHILE_STMT: {
            AstNode *expr = first_child->first_sibling->first_sibling,
                    *stmt = expr->first_sibling->first_sibling;
            expr->parseExp();
            stmt->parseStmt(retType);
            break;            
        }
        case IF_ELSE_STMT: {
            AstNode *expr = first_child->first_sibling->first_sibling,
                    *stmt1 = expr->first_sibling->first_sibling,
                    *stmt2 = stmt1->first_sibling->first_sibling;
            expr->parseExp();
            stmt1->parseStmt(retType);
            stmt2->parseStmt(retType);
            break;            
        }
        default: assert(false);
    }
}

void AstNode::parseExp() {
    assert(tag == TAG_EXP);
    switch(attr) {
        case INT_EXP: {
            lval = false;
            type = new Type(true);
            return;        
        }

        case FLOAT_EXP: {
            lval = false;
            type = new Type(false);
            return;
        }

        case ID_EXP: {
            lval = true;
            AstNode *id = first_child;
            assert(id->tag == TAG_ID);
            auto ptr = symTable.findGlobalSymbol(id->str);
            if (ptr == nullptr) {
                string msg = "Undefined variable ";
                msg += "\"" + id->str + "\"";
                reportError(1, msg, id->line_no);
                type = nullptr;
            } else
                type = new Type(ptr->getType());
            return;
        }

        case FUNC_EMPTY_EXP: case FUNC_ARGS_EXP: {
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
                type = nullptr;
                return;

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
                    type = nullptr;
                    return;
                }    
            }
            type = new Type(ptr->getRetType());
            return;
        }

        case STRUCT_EXP: {
            lval = true;
            AstNode *expr = first_child, *id = expr->first_sibling->first_sibling;
            expr->parseExp();
            if (expr->type == nullptr) {        // already encountered error
                type = nullptr;               // simply return
                return;
            }

            if (not expr->type->isStruct()) {
                string msg = "Illegal use of \".\"";
                reportError(13, msg, expr->line_no);
                type = nullptr;
                return;
            }

            auto field = expr->type->findField(id->str);
            if (field == nullptr) {
                string msg = "Non-existent field ";
                msg += "\"" + id->str + "\"";
                reportError(14, msg, expr->line_no);
                type = nullptr;
                return;
            }
            
            type = new Type(field->getType());
            return;
        }

        case ARRAY_EXP: {
            lval = true;
            AstNode *exp1 = first_child, *exp2 = exp1->first_sibling->first_sibling;
            exp1->parseExp();
            exp2->parseExp();
            if (exp1->type == nullptr or exp2->type == nullptr) {
                type = nullptr;
                return;
            }

            if (not exp1->type->isArray()) {
                string msg = "\"";
                msg += exp1->str + "\" is not a array";
                reportError(10, msg, exp1->line_no);
                type = nullptr; 
                return;
            }

            if (not exp2->type->isInt()) {
                string msg = "\"";
                msg += exp2->str + "\" is not a integer";
                reportError(12, msg, exp2->line_no);
                type = nullptr;
                return;
            }

            type = new Type(exp1->type->arrayElemType());
            return;
        }

        case NEST_EXP: {
            AstNode *expr = first_child->first_sibling;
            lval = expr->lval;
            expr->parseExp();
            if (expr->type == nullptr) {
                type = nullptr;
                return;
            } else {
                type = expr->type;
                return;   
            }         
        }

        case NEG_EXP: case NOT_EXP: {
            lval = false;
            AstNode *expr = first_child->first_sibling;
            expr->parseExp();
            if (expr->type == nullptr) {
                type = nullptr;
                return;
            }
            bool error1 = (attr == NOT_EXP) and (not expr->type->isInt()),
                error2 = (attr == NEG_EXP) and (not expr->type->isBasic());
            if (error1 or error2) {
                string msg = "Type mismatched for operands";
                reportError(7, msg, expr->line_no);
            }
            type = expr->type;
            return ;            
        }

        case ASSIGN_EXP: {
            lval = false;
            AstNode *exp1 = first_child, 
                    *exp2 = exp1->first_sibling->first_sibling;

            exp1->parseExp(),
            exp2->parseExp();

            if (not exp1->lval) {
                string msg = "The left-hand side of an assignment must be a variable";
                reportError(6, msg, exp1->line_no);
                type = nullptr;
                return;
            }

            if (exp1->type == nullptr or exp2->type == nullptr) {
                type = nullptr;
                return;
            }
            

            if (not (*exp1->type == *exp2->type)) {
                string msg = "Type mismatched for assignment";
                reportError(5, msg, exp1->line_no);
                type = nullptr;
                return;
            }

            type = exp1->type;
            return;           
        }

        case AND_EXP: case OR_EXP: case REL_EXP:
        case PLUS_EXP: case MINUS_EXP: case STAR_EXP: case DIV_EXP: {
            lval = false;
            AstNode *exp1 = first_child, 
                    *exp2 = exp1->first_sibling->first_sibling;
            exp1->parseExp(),
            exp2->parseExp();
            if (exp1->type == nullptr or exp2->type == nullptr) { 
                type = nullptr;
                return;
            }
            
            bool error;
            if (attr == AND_EXP or attr == OR_EXP or attr == REL_EXP)
                error = not (exp1->type->isInt() and exp2->type->isInt());
            else 
                error = not ((exp1->type->isInt() and exp2->type->isInt()) or 
                            (exp1->type->isFloat() and exp2->type->isFloat()));

            if (error) {
                string msg = "Type mismatched for operands";
                reportError(7, msg, exp1->line_no);
                type = nullptr;
                return;
            }
            type = exp1->type;
            return;       
        } 
        default: assert(false);
    }
}

vector<Type> AstNode::parseArgs() {
    assert(tag == TAG_ARGS);
    vector<Type> types;
    AstNode *args = this, *expr = first_child;
    while (true) {
        expr->parseExp();
        types.emplace_back(*expr->type);

        if (expr->first_sibling == nullptr)
            break;
        args = expr->first_sibling->first_sibling;
        expr = args->first_child;
    }
    return types;
}