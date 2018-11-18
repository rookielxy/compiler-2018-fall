#include "type.h"
#include "ast.h"
#include "symbol.h"

Type::Type(AstNode *specifier) {
    AstNode *child = specifier->first_child;
    assert(child->tag == TAG_STRUCT_SPECIFIER or child->tag == TAG_TYPE);

    if (child->tag == TAG_TYPE) {                              
        kind = BASIC;               // Specifier -> Type
        if (child->str == "int")
            basic = TYPE_INT;
        else if (child->str == "float")
            basic = TYPE_FLOAT;
        else
            assert(false);  
    } else {                       
        kind = STRUCTURE;           // Specifier -> StructSpecifier
        AstNode *structTag = child->first_child->first_sibling,
                *defList = structTag->first_sibling->first_sibling;
        assert(child->attr == STRUCT_DEF or child->attr == STRUCT_DEC);
        if (child->attr == STRUCT_DEF) {
    		if (structTag->first_child->tag == TAG_ID) {
                structure.name = structTag->first_child->str;
            } else
                structure.name = "anonymous";           // anonymous structure

            structure.fields = defList->parseDefList(false);      // assign not permitted in fields definition
        } else {
            assert(false);                              // only declaration cannot 
                                                        // construct a type
        }
    }

    line_no = specifier->line_no;
}

Type::Type(bool integer) {
    kind = BASIC;
    if (integer)
        basic = TYPE_INT;
    else
        basic = TYPE_FLOAT;
}

Type::Type(AstNode *varDec, const Type &type) {
    line_no = varDec->line_no;
    assert(varDec->tag == TAG_VAR_DEC or varDec->tag == TAG_ID);
    varDec = varDec->first_child;
    if (varDec->tag == TAG_ID) {
        *this = type;
    } else {
        kind = ARRAY;
        AstNode *size = varDec->first_sibling->first_sibling;
        array.size = size->ival;
        array.elem = new Type(varDec, type);
    }
}

Type::Type(const Type &type) {
    kind = type.kind;
    line_no = type.line_no;
    if (type.kind == BASIC)
        basic = type.basic;
    else if (type.kind == ARRAY) {
        array.elem = new Type(*type.array.elem);
        array.size = type.array.size;
    } else {
        structure.name = type.structure.name;
        for (auto ele : type.structure.fields)
            structure.fields.emplace_back(ele);
    }
}

Type& Type::operator=(const Type &type) {
    kind = type.kind;
    line_no = type.line_no;
    if (type.kind == BASIC)
        basic = type.basic;
    else if (type.kind == ARRAY) {
        array.elem = new Type(*type.array.elem);
        array.size = type.array.size;
    } else {
        structure.name = type.structure.name;
        for (auto ele : type.structure.fields)
            structure.fields.emplace_back(ele);
    }
    return *this;
}

Type::~Type() {
    if (kind == ARRAY) {
        delete array.elem;
        array.elem = nullptr;
    }
}

bool Type::equalStructure(const Type &type) {
	if (structure.fields.size() != type.structure.fields.size())
		return false;

	for (int i = 0; i < structure.fields.size(); ++i) {
		if (not (structure.fields[i].type == type.structure.fields[i].type))
			return false;
	}
	return true;
}

bool Type::equalArray(const Type &type) {
    int d1 = 0, d2 = 0;
    Type *t1 = array.elem, *t2 = type.array.elem;
    while(t1->kind == ARRAY) {
        ++d1;
        t1 = t1->array.elem;
    }

    while(t2->kind == ARRAY) {
        ++d2;
        t2 = t2->array.elem;
    }

	return (d1 == d2) and (*t1 == *t2);
}

bool Type::operator==(const Type &type) {
	if (kind == type.kind and kind == BASIC)
		return basic == type.basic;
	else if (kind == type.kind and kind == ARRAY)
		return equalArray(type);
	else if (kind == type.kind and kind == STRUCTURE)
		return equalStructure(type);
	return false;
}

string Type::getTypeName() const {
    if (kind == BASIC)
        return basic == TYPE_INT? "int" : "float";
    else if (kind == STRUCTURE)
        return string("struct ") + structure.name;
    else {
        auto ptr = this;
        int dimension = 0;
        while (ptr->kind != ARRAY) {
            ++dimension;
            ptr = ptr->array.elem;
        }
        string re = ptr->getTypeName();
        for (int i = 0; i < dimension; ++i)
            re += "[]";
        return re;
    } 
}

int Type::getTypeSize() {
    if (kind == BASIC)
        return basic == TYPE_INT? 4 : 8;
    else if (kind == ARRAY)
        return array.size*array.elem->getTypeSize();
    else {
        int ret = 0;
        for (auto &ele : structure.fields)
            ret += ele.getType().getTypeSize();
        return ret;
    }
}

string transferArgsToName(const vector<Type> &argTypes) {
    if (argTypes.empty())
        return "()";
    string msg = "(";
    msg += argTypes.front().getTypeName();
    for (int i = 1; i < argTypes.size(); ++i)
        msg += ", " + argTypes[i].getTypeName();
    msg += ")";
    return msg;
}

Symbol* Type::findField(const string &fieldName) {
    for (auto &ele : structure.fields) {
        if (ele.getName() == fieldName)
            return &ele;
    }
    return nullptr;
}
