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

#include "LxElfStripCmd.h"
#include "LxElfException.h"
#include "LxElfFile.h"

#include <iostream>
#include <fstream>

using namespace std;


LxElfStripCmd::
LxElfStripCmd()
{
}


void LxElfStripCmd::
Execute(LxElfFile & elfFile, bool verbose)
{
  if (verbose)
    cout << "Stripping elf file" << endl;

  for (unsigned int i = 0; i < elfFile.GetNrOfSections(); ++i)
  {
    if (ShallSectionBeRemoved(i, elfFile.GetSection(i), elfFile))
    {
      if (!elfFile.RemoveSection(i))
        throw LxStripException();
      i--;
    }
  }
}


bool LxElfStripCmd::
ShallSectionBeRemoved(Elf32_Half        index,
                      LxElfSection*     section,
                      LxElfFile const & file) const
{
  if (!section)
    throw LxStripException();

  // Do not remove the section string table or the NULL section
  if (index == file.GetSectionLabelsIdx() || section->IsNull())
    return false;

  // Remove non-alloc sections
  return (section->IsSymbolTable() || !section->IsAlloc());
}
