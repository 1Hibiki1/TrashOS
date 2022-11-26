#pragma once

#include <libk/vector.h>
#include "types.h"

using arglist_t = kernel::Vector<LObject*>;
typedef LObject* (*builtin_fun_t)(arglist_t*);

builtin_fun_t get_builtin_at_idx(kernel::size_t idx);
kernel::u8 get_argc_at_idx(kernel::size_t idx);

LObject* locks_print(arglist_t* arglist);
LObject* locks_println(arglist_t* arglist);
LObject* locks_input(arglist_t* arglist);
LObject* locks_len(arglist_t* arglist);
LObject* locks_int(arglist_t* arglist);
LObject* locks_str(arglist_t* arglist);
LObject* locks_is_integer(arglist_t* arglist);
