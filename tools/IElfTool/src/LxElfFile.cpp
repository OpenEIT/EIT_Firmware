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

/* $Rev: 25169 $ */

// This class implements low-level elf file handling

#include "LxElfFile.h"

#include "LxElfException.h"

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string.h>

using namespace std;


class LxElfFileObserver
{
public:
  LxElfFileObserver(LxElfFile & elfFile) : mElfFile(elfFile)
  {
    mElfFile.RegisterObserver(this);
  }

  virtual ~LxElfFileObserver()
  {
    mElfFile.UnRegisterObserver(this);
  }

  virtual void BytesInserted(Elf32_Off off, Elf32_Word size) = 0;
  virtual void BytesRemoved (Elf32_Off off, Elf32_Word size) = 0;

private:
  LxElfFile & mElfFile;
};

namespace
{
  class LxOffsetAdjuster : public LxElfFileObserver
  {
  public:
    LxOffsetAdjuster(LxElfFile & elfFile, Elf32_Off & off)
      : LxElfFileObserver(elfFile), mOff(off) {}

    virtual void BytesInserted(Elf32_Off off, Elf32_Word size)
    {
      if (off <= mOff)
        mOff += size;
    }

    virtual void BytesRemoved (Elf32_Off off, Elf32_Word size)
    {
      if (off < mOff)
      {
        assert(off + size <= mOff);
        mOff -= size;
      }
    }

  private:
    Elf32_Off & mOff;
  };

  template<class T>
  struct Delete : public unary_function<T, void>
  {
    void operator()(T p)
    {
      delete p;
    }
  };
}

LxElfFile::
LxElfFile(bool bigEndian,
          Elf32_Half machine,
          Elf32_Word flags,
          Elf32_Addr entry)
  : mElfBigEndian(bigEndian), mSymTabHdrIdx(-1), mHasSymbolTable(false)
{
  static const ElfHeader sNull = {{ 0 }};
  mElfHdr = sNull;
  mElfHdr.e_ident[EI_MAG0]    = ELFMAG0;
  mElfHdr.e_ident[EI_MAG1]    = ELFMAG1;
  mElfHdr.e_ident[EI_MAG2]    = ELFMAG2;
  mElfHdr.e_ident[EI_MAG3]    = ELFMAG3;
  mElfHdr.e_ident[EI_CLASS]   = ELFCLASS32;
  mElfHdr.e_ident[EI_DATA]    = bigEndian ? ELFDATA2MSB : ELFDATA2LSB;
  mElfHdr.e_ident[EI_VERSION] = EV_CURRENT;

  mElfHdr.e_type      = ET_EXEC;
  mElfHdr.e_machine   = machine;
  mElfHdr.e_version   = EV_CURRENT;
  mElfHdr.e_entry     = entry;
  mElfHdr.e_phoff     = ELF_HEADER_SIZE;
  mElfHdr.e_shoff     = ELF_HEADER_SIZE;
  mElfHdr.e_flags     = flags;
  mElfHdr.e_ehsize    = ELF_HEADER_SIZE;
  mElfHdr.e_phentsize = ELF_PROGRAM_HEADER_SIZE;
  mElfHdr.e_phnum     = 0;
  mElfHdr.e_shentsize = ELF_SECTION_HEADER_SIZE;
  mElfHdr.e_shnum     = 0;
  mElfHdr.e_shstrndx  = 0;
}

LxElfFile::
LxElfFile(std::string const & filename)
  : mElfBigEndian(false),
    mSymTabHdrIdx(-1),
    mHasSymbolTable(false),
    mFileName(filename)
{
  Load(filename);
}


LxElfFile::
~LxElfFile()
{
  for_each(mScns.begin(), mScns.end(), Delete<LxElfSection*>());
}


void LxElfFile::
swap(LxElfFile & x)
{
  using std::swap;
  swap(mElfBigEndian, x.mElfBigEndian);
  swap(mElfHdr,       x.mElfHdr);
  swap(mSegments,     x.mSegments);
  swap(mScns,         x.mScns);
  swap(mSymTabHdrIdx, x.mSymTabHdrIdx);
}


LxElfSegment * LxElfFile::
AddLoadSegment(Elf32_Word          memSize,
               Elf32_Word          fileSize,
               Elf32_Word          align,
               Elf32_Word          flags,
               Elf32_Addr          addr,
               Elf32_Off           pos,
               std::string const & secName)
{
  // Make space for the data.
  InsertSpace(pos, fileSize);

  // Build program header
  Elf32_Phdr pgHdr;
  pgHdr.p_align  = align;
  pgHdr.p_filesz = fileSize;
  pgHdr.p_flags  = flags;
  pgHdr.p_memsz  = memSize;
  pgHdr.p_offset = pos;
  pgHdr.p_paddr  = addr;
  pgHdr.p_type   = PT_LOAD;
  pgHdr.p_vaddr  = addr;

  LxOffsetAdjuster posAdj(*this, pos);
  LxOffsetAdjuster pgAdj (*this, pgHdr.p_offset);

  InsertSpace(mElfHdr.e_phoff, mElfHdr.e_phentsize);
  mElfHdr.e_phoff -= mElfHdr.e_phentsize; // Undo adjustment
  mElfHdr.e_phnum++;
  LxElfSegment * seg = new LxElfSegment(pgHdr, IsBigEndian());
  seg->mData.Allocate(fileSize);
  mSegments.push_back(seg);

  if (!secName.empty())
  {
    Elf32_Word secFlags = SHF_ALLOC;
    if (flags & PF_X)
      secFlags |= SHF_EXECINSTR;
    if (flags & PF_W)
      secFlags |= SHF_WRITE;
    LxElfSection * sect = AddSection(secName,
                                     SHT_PROGBITS,
                                     secFlags,
                                     addr,
                                     /*size=*/0,
                                     /*link=*/0,
                                     /*info=*/0,
                                     align,
                                     /*entSize=*/1,
                                     pos);
    sect->mHdr.sh_size = fileSize;
    sect->mData.SetWindow(seg->mData, 0, fileSize);
    if (memSize != fileSize)
    {
      AddSection(secName + "_zi",
                 SHT_NOBITS,
                 secFlags,
                 addr + fileSize,
                 /*size=*/memSize - fileSize,
                 /*link=*/0,
                 /*info=*/0,
                 1,
                 /*entSize=*/1,
                 pos + fileSize);
    }
  }

  return seg;
}

