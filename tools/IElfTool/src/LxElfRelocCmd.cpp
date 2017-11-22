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

// Class that implements the command for creating a self-relocating image

#include "LxElfRelocCmd.h"
#include "LxElfFile.h"

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace
{
  class DynEntry
  {
  public:
    DynEntry() : mTag(DT_NULL), mVal(0) {};
    DynEntry(Elf32_Sword tag, Elf32_Word val) : mTag(tag), mVal(val) {};

    Elf32_Sword mTag;
    Elf32_Word  mVal;

    bool IsNull() const {return mTag == DT_NULL;};
    bool IsPtr()  const;
  };

  bool DynEntry::
  IsPtr() const
  {
    return  (mTag == DT_PLTGOT
          || mTag == DT_HASH
          || mTag == DT_STRTAB
          || mTag == DT_SYMTAB
          || mTag == DT_RELA
          || mTag == DT_INIT
          || mTag == DT_FINI
          || mTag == DT_REL
          || mTag == DT_DEBUG
          || mTag == DT_JMPREL);
  }

  class DynEntries
  {
  public:
    typedef std::vector<DynEntry>   DynList;
    typedef DynList::const_iterator DynConstIter;

    void Add(const DynEntry & entry) {mEntries.push_back(entry);};
    DynEntry Find(Elf32_Sword tag) const;

    DynConstIter begin() const {return mEntries.begin();};
    DynConstIter end()   const {return mEntries.end();};

  protected:
    DynList mEntries;
  };

  DynEntry DynEntries::
  Find(Elf32_Sword tag) const
  {
    for (DynEntries::DynConstIter iter = begin();
        iter != end();
        iter++)
    {
      if ((*iter).mTag == tag)
        return *iter;
    }

    return DynEntry(DT_NULL, 0);
  }

  size_t
  WriteUleb(size_t offs, size_t last, std::ostream & o)
  {
    o << "    ";
    size_t n = 0;
    size_t x = (offs - last) / 4;
    uint8_t byte;
    do
    {
      byte = (uint8_t)(x & 0x7F);
      x >>= 7;
      if (x != 0)
        byte |= 0x80;
      o << "0x" << std::hex << byte << std::dec << ", ";
      ++n;
    } while (x != 0);
    o << "// 0x" << std::hex << offs << std::dec << "\n"; 
    return n;
  }

  size_t
  WriteUleb(size_t offs, size_t last, std::vector<uint8_t> & tab)
  {
    size_t n = 0;
    size_t x = (offs - last) / 4;
    uint8_t byte;
    do
    {
      byte = (uint8_t)(x & 0x7F);
      x >>= 7;
      if (x != 0)
        byte |= 0x80;
      tab.push_back(byte);
      ++n;
    } while (x != 0);
    return n;
  }

  typedef std::vector<LxElfSegment const *> Segments;
  Segments FindSegments(LxElfFile const & file, Elf32_Word type)
  {
    Segments found;
    for (Elf32_Word i = 0, n = file.GetNrOfSegments(); i != n; ++i)
    {
      LxElfSegment const * seg = file.GetSegment(i);
      if (seg->mHdr.p_type == type)
        found.push_back(seg);
    }
    return found;
  }

  void Pad(LxElfWriter & wi, size_t n)
  {
    for (; n != 0; --n)
    {
      wi.PutByte(0);
    }
  }

  template<typename T>
  T AlignUp(T x, int align)
  {
    T n = (x + align - 1) / align;
    return n * align;
  }
}

LxElfRelocCmd::LxElfRelocCmd(std::string const & filename,
                             unsigned long nJumpTableEntries,
                             bool withDebug)
: mFilename(filename),
  mJumpTableEntries(nJumpTableEntries),
  mWithDebug(withDebug)
{
}

