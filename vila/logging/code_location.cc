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

class DbgHelper {
 public:
  DbgHelper() {
    module_ = LoadLibraryA("dbghelp.dll");
    process_ = GetCurrentProcess();
    thread_ = GetCurrentThread();
    this->fnSymInitialize();
  }

  ~DbgHelper() {
    this->fnSymCleanup();
    FreeLibrary(module_);
  }

  BOOL StackWalk(LPSTACKFRAME64 stack, PVOID ctx) {
    typedef BOOL (*func_t)(
        DWORD, HANDLE, HANDLE, LPSTACKFRAME64, PVOID,
        PREAD_PROCESS_MEMORY_ROUTINE64, PFUNCTION_TABLE_ACCESS_ROUTINE64,
        PGET_MODULE_BASE_ROUTINE64, PTRANSLATE_ADDRESS_ROUTINE64
    );
    typedef PVOID (*function_table_access_t)(HANDLE, DWORD64);
    typedef DWORD64 (*get_module_base_t)(HANDLE, DWORD64);
    static func_t fn = (func_t)GetProcAddress(module_, "StackWalk64");
    static function_table_access_t function_table_access =
        (function_table_access_t
        )GetProcAddress(module_, "SymFunctionTableAccess64");
    static get_module_base_t get_module_base =
        (get_module_base_t)GetProcAddress(module_, "SymGetModuleBase64");

    if (fn)
      return fn(
          IMAGE_FILE_MACHINE_AMD64, process_, thread_, stack, ctx, nullptr,
          function_table_access, get_module_base, nullptr
      );
    return 0;
  }

  BOOL SymFromAddr(DWORD64 addr, PDWORD64 disp, PSYMBOL_INFO sym) {
    typedef BOOL (*func_t)(HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO);
    static func_t fn = (func_t)GetProcAddress(module_, "SymFromAddr");
    if (fn) return fn(process_, addr, disp, sym);
    return 0;
  }

  int SymGetLineFromAddr64(DWORD64 addr, PDWORD disp, PIMAGEHLP_LINE64 line) {
    typedef int (*func_t)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
    static func_t fn = (func_t)GetProcAddress(module_, "SymGetLineFromAddr64");
    if (fn) return fn(process_, addr, disp, line);
    return 0;
  }

 private:
  BOOL fnSymInitialize() {
    typedef BOOL (*func_t)(HANDLE, PCSTR, BOOL);
    static func_t fn = (func_t)GetProcAddress(module_, "SymInitialize");
    if (fn) return fn(process_, nullptr, true);
    return 0;
  }

  BOOL fnSymCleanup() {
    typedef BOOL (*func_t)(HANDLE);
    static func_t fn = (func_t)GetProcAddress(module_, "SymCleanup");
    if (fn) return fn(process_);
    return 0;
  }

  HMODULE module_;
  HANDLE process_;
  HANDLE thread_;
};

static DbgHelper g_dbg;

std::vector<std::string> vila::CaptureBackTrace() {
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
  for (int frame = 0;; frame++) {
    // get next call from stack
    if (!g_dbg.StackWalk64(&stack, &context)) break;
    // get symbol name for address
    DWORD64 displacement = 0;
    g_dbg.SymFromAddr(stack.AddrPC.Offset, &displacement, syminfo);
    // try to get line
    DWORD disp = 0;
    if (g_dbg.SymGetLineFromAddr64(stack.AddrPC.Offset, &disp, &line)) {
      bt.push_back(fmt::format(
          "at {} in {}({:d}). addr {:#08x}", syminfo->Name, line.FileName,
          line.LineNumber, syminfo->Address
      ));
    } else {
      // failed to get line, at least print module name
      HMODULE mod;
      char filename[kMaxNameLen];
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
  return bt;
}
#else
std::vector<std::string> vila::CaptureBackTrace() { return {}; }
#endif
