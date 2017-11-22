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

// Elf constants and structures

#ifndef ELFTYPES_H
#define ELFTYPES_H

#include <functional>
#include <utility>
#include <set>
#include <vector>
#include <string>

#ifdef __GNUG__
#include <stdint.h>
#else // __GNUG__
typedef signed char        int8_t;
typedef unsigned char      uint8_t;
typedef signed short       int16_t;
typedef unsigned short     uint16_t;
typedef signed int         int32_t;
typedef unsigned int       uint32_t;
typedef signed long long   int64_t;
typedef unsigned long long uint64_t;
#endif // __GNUG__

typedef uint64_t Elf64_Word;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Word;
typedef uint32_t Elf32_Sword;
typedef uint16_t Elf32_Half;
typedef uint8_t  Elf32_UC;



// ELF HEADER CONSTANTS
#define ELF_HEADER_SIZE 52
#define ELF_PROGRAM_HEADER_SIZE 32
#define ELF_SECTION_HEADER_SIZE 40

#define ET_NONE   0       // No file type
#define ET_REL    1       // Relocatable file
#define ET_EXEC   2       // Executable file
#define ET_DYN    3       // Shared object file
#define ET_CORE   4       // Core file
#define ET_LOOS   0xfe00  // Operating system-specific
#define ET_HIOS   0xfeff  // Operating system-specific
#define ET_LOPROC 0xff00  // Processor-specific
#define ET_HIPROC 0xffff  // Processor-specific


#define EI_MAG0         0  // File identification
#define EI_MAG1         1  // File identification
#define EI_MAG2         2  // File identification
#define EI_MAG3         3  // File identification
#define EI_CLASS        4  // File class
#define EI_DATA         5  // Data encoding
#define EI_VERSION      6  // File version
#define EI_OSABI        7  // Operating system/ABI identification
#define EI_ABIVERSION   8  // ABI version
#define EI_PAD          9  // Start of padding bytes
#define EI_NIDENT      16  // Size of e_ident[]

#define ELFMAG0         0x7f // e_ident[EI_MAG0]
#define ELFMAG1         'E'  // e_ident[EI_MAG1]
#define ELFMAG2         'L'  // e_ident[EI_MAG2]
#define ELFMAG3         'F'  // e_ident[EI_MAG3]

#define ELFCLASSNONE    0  // Invalid class
#define ELFCLASS32      1  // 32-bit objects
#define ELFCLASS64      2  // 64-bit objects

#define ELFDATANONE     0  // Invalid data encoding
#define ELFDATA2LSB     1  // 2's complement little-endian
#define ELFDATA2MSB     2  // 2's complement big endian

#define EV_NONE         0 // Invalid version
#define EV_CURRENT      1 // Current version

#define EM_NONE         0    // No machine
#define EM_M32          1    // AT&T WE 32100
#define EM_SPARC        2    // SPARC
#define EM_386          3    // Intel 80386
#define EM_68K          4    // Motorola 68000
#define EM_88K          5    // Motorola 88000
//#define RESERVED      6    // Reserved for future use
#define EM_860          7    // Intel 80860
#define EM_MIPS         8    // MIPS I Architecture
//#define RESERVED      9    // Reserved for future use
#define EM_MIPS_RS3_LE 10   // MIPS RS3000 Little-endian
//#define RESERVED     11-14  // Reserved for future use
#define EM_PARISC      15   // Hewlett-Packard PA-RISC
//#define RESERVED     16   // Reserved for future use
#define EM_VPP500      17   // Fujitsu VPP500
#define EM_SPARC32PLUS 18   // Enhanced instruction set SPARC
#define EM_960         19   // Intel 80960
#define EM_PPC         20   // Power PC
//#define RESERVED     21-35  // Reserved for future use
#define EM_V800        36   // NEC V800
#define EM_FR20        37   // Fujitsu FR20
#define EM_RH32        38   // TRW RH-32
#define EM_RCE         39   // Motorola RCE
#define EM_ARM         40   // Advanced RISC Machines ARM
#define EM_ALPHA       41   // Digital Alpha
#define EM_SH          42   // Hitachi SH
#define EM_SPARCV9     43   // SPARC Version 9
#define EM_TRICORE     44   // Siemens Tricore embedded processor
#define EM_ARC         45   // Argonaut RISC Core, Argonaut Technologies Inc.
#define EM_H8_300      46   // Hitachi H8/300
#define EM_H8_300H     47   // Hitachi H8/300H
#define EM_H8S         48   // Hitachi H8S
#define EM_H8_500      49   // Hitachi H8/500
#define EM_IA_64       50   // Intel MercedTM Processor
#define EM_MIPS_X      51   // Stanford MIPS-X
#define EM_COLDFIRE    52   // Motorola Coldfire
#define EM_68HC12      53   // Motorola M68HC12




