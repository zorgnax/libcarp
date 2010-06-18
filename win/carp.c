/*
Get a stacktrace on windows using dbghelp.dll

Referenced
    - http://stackwalker.codeplex.com and
    - http://msdn.microsoft.com/en-us/library/ms680650(VS.85).aspx
    - http://github.com/mirrors/wine/blob/master/programs/winedbg/stack.c
    - http://jpassing.com/2008/03/12/walking-the-stack-of-the-current-thread/
*/

#pragma warning (disable: 4100 4619 4514 4668 4820 4191 4255 4127)
#include <stdio.h>
#include <windows.h>
#include <dbghelp.h>

void cluck () {
    CONTEXT context = {0};
    STACKFRAME64 stackframe = {0};
    HANDLE process = GetCurrentProcess();
    int i = 0;
    
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
        SymFromAddr(process, pc, 0, symbol);
        
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
        
        printf("name: %s, line: %d, file: %s, module: %s, image: %s\n",
            symbol->Name,
            line->LineNumber,
            line->FileName,
            module->ModuleName,
            module->LoadedImageName);
        
        free(symbol);
        free(line);
        free(module);
    }
    
    SymCleanup(process);
}

void foo5 () {
    /* get the stack trace here */
    cluck();
}

void foo4 () {foo5();}
void foo3 () {foo4();}
void foo2 () {foo3();}
void foo1 () {foo2();}

int main () {
    foo1();
    return 0;
}
