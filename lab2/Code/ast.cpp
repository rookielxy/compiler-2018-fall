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
        
        for(int i = 1; i < n; ++i) {
            child->first_sibling = va_arg(valist, AstNode*);
            child = child->first_sibling;
        }
    } else {
        line_no = va_arg(valist, int);
        first_child = first_sibling = nullptr;
    }

    va_end(valist);
}

void AstNode::extraInfo(enum Tag tag, char *yytext) {
    switch (tag) {
        case TAG_ID: case TAG_TYPE:
            str = string(yytext);
            break;
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
    while (extDef->tag != TAG_EMPTY) {
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
        //auto func = Function(specifier->first_sibling, type, false);

        return;
    } else if (attr == FUNC_DEF) {          // ExtDef -> Specifier FunDec Compst
        //auto func = Function(specifier->first_sibling, type, true);

        return;
    } else if (attr == VOID_DEC) {          // ExtDef -> Specifier SEMI
        return;                             // do nothing
    } else if (attr == DEC_LIST) {          // ExtDef -> Specifier ExtDecList SEMI
        AstNode *extDecList = specifier->first_sibling;
        extDecList->parseExtDecList(type);
        return;
    }
    assert(false);
}

Type AstNode::parseSpecifier() {
    assert(tag == TAG_SPECIFIER);
    AstNode *child = first_child;
    if (tag == TAG_STRUCT_SPECIFIER and child->attr == STRUCT_DEC) {
        AstNode *optTag = child->first_child->first_sibling;
        string name = optTag->first_child->str;
        auto ptr = symTable.findStruct(name);
        if (ptr == nullptr) {
            string msg = "Undefined structure ";        // Error: use undefined structure to
            msg += "\"" + name + "\".";                 // define variable
            reportError(17, msg, line_no);              // return empty Type;
            return Type();
        } else {
            return *ptr;
        }
    } else {
        auto type = Type(this);
        if (not type.isBasic() and
            type.getStructName() != "anonymous")
            symTable.defineStruct(type);
        return type;
    }
}

void AstNode::parseExtDecList(const Type &type) {
    assert(tag == TAG_EXT_DEC_LIST);
    AstNode *extDecList = this, *varDec = first_child;
    while (varDec->first_sibling != nullptr) {
        symTable.defineSymbol(Symbol(varDec, type));
        extDecList = varDec->first_sibling->first_sibling;
        varDec = extDecList->first_child;
    }
}

vector<Symbol> AstNode::parseDefList(bool assign) {
    assert(tag == TAG_DEF_LIST);
    vector<Symbol> result;
    AstNode *defList = this, *def = first_child;
    while (defList->tag != TAG_EMPTY) {
        AstNode *specifier = def->first_child, 
                *decList = specifier->first_sibling;
        decList->parseDecList(result, Type(specifier), assign);
        defList = def->first_sibling;
        def = defList->first_child;
    }
}

void AstNode::parseDecList(vector<Symbol> &symbols, 
                            const Type &type, bool assign) {
    assert(tag == TAG_DEC_LIST);
    AstNode *decList = this, *dec = first_child;
    while (dec->first_sibling != nullptr) {
        assert(dec->attr == EMPTY_DEC or dec->attr == ASSIGN_DEC);
        AstNode *varDec = dec->first_child;
        Symbol symbol = Symbol(varDec, type);

        bool conflict = false;
        for (auto ele : symbols) {
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

        if (dec->attr == ASSIGN_DEC and not assign) {
            string msg = "Initialize field ";
            msg += "\"" + symbol.getName() + "\" in definition.";
            reportError(15, msg, symbol.getLineNo());
        }

        decList = dec->first_sibling->first_sibling;
        dec = decList->first_child;
    }
}

vector<Symbol> AstNode::parseVarList() {
    assert(tag == TAG_VAR_LIST);
    AstNode *param = first_child;
    vector<Symbol> result;
    while (param->first_sibling != nullptr) {
        result.emplace_back(Symbol(param));
        param = param->first_sibling->first_child;
    }
    return result;
}