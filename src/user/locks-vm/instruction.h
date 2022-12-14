#pragma once

enum opcode{
    END = 0xff,
    LOAD_NIL = 0x01,
    LOAD_TRUE = 0x02,
    LOAD_FALSE = 0x03,
    LOAD_CONST = 0x64,  //arg = u8 x2

    BINARY_ADD = 0x17,
    BINARY_SUBTRACT = 0x18,
    BINARY_MULTIPLY = 0x14,
    BINARY_DIVIDE = 0x15,
    BINARY_MODULO = 0x16,
    BINARY_AND = 0x40,
    BINARY_OR = 0x42,

    UNARY_NOT = 0x0c,
    UNARY_NEGATIVE = 0x0b,
    
    // arg = u8
    STORE_LOCAL = 0x5a,
    STORE_GLOBAL = 0x61,

    BIPUSH = 0x10,  //arg = u8
    LOAD_LOCAL = 0x52,   //arg = u8
    LOAD_GLOBAL = 0x74,   //arg = u8

    BUILD_LIST = 0x67, //arg = u8 x2
    BINARY_SUBSCR = 0x19,
    STORE_SUBSCR = 0x3c,

    CMPEQ = 0x9f,
    CMPNE = 0xa0,
    CMPGT = 0xa3,
    CMPLT = 0xa1,
    CMPGE = 0xa2,
    CMPLE = 0xa4,

    // arg = u8 x2
    POP_JMP_IF_TRUE = 0x70,
    POP_JMP_IF_FALSE = 0x6f,
    GOTO = 0xa7,

    CALL_FUNCTION = 0x83,  // arg= u8
    CALL_NATIVE = 0x84,  // arg= u8
    RETURN_VALUE = 0x53
};
