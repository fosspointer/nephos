#pragma once
#include "types.hpp"

class FAT final
{
public:
    struct __attribute__((packed)) BootSector
    {
        u8 jumpInstruction[3];
        u8 oemName[8];
        u16 bytesPerSector;
        u8 sectorsPerCluster;
        u16 reservedSectors;
        u8 fileAllocationTables;
        u16 rootDirectoryEntries;
        u16 totalSectors;
        u8 mediaDescriptorType;
        u16 sectorsPerFAT;
        u16 sectorsPerTrack;
        u16 heads;
        u32 hiddenSectors;
        u32 largeSectorCount;
        u8 driveNumber;
        u8 _reserved;
        u8 signature;
        u32 volumeId;
        u8 volumeLabel[11];
        u8 systemId[8];
    };

    struct __attribute__((packed)) DirectoryEntry
    {
        u8 name[11];
        u8 attributes;
        u8 _reserved;
        u8 createdTimeSub;
        u16 createdTime;
        u16 createdDate;
        u16 lastAccessedDate;
        u16 clusterHigh;
        u16 lastModifiedTime;
        u16 lastModifiedDate;
        u16 clusterLow;
        u32 size;
    };

    static BootSector bootSector;
};