LxElfSection * LxElfFile::
AddSection(std::string const & name,
           Elf32_Word type,
           Elf32_Word flags,
           Elf32_Addr addr,
           Elf32_Word size,
           Elf32_Word link,
           Elf32_Word info,
           Elf32_Word align,
           Elf32_Word entSize,
           Elf32_Off  pos)
{
  // Make space for the data.
  if (type != SHT_NOBITS)
    InsertSpace(pos, size);

  LxOffsetAdjuster posAdj(*this, pos);

  if (mElfHdr.e_shnum == 0)
  {
//    mElfHdr.e_shoff = GetAlignedValue(pos + size);
    Elf32_Shdr nullSec = { 0 };
    AddSectionHeader(nullSec);
  }

  if (GetSectionLabelsIdx() == 0)
  {
    mElfHdr.e_shstrndx = mElfHdr.e_shnum;
    std::string name(".shstrtab");
    Elf32_Shdr shdr;
    shdr.sh_addr      = 0;
    shdr.sh_offset    = mElfHdr.e_shoff;
    shdr.sh_addralign = 0;
    shdr.sh_name      = 1;
    shdr.sh_flags     = 0;
    shdr.sh_type      = SHT_STRTAB;
    shdr.sh_entsize   = 0;
    shdr.sh_link      = 0;
    shdr.sh_info      = 0;
    shdr.sh_size      = name.size() + 2;
    InsertSpace(shdr.sh_offset, shdr.sh_size);
    LxElfSection * sec = AddSectionHeader(shdr);
    sec->mData.Allocate(name.size() + 2);
    char * p = (char *)&*sec->mData.begin();
    *p++ = '\0';
    strcpy(p, name.c_str());
  }

  // Create a new name for the section
  Elf32_Word newStrIdx = AddStringToStringTable(name, GetSectionLabelsIdx());

  Elf32_Shdr scnHdr;
  scnHdr.sh_addr      = addr;
  scnHdr.sh_offset    = pos;
  scnHdr.sh_addralign = align;
  scnHdr.sh_name      = newStrIdx;
  scnHdr.sh_flags     = flags;
  scnHdr.sh_type      = type;
  scnHdr.sh_entsize   = entSize;
  scnHdr.sh_link      = link;
  scnHdr.sh_info      = info;
  scnHdr.sh_size      = size;

  LxOffsetAdjuster shAdj(*this, scnHdr.sh_offset);

  LxElfSection * sect = AddSectionHeader(scnHdr);
  if (type != SHT_NOBITS)
    sect->mData.Allocate(size);

  return sect;
}

void LxElfFile::
ExpandSection(LxElfSection * sec, Elf32_Word extra)
{
  InsertSpace(sec->mHdr.sh_offset + sec->mHdr.sh_size, extra);
  sec->mHdr.sh_size += extra;
  sec->mData.Expand(extra);
}

LxElfSection * LxElfFile::
AddSectionHeader(Elf32_Shdr const & shdr)
{
  InsertSpace(mElfHdr.e_shoff, mElfHdr.e_shentsize);
  mElfHdr.e_shoff -= mElfHdr.e_shentsize; // Undo adjustment
  ++mElfHdr.e_shnum;
  LxElfSection * sect = new LxElfSection(shdr, mElfBigEndian);
  mScns.push_back(sect);
  return sect;
}

bool LxElfFile::
RemoveSection(Elf32_Word idx)
{
  if (!IsRemovableSection(idx))
    return false;

  LxElfSection* delScn = mScns[idx];
  if (delScn == NULL)
    return false;

  Elf32_Word delFileSize = delScn->mHdr.sh_size;

  // Remove the section from the vector
  mScns.erase(remove(mScns.begin(),
                     mScns.end(),
                     delScn),
               mScns.end());

  if (!delScn->IsNoBits() && delScn->mData.IsOwner())
  {
    // Update all offsets
    RemoveSpace(delScn->mHdr.sh_offset, delFileSize);
  }

  --mElfHdr.e_shnum;

  if (idx < mElfHdr.e_shstrndx)
    --mElfHdr.e_shstrndx;

  return true;
}

void LxElfFile::
InsertSpace(Elf32_Off offset, Elf32_Word length0)
{
  // For ARM, adjust length to even 4, so as not to disturb sections that need
  // align 4.
  AdjustSize(offset, length0, true);
}


void LxElfFile::
RemoveSpace(Elf32_Off offset, Elf32_Word length)
{
  AdjustSize(offset, length, false);
}

namespace
{
  LxOffsetAdjuster*
  CreateOffsetAdjusterFromSection(LxElfSection* s, LxElfFile * f)
  {
    return new LxOffsetAdjuster(*f, s->mHdr.sh_offset);
  }