// SECTION HEADER CONSTANTS
// Values for sh_type
#define SHT_NULL     0
#define SHT_PROGBITS 1
#define SHT_SYMTAB   2
#define SHT_STRTAB   3
#define SHT_RELA     4
#define SHT_HASH     5
#define SHT_DYNAMIC  6
#define SHT_NOTE     7
#define SHT_NOBITS   8
#define SHT_REL      9
#define SHT_SHLIB    10
#define SHT_DYNSYM   11
#define SHT_LOOS     0x60000000
#define SHT_HIOS     0x6fffffff
#define SHT_LOPROC   0x70000000
#define SHT_HIPROC   0x7fffffff
#define SHT_LOUSER   0x80000000
#define SHT_HIUSER   0xffffffff

// Values for sh_flags
#define SHF_WRITE     0x1
#define SHF_ALLOC     0x2
#define SHF_EXECINSTR 0x4
#define SHF_MASKOS    0x0f000000
#define SHF_MASKPROC  0xf0000000

// Special Section Indexes
#define SHN_UNDEF     0
#define SHN_LORESERVE 0xff00
#define SHN_LOPROC    0xff00
#define SHN_HIPROC    0xff1f
#define SHN_LOOS      0xff20
#define SHN_HIOS      0xff3f
#define SHN_ABS       0xfff1
#define SHN_COMMON    0xfff2
#define SHN_HIRESERVE 0xffff


// SYMBOL HEADER CONSTANTS
#define STB_LOCAL    0
#define STB_GLOBAL   1
#define STB_WEAK     2
#define STB_LOOS    10
#define STB_HIOS    12
#define STB_LOPROC  13
#define STB_HIPROC  15

#define STT_NOTYPE   0
#define STT_OBJECT   1
#define STT_FUNC     2
#define STT_SECTION  3
#define STT_FILE     4
#define STT_LOOS    10
#define STT_HIOS    12
#define STT_LOPROC  13
#define STT_HIPROC  15

// SEGMENT TYPES
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
#define PT_LOOS    0x60000000
#define PT_HIOS    0x6fffffff
#define PT_LOPROC  0x70000000
#define PT_HIPROC  0x7fffffff

// SEGMENT FLAG BITS
#define PF_X 0x1 // Execute
#define PF_W 0x2 // Write
#define PF_R 0x4 // Read
#define PF_MASKOS   0x0ff00000 // Unspecified
#define PF_MASKPROC 0xf0000000 // Unspecified

#define ST_BIND(i)   ((i)>>4)
#define ST_TYPE(i)   ((i)&0xf)
#define ST_INFO(b,t) (((b)<<4)+((t)&0xf))

#define ELF_REL_ENTRY_SIZE 8

#define R_SYM(i)    ((i)>>8)
#define R_TYPE(i)   ((i)&0xff)
#define R_INFO(s,t) (((s)<<8)+((t)&0xff))

