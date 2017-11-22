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

// This class creates commands used for transforming an elf file

#include "LxElfCmdFactory.h"
#include "LxElfSaveCmd.h"
#include "LxElfSaveSRecCmd.h"
#include "LxElfSaveIHexCmd.h"
#include "LxElfSaveTiTxtCmd.h"
#include "LxElfSaveSimpleCode.h"
#include "LxElfSaveBinCmd.h"
#include "LxElfChecksumCmd.h"
#include "LxElfParityCmd.h"
#include "LxElfFillCmd.h"
#include "LxElfStripCmd.h"
#include "LxElfRelocCmd.h"

using namespace std;

LxElfCmdFactory::
LxElfCmdFactory()
{
}


LxElfCmd * LxElfCmdFactory::
CreateSaveCmd(const string & fileName)
{
  return new LxElfSaveCmd(fileName);
}


LxElfCmd * LxElfCmdFactory::
CreateSaveSRecCmd(const std::string & fileName,
                  unsigned char len,
                  SRecVariant variant)
{
  return new LxElfSaveSRecCmd(fileName,
                              variant,
                              len);
}


LxElfCmd * LxElfCmdFactory::
CreateSaveIHexCmd(const std::string & fileName)
{
  return new LxElfSaveIHexCmd(fileName);
}

LxElfCmd * LxElfCmdFactory::
CreateSaveTiTxtCmd(const std::string & fileName)
{
  return new LxElfSaveTiTxtCmd(fileName);
}

LxElfCmd * LxElfCmdFactory::
CreateSaveSimpleCodeCmd(const std::string &fileName,
                        bool entryRecordWanted)
{
  return new LxElfSaveSimpleCodeCmd(fileName,
                                    entryRecordWanted);
}

LxElfCmd * LxElfCmdFactory::
CreateSaveBinCmd(const std::string & fileName)
{
  return new LxElfSaveBinCmd(fileName);
}

LxElfCmd * LxElfCmdFactory::
CreateChecksumCmd(uint8_t         symSize,
                  LxAlgo          algorithm,
                  LxCompl         complement,
                  bool            mirror,
                  bool            reverse,
				  bool            rSign,
                  uint64_t        polynomial,
                  LxSymbolicRanges  const & ranges,
                  LxSymbolicAddress const & symbol,
                  uint64_t        startValue,
                  StartValueType  startValueType,
                  uint8_t         unitSize)
{
  return new LxElfChecksumCmd(symSize,
                              algorithm,
                              complement,
                              mirror,
                              reverse,
							  rSign,
                              polynomial,
                              ranges,
                              symbol,
                              startValue,
                              startValueType,
                              unitSize);
}

LxElfCmd * LxElfCmdFactory::
CreateFillCmd(LxSymbolicRange      range,
              const FillPattern &  pattern,
              bool                 virtual_fill)
{
  return new LxElfFillCmd(range, pattern, virtual_fill);
}

LxElfCmd * LxElfCmdFactory::
CreateParityCmd(uint32_t                   symSize,
                              bool                      even,
                              bool                      reverse,
							  LxSymbolicRanges const &  ranges,
                              LxSymbolicAddress const & symbol,
                              uint32_t                  unitSize,
				              LxSymbolicAddress         flashBase)

{
  return new LxElfParityCmd(symSize,
							even,
                            reverse,
							ranges,
                            symbol,
                            unitSize,
							flashBase);
}

LxElfCmd * LxElfCmdFactory::
CreateFillValidateCmd(LxSymbolicRanges const & fillRanges)
{
  return new LxElfFillValidateCmd(fillRanges);
}

LxElfCmd * LxElfCmdFactory::
CreateStripCmd()
{
  return new LxElfStripCmd;
}

LxElfCmd * LxElfCmdFactory::
CreateRelocCmd(std::string const & filename,
               unsigned long nJumpTableEntries,
               bool withDebug)
{
  return new LxElfRelocCmd(filename, nJumpTableEntries, withDebug);
}