  LxOffsetAdjuster*
  CreateOffsetAdjusterFromSegment(LxElfSegment* s, LxElfFile * f)
  {
    return new LxOffsetAdjuster(*f, s->mHdr.p_offset);
  }
}

void LxElfFile::
AdjustSize(Elf32_Off offset, Elf32_Word length, bool larger)
{
  LxElfFileObservers obs;
  transform(mScns.begin(),
            mScns.end(),
            back_inserter(obs),
            bind2nd(ptr_fun(CreateOffsetAdjusterFromSection), this));

  transform(mSegments.begin(),
            mSegments.end(),
            back_inserter(obs),
            bind2nd(ptr_fun(CreateOffsetAdjusterFromSegment), this));

  LxOffsetAdjuster phoffAdj(*this, mElfHdr.e_phoff);
  LxOffsetAdjuster shoffAdj(*this, mElfHdr.e_shoff);

  NotifySizeChange(offset, length, larger);

  // Observers are not needed anymore
  for_each(obs.begin(), obs.end(), Delete<LxElfFileObserver*>());
}

namespace
{
  bool IsProgAlloc(const LxElfSection* section)
  {
    return section != NULL && section->IsAlloc() && section->IsProgBits();
  }

  bool IsProgAllocAndIntersectsRange(const LxElfSection* section,
                                     LxAddressRange range)
  {
    return (IsProgAlloc(section) && section->GetRange().Intersects(range));
  }

  bool IsProgAllocAndContainsAddr(const LxElfSection* section,
                                  Elf32_Addr addr)
  {
    return (IsProgAlloc(section) &&
            section->GetRange().ContainsAddress(addr));
  }

  bool SegmentIntersectsRange(const LxElfSegment* segment,
                              LxAddressRange range)
  {
    return (segment->GetRange().Intersects(range));
  }
}


LxElfSection* LxElfFile::
GetSectionAtAddr(Elf32_Addr addr)
{
  typedef LxElfSections::const_iterator Iter;
  Iter i = find_if(mScns.begin(),
                   mScns.end(),
                   bind2nd(ptr_fun(IsProgAllocAndContainsAddr), addr));

  return i == mScns.end() ? (LxElfSection*) NULL : *i;
}

void LxElfFile::
GetSectionsInAddrRange(LxAddressRange  range,
                       LxElfConstSections & scns) const
{
  scns.clear();

  typedef LxElfSections::const_iterator Iter;
  for (Iter p = mScns.begin(), q = mScns.end() ; p != q ; ++p)
  {
    if (IsProgAllocAndIntersectsRange(*p, range))
      scns.push_back(*p);    
  }
}

void LxElfFile::
GetSectionsInAddrRange(LxAddressRange  range,
                       LxElfSections & scns)
{
  LxElfConstSections tmp;
  GetSectionsInAddrRange(range, tmp);
  scns.clear();
  typedef LxElfConstSections::const_iterator Iter;
  for (Iter i = tmp.begin(), n = tmp.end(); i != n; ++i)
  {
    scns.push_back(const_cast<LxElfSection *>(*i));
  }
}

void LxElfFile::
GetSegmentsInAddrRange(LxAddressRange  range,
                       LxElfConstSegments & segments) const
{
  segments.clear();

  // Collect segments overlapping the given range. Ignore empty segments.
  typedef Segments::const_iterator SIter;
  for (SIter i = mSegments.begin(), n = mSegments.end(); i != n; ++i)
  {
    if ((*i)->mHdr.p_filesz != 0 && SegmentIntersectsRange(*i, range))
      segments.push_back(*i);
  }

  std::sort(segments.begin(), segments.end(), LxElfSegmentSortByVAddr);
}

int LxElfFile::
GetByteSections(LxElfConstSections & scns) const
{
  int nBytes = 0;
  scns.clear();

  for (Elf32_Word i = 0, n = GetNrOfSections(); i !=n; ++i)
  {
    LxElfSection const * scn = GetSection(i);
    
    if ( scn->IsAlloc() &&
        !scn->IsNoBits() &&
        !scn->IsNull())
    {
      scns.push_back(scn);
      nBytes += scn->mHdr.sh_size;
    }
  }

  std::sort(scns.begin(), scns.end(), LxElfScnSortByAddr);

  for (size_t i = 1, n = scns.size(); i < n; ++i)
  {
    LxElfSection const * prev = scns[i-1];
    LxElfSection const * curr = scns[i];
    if (prev->mHdr.sh_addr + prev->mHdr.sh_size > curr->mHdr.sh_addr)
    {
      std::ostringstream os;
      os << "Section overlap: ";
      DescribeSectionWithRange(prev, os);
      os << " overlaps ";
      DescribeSectionWithRange(curr, os);
      throw LxMessageException(os.str());
    }
  }

  return nBytes;
}

bool LxElfFile::
IsRemovableSection(Elf32_Half idx)
{
  if (idx > mScns.size() - 1)
    return false;

  LxElfSection* scn = mScns[idx];

  // Do not remove the section string table or the NULL section
  if (idx == mElfHdr.e_shstrndx || !scn || scn->IsNull())
  {
    return false;
  }

  // Remove non-alloc sections
  return (scn->IsSymbolTable() || !scn->IsAlloc());
}


// Returns the index in the string table for the given symbol name
// strShdrIdx is the index of the string table
Elf32_Word LxElfFile::
GetStringIndex(const string & symbol,
                          Elf32_Word     sectionIdx) const
{
  LxElfSection const* strScn = GetSection(sectionIdx);
  char const * start = (char const *)&*strScn->mData.begin();
  char const * end   = (char const *)&*strScn->mData.end();

  for (char const * p = start; p != end; ++p)
  {
    if (symbol.compare(p) == 0)
    {
      return p - start;
    }
    p += strlen(&*p);
  }

  throw LxSymbolException(symbol, LxSymbolException::kStringNotFound);
}


