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

//
// Class that implements the command for saving in TI TXT format.
//
// See:
//
// * http://www.ti.com/lit/pdf/slau101 -- Document containing format
//   description.
//

//
// Quote from "slau101q" (November 2011):
// -------------------------------------
//
// The TI-txt file format [...] is:
//
// @ADDR1
// DATA01 DATA02 ........ DATA16
// DATA17 DATA18 ........ DATA32
// ........
// DATAm ........ DATAn
// @ADDR2
// DATA01 ........ DATAn
// q
//
// Where:
//
// @ADDR is the start address of a section (hexadecimal).
// DATAn represents a data byte (hexadecimal).
// q is the termination of the file.
//
// For example:
//
// @F000
// 31 40 00 03 B2 40 80 5A 20 01 D2 D3 22 00 D2 E3
// 21 00 3F 40 E8 FD 1F 83 FE 23 F9 3F
// @FFFE
// 00 F0
// Q
//
// Restrictions:
//
// * The number of sections is unlimited.
//
// * Each line can have a maximum of 16 data bytes, except the last
//   line of a section.
//
// * Data bytes are separated by a single space.
//
// * The termination tag q indicates end-of-file is mandatory.
//

#ifndef LX_ELF_SAVE_TITXT_CMD
#define LX_ELF_SAVE_TITXT_CMD

#include "LxElfSaveCmdBase.h"

class LxElfSaveTiTxtCmd : public LxElfSaveCmdBase
{
public:
  LxElfSaveTiTxtCmd(std::string const & fileName);

  virtual void DumpData  (Elf32_Addr addr,
                          LxElfDataBuffer const & data,
                          bool verbose,
                          std::ostream & os);

  virtual void DumpFooter(LxElfFile const & elfFile, std::ostream & os);
};

#endif // LX_ELF_SAVE_TITXT_CMD
