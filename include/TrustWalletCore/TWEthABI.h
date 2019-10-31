#pragma once

#include "TWBase.h"
#include "TWString.h"
#include "TWData.h"

TW_EXTERN_C_BEGIN

/*
 * Handle to a function object.
 * It can be created, parameters get/set, used in encode, and deleted at the end.
 * Create allocates memory, the delete call should be called at the end to release memory.
 * Example:
 *   TWEthABIFunction func = TWEthABIFunctionCreate("baz");
 *   int p1index = TWEthABIFuncAddInParamByte(func, 69);
 *   int p2index = TWEthABIFuncAddOutParamByte(func);
 *   uint8_t p2val = TWEthABIFuncGetOutParamByte(func, p1index);
 *   TWEthABIFunctionDelete(&func);
 */
typedef void *_Nonnull TWEthABIFunction;

/// Creates a function object, with the given name and empty parameter list.  It must be deleted at the end.
TWEthABIFunction TWEthABIFunctionCreate(const char *_Nonnull name);

/// Deletes a function object created with a 'TWEthABIFunctionCreate' method.  The handle (passed by address) is nulled.  After delete it must not be used (can segfault)!
void TWEthABIFunctionDelete(TWEthABIFunction *_Nonnull function_inout);

/// Adds an input parameter of the given type. Returns the index of the parameter (0-based).
int TWEthABIFuncAddInParamByte(TWEthABIFunction func, uint8_t val);
int TWEthABIFuncAddInParamUInt256(TWEthABIFunction func, TWData *_Nonnull val);

/// Adds an output parameter of the given type. Returns the index of the parameter (0-based).
int TWEthABIFuncAddOutParamByte(TWEthABIFunction func);
int TWEthABIFuncAddOutParamUInt256(TWEthABIFunction func);

/// Obtain an output parameter value.
uint8_t TWEthABIFuncGetOutParamByte(TWEthABIFunction func, int paramIndex);
/// Obtain an output parameter value. Return value should be deleted.
TWData *_Nonnull TWEthABIFuncGetOutParamUInt256(TWEthABIFunction func, int paramIndex);





//TW_EXPORT_STATIC_METHOD
TWString *_Nonnull TWEthABIEncodeUint32(uint32_t i);

//TW_EXPORT_STATIC_METHOD
TWString *_Nonnull TWEthABIEncodeString(TWString *_Nonnull string);

//TW_EXPORT_STATIC_METHOD
bool TWEthABIDecodeString(TWString *_Nonnull encoded_in, TWString *_Nonnull *_Nonnull decoded_out);

TW_EXTERN_C_END