// Returns the string in the string table that corresponds to the given
// string index. strShdrIdx is the index of the string table
string LxElfFile::
GetString(Elf32_Word index, Elf32_Half strShdrIdx) const
{
  LxElfSection const * strScn  = GetSection(strShdrIdx);

  if (strScn && index < strScn->mHdr.sh_size)
  {
    return (char const *) &*(strScn->mData.begin() + index);
  }
  else
  {
    throw LxSymbolException("", LxSymbolException::kStringNotFound);
  }
}


// Returns the symbol in the symbol table that corresponds to the given
// string index. symIdx is the index of the symbol table
pair<Elf32_Sym, Elf32_Word> LxElfFile::
GetSymbol(Elf32_Word strIndex) const
{
  Elf32_Sym  sym;
  Elf32_Word symIdx = 0;
  LxElfSection* symScn = GetSymbolSection();

  // Use string index to find symbol in symbol table
  LxElfReader ri(symScn->mData);
  while (!ri.AtEnd())
  {
    // Check if this is the symbol that we are looking for
    sym.st_name  = ri.GetWord();
    sym.st_value = ri.GetAddr();
    sym.st_size  = ri.GetWord();
    sym.st_info  = ri.GetByte();
    sym.st_other = ri.GetByte();
    sym.st_shndx = ri.GetHalf();
    if (sym.st_name == strIndex)
      return make_pair(sym, symIdx);
    ++symIdx;
  }

  throw LxSymbolException("", LxSymbolException::kSymbolNotFound);
}

Elf32_Addr LxElfFile::
GetSymbolAddress(std::string const & name) const
{
  Elf32_Word strIndex = GetStringIndex(name, GetSymbolSection()->mHdr.sh_link);

  try
  {
    pair<Elf32_Sym, Elf32_Word> sw = GetSymbol(strIndex);
    return sw.first.st_value;
  }
  catch(...)
  {
    throw LxSymbolException(name, LxSymbolException::kSymbolNotFound);
  }
}


Elf32_Word LxElfFile::
AddStringToStringTable(string const & str, Elf32_Half sectionIdx)
{
  LxElfSection* strScn = GetSection(sectionIdx);

  // Expand the existing string table
  Elf32_Off strlen((Elf32_Off) str.length() + 1);
  strScn->mData.Expand(strlen);

  Elf32_Word oldSize = strScn->mHdr.sh_size;
  strScn->mHdr.sh_size += strlen;

  // For ARM, make sure the next section is 4-byte aligned
  InsertSpace(strScn->mHdr.sh_offset + oldSize, strlen);

  // Store the null-terminated string
  strcpy((char *)(strScn->mData.begin() + oldSize), str.c_str());

  return strScn->mHdr.sh_size - strlen;
}


void LxElfFile::
AddSymbol(const Elf32_Sym & sym)
{
  const Elf32_Off symbolEntrySize = 16;
  LxElfSection* symScn = GetSymbolSection();

  Elf32_Word oldSize = symScn->mHdr.sh_size;

  // Expand the existing symbol table
  symScn->mData.Expand(symbolEntrySize);
  symScn->mHdr.sh_size += symbolEntrySize;

  // For ARM, make sure the next section is 4-byte aligned
  InsertSpace(symScn->mHdr.sh_offset + oldSize, symbolEntrySize);

  // Store the new symbol
  LxElfWriter wi(symScn->mData, oldSize);
  wi.PutWord(sym.st_name);
  wi.PutAddr(sym.st_value);
  wi.PutWord(sym.st_size);
  wi.PutByte(sym.st_info);
  wi.PutByte(sym.st_other);
  wi.PutHalf(sym.st_shndx);
}


Elf32_Word LxElfFile::
AlignUp(Elf32_Word value)
{
  while (value % 4 != 0)
    value++;

  return value;
}

Elf32_Word LxElfFile::
AlignDown(Elf32_Word value)
{
  while (value % 4 != 0)
    value--;

  return value;
}


ElfHeader LxElfFile::
GetElfHeader() const
{
  return mElfHdr;
}


bool LxElfFile::
IsBigEndian() const
{
  return mElfBigEndian;
}

bool LxElfFile::
HasSymbolTable() const
{
  return mHasSymbolTable;
}



Elf32_Word LxElfFile::
GetNrOfSegments() const
{
  return static_cast<Elf32_Word>(mSegments.size());
}


LxElfSegment const * LxElfFile::
GetSegment(Elf32_Word idx) const
{
  return mSegments[idx];
}


LxElfSegment * LxElfFile::
GetSegment(Elf32_Word idx)
{
  return mSegments[idx];
}


namespace
{
  bool SegAddrLess(LxElfSegment const * x, LxElfSegment const * y)
  {
    return x->mHdr.p_vaddr < y->mHdr.p_vaddr;
  }
}

LxElfConstSegments LxElfFile::
GetLoadSegments() const
{
  LxElfConstSegments segments;

  // Copy all load segments
  remove_copy_if(mSegments.begin(),
                 mSegments.end(),
                 back_inserter(segments),
                 not1(mem_fun(&LxElfSegment::IsLoad)));

  // Sort by address
  std::sort(segments.begin(), segments.end(), SegAddrLess);

  return segments;
}


Elf32_Word LxElfFile::
GetNrOfSections() const
{
  return static_cast<Elf32_Word>(mScns.size());
}


Elf32_Half LxElfFile::
GetSectionLabelsIdx() const
{
  return mElfHdr.e_shstrndx;
}

