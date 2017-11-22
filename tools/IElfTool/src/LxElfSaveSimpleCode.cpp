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

/* $Rev: 23409 $ */

// Class that implements the command for saving a SimpleCode file

#include <iostream>
#include <algorithm>

#include "LxElfException.h"
#include "LxElfFile.h"
#include "LxElfSaveSimpleCode.h"


using namespace std;

void LxElfSaveSimpleCodeCmd::
Dump8(unsigned char in)
{
  /* Everything goes through here - so here we build up the checksum as well */
  mCheckSum += in;
  mOutFile << in;
}

void LxElfSaveSimpleCodeCmd::
Dump16(unsigned int in)
{
  Dump8 ((in >> 8) & 0xFF);
  Dump8 (in & 0xFF);
}

void LxElfSaveSimpleCodeCmd::
Dump32(unsigned int in)
{
  Dump16 ((in >> 16) & 0xFFFF);
  Dump16 (in & 0xFFFF);
}


void LxElfSaveSimpleCodeCmd::
GetSectionsToSave(LxElfConstSections &scns,
                  int & programBytes,
                  LxElfFile const & elfFile) const
{
  programBytes = elfFile.GetByteSections(scns);
}


void LxElfSaveSimpleCodeCmd::
DumpFileHeader(int programBytes)
{
  Dump32 (SIMPLE_CODE_MAGIC);    // Magic
  Dump32 (0);               // Program flags - just 4 zeroes for the time being
  Dump32 (programBytes);         // Number of code bytes
  Dump16 (SIMPLE_CODE_VERSION);  // Version
  if (mVerbose)
  {
    cout << "  Dumping " << programBytes << " code bytes\n";
  }
}


void LxElfSaveSimpleCodeCmd::
DumpEntryRecord(int startAddress)
{
  Dump8 (ENTRY_RECORD);
  Dump32(startAddress);
  Dump8 (SEGMENT_TYPE_CODE);
  if (mVerbose)
  {
    cout << "  Entry address: 0x" << (hex) << (uppercase)
         << startAddress << (dec) << "\n";
  }
}


void LxElfSaveSimpleCodeCmd::
DumpDataRecord(LxElfSection const * scn)
{
  if (!scn)
    throw LxSaveException();

  const Elf32_Addr scnStartAddr = scn->mHdr.sh_addr;
  const Elf32_Word scnSize      = scn->mHdr.sh_size;

  Dump8 (DATA_RECORD);
  Dump8 (SEGMENT_TYPE_CODE);
  Dump16(0);               // Record flags - currently zeroes
  Dump32(scnStartAddr);
  Dump32(scnSize);
  if (mVerbose)
  {
    cout << "  Data record - address: 0x" << (hex) << (uppercase)
         << scnStartAddr << (dec) << "\n";
    cout << "                   size: 0x" << (hex) << (uppercase)
         << scnSize << (dec) << "\n";
  }

  // Add the data bytes
  typedef LxElfDataBuffer::const_iterator Iter;
  for (Iter p = scn->mData.begin(), e = scn->mData.end(); p != e; ++p)
  {
    Dump8(*p);
  }
}


void LxElfSaveSimpleCodeCmd::
DumpEndRecord ()
{
  Dump8(END_RECORD);
  /* Now we can take out the checksum */
  int theChecksum = 0 - mCheckSum;
  Dump32(theChecksum);
  if (mVerbose)
  {
    cout << "  Checksum: 0x" << (hex) << (uppercase)
         << theChecksum << (dec) << "\n";
  }
}



void LxElfSaveSimpleCodeCmd::
Save(LxElfFile const & elfFile)
{
  int numberOfBytes;
  LxElfConstSections scns;
  GetSectionsToSave(scns, numberOfBytes, elfFile);

  /* File Header */
  DumpFileHeader(numberOfBytes);

  /* Entry record */
  if (mEntryRecord)
  {
    /* There is only ONE reason for the bool mEntryRecord and for	*/
    /* skipping entry records: CSPY does not emit entry records and	*/
    /* for being able to do some tests that we are compatible with	*/
    /* CSPY on how the .sim file should look. So the reason is only	*/
    /* IAR internal							*/
    Elf32_Addr entryAddr = elfFile.GetEntryAddr();
    DumpEntryRecord(entryAddr);
  }

  /* Data records */
  for (LxElfConstSections::const_iterator i = scns.begin(),
                                          e = scns.end();
       i != e; ++i)
  {
    DumpDataRecord(*i);
  }

  /* End record */
  DumpEndRecord();
}

/*================*/
/* public methods */
/*================*/

LxElfSaveSimpleCodeCmd::
LxElfSaveSimpleCodeCmd(const string & fileName,
                       bool entryRecord)
: mFileName(fileName),
  mEntryRecord(entryRecord)
{
  /* mOutFile gets a default construction here, it is opened in Execute () */
  mVerbose  = false;
  mCheckSum = 0;
}


void LxElfSaveSimpleCodeCmd::
Execute(LxElfFile & elfFile, bool verbose)
{
  if (verbose)
  {
    cout << "Saving SimpleCode file to " << mFileName << endl;
    mVerbose = true;
  }

  mOutFile.open(mFileName.c_str(), ios::binary);
  Save(elfFile);
}
