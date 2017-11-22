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

// Class that implements the command for stripping an elf file

#ifndef LX_ELF_STRIP_CMD
#define LX_ELF_STRIP_CMD

#include "LxElfCmd.h"


class LxElfStripCmd : public LxElfCmd
{
public:
  LxElfStripCmd();

  virtual void Execute(LxElfFile & elfFile, bool verbose);

private:
  bool ShallSectionBeRemoved(Elf32_Half        index,
                             LxElfSection*     section,
                             LxElfFile const & file) const;
};

#endif // LX_ELF_STRIP_CMD