LxElfSection const * LxElfFile::
GetSection(Elf32_Word idx) const
{
  return mScns[idx];
}

LxElfSection* LxElfFile::
GetSection(Elf32_Word idx)
{
  return mScns[idx];
}


LxElfSection* LxElfFile::
GetSymbolSection() const
{
  if (!HasSymbolTable())
    throw LxFileException(GetFileName(), LxFileException::kSymTableNotFound);

  return mScns[mSymTabHdrIdx];
}

Elf32_Half LxElfFile::
GetSymTabHdrIdx() const
{
  return mSymTabHdrIdx;
}


// Loading
void LxElfFile::
Load(std::string const & filename)
{
  // Open the source elf file
  ifstream inFile(filename.c_str(), ios::in|ios::binary|ios::ate);
  if (!inFile)
    throw LxFileException(filename, LxFileException::kFileOpenError);

  mFileName = filename;
  Load(inFile);
}


void LxElfFile::
Load(istream & inFile)
{
  // Load the elf header
  LoadElfHeader(inFile);

  // Only 32 bit executable elf files are allowed
  if (mElfHdr.e_type != ET_EXEC)
    throw LxFileException(mFileName, LxFileException::kWrongElfType);

  if (mElfHdr.e_ident[EI_CLASS] != ELFCLASS32)
    throw LxFileException(mFileName, LxFileException::kWrongElfClass);

  // Read program headers
  LoadPgHeaders(mElfHdr.e_phnum,
                mElfHdr.e_phoff,
                mElfHdr.e_phentsize,
                inFile);

  // Read section headers
  LoadSectionHeaders(mElfHdr.e_shnum,
                     mElfHdr.e_shoff,
                     mElfHdr.e_shentsize,
                     inFile);

  // Load contents of segments and sections
  LoadContents(inFile);
}

void LxElfFile::
LoadElfHeader(istream & inFile)
{
  const unsigned long kElfHdrSize = 52;

  LxElfDataBuffer elfHdrBuf;
  if (LxLoad(elfHdrBuf, inFile, 0, kElfHdrSize))
  {
    switch (*(elfHdrBuf.begin() + EI_DATA))
    {
    case ELFDATA2LSB:
      mElfBigEndian = false;
      break;

    case ELFDATA2MSB:
      mElfBigEndian = true;
      break;

    default:
      throw LxFileException(mFileName, LxFileException::kParseError);
    }

    elfHdrBuf.SetEndian(IsBigEndian());

    // Parse the elf header
    LxElfReader ri(elfHdrBuf);
    for (int i = 0; i < EI_NIDENT; ++i)
    {
      mElfHdr.e_ident[i] = ri.GetByte();
    }
    mElfHdr.e_type      = ri.GetHalf();
    mElfHdr.e_machine   = ri.GetHalf();
    mElfHdr.e_version   = ri.GetWord();
    mElfHdr.e_entry     = ri.GetAddr();
    mElfHdr.e_phoff     = ri.GetOff();
    mElfHdr.e_shoff     = ri.GetOff();
    mElfHdr.e_flags     = ri.GetWord();
    mElfHdr.e_ehsize    = ri.GetHalf();
    mElfHdr.e_phentsize = ri.GetHalf();
    mElfHdr.e_phnum     = ri.GetHalf();
    mElfHdr.e_shentsize = ri.GetHalf();
    mElfHdr.e_shnum     = ri.GetHalf();
    mElfHdr.e_shstrndx  = ri.GetHalf();
  }
}


void LxElfFile::
LoadPgHeaders(Elf32_Half phnum,
              Elf32_Off  phoff,
              Elf32_Half phentsize,
              istream &  inFile)
{
  if (phnum > 0)
  {
    LxElfDataBuffer elfPgHdrsBuf(IsBigEndian());
    if (!LxLoad(elfPgHdrsBuf, inFile, phoff, phentsize * phnum))
      throw LxFileException(mFileName, LxFileException::kFileReadError);

    // Parse all program headers and store them in the mPgHdrs vector
    LxElfReader ri(elfPgHdrsBuf);
    for (Elf32_Half i = 0; i < phnum; ++i)
    {
      Elf32_Phdr pgHdr;
      pgHdr.p_type    = ri.GetWord();
      pgHdr.p_offset  = ri.GetWord();
      pgHdr.p_vaddr   = ri.GetWord();
      pgHdr.p_paddr   = ri.GetWord();
      pgHdr.p_filesz  = ri.GetWord();
      pgHdr.p_memsz   = ri.GetWord();
      pgHdr.p_flags   = ri.GetWord();
      pgHdr.p_align   = ri.GetWord();

      mSegments.push_back(new LxElfSegment(pgHdr, IsBigEndian()));
    }
  }
}

bool LxElfFile::
IsARM() const
{
  return (mElfHdr.e_machine == EM_ARM);
}

