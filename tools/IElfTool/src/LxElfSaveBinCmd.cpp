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

// Class that implements the command for saving as binary

#include "LxElfSaveBinCmd.h"

#include "LxElfException.h"
#include "LxElfFile.h"
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;


LxElfSaveBinCmd::
LxElfSaveBinCmd(string const & fileName)
  : mFileName(fileName)
{
}


void LxElfSaveBinCmd::
Execute(LxElfFile & elfFile, bool verbose)
{
  if (verbose)
    cout << "Saving binary file to " << mFileName << endl;

  ofstream outFile(mFileName.c_str(), ios::binary);
  Save(outFile, elfFile, verbose);
}



void LxElfSaveBinCmd::
GetSectionsToSave(LxElfConstSections & scns,
                  LxElfFile const & elfFile) const
{
  elfFile.GetByteSections(scns);
}


void LxElfSaveBinCmd::
Save(ofstream & outFile,
     LxElfFile const & elfFile,
     bool verbose) const
{
  LxElfConstSections scns;
  GetSectionsToSave(scns, elfFile);

  Elf32_Addr lastAddr(0);
  for (unsigned int i = 0; i < scns.size(); i++)
  {
    LxElfSection const * scn = scns[i];
    if (scn == NULL)
      throw LxSaveException();

    const Elf32_Addr scnStartAddr(scn->mHdr.sh_addr);
    const Elf32_Word scnSize(scn->mHdr.sh_size);

    // Add padding for gaps between sections
    if (i != 0 && scnStartAddr-1 > lastAddr)
    {
      if (verbose)
      {
        cout << "ielftool warning: Adding zero padding in "
             << "address range 0x" << hex << lastAddr + 1 << "-0x" << scnStartAddr-1 << endl;
      }
      PadFile(outFile, scnStartAddr-lastAddr-1);
    }
    SaveSection(outFile, scn);

    lastAddr = scnStartAddr + scnSize - 1;
  }
}


void LxElfSaveBinCmd::
SaveSection(std::ofstream & outFile,
            LxElfSection const * scn) const
{
  typedef LxElfDataBuffer::const_iterator Iter;
  for (Iter p = scn->mData.begin(), e = scn->mData.end(); p != e; ++p)
  {
    outFile << *p;
  }
}

void LxElfSaveBinCmd::
PadFile(std::ofstream & outFile, Elf32_Word len) const
{
  for (Elf32_Word i = 0; i < len; i++)
  {
    outFile << '\0';
  }
}
