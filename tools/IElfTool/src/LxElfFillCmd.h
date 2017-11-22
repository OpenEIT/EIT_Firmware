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

// Class that implements the command for filling an elf file

#ifndef LX_ELF_FILL_CMD
#define LX_ELF_FILL_CMD

#include "LxElfCmd.h"
#include "LxElfTypes.h"
#include "LxMain.h"
#include <string>

#include <stdexcept>

class LxElfSection;


class LxElfFillCmd : public LxElfCmd
{
public:
  LxElfFillCmd(LxSymbolicRange      range,
               FillPattern const &  pattern,
               bool                 virtual_fill);

  virtual void Execute(LxElfFile & file, bool verbose);

private:
  void AddFillScn(LxAddressRange range, LxElfFile & file, bool verbose);
  std::string CreateFillSectionName(LxElfFile const & file) const;

  LxSymbolicRange mRange;
  FillPattern     mPattern;
  bool            mVirtual;
};

// Validates that there is no overlap between fill commands
class LxElfFillValidateCmd : public LxElfCmd
{
public:
  LxElfFillValidateCmd(LxSymbolicRanges const & fillRanges)
    : mRanges(fillRanges)
  {
  }

  virtual void Execute(LxElfFile & file, bool verbose)
  {
    if (LxRangesOverlap(LxGetAddressRanges(mRanges, file)))
      throw std::runtime_error("Fill ranges overlap!");
  }

private:
  LxSymbolicRanges mRanges;
};

#endif // LX_ELF_FILL_CMD