void LxElfFile::
LoadSectionHeaders(Elf32_Half shnum,
                   Elf32_Off  shoff,
                   Elf32_Half shentsize,
                   istream &  inFile)
{
  if (shnum > 0)
  {
    LxElfDataBuffer elfScnHdrsBuf(IsBigEndian());
    if (!LxLoad(elfScnHdrsBuf, inFile, shoff, shentsize * shnum))
      throw LxFileException(mFileName, LxFileException::kFileReadError);

    // Parse all section headers and store them in the mScnHdrs vector
    LxElfReader ri(elfScnHdrsBuf);
    for (Elf32_Half i = 0; i < shnum; ++i)
    {
      Elf32_Shdr shdr;
      shdr.sh_name      = ri.GetWord();
      shdr.sh_type      = ri.GetWord();
      shdr.sh_flags     = ri.GetWord();
      shdr.sh_addr      = ri.GetWord();
      shdr.sh_offset    = ri.GetOff();
      shdr.sh_size      = ri.GetWord();
      shdr.sh_link      = ri.GetWord();
      shdr.sh_info      = ri.GetWord();
      shdr.sh_addralign = ri.GetWord();
      shdr.sh_entsize   = ri.GetWord();

      // Check if this is the symbol table section header
      if (SHT_SYMTAB == shdr.sh_type)
      {
        mSymTabHdrIdx = i;
        mHasSymbolTable = true;
      }

      // Put the section data into the mScns vector
      mScns.push_back(new LxElfSection(shdr, IsBigEndian()));
    }
  }
}

namespace
{
#ifdef __IAR_SYSTEMS_ICC__
#pragma diag_suppress=Pe177 // decl but not reffed
#endif

  struct SegOffsetLess
  {
    bool operator () (LxElfSegment const * a, LxElfSegment const * b) const
    {
      return a->mHdr.p_offset < b->mHdr.p_offset;
    }

    bool operator () (LxElfSegment const * a, Elf32_Off b) const
    {
      return a->mHdr.p_offset < b;
    }

    bool operator () (Elf32_Off a, LxElfSegment const * b) const
    {
      return a < b->mHdr.p_offset;
    }
  };

#ifdef __IAR_SYSTEMS_ICC__
#pragma diag_default=Pe177
#endif

  // Find segment containing offset. Return NULL if not in a segment.
  LxElfSegment *
  Find(LxElfFile::Segments const & segs, Elf32_Off offset)
  {
    LxElfFile::Segments::const_iterator f =
           std::lower_bound(segs.begin(), segs.end(), offset, SegOffsetLess());
    if (f != segs.begin() && (f == segs.end() || (*f)->mHdr.p_offset > offset))
      --f;
    while (f != segs.end() && (*f)->mHdr.p_filesz == 0)
    {
      ++f;
    }
    if (   f != segs.end()
        && (*f)->mHdr.p_offset <= offset
        && (*f)->mHdr.p_offset + (*f)->mHdr.p_filesz > offset)
    {
      return *f;
    }
    return NULL;
  }
}

void LxElfFile::
LoadContents(istream & inFile)
{
  // Segments first.
  for (Elf32_Word i = 0; i < GetNrOfSegments(); ++i)
  {
    LxElfSegment * seg = GetSegment(i);

    if (seg->mHdr.p_filesz != 0)
      LxLoad(seg->mData, inFile, seg->mHdr.p_offset, seg->mHdr.p_filesz);
  }
  Segments segs(mSegments);
  stable_sort(segs.begin(), segs.end(), SegOffsetLess());

  for (Elf32_Word i = 0; i < GetNrOfSections(); ++i)
  {
    LxElfSection* scn = GetSection(i);

    if (!scn->IsNull() && !scn->IsNoBits())
    {
      // Check if section contents are part of segment contents.
      LxElfSegment * seg = Find(segs, scn->mHdr.sh_offset);
      if (seg != NULL)
      {
        bool ok = seg->mHdr.p_offset + seg->mHdr.p_filesz
                                    >= scn->mHdr.sh_offset + scn->mHdr.sh_size;
        // Work-around for bug in Ilink
        if (   !ok
            && seg->mHdr.p_offset == scn->mHdr.sh_offset
            && seg->mHdr.p_memsz  == scn->mHdr.sh_size)
         ok = true;
        if (ok)
        {
          scn->mData.SetWindow(seg->mData,
                               scn->mHdr.sh_offset - seg->mHdr.p_offset,
                               scn->mHdr.sh_size);
        }
        else
          throw LxFileException(mFileName, LxFileException::kParseError);
      }
      else
        LxLoad(scn->mData, inFile, scn->mHdr.sh_offset, scn->mHdr.sh_size);
    }
  }
}


namespace
{
  bool IsGapBetween(const LxAddressRange & a, const LxAddressRange & b)
  {
    return a.GetEnd() + 1 != b.GetStart();
  }

  bool IsContinuous(const LxAddressSet & ranges)
  {
    return adjacent_find(ranges.begin(),
                         ranges.end(),
                         IsGapBetween) == ranges.end();
  }

  LxAddressRange GetTotalRange(const LxAddressSet & ranges)
  {
    if (ranges.empty())
      return LxAddressRange(0, 0);
    else
    {
      return LxAddressRange(ranges.begin()->GetStart(),
                            (--ranges.end())->GetEnd());
    }
  }

  struct Visitor
  {
    Visitor(LxByteVisitor & v, LxAddressRange const & r, bool rSign = false)
      : mVisitor(v), mRange(r), mRSIGN(rSign)
    {
    }

    virtual ~Visitor() {}

  protected:
    void VisitBytes(const LxAddressRange & blockRange,
                    const LxElfDataBuffer & data)
    {
      // currentRange is the range that we shall visit for this section/segment
      LxAddressRange currentRange = mRange.Intersection(blockRange);

      // Make sure that is it entirely inside the section
      if (!blockRange.Contains(currentRange))
      {
        // TODO: This should not be a checksum specific error
        throw LxChecksumException(LxChecksumException::kChecksumRangeError);
      }

      Elf32_Word len = currentRange.GetLength();
      const uint8_t* p = data.begin() +
                         (currentRange.GetStart() - blockRange.GetStart());

      // Tell the visitor which addresses it will be visiting, and in which direction
      mVisitor.SetVisitRange(currentRange, mRSIGN);

      if (mRSIGN)
      {
          /* Scan the bytes in reverse order */
          p += len; // move pointer to (just beyond) end of range

          while (len-- != 0)
          {
            mVisitor.VisitByte(*--p);
          }

          mRange.SetEnd(currentRange.GetStart() - 1);
      }
      else
      {
          while (len-- != 0)
          {
            mVisitor.VisitByte(*p++);
          }

          mRange.SetStart(currentRange.GetEnd() + 1);
      }
    }

