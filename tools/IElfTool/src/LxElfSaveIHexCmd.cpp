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

// Class that implements the command for saving an Intel hex file

#include "LxElfSaveIHexCmd.h"

#include "LxElfFile.h"
#include <iostream>
#include <algorithm>
#include <functional>

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
      // Two's complement of checksum
      mChecksum = 255 - (mChecksum & 0xff);
      mChecksum++; 
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
#endif

    void Add(unsigned char byte)
    {
      mData.push_back(byte);
    }

    template<class InputIterator>
    void Assign(InputIterator first, InputIterator last)
    {
      mData.assign(first, last);
    }

    void
    Dump(int type, Elf32_Addr addr)
    {
      mOs << ":";
      DumpLength();
      DumpOffset(addr);
      DumpType(type);
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

    void DumpLength()
    {
      DumpByte((unsigned char) mData.size());
    }

    void DumpOffset(Elf32_Addr addr)
    {
      DumpByte((unsigned char) ((addr >> 8) & 0xFF));
      DumpByte((unsigned char) (addr & 0xFF));
    }

    void DumpType(int type)
    {
      DumpByte(type);
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



LxElfSaveIHexCmd::
LxElfSaveIHexCmd(const string & fileName)
  : LxElfSaveCmdBase(fileName, "ihex"), mMaxRecordLength(16), mLastLBAAddr(0)
{
}


void LxElfSaveIHexCmd::
DumpFooter(LxElfFile const & file, ostream & os)
{
  Elf32_Addr entryAddr = file.GetEntryAddr();

   // Only linear address record for now...
  RecordDumper dumper(4, os);
  dumper.Add((entryAddr >> 24) & 0xFF);
  dumper.Add((entryAddr >> 16) & 0xFF);
  dumper.Add((entryAddr >> 8)  & 0xFF);
  dumper.Add((entryAddr)       & 0xFF);
  dumper.Dump(5, 0);

  // EOF record
  RecordDumper eofDumper(os);
  eofDumper.Dump(1, 0);
}


void LxElfSaveIHexCmd::
DumpLBA(std::ostream & os, Elf32_Addr addr)
{
  RecordDumper dumper(2, os);
  dumper.Add(addr >> 24 & 0xFF);
  dumper.Add(addr >> 16 & 0xFF);
  dumper.Dump(4, 0);

  mLastLBAAddr = addr;
}


Elf32_Addr LxElfSaveIHexCmd::
GetNextLBAAddr(Elf32_Addr currAddr)
{
  if (mLastLBAAddr == 0 && currAddr >= 0x0000FFFF)
    return currAddr;
  else if (mLastLBAAddr > 0 && (currAddr >> 16) != (mLastLBAAddr >> 16))
    return currAddr;
  else
    return ((currAddr & 0xFFFF0000) + 0x00010000);
}


void LxElfSaveIHexCmd::
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

Elf32_Addr LxElfSaveIHexCmd::
DumpRecord(Elf32_Addr currAddr,
           Elf32_Addr dataStartAddr,
           LxElfDataBuffer const & bytes,
           std::ostream & os)
{
  // One past last address
  const Elf32_Addr end = dataStartAddr + bytes.GetBufLen() - 1;

  // Data record length is the minimum of 
  // 1. Nr. of bytes left in the section
  // 2. Nr. of bytes until we need a new LBA
  // 3. mMaxRecordLength
  Elf32_Word recordLength = min<Elf32_Word>(end - currAddr + 1,
                                            mMaxRecordLength);

  // Calculate where the next LBA record should be written
  Elf32_Addr nextLBAAddr = GetNextLBAAddr(currAddr);
  recordLength = min<Elf32_Word>(recordLength, nextLBAAddr - currAddr);

  // Add the data bytes
  if (recordLength > 0)
  {
    typedef LxElfDataBuffer::const_iterator CDataIter;
    CDataIter from = bytes.begin() + (currAddr - dataStartAddr);

    RecordDumper dumper(recordLength, os);
    dumper.Assign(from, from + recordLength);
    dumper.Dump(0, currAddr);

    currAddr += recordLength;
  }

  // If needed, create an LBA address record.
  if (currAddr == nextLBAAddr && currAddr <= end)
  {
    DumpLBA(os, currAddr);
  }

  return currAddr;
}
