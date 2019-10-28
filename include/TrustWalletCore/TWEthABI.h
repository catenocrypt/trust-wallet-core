#pragma once

//#include "TWData.h"
#include "TWString.h"

TW_EXTERN_C_BEGIN

TW_EXPORT_STATIC_METHOD
TWString *_Nonnull TWEthABIEncodeUint32(uint32_t i);

TW_EXPORT_STATIC_METHOD
TWString *_Nonnull TWEthABI_AppendHello(TWString *_Nonnull str);

TW_EXTERN_C_END
