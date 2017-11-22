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

// Class that implements the command for saving S-records

#ifndef LX_ELF_SAVE_SREC_CMD
#define LX_ELF_SAVE_SREC_CMD

#include "LxElfSaveCmdBase.h"

class LxElfSaveSRecCmd : public LxElfSaveCmdBase
{
public:
  LxElfSaveSRecCmd(std::string const & fileName,
                   SRecVariant variant,
                   unsigned char len);

  virtual void DumpHeader(LxElfFile const & elfFile, std::ostream & o);
  virtual void DumpData  (Elf32_Addr addr,
                          LxElfDataBuffer const & data,
                          bool verbose,
                          std::ostream & o);
  virtual void DumpFooter(LxElfFile const & elfFile, std::ostream & o);

private:
  const SRecVariant & GetVariantToUse(Elf32_Addr addr) const;
  Elf32_Addr          GetNextVariantAddr(Elf32_Addr addr) const;
  Elf32_Addr          DumpRecord(Elf32_Addr currAddr,
                                 Elf32_Addr dataStartAddr,
                                 LxElfDataBuffer const & bytes,
                                 std::ostream & os);

  SRecVariant   mVariant;
  unsigned char mMaxRecordLength;
};

#endif // LX_ELF_SAVE_SREC_CMD