  private:
    LxByteVisitor & mVisitor;
    LxAddressRange mRange;
    const bool mRSIGN;
  };

  struct SectionVisitor : public Visitor
  {
    SectionVisitor(LxByteVisitor & v, LxAddressRange const & r)
      : Visitor(v, r)
    {
    }

    void operator () (const LxElfSection* section)
    {
      VisitBytes(section->GetRange(), section->mData);
    }
  };

  struct SegmentVisitor : public Visitor
  {
    SegmentVisitor(LxByteVisitor & v, LxAddressRange const & r, bool rSign, unsigned forceAlign)
      : Visitor(v, r, rSign), m_forceAlign(forceAlign)
    {
    }

    void operator () (const LxElfSegment* segment)
    {
	  LxAddressRange originalRange = segment->GetRange();

	  if (1u == m_forceAlign)
	  {
		  VisitBytes(segment->GetRange(), segment->mData);
	  }
	  else
	  {
		  Elf32_Addr start = originalRange.GetStart();
		  Elf32_Addr end   = originalRange.GetEnd();

		  start -= start % m_forceAlign;
		  end   -= (end + 1) % m_forceAlign;

		  LxAddressRange alignedRange (start, end);
		  VisitBytes(alignedRange, segment->mData);
	  }
    }

  private:
	  unsigned m_forceAlign;
  };


  struct SectionRangeVisitor
  {
    SectionRangeVisitor(LxByteVisitor & v, const LxElfFile & file)
      : mVisitor(v), mFile(file)
    {
    }

    void operator () (const LxAddressRange & range)
    {
      // Get all sections that span over this range
      LxElfConstSections scns;
      mFile.GetSectionsInAddrRange(range, scns);

      // Extract their ranges
      LxAddressSet sectionRanges;
      transform(scns.begin(),
                scns.end(),
                inserter(sectionRanges, sectionRanges.begin()),
                mem_fun(&LxElfSection::GetRange));

      // Check that the list of sections covers the range and that there are no
      // holes between the sections
      if (!IsContinuous(sectionRanges) ||
          !GetTotalRange(sectionRanges).Contains(range))
      {
        // TODO: This should not be a checksum specific error
        throw LxChecksumException(LxChecksumException::kChecksumRangeError);
      }

      SectionVisitor visitSections(mVisitor, range);
      for_each(scns.begin(), scns.end(), visitSections);
    }

  private:
    LxByteVisitor & mVisitor;
    const LxElfFile & mFile;
  };


struct SegmentRangeVisitor
  {
    SegmentRangeVisitor(LxByteVisitor & v, const LxElfFile & file, bool rSign, unsigned forceAlign)
      : mVisitor(v), mFile(file), mRSIGN(rSign), m_forceAlign(forceAlign)
    {
    }

    void operator () (const LxAddressRange & range)
    {
      // Get all segments that span over this range
      LxElfConstSegments segments;
      mFile.GetSegmentsInAddrRange(range, segments);

      // Extract their ranges
      LxAddressSet segmentRanges;
      transform(segments.begin(),
                segments.end(),
                inserter(segmentRanges, segmentRanges.begin()),
                mem_fun(&LxElfSegment::GetRange));

      // Check that the list of segments covers the range and that there are no
      // holes between the segments
      if (!IsContinuous(segmentRanges) ||
          !GetTotalRange(segmentRanges).Contains(range))
      {
        bool virtualFillCoversGap = false;
        if (mFile.AnyVirtualFill())
        {
          // collect the fake segments containing the addresses of the virtual fill
          LxElfConstSegments virtualSegments;
          for (LxElfFile::VirtualFillIter p = mFile.Begin(), q = mFile.End();
               p != q;
               ++p)
          {
            LxElfSegment const * seg = p->Segment();
            LxAddressRange addrRng   = seg->GetRange();
            // only care about the virtual segment if it intersects the current range
            if (range.Intersects(addrRng))
            {
              LxElfVirtualSegment * iSeg = (LxElfVirtualSegment *)seg;
              iSeg->mRange = range.Intersection(addrRng);
              segments.push_back(seg);
              virtualSegments.push_back(seg);
            }
          }
          // sort the complete collection of segments
          std::sort(segments.begin(), segments.end(), LxElfVirtualSegmentSortByVAddr);

          // generate segment ranges to check that the total fill spans the fill range
          transform(virtualSegments.begin(),
                    virtualSegments.end(),
                    inserter(segmentRanges, segmentRanges.begin()),
                    mem_fun(&LxElfSegment::GetRange));

          virtualFillCoversGap = IsContinuous(segmentRanges) 
            && GetTotalRange(segmentRanges).Contains(range);
        }
        
        // throw unless the gaps are covered by virtual fill
        if (!virtualFillCoversGap)
        {
          // TODO: This should not be a checksum specific error
          throw LxChecksumException(LxChecksumException::kChecksumRangeError);
        }
      }

      SegmentVisitor visitSegments(mVisitor, range, mRSIGN, m_forceAlign);

      if (mRSIGN)
      {
          // Reverse the order of the segments
          std::reverse(segments.begin(), segments.end());
      }

      for_each(segments.begin(), segments.end(), visitSegments);
    }

