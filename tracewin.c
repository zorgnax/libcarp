#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <dbghelp.h>
#include "trace.h"
#include "list.h"
#include "funcinfo.h"

List *get_stack_trace () {
    CONTEXT context = {0};
    STACKFRAME64 stackframe = {0};
    HANDLE process = GetCurrentProcess();
    int i = 0;
    List *stack = NULL;
    
    /* fill in current context  */
    RtlCaptureContext(&context);
    
    /* fill in current stackframe  */
    stackframe.AddrPC.Mode = AddrModeFlat;
    stackframe.AddrFrame.Mode = AddrModeFlat;
    stackframe.AddrStack.Mode = AddrModeFlat;
    stackframe.AddrPC.Offset = context.Eip;
    stackframe.AddrFrame.Offset = context.Ebp;
    stackframe.AddrStack.Offset = context.Esp;
    
    /* initialize the symbol handler  */
    if (!SymInitialize(process, NULL, TRUE)) {
        fprintf(stderr, "SymInitialize() error %d\n", GetLastError());
        exit(255);
    }
    
    while (StackWalk64(
            IMAGE_FILE_MACHINE_I386, /* machine type           */
            process,                 /* process                */
            GetCurrentThread(),      /* thread                 */
            &stackframe,             /* stack frame            */
            &context,                /* context                */
            NULL,                    /* read mem func          */
            NULL,                    /* func table access func */
            NULL,                    /* get module base func   */
            NULL))                   /* translate addr func    */
    {
        FuncInfo *f;
        DWORD64 pc = stackframe.AddrPC.Offset;
        DWORD offset = 0;
        size_t size;
        SYMBOL_INFO *symbol;
        IMAGEHLP_LINE64 *line;
        IMAGEHLP_MODULE64 *module;

        if (i++ > 200)
            break;
    
        /* symbol  */
        size = sizeof (SYMBOL_INFO) + (1000 - 1) * sizeof (TCHAR);
        symbol = calloc(1, size);
        symbol->MaxNameLen = 1000;
        if (!SymFromAddr(process, pc, 0, symbol)) {
            printf("pc at 0x%p\n", pc);
            fprintf(stderr, "SymFromAddr() error %d\n", GetLastError());
            //exit(255);
        }
        
        /* line  */
        line = calloc(1, sizeof *line);
        line->SizeOfStruct = sizeof *line;
        SymGetLineFromAddr64(process, pc, &offset, line);

        /* module  */
        module = calloc(1, sizeof *module);
        if (!SymGetModuleInfo64(process, pc, module)) {
            fprintf(stderr, "SymGetModuleInfo64() error %d\n", GetLastError());
            exit(255);
        }
        
        f = (FuncInfo *) calloc(sizeof (FuncInfo), 1);
        f->func = strdup(symbol->Name);
        f->file = strdup(line->FileName);
        f->line = line->LineNumber;
        f->lib = strdup(module->ModuleName);
        stack = list_push(stack, f);
        
        free(symbol);
        free(line);
        free(module);
    }
    
    SymCleanup(process);
    return stack;
}