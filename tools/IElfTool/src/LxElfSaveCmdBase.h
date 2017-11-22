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

// Base class for simple save commands.

#ifndef LX_ELF_SAVE_CMD_BASE
#define LX_ELF_SAVE_CMD_BASE

#include "LxElfCmd.h"
#include "LxElfTypes.h"

#include <string>
#include <fstream>

class LxElfSection;

class LxElfSaveCmdBase : public LxElfCmd
{
public:
  LxElfSaveCmdBase(std::string const & fileName,
                   std::string const & kind,
                   std::ios_base::openmode mode = std::ios_base::out);

  virtual void Execute(LxElfFile & file, bool verbose);

  // To be overridden by subclasses
  virtual void DumpHeader(LxElfFile const & file, std::ostream & o) { }
  virtual void DumpData  (Elf32_Addr addr,
                          LxElfDataBuffer const & data,
                          bool verbose,
                          std::ostream & o) = 0;
  virtual void DumpFooter(LxElfFile const & file, std::ostream & o) { }

  std::string GetFilename() const { return mFilename; }

private:
  void Save(LxElfFile const & file, bool verbose, std::ostream & o);

  std::string mFilename;
  std::string mKind;
  std::ios_base::openmode mMode;
};

#endif // LX_ELF_SAVE_CMD_BASE
