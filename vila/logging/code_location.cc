/*
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2021-2023 Intel Corporation
 *
 * This software and the related documents are Intel copyrighted materials,
 * and your use of them is governed by the express license under which they
 * were provided to you ("License"). Unless the License provides otherwise,
 * you may not use, modify, copy, publish, distribute, disclose or transmit
 * this software or the related documents without Intel's prior written
 * permission. This software and the related documents are provided as is, with
 * no express or implied warranties, other than those that are expressly stated
 * in the License.
 */
/****************************************
 * Description: Capture backtrace
 ****************************************/
#include "vila/logging/code_location.h"

#include <functional>

#if _WIN32 && defined(VILA_USE_BACKTRACE)
#include <Windows.h>
// must after windows header
#include <DbgHelp.h>

using PSTACK_WALK64 = BOOL (*)(
    DWORD,
    HANDLE,
    HANDLE,
    LPSTACKFRAME64,
    PVOID,
    PREAD_PROCESS_MEMORY_ROUTINE64,
    PFUNCTION_TABLE_ACCESS_ROUTINE64,
    PGET_MODULE_BASE_ROUTINE64,
    PTRANSLATE_ADDRESS_ROUTINE64
);
using PFROM_ADDR = BOOL (*)(HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO);
using PGET_LINE_FROM_ADDR64 =
    int (*)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
using PINITIALIZE = BOOL (*)(HANDLE, PCSTR, BOOL);
using PCLEANUP = BOOL (*)(HANDLE);

class DbgHelper {
 public:
  DbgHelper()
      : module_(LoadLibraryA("dbghelp.dll")),
        process_(GetCurrentProcess()),
        thread_(GetCurrentThread()),
        ddi_({}) {
    initDdiTable();
    fnSymInitialize();
  }

  ~DbgHelper() {
    fnSymCleanup();
    FreeLibrary(module_);
  }

  BOOL StackWalk(LPSTACKFRAME64 stack, PVOID ctx) {
    return ddi_.fnStackWalk(
        IMAGE_FILE_MACHINE_AMD64, process_, thread_, stack, ctx, nullptr,
        ddi_.fnFunctionTableAccess, ddi_.fnGetModuleBase, nullptr
    );
  }

  BOOL SymFromAddr(DWORD64 addr, PDWORD64 disp, PSYMBOL_INFO sym) {
    return ddi_.fnFromAddr(process_, addr, disp, sym);
  }

  int SymGetLineFromAddr64(DWORD64 addr, PDWORD disp, PIMAGEHLP_LINE64 line) {
    return ddi_.fnGetLineFromAddr(process_, addr, disp, line);
  }

 private:
  template <class T>
  void getAddressPointer(T*& func, const char* entry) {
    func = reinterpret_cast<T*>(GetProcAddress(module_, entry));
  }

  void initDdiTable() {
    getAddressPointer(ddi_.fnStackWalk, "StackWalk64");
    getAddressPointer(ddi_.fnFunctionTableAccess, "SymFunctionTableAccess64");
    getAddressPointer(ddi_.fnGetModuleBase, "SymGetModuleBase64");
    getAddressPointer(ddi_.fnFromAddr, "SymFromAddr");
    getAddressPointer(ddi_.fnGetLineFromAddr, "SymGetLineFromAddr64");
    getAddressPointer(ddi_.fnInitialize, "SymInitialize");
    getAddressPointer(ddi_.fnCleanup, "SymCleanup");
  }

  BOOL fnSymInitialize() { return ddi_.fnInitialize(process_, nullptr, true); }

  BOOL fnSymCleanup() { return ddi_.fnCleanup(process_); }

  struct DdiTable {
    PSTACK_WALK64 fnStackWalk;
    PFUNCTION_TABLE_ACCESS_ROUTINE64 fnFunctionTableAccess;
    PGET_MODULE_BASE_ROUTINE64 fnGetModuleBase;
    PFROM_ADDR fnFromAddr;
    PGET_LINE_FROM_ADDR64 fnGetLineFromAddr;
    PINITIALIZE fnInitialize;
    PCLEANUP fnCleanup;
  };

  HMODULE module_;
  HANDLE process_;
  HANDLE thread_;
  DdiTable ddi_;
};

std::vector<std::string> vila::CaptureBackTrace() {
  DbgHelper dbg;
  CONTEXT context;
  RtlCaptureContext(&context);
  STACKFRAME64 stack{};
  IMAGEHLP_LINE64 line{};
  line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
  std::vector<std::string> bt;

  constexpr ULONG kMaxNameLen = 256;
  std::vector<char> sym_overlap(sizeof(SYMBOL_INFO) + kMaxNameLen * 2);
  PSYMBOL_INFO syminfo = reinterpret_cast<PSYMBOL_INFO>(sym_overlap.data());
  syminfo->SizeOfStruct = sizeof(SYMBOL_INFO);
  syminfo->MaxNameLen = kMaxNameLen;
  for (;;) {
    // get next call from stack
    if (!dbg.StackWalk64(&stack, &context)) break;
    // get symbol name for address
    DWORD64 displacement = 0;
    dbg.SymFromAddr(stack.AddrPC.Offset, &displacement, syminfo);
    // try to get line
    DWORD disp = 0;
    if (dbg.SymGetLineFromAddr64(stack.AddrPC.Offset, &disp, &line)) {
      bt.push_back(fmt::format(
          "at {} in {}({:d}). addr {:#08x}", syminfo->Name, line.FileName,
          line.LineNumber, syminfo->Address
      ));
    } else {
      // failed to get line, at least print module name
      HMODULE mod;
      char filename[kMaxNameLen];  // NOLINT(*-c-arrays)
      if (GetModuleHandleEx(
              GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                  GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
              (LPCTSTR)(stack.AddrPC.Offset), &mod
          )) {
        GetModuleFileNameA(mod, filename, sizeof(filename));
        bt.push_back(fmt::format(
            "at {} in {}. addr {:#08x}", syminfo->Name, filename,
            syminfo->Address
        ));
      }
    }
  }
  // skip myself
  bt.erase(bt.begin());
  return bt;
}
#else
std::vector<std::string> vila::CaptureBackTrace() { return {}; }
#endif
