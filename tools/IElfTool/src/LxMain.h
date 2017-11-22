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

// Types and functions related to command line parsing

#ifndef LXMAIN_H
#define LXMAIN_H

#include <string>
#include "LxElfTypes.h"

class LxElfFile;

class LxSymbolicAddress
{
public:
  explicit LxSymbolicAddress(Elf32_Addr start)
    : mOffset(start)
  {
  }

  LxSymbolicAddress(std::string const & label, Elf32_Addr offset)
    : mLabel(label), mOffset(offset)
  {
  }

  bool IsAbsolute() const { return mLabel.empty(); }

  std::string GetLabel() const { return mLabel; }
  Elf32_Addr GetOffset() const { return mOffset; }

  friend std::ostream & operator << (std::ostream & o,
                                     LxSymbolicAddress const & x);

private:
  std::string mLabel;
  Elf32_Addr  mOffset;
};

class LxSymbolicRange
{
public:
  LxSymbolicRange(LxSymbolicAddress const & start,
                  LxSymbolicAddress const & end)
    : mStart(start), mEnd(end)
  {
  }

  LxSymbolicAddress GetStart() const { return mStart; };
  LxSymbolicAddress GetEnd()   const { return mEnd; };

  friend std::ostream & operator << (std::ostream & o,
                                     LxSymbolicRange const & x);

private:
  LxSymbolicAddress mStart;
  LxSymbolicAddress mEnd;
};

typedef std::vector<LxSymbolicRange> LxSymbolicRanges;

Elf32_Addr      LxGetAddress      (LxSymbolicAddress const & addr,
                                   LxElfFile const & ef);
LxAddressRange  LxGetAddressRange (LxSymbolicRange const & range,
                                   LxElfFile const & ef);
LxAddressRanges LxGetAddressRanges(LxSymbolicRanges const & ranges,
                                   LxElfFile const & ef);

// Returns true if ranges overlap
bool LxRangesOverlap(const LxAddressRanges & ranges);

#endif // LXMAIN_H
