//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"

//--//

//bool HAL_CONFIG_BLOCK::IsGoodBlock() const
//{
//    return TRUE;
//}
//
//bool HAL_CONFIG_BLOCK::IsGoodData() const
//{
//    return TRUE;
//}
//
//bool HAL_CONFIG_BLOCK::IsGood() const
//{
//    return TRUE;
//}
//
//const HAL_CONFIG_BLOCK* HAL_CONFIG_BLOCK::Next() const
//{
//    return NULL;
//}
//
//const void* HAL_CONFIG_BLOCK::Data() const
//{
//    return NULL;
//}
//
////--//
//
//bool HAL_CONFIG_BLOCK::Prepare( const char* Name, void* Data, unsigned int Size )
//{
//    return TRUE;
//}
//
////--//
//
//const HAL_CONFIG_BLOCK* HAL_CONFIG_BLOCK::Find( const char* Name, bool fSkipCurrent, bool fAppend ) const
//{
//    return NULL;
//}
//
////--//
//
//bool HAL_CONFIG_BLOCK::GetConfigSectorAddress(HAL_CONFIG_BLOCK_STORAGE_DATA& blData)
//{
//    return FALSE;
//}
//
//bool HAL_CONFIG_BLOCK::CompactBlock(HAL_CONFIG_BLOCK_STORAGE_DATA& blData, const ConfigurationSector* cfgStatic, const HAL_CONFIG_BLOCK* cfgEnd)
//{
//    return FALSE;
//}
//
//
//bool HAL_CONFIG_BLOCK::UpdateBlock( const HAL_CONFIG_BLOCK_STORAGE_DATA &blData, const void* pAddress, const HAL_CONFIG_BLOCK *Header, void* Data, size_t Length, const void* LastConfigAddress, bool isChipRO )
//{
//    return FALSE;
//}
//
//bool HAL_CONFIG_BLOCK::UpdateBlockWithName( const char* Name, void* Data, size_t Length, bool isChipRO )
//{
//    return FALSE;
//}
//

bool GetHalSystemInfo(HalSystemInfo& systemInfo)
{
    return FALSE;
}
