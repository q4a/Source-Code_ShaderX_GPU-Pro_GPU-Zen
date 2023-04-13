//*****************************************************************************
//* This file is part of the Dimension3 renderer library                      *
//* Copyright (C) 2003  Michal Valient (valient@host.sk)                      *
//*                                                                           *
//* Dimension3 renderer can be found at http://www.dimension3.host.sk         *
//*                                                                           *
//* This program is free software; you can redistribute it and/or modify      *
//* it under the terms of the GNU General Public License as published by      *
//* the Free Software Foundation; either version 2 of the License, or         *
//* (at your option) any later version.                                       *
//*                                                                           *
//* This program is distributed in the hope that it will be useful,           *
//* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
//* GNU General Public License for more details.                              *
//*                                                                           *
//* You should have received a copy of the GNU General Public License         *
//* along with this program; if not, write to the Free Software               *
//* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA * 
//*****************************************************************************
//hdr_utility_remove_tag


#include "stdafx.h"
#include "sc-StrHashTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStrHashTable::CStrHashTable(bool OwnsData, UINT InitialSize) {
    UINT tmpInitialSize = InitialSize;
    int logE = _LOGB("StrHashTable", NULL ,D3_DV_GRP3_LEV4,"Hashtable initialization (InitialSize is %i)",tmpInitialSize);
    this->PairCount = 0;
    this->OwnsData = OwnsData;
    //find good initial size
    if (tmpInitialSize<2) tmpInitialSize = 2;
    for (UINT i=0; i<32; i++) {
        if (hashsize(i)>=tmpInitialSize) {
            tmpInitialSize = hashsize(i);
            TableBitSize = i;
            break;
        }
    }
    _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"Real InitialSize is %i",tmpInitialSize);
    //alloc mem for primes
    AllocateMemory(tmpInitialSize, false);
    _LOGE(logE,"Initialized");
}

CStrHashTable::~CStrHashTable() {
    int logE = _LOGB("StrHashTable", NULL,D3_DV_GRP3_LEV4,"Hashtable destroyment");
    for (UINT i=0; i<this->PairCapacity; i++) {
        if (Pair[i].Key!=NULL) {
            MemMgr->FreeMem(Pair[i].Key);
            if ((OwnsData) && (Pair[i].Value!=NULL)) delete (CRootClass *)Pair[i].Value;
        }
    }
    MemMgr->FreeMem(Pair);
    _LOGE(logE,"Destroyed");
}

//////////////////////////////////////////////////////////////////////
// Grow / Shrink
//////////////////////////////////////////////////////////////////////
void CStrHashTable::AllocateMemory(const UINT ItemCount, bool Reallocate) {
    _LOG("StrHashTable", NULL,D3_DV_GRP3_LEV4,"Allocating memory for %u items", ItemCount);
    if (Reallocate) Pair = (HashPair *)MemMgr->ReAllocMem(Pair, ItemCount * sizeof(HashPair), 0);
    else Pair = (HashPair *)MemMgr->AllocMem(ItemCount * sizeof(HashPair), 0);
    for (UINT i=0; i<ItemCount; i++) {
        Pair[i].Key = NULL;
        Pair[i].Value = NULL;
    }
    this->PairCapacity = ItemCount;
}

void CStrHashTable::GrowTable(const UINT MinSize, const bool Rehash, const bool AddNew, const char *Key, const UINT Length, const void *Value) {
    UINT MinimumSize = MinSize;
    int logE = _LOGB("StrHashTable", NULL,D3_DV_GRP3_LEV4,"Grow. MinimumSize is %i", MinimumSize);
    if ((UINT)MinimumSize<PairCount + ((AddNew)?1:0)) {    //special case - bad parameter, that caues collisions.
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"Minimum size is less than item count (%i<%i) - change of parameter",MinimumSize, PairCount + ((AddNew)?1:0));
        MinimumSize = PairCount + ((AddNew)?1:0);
    }
    //Backup old items
    UINT OldCapacity = PairCapacity;
    HashPair *OldPair = Pair;
    bool RehashAgain = true;
    while (RehashAgain) {
        //find good new size
        if (MinimumSize<2) MinimumSize = 2;
        PairCapacity = MinimumSize;
        for (UINT i=0; i<32; i++) {
            if (hashsize(i)>=MinimumSize) {
                PairCapacity = hashsize(i);
                TableBitSize = i;
                break;
            }
        }
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"Real PairCapacity is %i",PairCapacity);
        AllocateMemory(PairCapacity, false);       //alloc new mem for values
        if (Rehash) {                       //rehash all stored values
            RehashAgain = false;            //collision detection in loop
            if (AddNew) {                   //If adding new one, add it first to catch collisions earlier
                UINT hashedKey = GetHash(Key, Length);
                Pair[hashedKey].Key   = (char *)MemMgr->AllocMem(Length+1,0);
                memcpy(Pair[hashedKey].Key, Key, Length);
                Pair[hashedKey].Key[Length] = 0;
                Pair[hashedKey].Value = (void *)Value;
            }
            for (UINT i=0; i<OldCapacity; i++) {
                if (OldPair[i].Key!=NULL) {
                    UINT hashedKey = GetHash(OldPair[i].Key, strlen(OldPair[i].Key));
                    if (Pair[hashedKey].Key!=NULL) {   //collision !!!
                        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"Collision! - Key %s collided with Key %s (Hash %u)",Pair[hashedKey].Key, OldPair[i].Key, hashedKey);
                        RehashAgain = true;
                        break;
                    } else {
                        Pair[hashedKey].Key   = (char *)MemMgr->AllocMem(strlen(OldPair[i].Key)+1,0);
                        memcpy(Pair[hashedKey].Key, OldPair[i].Key, strlen(OldPair[i].Key));
                        Pair[hashedKey].Key[strlen(OldPair[i].Key)] = 0;
                        Pair[hashedKey].Value = OldPair[i].Value;
                    }
                }
            }
            if (RehashAgain) {
                for (UINT i=0; i<this->PairCapacity; i++) {
                    if (Pair[i].Key!=NULL) MemMgr->FreeMem(Pair[i].Key);
                }
                MemMgr->FreeMem(Pair);
                MinimumSize = PairCapacity+1;
            } else {
                for (UINT i=0; i<OldCapacity; i++) {
                    if (OldPair[i].Key!=NULL) MemMgr->FreeMem(OldPair[i].Key);
                }
                MemMgr->FreeMem(OldPair);
                if (AddNew) PairCount++;
            }
        } else {
            RehashAgain = false;
            for (UINT i=0; i<OldCapacity; i++) {
                if (OldPair[i].Key!=NULL) MemMgr->FreeMem(OldPair[i].Key);
            }
            MemMgr->FreeMem(OldPair);
        }
    }
    _LOGE(logE,"Grown");
}

void CStrHashTable::Clear(const bool DoShrink) {
    int logE = _LOGB("StrHashTable", NULL,D3_DV_GRP3_LEV4,"Clear");
    for (UINT i=0; i<this->PairCapacity; i++) {
        if (Pair[i].Key!=NULL) {
            MemMgr->FreeMem(Pair[i].Key);
            if ((OwnsData) && (Pair[i].Value!=NULL)) delete (CRootClass *)Pair[i].Value;
        }
        Pair[i].Key = NULL;
        Pair[i].Value = NULL;
    }
    PairCount = 0;
    if (DoShrink) GrowTable(0, false, false, NULL, 0, NULL);
    _LOGE(logE,"Clear End");
}
