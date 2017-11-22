/*
 * Copyright (C) 2007-2009 IAR Systems AB.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* $Rev: 21495 $ */

// Class that implements the command for saving S-records

#include "LxElfSaveSRecCmd.h"

#include "LxElfFile.h"
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;

namespace
{
  class RecordChecksum
  {
  public:
    RecordChecksum() : mChecksum(0) {}

#if 0
    void Clear()
    {
      mChecksum = 0;
    }
#endif

    void Add(unsigned char data)
    {
      mChecksum += data;
    }

    void Finalize()
    {
      mChecksum = 255 - (mChecksum & 0xff);
    }

    unsigned int Get() const
    {
      return mChecksum;
    }

  private:
    unsigned int mChecksum;
  };

  class RecordDumper
  {
  public:
    RecordDumper(std::ostream & os) : mOs(os) {}

    RecordDumper(vector<unsigned char>::size_type size, std::ostream & os)
      : mOs(os)
    {
      mData.reserve(size);
    }

#if 0
    void Clear()
    {
      mData.clear();
      mChecksum.Clear();
    }

    void Add(unsigned char byte)
    {
      mData.push_back(byte);
    }
#endif

    template<class InputIterator>
    void Assign(InputIterator first, InputIterator last)
    {
      mData.assign(first, last);
    }

    void
    Dump(SRecType type, Elf32_Addr addr)
    {
      DumpType(type);
      DumpLength(type);
      DumpAddress(type, addr);
      DumpData();
      DumpChecksum();
      mOs << endl;
    }


  private:
    void
    DumpByte(unsigned char data)
    {
      static char c[] = {0,0,0};
      mOs << ToHex(data, c);
      mChecksum.Add(data);
    }

    void DumpType(SRecType type)
    {
      mOs << "S" << type;
    }

    void DumpLength(SRecType type)
    {
      int addrLen(0);
      switch (type)
      {
      case kS0:
      case kS1:
      case kS9:
        addrLen = 2;
        break;

      case kS2:
      case kS8:
        addrLen = 3;
        break;

      case kS3:
      case kS7:
        addrLen = 4;
        break;
      }

      DumpByte((unsigned char) (mData.size() + addrLen + 1));
    }

    void DumpAddress(SRecType type, Elf32_Addr addr)
    {
      switch (type)
      {
        case kS3:
        case kS7:
          DumpByte((addr >> 24) & 0xFF);
          // Fall-through

        case kS2:
        case kS8:
          DumpByte((addr >> 16) & 0xFF);
          // Fall-through

        case kS0:
        case kS1:
        case kS9:
          DumpByte((addr >> 8) & 0xFF);
          DumpByte(addr & 0xFF);
          break;
      }
    }

    void DumpData()
    {
      for_each(mData.begin(), mData.end(),
               bind1st(mem_fun(&RecordDumper::DumpByte), this));
    }

    void DumpChecksum()
    {
      mChecksum.Finalize();
      DumpByte(mChecksum.Get());
    }

  private:
    vector<unsigned char> mData;
    RecordChecksum mChecksum;
    std::ostream & mOs;
  };
} // Namespace


LxElfSaveSRecCmd::
LxElfSaveSRecCmd(string const & fileName,
                 SRecVariant variant,
                 unsigned char len)
  : LxElfSaveCmdBase(fileName, "srec"),
    mVariant(variant),
    mMaxRecordLength(len)
{
}


void LxElfSaveSRecCmd::
DumpHeader(LxElfFile const & file, std::ostream & os)
{
  std::string filename = GetFilename();
  size_t index = filename.find_last_of('\\');
  if (index == string::npos)
  {
    index = filename.find_last_of('/');
  }

  if (index != string::npos)
    filename = filename.substr(index + 1);

  // Use file name as data bytes
  RecordDumper dumper(os);
  dumper.Assign(filename.begin(), filename.end());
  dumper.Dump(kS0, 0);
}

const SRecVariant & LxElfSaveSRecCmd::
GetVariantToUse(Elf32_Addr addr) const
{
  // Set record type
  if (mVariant.IsAdaptive())
  {
    if (addr > 0xffffff)
      return kS37;
    else if (addr > 0xffff)
      return kS28;
    else
      return kS19;
  }
  else
    return mVariant;
}

Elf32_Addr LxElfSaveSRecCmd::
GetNextVariantAddr(Elf32_Addr addr) const
{
  return addr < 0x10000 ? 0x10000 : 0x1000000;
}

void LxElfSaveSRecCmd::
DumpData(Elf32_Addr startAddr,
         LxElfDataBuffer const & bytes,
         bool verbose,
         std::ostream & os)
{
  if (bytes.GetBufLen() > 0)
  {
    const LxAddressRange r(startAddr, startAddr + bytes.GetBufLen() - 1);

    Elf32_Addr addr = startAddr;
    while (r.ContainsAddress(addr))
    {
      addr = DumpRecord(addr, startAddr, bytes, os);
    }
  }
}


Elf32_Addr LxElfSaveSRecCmd::
DumpRecord(Elf32_Addr currAddr,
           Elf32_Addr dataStartAddr,
           LxElfDataBuffer const & bytes,
           std::ostream & os)
{
  const Elf32_Addr end = dataStartAddr + bytes.GetBufLen() - 1;

  // Data record length is the minimum of 
  // 1. Nr. of bytes left in the section
  // 2. Nr. of bytes until we need to change variant (if using adaptive)
  // 3. mMaxRecordLength
  Elf32_Word recordLength = min<Elf32_Word>(end - currAddr + 1,
                                            mMaxRecordLength);

  if (mVariant.IsAdaptive() && currAddr < 0x1000000)
  {
    // Calculate when we need to change variant
    recordLength = min<Elf32_Word>(recordLength,
                                   GetNextVariantAddr(currAddr) - currAddr);
  }

  // Add the data bytes
  if (recordLength > 0)
  {
    typedef LxElfDataBuffer::const_iterator CDataIter;
    CDataIter from = bytes.begin() + (currAddr - dataStartAddr);

    RecordDumper dumper(recordLength, os);
    dumper.Assign(from, from + recordLength);
    dumper.Dump(GetVariantToUse(currAddr).GetStartType(), currAddr);
  }

  return currAddr + recordLength;
}

void LxElfSaveSRecCmd::
DumpFooter(LxElfFile const & file, ostream & os)
{
  // Termination record
  // Entry address determines type of end record when using adaptive
  Elf32_Addr entryAddr = file.GetEntryAddr();

  RecordDumper dumper(os);
  dumper.Dump(GetVariantToUse(entryAddr).GetEndType(), entryAddr);
}
