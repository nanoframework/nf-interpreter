//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"

/////////////////////////////////////////////////////
// DeviceBlockInfo stubs

__nfweak SectorAddress DeviceBlockInfo_PhysicalToSectorAddress(DeviceBlockInfo* blockInfo, const BlockRegionInfo* pRegion, ByteAddress phyAddress)
{
	return phyAddress;
}

__nfweak bool DeviceBlockInfo_FindRegionFromAddress(DeviceBlockInfo* blockInfo, ByteAddress address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex)
{
	return false;
}

__nfweak bool DeviceBlockInfo_FindNextUsageBlock(DeviceBlockInfo* blockInfo, unsigned int blockUsage, unsigned int* address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex)
{
	return false;
}

__nfweak bool DeviceBlockInfo_FindForBlockUsage(DeviceBlockInfo* blockInfo, unsigned int blockUsage, unsigned int* address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex)
{
	return false;
}


/////////////////////////////////////////////////////
// BlockStorageStream stubs

__nfweak bool BlockStorageStream_IsXIP(BlockStorageStream *stream)
{
	return false;
}

__nfweak bool BlockStorageStream_IsReadModifyWrite(BlockStorageStream *stream)
{
	return false;
}

__nfweak bool BlockStorageStream_SetReadModifyWrite(BlockStorageStream *stream)
{
	return false;
}

__nfweak bool BlockStorageStream_Initialize(BlockStorageStream *stream, unsigned int blockUsage)
{
	return false;
}

__nfweak bool BlockStorageStream_InitializeWithBlockStorageDevice(BlockStorageStream *stream, unsigned int blockUsage, BlockStorageDevice *pDevice)
{
	return false;
}

bool BlockStorageStream_NextStream(BlockStorageStream* stream)
{
	if (stream->Device == NULL || BlockStorageDevice_Prev(stream->Device) == NULL) return false;

	const BlockRegionInfo* pRegion;
	const DeviceBlockInfo* pDevInfo;// UNDONE: FIXME:  = stream->Device->GetDeviceInfo();

	BlockStorageStream orig;

	memcpy(&orig, stream, sizeof(orig));

	do
	{
		bool fLastIndex = false;

		if (stream->RangeIndex == 0)
		{
			if (stream->RegionIndex == 0)
			{
				stream->Device = BlockStorageDevice_Prev(stream->Device);

				if (stream->Device == NULL || BlockStorageDevice_Prev(stream->Device) == NULL)
				{
					memcpy(stream, &orig, sizeof(orig));
					return false;
				}

				// UNDONE: FIXME: pDevInfo = stream->Device->GetDeviceInfo();

				stream->RegionIndex = pDevInfo->NumRegions - 1;
			}
			else
			{
				stream->RegionIndex--;
			}

			fLastIndex = true;
		}
		else
		{
			stream->RangeIndex--;
		}

		pRegion = &pDevInfo->Regions[stream->RegionIndex];

		if (fLastIndex)
		{
			stream->RangeIndex = pRegion->NumBlockRanges - 1;
		}

	} while (BlockRange_GetBlockUsage(pRegion->BlockRanges[stream->RangeIndex]) != stream->Usage);

	// if( pDevInfo->Attribute.SupportsXIP ) Flags |= c_BlockStorageStream__XIP;
	// else Flags &= ~c_BlockStorageStream__XIP;

	stream->BlockLength = pRegion->BytesPerBlock;
	stream->BaseAddress = pRegion->Start + pRegion->BlockRanges[stream->RangeIndex].StartBlock * stream->BlockLength;
	stream->Length = BlockRange_GetBlockCount(pRegion->BlockRanges[stream->RangeIndex]) * stream->BlockLength;
	stream->CurrentIndex = 0;

	return true;
}

bool BlockStorageStream_PrevStream(BlockStorageStream* stream)
{
	if (stream->Device == NULL || BlockStorageDevice_Prev(stream->Device) == NULL) return false;

	const BlockRegionInfo* pRegion;
	const DeviceBlockInfo* pDevInfo;// UNDONE: FIXME:  = this->Device->GetDeviceInfo();

	BlockStorageStream orig;

	memcpy(&orig, stream, sizeof(orig));

	do
	{
		bool fLastIndex = false;

		if (stream->RangeIndex == 0)
		{
			if (stream->RegionIndex == 0)
			{
				stream->Device = BlockStorageDevice_Prev(stream->Device);

				if (stream->Device == NULL || BlockStorageDevice_Prev(stream->Device) == NULL)
				{
					memcpy(stream, &orig, sizeof(orig));
					return false;
				}

				// UNDONE: FIXME: pDevInfo = this->Device->GetDeviceInfo();

				stream->RegionIndex = pDevInfo->NumRegions - 1;
			}
			else
			{
				stream->RegionIndex--;
			}

			fLastIndex = true;
		}
		else
		{
			stream->RangeIndex--;
		}

		pRegion = &pDevInfo->Regions[stream->RegionIndex];

		if (fLastIndex)
		{
			stream->RangeIndex = pRegion->NumBlockRanges - 1;
		}

	} while (BlockRange_GetBlockUsage(pRegion->BlockRanges[stream->RangeIndex]) != stream->Usage);

	// if( pDevInfo->Attribute.SupportsXIP ) Flags |= c_BlockStorageStream__XIP;
	// else Flags &= ~c_BlockStorageStream__XIP;

	stream->BlockLength = pRegion->BytesPerBlock;
	stream->BaseAddress = pRegion->Start + pRegion->BlockRanges[stream->RangeIndex].StartBlock * stream->BlockLength;
	stream->Length = BlockRange_GetBlockCount(pRegion->BlockRanges[stream->RangeIndex]) * stream->BlockLength;
	stream->CurrentIndex = 0;

	return true;
}

