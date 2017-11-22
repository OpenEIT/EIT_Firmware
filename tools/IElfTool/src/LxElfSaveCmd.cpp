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

// Class that implements the command for saving an elf file

#include "LxElfSaveCmd.h"
#include "LxElfException.h"
#include "LxElfFile.h"

#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

namespace
{
  void
  PadFile(Elf32_Off nrOfBytes, ostream & outFile)
  {
    for (Elf32_Word i = 0; i < nrOfBytes; i++)
    {
      outFile << '\0';
    }
  }

  class Contents
  {
  public:
    void AddBuffer(Elf32_Off off, LxElfDataBuffer const & buf)
    {
      LxElfDataBuffer const * & tbuf = mTable[off];
      if (tbuf != NULL)
        throw LxSaveException();
      tbuf = &buf;
    }

    LxElfDataBuffer & MkBuffer(Elf32_Off off, Elf32_Off size, bool bigEndian)
    {
      LxElfDataBuffer tmp(size, bigEndian);
      mBuffers.resize(mBuffers.size() + 1);
      mBuffers.back().swap(tmp);
      AddBuffer(off, mBuffers.back());
      return mBuffers.back();
    }

    void Save(std::ostream & o) const
    {
      Elf32_Off pos = 0;
      typedef Table::const_iterator TIter;
      for (TIter i = mTable.begin(), n = mTable.end(); i != n; ++i)
      {
        if (pos > i->first)
          throw LxSaveException();
        PadFile(i->first - pos, o);
        LxSave(*i->second, o);
        pos = i->first + i->second->GetBufLen();
      }
    }

  private:
    typedef std::map<Elf32_Off, LxElfDataBuffer const *> Table;
    Table mTable;
    std::deque<LxElfDataBuffer> mBuffers;
  };

  void
  SaveElfHeader(Contents & c, LxElfFile const & elfFile)
  {
    // Save elf header
    ElfHeader elfHdr = elfFile.GetElfHeader();

    LxElfDataBuffer & elfHeaderBuf =
                         c.MkBuffer(0, elfHdr.e_ehsize, elfFile.IsBigEndian());
    LxElfWriter wi(elfHeaderBuf);
    for (int i = 0; i < EI_NIDENT; i++)
    {
      wi.PutByte(elfHdr.e_ident[i]);
    }
    wi.PutHalf(elfHdr.e_type);
    wi.PutHalf(elfHdr.e_machine);
    wi.PutWord(elfHdr.e_version);
    wi.PutAddr(elfHdr.e_entry);
    wi.PutOff (elfHdr.e_phnum != 0 ? elfHdr.e_phoff : 0);
    wi.PutOff (elfHdr.e_shnum != 0 ? elfHdr.e_shoff : 0);
    wi.PutWord(elfHdr.e_flags);
    wi.PutHalf(elfHdr.e_ehsize);
    wi.PutHalf(elfHdr.e_phentsize);
    wi.PutHalf(elfHdr.e_phnum);
    wi.PutHalf(elfHdr.e_shentsize);
    wi.PutHalf(elfHdr.e_shnum);
    wi.PutHalf(elfHdr.e_shstrndx);
  }

  void
  SavePgHeaders(Contents & c, LxElfFile const & elfFile)
  {
    ElfHeader elfHdr = elfFile.GetElfHeader();

    //Elf32_Half phnum     = elfHdr.e_phnum;
    Elf32_Off  phoff     = elfHdr.e_phoff;
    Elf32_Half phentsize = elfHdr.e_phentsize;

    Elf32_Off pgHdrsSize  = elfFile.GetNrOfSegments() * phentsize;

    // Save all program headers
    LxElfDataBuffer & pgHdrsBuf =
                          c.MkBuffer(phoff, pgHdrsSize, elfFile.IsBigEndian());
    LxElfWriter wi(pgHdrsBuf);
    for (Elf32_Half i = 0; i < elfFile.GetNrOfSegments(); i++)
    {
      Elf32_Phdr pgHdr = elfFile.GetSegment(i)->mHdr;

      wi.PutWord(pgHdr.p_type);
      wi.PutWord(pgHdr.p_offset);
      wi.PutWord(pgHdr.p_vaddr);
      wi.PutWord(pgHdr.p_paddr);
      wi.PutWord(pgHdr.p_filesz);
      wi.PutWord(pgHdr.p_memsz);
      wi.PutWord(pgHdr.p_flags);
      wi.PutWord(pgHdr.p_align);
    }
  }

  void
  SaveSectionHeaders(Contents & c, LxElfFile const & elfFile)
  {
    ElfHeader elfHdr = elfFile.GetElfHeader();

    Elf32_Half shnum     = elfHdr.e_shnum;
    Elf32_Off  shoff     = elfHdr.e_shoff;
    Elf32_Half shentsize = elfHdr.e_shentsize;

    Elf32_Off scnHdrsSize = (Elf32_Half) elfFile.GetNrOfSections() * shentsize;

    if (shnum != 0)
    {
      // Save all section headers
      LxElfDataBuffer & elfScnHdrsBuf =
                           c.MkBuffer(shoff, scnHdrsSize, elfFile.IsBigEndian());
      LxElfWriter wi(elfScnHdrsBuf);
      for (Elf32_Half i = 0; i < elfFile.GetNrOfSections(); i++)
      {
        LxElfSection const * scn = elfFile.GetSection(i);

        Elf32_Shdr const & scnHdr(scn->mHdr);
        wi.PutWord(scnHdr.sh_name);
        wi.PutWord(scnHdr.sh_type);
        wi.PutWord(scnHdr.sh_flags);
        wi.PutWord(scnHdr.sh_addr);
        wi.PutOff (scnHdr.sh_offset);
        wi.PutWord(scnHdr.sh_size);
        wi.PutWord(scnHdr.sh_link);
        wi.PutWord(scnHdr.sh_info);
        wi.PutWord(scnHdr.sh_addralign);
        wi.PutWord(scnHdr.sh_entsize);
      }
    }
  }

  void
  SaveContents(Contents & c, LxElfFile const & elfFile)
  {
    for (Elf32_Word i = 0; i < elfFile.GetNrOfSegments(); i++)
    {
      LxElfSegment const * seg = elfFile.GetSegment(i);
      if (seg->mData.IsOwner() && seg->mData.GetBufLen() != 0)
        c.AddBuffer(seg->mHdr.p_offset, seg->mData);
    }

    for (Elf32_Word i = 0; i < elfFile.GetNrOfSections(); i++)
    {
      LxElfSection const * scn = elfFile.GetSection(i);
      if (scn->mData.IsOwner() && scn->mData.GetBufLen() != 0)
        c.AddBuffer(scn->mHdr.sh_offset, scn->mData);
    }
  }
}


LxElfSaveCmd::
LxElfSaveCmd(string const & fileName)
  : mFileName(fileName)
{
}


void LxElfSaveCmd::
Execute(LxElfFile & elfFile, bool verbose)
{
  if (verbose)
    cout << "Saving ELF file to " << mFileName << endl;

  Contents c;
  SaveElfHeader     (c, elfFile);
  SavePgHeaders     (c, elfFile);
  SaveSectionHeaders(c, elfFile);
  SaveContents      (c, elfFile);

  ofstream outFile(mFileName.c_str(), ios::binary);
  c.Save(outFile);
}
