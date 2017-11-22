/*
 * Copyright (C) 2011 IAR Systems AB.
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

// Class that implements the command for saving Ti Txt format.

#include "LxElfSaveTiTxtCmd.h"

#include "LxElfFile.h"
#include <iostream>


// Hex print functions.
namespace
{
  void Dump8(std::ostream & os, unsigned char data)
  {
    static char c[] = {0,0,0};
    os << ToHex(data, c);
  }

  void Dump16(std::ostream & os, unsigned short data)
  {
    Dump8(os, static_cast<uint8_t>(data >> 8));
    Dump8(os, static_cast<uint8_t>(data >> 0));
  }

  void Dump32(std::ostream & os, unsigned long data)
  {
    Dump8(os, static_cast<uint8_t>(data >> 24));
    Dump8(os, static_cast<uint8_t>(data >> 16));
    Dump8(os, static_cast<uint8_t>(data >>  8));
    Dump8(os, static_cast<uint8_t>(data >>  0));
  }
};


// Data dumper.
//
// Emits data, starts new blocks when needed and keep track of line
// endings etc.
class DataDumper
{
public:
  DataDumper()
    : mInBlock(false),
      mLineCount(0),
      mAddress(0)
  {
  }

  void DumpData(std::ostream & os,
                Elf32_Addr address,
                unsigned char data)
  {
    if (mInBlock)
    {
      if (address != mAddress)
      {
        Flush(os);
      }
    }

    if (!mInBlock)
    {
      OpenBlock(os, address);
    }
    else
    {
      if ((mLineCount % 16) == 0)
      {
        os << "\n";
      }
      else
      {
        os << " ";
      }
    }

    Dump8(os, data);

    ++mAddress;
    ++mLineCount;
  }

  void OpenBlock(std::ostream & os, Elf32_Addr address)
  {
    mInBlock = true;
    mAddress = address;
    mLineCount = 0;

    os << "@";
    if (address <= 0xFFFF)
    {
      Dump16(os, address);
    }
    else
    {
      Dump32(os, address);
    }
    os << "\n";
  }

  void Flush(std::ostream & os)
  {
    if (mInBlock)
    {
      os << "\n";
      mInBlock = false;
    }
  }

private:
  bool          mInBlock;
  Elf32_Addr    mAddress;
  unsigned long mLineCount;
};


static DataDumper sDataDumper;


LxElfSaveTiTxtCmd::
LxElfSaveTiTxtCmd(std::string const & fileName)
  : LxElfSaveCmdBase(fileName, "titxt")
{
}


void LxElfSaveTiTxtCmd::
DumpData(Elf32_Addr startAddr,
         LxElfDataBuffer const & bytes,
         bool verbose,
         std::ostream & os)
{
  for (LxElfDataBuffer::const_iterator
         i = bytes.begin(),
         e = bytes.end();
       i != e;
       ++i)
  {
    sDataDumper.DumpData(os, startAddr++, *i);
  }
}

void LxElfSaveTiTxtCmd::
DumpFooter(LxElfFile const & file, std::ostream & os)
{
  sDataDumper.Flush(os);

  // Note: The specification says that the "q" should be lower case,
  // but the examples show "Q".
  os << "q\n";
}