// Make a thumb branch instruction for a destination offset bytes away
#if 0
static
Elf32_Word
MakeBranch(Elf32_Word offset)
{
  offset -= 8;
  if (offset & ~0x007FFFFE)
  {
    std::ostringstream os;
    os << "Unable to construct jump table branch with offset 0x"
       << std::hex << offset;
    throw std::runtime_error(os.str());
  }

  Elf32_Word imm11_high = (offset & 0x007FF000) >> 12;
  Elf32_Word imm11_low  = (offset & 0x00000FFE) >> 1;

  // Add components extracted from immediate to opcode
  Elf32_Word addend = (imm11_high << 16) | imm11_low;

  Elf32_Word mask = 0x07FF07FF;
  return 0xe800b800 | addend;
//  0xebfffffe
}
#else
static
Elf32_Half
MakeBranch(Elf32_Word offset)
{
  offset -= 4;
  if (offset & ~0xFFE)
  {
    std::ostringstream os;
    os << "Relocation overhead too large. Unable to construct jump table "
          "branch with offset "
       << offset + 4;
    throw std::runtime_error(os.str());
  }

  Elf32_Word imm = (offset & 0xFFE) >> 1;

  return 0xE000 | imm;
}
#endif

#define R_ARM_RABS32             253
#define R_ARM_RBASE              255