/* Dynamic segment stuff */
#define DT_NULL                    0
#define DT_NEEDED                  1
#define DT_PLTRELSZ                2
#define DT_PLTGOT                  3
#define DT_HASH                    4
#define DT_STRTAB                  5
#define DT_SYMTAB                  6
#define DT_RELA                    7
#define DT_RELASZ                  8
#define DT_RELAENT                 9
#define DT_STRSZ                  10
#define DT_SYMENT                 11
#define DT_INIT                   12
#define DT_FINI                   13
#define DT_SONAME                 14
#define DT_RPATH                  15
#define DT_SYMBOLIC               16
#define DT_REL                    17
#define DT_RELSZ                  18
#define DT_RELENT                 19
#define DT_PLTREL                 20
#define DT_DEBUG                  21
#define DT_TEXTREL                22
#define DT_JMPREL                 23
#define DT_BIND_NOW               24
#define DT_LOPROC         0x70000000
#define DT_ARM_SYMTABSZ  DT_LOPROC+1

class ElfHeader
{
public:
  Elf32_UC        e_ident[EI_NIDENT];
  Elf32_Half      e_type;
  Elf32_Half      e_machine;
  Elf32_Word      e_version;
  Elf32_Addr      e_entry;
  Elf32_Off       e_phoff;
  Elf32_Off       e_shoff;
  Elf32_Word      e_flags;
  Elf32_Half      e_ehsize;
  Elf32_Half      e_phentsize;
  Elf32_Half      e_phnum;
  Elf32_Half      e_shentsize;
  Elf32_Half      e_shnum;
  Elf32_Half      e_shstrndx;
};


class Elf32_Phdr
{
public:
  Elf32_Word   p_type;
  Elf32_Off    p_offset;
  Elf32_Addr   p_vaddr;
  Elf32_Addr   p_paddr;
  Elf32_Word   p_filesz;
  Elf32_Word   p_memsz;
  Elf32_Word   p_flags;
  Elf32_Word   p_align;
};

class Elf32_Shdr
{
public:
  Elf32_Word      sh_name;
  Elf32_Word      sh_type;
  Elf32_Word      sh_flags;
  Elf32_Addr      sh_addr;
  Elf32_Off       sh_offset;
  Elf32_Word      sh_size;
  Elf32_Word      sh_link;
  Elf32_Word      sh_info;
  Elf32_Word      sh_addralign;
  Elf32_Word      sh_entsize;
};

class Elf32_Sym
{
public:
  Elf32_Word      st_name;
  Elf32_Addr      st_value;
  Elf32_Word      st_size;
  Elf32_UC        st_info;
  Elf32_UC        st_other;
  Elf32_Half      st_shndx;
};

enum LxAlgo
{
  kCrcSimple=1,
  kCrc16,
  kCrc32,
  kCrc64iso,
  kCrc64ecma,
  kCrcPoly,
  kCrcSimple32,
  kCrcSimpleWide
};

enum LxCompl
{
  kNoCompl=0,
  k1sCompl,
  k2sCompl
};

enum SRecType {kAdaptive = -1,
               kS0 = 0, kS1 = 1, kS2 = 2, kS3 = 3, kS7 = 7, kS8 = 8, kS9 = 9};

class SRecVariant
{
public:
  SRecVariant() : mStart(kAdaptive), mEnd(kAdaptive) {}
  SRecVariant(SRecType start, SRecType end) : mStart(start), mEnd(end) {}

  SRecType GetStartType() const {return mStart;}
  SRecType GetEndType()   const {return mEnd;}

  bool IsAdaptive() const {return mStart == kAdaptive;}

  bool operator == (const SRecVariant & other) const
  {
    return mStart == other.mStart && mEnd == other.mEnd;
  }

private:
  SRecType mStart;
  SRecType mEnd;
};

static const SRecVariant kAdaptiveVariant;
static const SRecVariant kS19(kS1, kS9);
static const SRecVariant kS28(kS2, kS8);
static const SRecVariant kS37(kS3, kS7);

