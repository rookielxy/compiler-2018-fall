#include "type.h"
#include "ast.h"

Type::Type() {
	// temporary
	// to supress the warning
}

Type::Type(AstNode *specifier) {
    AstNode *child = specifier->first_child;
    assert(child->tag == TAG_STRUCT_SPECIFIER or child->tag == TAG_TYPE);

    if (child->tag == TAG_TYPE) {
        // Specifier -> Type 
        kind = BASIC;
        if (child->str == "int")
            basic = TYPE_INT;
        else if (child->str == "float")
            basic = TYPE_FLOAT;
        else
            assert(false);  
    } else {
        // Specifier -> StructSpecifier
        kind = STRUCTURE;
        AstNode *structSpecifier = child,
                *structTag = child->first_sibling;
        if (structSpecifier->attr == STRUCT_DEF and structTag->first_child->tag == TAG_ID) {
    		structure.name = structTag->first_child->str;
        } else if (structSpecifier->attr == STRUCT_DEC) {
            structure.name = structTag->first_child->str;
        } else {
            assert(false);
        }
    }
    line_no = specifier->line_no;
}

Type::Type(AstNode *varDec, const Type &type) {
    kind = ARRAY;
    line_no = varDec->line_no;
    assert(varDec->tag == TAG_VAR_DEC or varDec->tag == TAG_ID);
    if (varDec->tag == TAG_VAR_DEC) {
        varDec = varDec->first_child;
        AstNode *size = varDec->first_sibling->first_sibling;
        array.size = size->ival;
        array.elem = new Type(varDec, type);
    } else {
        *this = type;
    }
}

Type::Type(const Type &type) {
    line_no = type.line_no;
    if (type.kind == BASIC) {
        kind = BASIC;
        basic = type.basic;
    } else if (type.kind == ARRAY){
        kind = ARRAY;
        array.size = type.array.size;
        array.elem = new Type(*type.array.elem);
    } else {
        kind = STRUCTURE;
        structure.name = type.structure.name;
        structure.fields = type.structure.fields;
    }
}

Type::~Type() {
	if (kind == BASIC) {
        // do nothing
    } else if (kind == ARRAY) {
        (*array.elem).~Type();
    } else {
        structure.name.~string();
        structure.fields.~vector();
    }
}

Type& Type::operator=(const Type &type) {
    if (type.kind == BASIC) {
        kind = BASIC;
        basic = type.basic;
    } else if (type.kind == ARRAY) {
        kind = ARRAY;
        array.size = type.array.size;
        array.elem = new Type(*type.array.elem);
    } else {
        kind = STRUCTURE;
        structure.name = type.structure.name;
        structure.fields = type.structure.fields;
    }
    return *this;
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

bool Type::isBasic() {
	return kind == BASIC;
}

string Type::getStructName() const {
	return structure.name;
}

int Type::getLineNo() const {
    return line_no;
}

Field::Field(AstNode *param) {
    AstNode *specifier = param->first_child, *varDec = specifier->first_sibling,
            *id = varDec;
    while (id->tag == TAG_VAR_DEC)
        id = id->first_child;
    name = id->str;
    type = Type(specifier);

}

Field& Field::operator=(const Field &field) {
    name = field.name;
    type = field.type;
    return *this;
}