void LxElfRelocCmd::
Execute(LxElfFile & elfFile, bool verbose)
{
  if (!elfFile.IsARM())
    throw std::runtime_error("--self-reloc can only be used with ARM files.");

  if (verbose)
  {
    std::cout << "Building self-relocating image\n"
              << "  Relocator:  " << mFilename << "\n"
              << "  Jump table: " << mJumpTableEntries << " entries"
              << std::endl;
  }

  Segments loadSegs(FindSegments(elfFile, PT_LOAD));
  Segments dynSegs (FindSegments(elfFile, PT_DYNAMIC));

  if (loadSegs.size() != 1 && loadSegs.size() != 2)
    throw std::runtime_error("ELF file must have one or two load segments");

  if (dynSegs.size() != 1)
    throw std::runtime_error("ELF file must have exactly one dynamic segment");

  LxElfSegment const * textSeg = loadSegs[0];
  LxElfSegment const * dataSeg = NULL;
  if (loadSegs.size() == 2)
  {
    dataSeg = loadSegs[1];
    if (textSeg->mHdr.p_vaddr > dataSeg->mHdr.p_vaddr)
      std::swap(textSeg, dataSeg);
    Elf32_Addr afterText = textSeg->mHdr.p_vaddr + textSeg->mHdr.p_memsz;
    Elf32_Word align = dataSeg->mHdr.p_align;
    if (align < 4)
      align = 4;
    if (dataSeg->mHdr.p_vaddr - afterText > align)
      throw std::runtime_error("ELF file load segments must be contiguous");
  }
  LxElfSegment const * dynSeg = dynSegs[0];

  LxElfFile relFile(mFilename);
  Segments relSegs(FindSegments(relFile, PT_LOAD));
  if (relSegs.size() != 1)
  {
    throw std::runtime_error("Relocator file must have exactly one load "
                             "segment");
  }
  LxElfSegment const * relSeg = relSegs[0];

  if (elfFile.IsBigEndian() != relFile.IsBigEndian())
  {
    throw std::runtime_error("ELF file and relocator file must have same "
                             "endianess");
  }

  if (   elfFile.GetElfHeader().e_machine != EM_ARM
      || relFile.GetElfHeader().e_machine != EM_ARM)
  {
    throw std::runtime_error("ELF file and relocator file must both be "
                             "for the ARM processor");
  }

  DynEntries de;
  LxElfReader di(dynSeg->mData);
  while (!di.AtEnd())
  {
    Elf32_Sword tag = di.GetSword();
    Elf32_Word  val = di.GetWord ();

    DynEntry dyn(tag, val);
    de.Add(dyn);
    if (dyn.IsNull())
      break;
  }

  std::vector<uint8_t> table;

  DynEntry relTab = de.Find(DT_REL);
  DynEntry relSz  = de.Find(DT_RELSZ);
  DynEntry relEnt = de.Find(DT_RELENT);
  if (!relTab.IsNull() && !relSz.IsNull() && !relEnt.IsNull())
  {
    // Collect offsets of relocations
    typedef std::vector<Elf32_Addr> Offsets;
    Offsets offsets;
    LxElfReader ri(dynSeg->mData, relTab.mVal, relTab.mVal + relSz.mVal);
    while (!ri.AtEnd())
    {
      Elf32_Addr offset = ri.GetAddr();
      Elf32_Word info   = ri.GetWord();
      bool ok = false;
      switch (R_TYPE(info))
      {
      case R_ARM_RBASE:
        ok = R_SYM(info) < elfFile.GetNrOfSegments();
        break;

      case R_ARM_RABS32:
        ok = R_SYM(info) < elfFile.GetNrOfSegments();
        offsets.push_back(offset);
        break;
      }
      assert(ok);
      if (!ok)
      {
        throw std::runtime_error(
                               "Unexpected entry in dynamic relocation table");
      }
    }

    // Sort in order of increasing values.
    std::sort(offsets.begin(), offsets.end());

    // Generate table.
    size_t last = textSeg->mHdr.p_vaddr;
    size_t size = 0;
    typedef Offsets::const_iterator OIter;
    for (OIter i = offsets.begin(), n = offsets.end(); i != n; ++i)
    {
      size += WriteUleb(*i, last, table);
      last = *i;
    }
    assert(size == table.size());
  }

  Elf32_Word align = textSeg->mHdr.p_align;
  if (align < 4)
    align = 4;
  if (dataSeg != NULL && dataSeg->mHdr.p_align > align)
    align = dataSeg->mHdr.p_align;
  Elf32_Word relMemSize = AlignUp(relSeg->mHdr.p_memsz, 4);
  Elf32_Word tabSize = AlignUp(table.size(), 4);
  // Size of the mini header
  Elf32_Word hdrSize = 16;
  // Size of the jump table
  Elf32_Word jtSize = AlignUp(mJumpTableEntries * 2, relSeg->mHdr.p_align);
  // 16 = four words: self, start, entry, and size
  Elf32_Word fldSize = 16;
  Elf32_Word preSize  = hdrSize + jtSize + relMemSize + fldSize + tabSize;
  if (preSize % align != 0)
  {
    Elf32_Word xpre = AlignUp(preSize, align);
    tabSize += xpre - preSize;
    preSize = xpre;
  }
  Elf32_Word memSize  = preSize + textSeg->mHdr.p_memsz;
  Elf32_Word fileSize = preSize + textSeg->mHdr.p_filesz;
  if (verbose)
  {
    std::cout << "  Output:\n"
              << "    Mini header:      " << hdrSize    << " bytes\n"
              << "    Jump table:       " << jtSize     << " bytes\n"
              << "    Self-relocator:   " << relMemSize << " bytes\n"
              << "    Relocation table: " << fldSize + tabSize << " bytes"
              << std::endl;
  }
  if (dataSeg != NULL)
  {
    Elf32_Word textSize = dataSeg->mHdr.p_vaddr - textSeg->mHdr.p_vaddr;
    memSize  = preSize + textSize + dataSeg->mHdr.p_memsz;
    fileSize = preSize + textSize + dataSeg->mHdr.p_filesz;
  }

  if (textSeg->mHdr.p_vaddr < preSize)
  {
    throw std::runtime_error(
                   "ELF file segment address too low - no room for relocator");
  }
  Elf32_Addr loc = mWithDebug ? textSeg->mHdr.p_vaddr - preSize : 0;
  Elf32_Addr entry = loc + hdrSize;
  if (relFile.GetEntryAddr() & 1)
    entry |= 1;
  // Start building output file
  LxElfFile outFile(elfFile.IsBigEndian(),
                    EM_ARM,
                    elfFile.GetElfHeader().e_flags,
                    entry);

  LxElfSegment * outSeg =
                   outFile.AddLoadSegment(memSize,
                                          fileSize,
                                          align,
                                          textSeg->mHdr.p_flags | PF_W | PF_X,
                                          loc,
                                          ELF_HEADER_SIZE,
                                          ".load");
  // These two are only needed when generating output with debug info
  Elf32_Word ziSecIndex = outFile.GetNrOfSections() - 1;
  Elf32_Word loadSecIndex = memSize == fileSize ? ziSecIndex : ziSecIndex - 1;
  LxElfWriter wi(outSeg->mData);
  // Mini header
  wi.PutWord(relSeg->mHdr.p_filesz);                      // RO
  wi.PutWord(fileSize - relSeg->mHdr.p_filesz - hdrSize); // RW
  wi.PutWord(memSize - fileSize);                         // ZI
  wi.PutWord(0);                                          // ??
  // Jump table
  if (jtSize != 0)
  {
    wi.PutHalf(MakeBranch(jtSize));
    Elf32_Word n = 2;
    Elf32_Half extra = MakeBranch(preSize - hdrSize);
    for (unsigned long i = 1; i < mJumpTableEntries; ++i)
    {
      wi.PutHalf(extra);
      n += 2;
    }
    Pad(wi, jtSize - n);
  }
  // Relocator
  wi.Put(relSeg->mData);
  Pad(wi, relMemSize - relSeg->mHdr.p_filesz);
  Elf32_Addr self =
               textSeg->mHdr.p_vaddr - preSize + hdrSize + jtSize + relMemSize;
  wi.PutAddr(self);                    // self
  wi.PutAddr(textSeg->mHdr.p_vaddr);   // start
  wi.PutAddr(elfFile.GetEntryAddr());  // entry
  wi.PutWord(table.size());            // size
  for (size_t i = 0, n = table.size(); i != n; ++i)
  {
    wi.PutByte(table[i]);
  }
  // Pad to even 4 bytes
  Pad(wi, tabSize - table.size());
  wi.Put(textSeg->mData);
  if (dataSeg != NULL)
  {
    Elf32_Addr afterText = textSeg->mHdr.p_vaddr + textSeg->mHdr.p_filesz;
    Pad(wi, dataSeg->mHdr.p_vaddr - afterText);
    wi.Put(dataSeg->mData);
  }

  if (mWithDebug)
  {
    // Add debug sections from original file

    // Calculate new indices for all sections.
    std::vector<size_t> indices(elfFile.GetNrOfSections(), 0);
    Elf32_Word nSections = outFile.GetNrOfSections();
    if (nSections == 0)
      nSections = 2; // .shstrtab section
    for (Elf32_Word i = 1; i < elfFile.GetNrOfSections(); ++i)
    {
      LxElfSection const * sec = elfFile.GetSection(i);
      if (   (sec->mHdr.sh_flags & SHF_ALLOC) == 0
          && (   sec->mHdr.sh_type == SHT_PROGBITS
              || (   sec->mHdr.sh_type == SHT_STRTAB
                  && i != elfFile.GetSectionLabelsIdx())
              || sec->mHdr.sh_type == SHT_SYMTAB))
      {
        indices[i] = nSections;
        ++nSections;
      }
    }

    Elf32_Word pos = outSeg->mHdr.p_offset + outSeg->mHdr.p_filesz;
    for (Elf32_Word i = 1; i < elfFile.GetNrOfSections(); ++i)
    {
      if (indices[i] != 0)
      {
        LxElfSection const * sec = elfFile.GetSection(i);
        std::string secName = elfFile.GetString(sec->mHdr.sh_name,
                                                elfFile.GetSectionLabelsIdx());
        Elf32_Word link = sec->mHdr.sh_link;
        if (sec->mHdr.sh_type == SHT_SYMTAB)
          link = indices[link];
        Elf32_Word info = sec->mHdr.sh_info;
        Elf32_Word size = sec->mHdr.sh_size;
        LxElfSection * newSec = outFile.AddSection(secName,
                                                   sec->mHdr.sh_type,
                                                   sec->mHdr.sh_flags,
                                                   sec->mHdr.sh_addr,
                                                   size,
                                                   link,
                                                   info,
                                                   sec->mHdr.sh_addralign,
                                                   sec->mHdr.sh_entsize,
                                                   pos);
        pos += sec->mHdr.sh_size;
        if (sec->mHdr.sh_type == SHT_SYMTAB)
        {
          LxElfReader ri(sec->mData);
          newSec->mData.Allocate(sec->mData.GetBufLen());
          LxElfWriter wi(newSec->mData);
          while (!ri.AtEnd())
          {
            Elf32_Sym sym;
            sym.st_name  = ri.GetWord();
            sym.st_value = ri.GetAddr();
            sym.st_size  = ri.GetWord();
            sym.st_info  = ri.GetByte();
            sym.st_other = ri.GetByte();
            sym.st_shndx = ri.GetHalf();

            if (sym.st_shndx != SHN_ABS && sym.st_shndx != 0)
            {
              if (sym.st_value >= loc + fileSize)
                sym.st_shndx = ziSecIndex;
              else
                sym.st_shndx = loadSecIndex;
            }

            wi.PutWord(sym.st_name);
            wi.PutAddr(sym.st_value);
            wi.PutWord(sym.st_size);
            wi.PutByte(sym.st_info);
            wi.PutByte(sym.st_other);
            wi.PutHalf(sym.st_shndx);
          }
        }
        else
          newSec->mData = sec->mData;
      }
    }
  }

  // Install self-relocating image
  elfFile.swap(outFile);
}
