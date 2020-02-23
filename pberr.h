// ============ PBERR.H ================

#ifndef PBERR_H
#define PBERR_H

// ================= Include =================

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <execinfo.h>
#include <errno.h>
#include <string.h>

// ================= Define ==================

#define PBERR_MAXSTACKHEIGHT 10
#define PBERR_MSGLENGTHMAX 256

// ================= Data structure ===================

typedef enum PBErrType {
  PBErrTypeUnknown,
  PBErrTypeMallocFailed,
  PBErrTypeNullPointer,
  PBErrTypeInvalidArg,
  PBErrTypeUnitTestFailed,
  PBErrTypeOther,
  PBErrTypeInvalidData,
  PBErrTypeIOError,
  PBErrTypeNotYetImplemented,
  PBErrTypeRuntimeError,
  PBErrTypeNb
} PBErrType;

typedef struct PBErr {
  // Error message
  char _msg[PBERR_MSGLENGTHMAX];
  // Error type
  PBErrType _type;
  // Stream for output
  FILE* _stream;
  // Fatal mode, if true exit when catch
  bool _fatal;
} PBErr;

// ================= Global variable ==================

extern PBErr thePBErr;
extern PBErr* PBMathErr;
extern PBErr* GSetErr;
extern PBErr* ELORankErr;
extern PBErr* ShapoidErr;
extern PBErr* BCurveErr;
extern PBErr* GenBrushErr;
extern PBErr* FracNoiseErr;
extern PBErr* GenAlgErr;
extern PBErr* GradErr;
extern PBErr* KnapSackErr;
extern PBErr* NeuraNetErr;
extern PBErr* PBPhysErr;
extern PBErr* GenTreeErr;
extern PBErr* JSONErr;
extern PBErr* MiniFrameErr;
extern PBErr* PixelToPosEstimatorErr;
extern PBErr* PBDataAnalysisErr;
extern PBErr* PBImgAnalysisErr;
extern PBErr* PBFileSysErr;
extern PBErr* SDSIAErr;
extern PBErr* GDataSetErr;
extern PBErr* ResPublishErr;
extern PBErr* TheSquidErr;
extern PBErr* CBoErr;
extern PBErr* CrypticErr;

// ================ Functions declaration ====================

// Static constructor
PBErr PBErrCreateStatic(void);

// Reset thePBErr
void PBErrReset(PBErr* const that);

// Hook for error handling
void PBErrCatch(PBErr* const that);

// Print the PBErr 'that' on 'stream'
void PBErrPrintln(const PBErr* const that, FILE* const stream);

// Secured malloc
#if defined(PBERRALL) || defined(PBERRSAFEMALLOC)
  void* PBErrMalloc(PBErr* const that, const size_t size);
#else
  #define PBErrMalloc(That, Size) malloc(Size)
#endif

// Secured I/O
#if defined(PBERRALL) || defined(PBERRSAFEIO)
  FILE* PBErrOpenStreamIn(PBErr* const that, const char* const path);
  FILE* PBErrOpenStreamOut(PBErr* const that, const char* const path);
  void PBErrCloseStream(PBErr* const that, FILE* const fd);

  bool _PBErrScanfShort(PBErr* const that, 
    FILE* const stream, const char* const format, short* const data);
  bool _PBErrScanfInt(PBErr* const that, 
    FILE* const stream, const char* const format, int* const data);
  bool _PBErrScanfFloat(PBErr* const that, 
    FILE* const stream, const char* const format, float* const data);
  bool _PBErrScanfStr(PBErr* const that, 
    FILE* const stream, const char* const format, char* const data);
    
  bool _PBErrPrintfShort(PBErr* const that, 
    FILE* const stream, const char* const format, const short data);
  bool _PBErrPrintfInt(PBErr* const that, 
    FILE* const stream, const char* const format, const int data);
  bool _PBErrPrintfLong(PBErr* const that, 
    FILE* const stream, const char* const format, const long data);
  bool _PBErrPrintfFloat(PBErr* const that, 
    FILE* const stream, const char* const format, const float data);
  bool _PBErrPrintfStr(PBErr* const that, 
    FILE* const stream, const char* const format, 
    const char* const data);
#else
  #define PBErrOpenStreamIn(Err, Path) \
    fopen(Path, "r")
  #define PBErrOpenStreamOut(Err, Path) \
    fopen(Path, "w")
  #define PBErrCloseStream(Err, Stream) \
    fclose(Stream)

  #define PBErrScanf(Err, Stream, Format, Data) \
    (fscanf(Stream, Format, Data) == EOF)
  #define PBErrPrintf(Err, Stream, Format, Data) \
    (fprintf(Stream, Format, Data) < 0)
#endif

// Hook for invalid polymorphisms
void PBErrInvalidPolymorphism(void*t, ...); 


// ================= Polymorphism ==================

#if defined(PBERRALL) || defined(PBERRSAFEIO)
  #define PBErrScanf(Err, Stream, Format, Data) _Generic(Data, \
    short*: _PBErrScanfShort, \
    int*: _PBErrScanfInt, \
    float*: _PBErrScanfFloat, \
    char*: _PBErrScanfStr, \
    default: PBErrInvalidPolymorphism) (Err, Stream, Format, Data)

  #define PBErrPrintf(Err, Stream, Format, Data) _Generic(Data, \
    short: _PBErrPrintfShort, \
    int: _PBErrPrintfInt, \
    long: _PBErrPrintfLong, \
    float: _PBErrPrintfFloat, \
    char*: _PBErrPrintfStr, \
    default: PBErrInvalidPolymorphism) (Err, Stream, Format, Data)
#endif

#endif
