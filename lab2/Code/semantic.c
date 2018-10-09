#include "semantic.h"

void parseExtDef(struct ast *extDef) {
    struct ast *specifier = extDef->first_child;
    if(extDef->attr == DEC_LIST) {
        if(specifier->attr == BUILT_IN) {

        } else {

        }
    } else if(extDef->attr == VOID_DEC) {
        if(specifier->attr == BUILT_IN)
            return;
        // skip unsignificant declaration: int; float;
        assert(specifier->attr == TYPE_STRUCT);
        /*
            TODO:
                insert the struct definition into symbol table
         */
    } else if(extDef->attr == FUNC_DEC) {
        Type type = buildType(specifier);
        SymbolList func = buildFunc(specifier->first_sibling, type);
        /*
            TODO:
                check if function declaration
                    1. conflict with definition
                    2.
                insert into symbol table if OK
         */
        insertSymbol(func);
    } else {
        Type type = buildType(specifier);
        SymbolList func = buildFunc(specifier->first_sibling, type);
        insertSymbol(func);
        /*
            TODO:
                check if function definition
                    1. conflict with declartion
                    2. duplicate definitions
         */
        struct ast *compSt = specifier->first_sibling->first_sibling;

    }
}

void parseCompSt(struct ast *compSt) {
    pushLocalScope();
    struct ast *defList = compSt->first_child->first_sibling,
                *stmtList = defList->first_sibling;
    SymbolList symbol = parseDefList(defList);
    while(symbol != NULL) {
        insertSymbol(symbol);
        /*
            TODO:
                check symbol redefinition
         */
        symbol = symbol->next;
    }
}

SymbolList parseDefList(struct ast *defList) {
    /*  DefList is used in both FieldList and CompSt,
        thus return a link-table of symbol,
        to transfer to a field list or insert into symbol table
     */
    SymbolList head, tail;
    head = tail = (SymbolList)malloc(sizeof(struct Symbol));
    while(defList->tag != TAG_EMPTY) {
        SymbolList symbol = parseDef(defList->first_child);
        while(symbol != NULL) {
            tail->next = symbol;
            tail = symbol;
        }
        defList = defList->first_child->first_sibling;
    }
    tail->next = NULL;
    SymbolList q = head;
    head = head->next;
    free(q);
    return head;
}

SymbolList parseDef(struct ast *def) {
    struct ast *specifier = def->first_child,
            *decList = specifier->first_sibling,
            *dec = decList->first_child;
    Type type = buildType(specifier);
    SymbolList head, tail;
    head = tail = (SymbolList)malloc(sizeof(struct Symbol));
    while(dec->first_sibling != NULL) {
        SymbolList symbol = parseDec(dec, type);
        tail->next = symbol;
        tail = symbol;
    }
    tail->next = NULL;
    SymbolList p = head;
    head = head->next;
    free(p);
    return head;
}

SymbolList parseDec(struct ast *dec, Type type) {
    struct ast *varDec = dec->first_child;
    SymbolList symbol = parseVarDec(varDec, type);
    if(varDec->first_sibling != NULL) {
        struct ast *exp = varDec->first_sibling->first_sibling;
        // check type match;
    }
    return symbol;
}

SymbolList parseVarDec(struct ast *varDec, Type type) {
    if(varDec->attr == VAR_DEF) {
        // VarDec := ID
        struct ast *id = varDec->first_child;
        SymbolList symbol = (SymbolList)malloc(sizeof(struct Symbol));
        symbol->kind = VAR;
        symbol->u.type = type;
        strcpy(symbol->str, id->u.str);
        return symbol;
    } else {
        // VarDec := VarDec LB INT RB
        assert(varDec->attr == ARRAY_DEF);
        return buildArray(varDec, type);
    }
}


Type parseExp(struct ast *exp) {
    struct ast *child = exp->first_child;
    Type type = NULL;
    switch (child->tag) {
        case TAG_EXP:
            type = parseExp(child);
            break;
        case TAG_LP:
            type = parseExp(child->first_sibling);
            break;
        case TAG_MINUS:
            type = parseExp(child->first_sibling);
            if (type && type->kind != BASIC) {
                // TODO: report error
                type = NULL;
            }
            break;
        case TAG_NOT:
            type = parseExp(child->first_sibling);
            if (type && !(type->kind == BASIC && type->u.basic == TYPE_INT)) {
                // TODO: report error
                type = NULL;
            }
            break;
        case TAG_ID: {
            // TODO: SymbolList getSymbol(strcut ast*): get symbol in symbol list
            SymbolList symbol = NULL; //getSymbol(child);
            if (!symbol) {
                type = NULL;
            }
            if (symbol->kind == VAR) {
                type = symbol->u.type;
            } else {
                type = symbol->u.func->ret;
            }
            break;
        }
        case TAG_INT:
            type = (Type)malloc(sizeof(struct Type_));
            type->kind = BASIC;
            type->u.basic = TYPE_INT;
            break;
        case TAG_FLOAT:
            type = (Type)malloc(sizeof(struct Type_));
            type->kind = BASIC;
            type->u.basic = TYPE_FLOAT;
            break;
        default:
            assert(false);
            // TODO: report error
    }
    return type;
}