  private:
	unsigned m_forceAlign;
    LxByteVisitor & mVisitor;
    const LxElfFile & mFile;
    const bool mRSIGN;
  };
}

void LxElfFile::
VisitSectionRanges(LxByteVisitor & v,
                   LxAddressRanges const & ranges)
{
  // TODO: Remove overlaps between ranges
  v.VisitBegin();
  for_each(ranges.begin(), ranges.end(), SectionRangeVisitor(v, *this));
  v.VisitEnd();
}

void LxElfFile::
VisitSegmentRanges(LxByteVisitor & v,
                   LxAddressRanges const & ranges,
                   bool rSign,
				   unsigned forceAlign)
{
    // Copy the ranges, so that their order can be reversed if required
    LxAddressRanges useRanges = ranges;

    if (rSign)
    {
          // Reverse the order of the ranges
          std::reverse(useRanges.begin(),useRanges.end());
    }

  // TODO: Remove overlaps between ranges
  v.VisitBegin();
  for_each(useRanges.begin(), useRanges.end(), SegmentRangeVisitor(v, *this, rSign, forceAlign));
  v.VisitEnd();
}


string LxElfFile::
GetFileName() const
{
  return mFileName;
}

void LxElfFile::
RegisterObserver(LxElfFileObserver* o)
{
  if (find(mObservers.begin(), mObservers.end(), o) == mObservers.end())
    mObservers.push_back(o);
}

void LxElfFile::
UnRegisterObserver(LxElfFileObserver* o)
{
  mObservers.erase(remove(mObservers.begin(),
                          mObservers.end(),
                          o),
                   mObservers.end());
}

void LxElfFile::
DescribeSectionWithRange(LxElfSection const * sec, std::ostream & o) const
{
  o << "section ";
  // Try to find out the section index.
  Elf32_Word index = 0;
  for (Elf32_Word i = 0, n = GetNrOfSections(); i !=n; ++i)
  {
    if (GetSection(i) == sec)
    {
      index = i;
      break;
    }
  }
  if (index != 0)
  {
    o << "#" << index << " ";
  }
  if (sec->mHdr.sh_name)
  {
    o << '"' << GetString(sec->mHdr.sh_name, GetSectionLabelsIdx()) << '"';
  }
  o << std::hex << " [0x" << sec->mHdr.sh_addr;
  if (sec->mHdr.sh_size)
  {
    o << "-0x" << sec->mHdr.sh_addr + sec->mHdr.sh_size - 1;
  }
  o << std::dec << "]";
}

namespace
{
  struct RemoveNotifier
  {
    RemoveNotifier(Elf32_Off off, Elf32_Word size)
      : mOff(off), mSize(size) {}

    void operator () (LxElfFileObserver* o) const
    {
      o->BytesRemoved(mOff, mSize);
    }

    Elf32_Off  mOff;
    Elf32_Word mSize;
  };

  struct InsertNotifier
  {
    InsertNotifier(Elf32_Off off, Elf32_Word size)
      : mOff(off), mSize(size) {}

    void operator () (LxElfFileObserver* o) const
    {
      o->BytesInserted(mOff, mSize);
    }

    Elf32_Off  mOff;
    Elf32_Word mSize;
  };

}
void LxElfFile::
NotifySizeChange(Elf32_Off off, Elf32_Word size, bool larger)
{
  if (larger)
    for_each(mObservers.begin(), mObservers.end(), InsertNotifier(off, size));
  else
    for_each(mObservers.begin(), mObservers.end(), RemoveNotifier(off, size));
}

void LxElfFile::
AddVirtualFill(LxElfVirtualFill const & fill, bool verbose)
{
  mVirtualFills.push_back(fill);
  if (verbose)
  {
    std::cout << "Adding virtual fill over range "
              << hex << fill.Range().GetStart() << "-" << fill.Range().GetEnd() << endl;
  }
}

bool LxElfFile::
AnyVirtualFill(void) const
{
  return mVirtualFills.size() > 0;
}

LxElfFile::VirtualFillIter LxElfFile::
Begin(void) const
{
  return mVirtualFills.begin();
}

LxElfFile::VirtualFillIter LxElfFile::
End(void) const
{
  return mVirtualFills.end();
}

LxElfVirtualFill::LxElfVirtualFill(LxAddressRange const & range, FillPattern const & fill)
  : mRange(range), mFill(&fill)
{
  Elf32_Phdr hdr;
  Elf32_Addr start = range.GetStart();
  Elf32_Addr size  = (range.GetEnd() - start) + 1;
  hdr.p_type   = 0;
  hdr.p_offset = 0;
  hdr.p_vaddr  = start;
  hdr.p_paddr  = start;
  hdr.p_filesz = size;
  hdr.p_memsz  = size;
  hdr.p_flags  = 0;
  hdr.p_align  = 0;

  // don't care about endianess (use little), the segment is virtual
  mSegment = new LxElfVirtualSegment(hdr, range, fill);

}

LxElfVirtualFill::~LxElfVirtualFill()
{
  //if (mSegment)
  //delete mSegment;
}

LxElfVirtualFill::LxElfVirtualFill(LxElfVirtualFill const & src)
  : mRange(src.mRange), mFill(src.mFill), mSegment(src.mSegment)
{
  
}

LxElfVirtualFill const & 
LxElfVirtualFill::operator=(LxElfVirtualFill const & src)
{
  if (this != &src)
  {
    mRange   = src.mRange;
    mFill    = src.mFill;
    if (mSegment)
      delete mSegment;
    mSegment = src.mSegment;
  }
  return *this;
}
