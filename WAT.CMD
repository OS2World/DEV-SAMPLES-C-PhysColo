REM This program does compilation and linking with Watcom C 386 9.0 using
REM the appropriate "/l" flag, an 8K stack, and the "benchmarking" options
REM Invoke it with "wat physcolo"
wcl386 /l=os2v2_pm /k8192 /omaxet /zp4 /4r %1