bool BlockStorageStream_Seek(BlockStorageStream *stream, unsigned int offset, SeekOrigin origin)
{
	unsigned int seekIndex;

	switch (origin)
	{
	case BlockStorageStream_SeekBegin:
		seekIndex = 0;
		break;

	case BlockStorageStream_SeekCurrent:
		seekIndex = stream->CurrentIndex;
		break;

	case BlockStorageStream_SeekEnd:
		seekIndex = stream->Length;
		break;

	default:

		return false;
	}

	seekIndex += offset;

	while (seekIndex >= (int32_t)stream->Length)
	{
		// out of region address space
		return false;
	}

	while (seekIndex < 0)
	{
		// out of region address space
		return false;
	}

	stream->CurrentIndex = seekIndex;

	return true;
}

__nfweak bool BlockStorageStream_Write(BlockStorageStream *stream, unsigned char *data, unsigned int length)
{
	return false;
}

__nfweak bool BlockStorageStream_Erase(BlockStorageStream *stream, unsigned int length)
{
	return false;
}

__nfweak bool BlockStorageStream_ReadIntoBuffer(BlockStorageStream *stream, unsigned char *buffer, unsigned int length)
{
	return false;
}

bool BlockStorageStream_Read(BlockStorageStream* stream, unsigned char** buffer, unsigned int length)
{
	if (stream == NULL || !buffer) return false;

	unsigned char* pBuffer = *buffer;
	bool fXIPFound = false;

	while (length > 0)
	{
		int readLen = length;

		if ((stream->CurrentIndex + readLen) > stream->Length)
		{
			readLen = stream->Length - stream->CurrentIndex;
		}

		if (!fXIPFound)
		{
			*buffer = (unsigned char*)(BlockStorageStream_CurrentAddress(stream));

			fXIPFound = true;
		}

		length -= readLen;
		pBuffer += readLen;
		stream->CurrentIndex += readLen;
	}

	return true;
}

unsigned int BlockStorageStream_CurrentAddress(BlockStorageStream *stream)
{
	return stream->BaseAddress + stream->CurrentIndex;
}

__nfweak bool BlockStorageStream_IsErased(BlockStorageStream *stream, unsigned int length)
{
	return false;
}

/////////////////////////////////////////////////////
// BlockStorageDevice stubs


__nfweak BlockStorageDevice* BlockStorageDevice_Next(BlockStorageDevice* device)
{
	return NULL;
}

__nfweak BlockStorageDevice* BlockStorageDevice_Prev(BlockStorageDevice* device)
{
	return NULL;
}

__nfweak bool BlockStorageDevice_InitializeDevice(BlockStorageDevice* device)
{
	return false;
}

__nfweak bool BlockStorageDevice_UninitializeDevice(BlockStorageDevice* device)
{
	return false;
}

__nfweak DeviceBlockInfo* BlockStorageDevice_GetDeviceInfo(BlockStorageDevice* device)
{
	return NULL;
}

__nfweak bool BlockStorageDevice_Read(BlockStorageDevice* device, unsigned int address, unsigned int numBytes, unsigned char* pSectorBuff)
{
	return false;
}

__nfweak bool BlockStorageDevice_Write(BlockStorageDevice* device, unsigned int address, unsigned int numBytes, unsigned char* pSectorBuf, bool readModifyWrite)
{
	return false;
}

__nfweak bool BlockStorageDevice_Memset(BlockStorageDevice* device, unsigned int address, unsigned char data, unsigned int NumBytes)
{
	return false;
}

//__nfweak bool BlockStorageDevice_GetSectorMetadata(BlockStorageDevice* device, unsigned int sectorStart, SectorMetadata* pSectorMetadata);
//__nfweak bool BlockStorageDevice_SetSectorMetadata(BlockStorageDevice* device, unsigned int sectorStart, SectorMetadata* pSectorMetadata);

__nfweak bool BlockStorageDevice_IsBlockErased(BlockStorageDevice* device, unsigned int blockStartAddress, unsigned int blockLength)
{
	return false;
}

__nfweak bool BlockStorageDevice_EraseBlock(BlockStorageDevice* device, unsigned int address)
{
	return false;
}

__nfweak void BlockStorageDevice_SetPowerState(BlockStorageDevice* device, unsigned int state)
{
	return;
}

__nfweak bool BlockStorageDevice_FindRegionFromAddress(BlockStorageDevice* device, unsigned int address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex)
{
	return false;
}

__nfweak bool BlockStorageDevice_FindForBlockUsage(BlockStorageDevice* device, unsigned int blockUsage, unsigned int* address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex)
{
	return false;
}

__nfweak bool BlockStorageDevice_FindNextUsageBlock(BlockStorageDevice* device, unsigned int blockUsage, unsigned int* address, unsigned int* blockRegionIndex, unsigned int* blockRangeIndex)
{
	return false;
}

/////////////////////////////////////////////////////
