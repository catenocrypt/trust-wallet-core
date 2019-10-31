#pragma once

//#include "TWData.h"
#include "TWString.h"

TW_EXTERN_C_BEGIN

TW_EXPORT_STATIC_METHOD
TWString *_Nonnull TWEthABIEncodeUint32(uint32_t i);

TW_EXPORT_STATIC_METHOD
TWString *_Nonnull TWEthABIEncodeString(TWString *_Nonnull string);

TW_EXPORT_STATIC_METHOD
bool TWEthABIDecodeString(TWString *_Nonnull encoded_in, TWString *_Nonnull *_Nonnull decoded_out);

TW_EXTERN_C_END
