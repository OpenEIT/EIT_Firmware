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

#ifndef LX_ELF_CMD_FACTORY
#define LX_ELF_CMD_FACTORY

#include "LxElfCmd.h"
#include "LxMain.h"


class LxElfCmdFactory
{
public:
  LxElfCmdFactory();

  LxElfCmd* CreateSaveCmd(const std::string & fileName);

  LxElfCmd* CreateSaveSRecCmd(const std::string & fileName,
                              unsigned char len,
                              SRecVariant variant);

  LxElfCmd* CreateSaveIHexCmd(const std::string & fileName);

  LxElfCmd* CreateSaveTiTxtCmd(const std::string & fileName);

  LxElfCmd* CreateSaveSimpleCodeCmd(const std::string & fileName,
                                    bool entryRecordWanted);
  
  LxElfCmd* CreateSaveBinCmd(const std::string & fileName);  

  LxElfCmd* CreateChecksumCmd(uint8_t                   symSize,
                              LxAlgo                    algorithm,
                              LxCompl                   complement,
                              bool                      mirror,
                              bool                      reverse,
							  bool                      rSign,
                              uint64_t                  polynomial,
                              LxSymbolicRanges const &  ranges,
                              LxSymbolicAddress const & symbol,
                              uint64_t                  startValue,
                              StartValueType            startValueType,
                              uint8_t                   unitSize);

  LxElfCmd* CreateFillCmd(LxSymbolicRange      range,
                          const FillPattern &  pattern,
                          bool                 virtual_fill);

  LxElfCmd* CreateParityCmd(uint32_t                   symSize,
                              bool                      even,
                              bool                      reverse,
							  LxSymbolicRanges const &  ranges,
                              LxSymbolicAddress const & symbol,
                              uint32_t                  unitSize,
				              LxSymbolicAddress         flashBase);

  LxElfCmd* CreateFillValidateCmd(LxSymbolicRanges const & fillRanges);

  LxElfCmd* CreateStripCmd();

  LxElfCmd* CreateRelocCmd(std::string const & args,
                           unsigned long nJumpTableEntries,
                           bool withDebug);
};

#endif //LX_ELF_CMD_FACTORY