enum StartValueType
{
  kNone, kInitial, kPrepended
};

template <class T>
class LxRange
{
public:
  LxRange(T start, T end) :
    mStart(start), mEnd(end)
  {
  }

  LxRange(const LxRange<T> & r) :
    mStart(r.mStart), mEnd(r.mEnd)
  {
  }

  bool operator == (const LxRange<T> & r) const
  {
    return (r.mStart == mStart && r.mEnd == mEnd);
  }

  LxRange & operator += (T offset)
  {
    mStart += offset;
    mEnd   += offset;
    return *this;
  }

  LxRange & operator -= (T offset)
  {
    mStart -= offset;
    mEnd   -= offset;
    return *this;
  }

  const LxRange<T> operator - (T offset)
  {
    LxRange<T> r(GetStart() - offset, GetEnd() - offset);
    return r;
  }

  T GetStart() const
  {
    return mStart;
  }

  T GetEnd() const
  {
    return mEnd;
  }

  T GetLength() const
  {
    return mEnd - mStart + 1;
  }

  void SetStart(T addr)
  {
    mStart = addr;
  }

  void SetEnd(T addr)
  {
    mEnd = addr;
  }

  bool ContainsAddress(T addr) const
  {
    return (addr >= mStart && addr <= mEnd);
  }

  bool Contains(const LxRange<T> & r) const
  {
    return (ContainsAddress(r.mStart) &&
            ContainsAddress(r.mEnd));
  }

  bool Intersects(const LxRange<T> & r) const
  {
    return (r.ContainsAddress(mStart) ||
            r.ContainsAddress(mEnd) ||
            ContainsAddress(r.mStart) ||
            ContainsAddress(r.mEnd));
  }

  LxRange<T> Intersection(const LxRange<T> & r) const
  {
    LxRange<T> un(*this);

    if (r.GetStart() > GetEnd() || r.GetEnd() < GetStart())
    {
      // The two ranges do not intersect. Return invalid range.
      un.SetStart(un.GetStart() - 1);
      return un;
    }

    if (r.GetStart() > GetStart())
      un.SetStart(r.GetStart());

    if (r.GetEnd() < GetEnd())
      un.SetEnd(r.GetEnd());

    return un;
  }

  void AdjustStartToAfter(T addr)
  {
    if (StartsBefore(addr + 1))
      SetStart(addr + 1);
  }

  void AdjustEndToBefore(T addr)
  {
    if (EndsAfter(addr - 1))
      SetEnd(addr - 1);
  }

  bool IsReversed() const
  {
    return mStart > mEnd;
  }

  bool IsAligned(uint8_t align) const
  {
    return (mStart % align) == 0 && ((mEnd + 1) % align) == 0;
  }

  bool StartsBefore(T addr) const
  {
    return mStart < addr;
  }

  bool StartsAfter(T addr) const
  {
    return mStart > addr;
  }

  bool EndsBefore(T addr) const
  {
    return mEnd < addr;
  }

  bool EndsAfter(T addr) const
  {
    return mEnd > addr;
  }

private:
  T mStart;
  T mEnd;
};

typedef LxRange<Elf32_Addr> LxAddressRange;
typedef std::vector<LxAddressRange> LxAddressRanges;

typedef std::vector<char> FillPattern;


// Class used for sorting ranges by start address
struct LxRangeSort
  : public std::binary_function<LxAddressRange, LxAddressRange, bool> 
{
 bool operator () (const LxAddressRange & a,
                   const LxAddressRange & b) const
  {
    return (a.GetStart() < b.GetStart());
  }
};

typedef std::set<LxAddressRange, LxRangeSort> LxAddressSet;

static const char hexchars[] = "0123456789ABCDEF";

static
inline
char*
ToHex(unsigned char data, char* dest)
{
  dest[0] = hexchars[(data & 0xF0) >> 4];
  dest[1] = hexchars[(data & 0xF)];
  return dest;
}

#endif // ELFTYPES_H