FieldList buildVarList(struct ast *varList) {
    FieldList head, tail;
    head = tail = (FieldList)malloc(sizeof(struct Field));

    struct ast *param = varList->first_child;
    while(param->first_sibling != NULL) {
        struct ast *specifier = param->first_child,
            *varDec = specifier->first_sibling;
        Type type = buildType(specifier);
        SymbolList symbol = parseVarDec(varDec, type);
        assert(symbol->kind == VAR);

        FieldList field = (FieldList)malloc(sizeof(struct Field));
        strcpy(field->str, symbol->str);
        field->type = symbol->u.type;
        free(symbol);

        tail->next = field;
        tail = field;

        param = param->first_sibling->first_child;
    }
    tail->next = NULL;
    FieldList q = head;
    head = head->next;
    free(q);
    return head;
}

SymbolList buildFunc(struct ast *func, Type type) {
    SymbolList symbol = (SymbolList)malloc(sizeof(struct Symbol));
    symbol->kind = DEC_FUNC;

    struct ast *id = func->first_child;
    strcpy(symbol->str, id->u.str);
    Func function = (Func)malloc(sizeof(struct Function));
    symbol->u.func = function;

    function->ret = type;
    if(id->first_sibling->first_sibling->first_sibling = NULL)
        function->args = NULL;
    else {
        struct ast *varList = id->first_sibling->first_sibling;
        function->args = buildVarList(varList);
    }
    return symbol;
}

Type buildType(struct ast *specifier) {
	if(specifier->attr == BUILT_IN) {
		Type type = (void *)malloc(sizeof(struct Type_));
		type->kind = BASIC;
		if(strcmp(specifier->u.str, "int") == 0)
			type->u.basic = TYPE_INT;
		else if(strcmp(specifier->u.str, "float") == 0)
			type->u.basic = TYPE_FLOAT;
		else
			assert(false);
		return type;
	} else {
		struct ast *structspecifier = specifier->first_child;
		if(structspecifier->attr == STRUCT_REF) {
			Type type = searchStruct(structspecifier->u.str);
			if(type == NULL)
				assert(false);
				// structrue undefined
			else
				return type;
		} else {
			assert(structspecifier->attr == STRUCT_DEC);
            return buildStruct(structspecifier->first_child);
		}
	}
}

Type buildStruct(struct ast *struct_) {
	struct ast *optTag = struct_->first_sibling,
		*defList = optTag->first_sibling->first_sibling;
	if(optTag->first_child->tag == TAG_EMPTY)
		strcpy(optTag->first_child->u.str, "NULL");

	Type dest = (Type)malloc(sizeof(struct Type_));
	dest->kind = STRUCTURE;
	strcpy(dest->u.structure.name, optTag->first_child->u.str);
	dest->u.structure.field = buildFieldList(defList);

	return dest;
}

SymbolList buildArray(struct ast *varDec, Type type) {
    SymbolList symbol = (SymbolList)malloc(sizeof(struct Symbol));
    symbol->kind = VAR;

    Type head, tail;
    head = tail = (Type)malloc(sizeof(struct Type_));
    struct ast *p = varDec->first_child;
    while(p->tag == TAG_VAR_DEC) {
        int size = p->first_sibling->first_sibling->u.ival;
        Type elem = (Type)malloc(sizeof(struct Type_));
        elem->kind =ARRAY;
        elem->u.array.size = size;

        tail->u.array.elem = elem;
        tail = elem;

        p = p->first_child;
    }
    assert(p->tag == TAG_ID);
    strcpy(symbol->str, p->u.str);
    tail->u.array.elem = type;
    symbol->u.type = head->u.array.elem;
    free(head);

    return symbol;
}


FieldList buildFieldList(struct ast *defList) {
	FieldList head, tail;
    head = tail = (FieldList)malloc(sizeof(struct Field));
	SymbolList symbol = parseDefList(defList);
    while(symbol != NULL) {
        assert(symbol->kind == VAR);

        FieldList p = (FieldList)malloc(sizeof(struct Field));
        strcpy(p->str, symbol->str);
        p->type = symbol->u.type;

        tail->next = p;
        tail = p;

        SymbolList q = symbol;
        symbol = symbol->next;
        free(q);
    }
    FieldList q = head;
    head = head->next;
    free(q);
    return head;
}

bool typeEqual(Type s1, Type s2) {
    if(s1->kind == s2->kind && s1->kind == BASIC)
        return s1->u.basic == s2->u.basic;
    if(s1->kind == s2->kind && s1->kind == ARRAY)
        return arrayEqual(s1, s2);
    if(s1->kind == s2->kind && s1->kind == STRUCTURE)
        return structEqual(s1, s2);
    return false;
}

bool structEqual(Type s1, Type s2) {
    assert(s1->kind == STRUCTURE);
    assert(s2->kind == STRUCTURE);

    FieldList f1 = s1->u.structure.field,
        f2 = s2->u.structure.field;
    bool equal = true;
    while(f1 != NULL && f2 != NULL) {
        equal = equal && typeEqual(f1->type, f2->type);
        f1 = f1->next;
        f2 = f2->next;
    }
    if(f1 != NULL || f2 != NULL)
        return false;
    else
        return equal;
}

bool arrayEqual(Type s1, Type s2) {
    assert(s1->kind == ARRAY);
    assert(s2->kind == ARRAY);

    int d1 = 0, d2 = 0;
    Type t1 = s1->u.array.elem;
    while(t1->kind == ARRAY) {
        ++d1;
        t1 = t1->u.array.elem;
    }

    Type t2 = s2->u.array.elem;
    while(t2->kind == ARRAY) {
        ++d2;
        t2 = t2->u.array.elem;
    }

    return (d1 == d2) && typeEqual(t1, t2);
}
