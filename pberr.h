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
#include <setjmp.h>
#include <signal.h>


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
extern PBErr* GradAutomatonErr;
extern PBErr* SmallyErr;
extern PBErr* BuzzyErr;
extern PBErr* NeuraMorphErr;

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

// Try/catch

// List of exceptions ID, must starts at 1 (0 is reserved for the setjmp at
// the beginning of the TryCatch blocks). One can extend the list at will
// here, or user defined exceptions can be added directly in the user code
// as follow:
// enum UserDefinedExceptions {
//
//   myUserExceptionA = TryCatchException_LastID,
//   myUserExceptionB,
//   myUserExceptionC
//
// };
// TryCatchException_LastID is not an exception but a convenience to
// create new exceptions (as in the example above) while ensuring
// their ID doesn't collide with the ID of exceptions in TryCatchException.
// Exception defined here are only examples, one should create a list of
// default exceptions according to the planned use of this trycatch module.
enum TryCatchException {

  TryCatchException_IOError = 1,
  TryCatchException_NaN,
  TryCatchException_Segv,
  TryCatchException_LastID

};

// Function called at the beginning of a TryCatch block to guard against
// overflow of the stack of jump_buf
void TryCatchGuardOverflow(
  // No arguments
  void);

// Function called to get the jmp_buf on the top of the stack when
// starting a new TryCatch block
jmp_buf* TryCatchGetJmpBufOnStackTop(
  // No arguments
  void);

// Function called when a raised TryCatchException has not been catched
// by a Catch segment
void TryCatchDefault(
  // No arguments
  void);

// Function called at the end of a TryCatch block
void TryCatchEnd(
  // No arguments
  void);

// Head of the TryCatch block, to be used as
//
// Try {
//   /*... code of the TryCatch block here ...*/
//
// Comments on the macro:
//   // Guard against recursive incursion overflow
//   TryCatchGuardOverflow();
//   // Memorise the jmp_buf on the top of the stack, setjmp returns 0
//   switch (setjmp(*TryCatchGetJmpBufOnStackTop())) {
//     // Entry point for the code of the TryCatch block
//     case 0:
#define Try \
  TryCatchGuardOverflow(); \
  switch (setjmp(*TryCatchGetJmpBufOnStackTop())) { \
    case 0:

// Catch segment in the TryCatch block, to be used as
//
// Catch (/*... one of TryCatchException or user defined exception ...*/) {
//   /*... code executed if the exception has been raised in the
//     TryCatch block ...*/
//
// Comments on the macro:
//      // End of the previous case
//      break;
//    // case of the raised exception
//    case e:
#define Catch(e) \
      break;\
    case e:

// Tail of the TryCatch block, to be used as
//
// } EndTry;
//
// Comments on the macro:
//      // End of the previous case
//      break;
//    // default case, i.e. any raised exception which hasn't been catched
//    // by a previous Catch is catched here
//    default:
//      // Processing of uncatched exception
//      TryCatchDefault();
//  // End of the switch statement at the head of the TryCatch block
//  }
//  // Post processing of the TryCatchBlock
//  TryCatchEnd()
#define EndTry \
      break; \
    default: \
      TryCatchDefault(); \
  } \
  TryCatchEnd()

// Function called to raise the TryCatchException 'exc'
void Raise(
  // The TryCatchException to raise. Do not use the type enum
  // TryCatchException to allow the user to extend the list of exceptions
  // with user defined exception outside of enum TryCatchException.
  int exc);

// The struct siginfo_t used to handle the SIGSEV is not defined in
// ANSI C, guard against this.
#ifndef __STRICT_ANSI__

// Function to set the handler function of the signal SIGSEV and raise
// TryCatchException_Segv upon reception of this signal. Must have been
// called before using Catch(TryCatchException_Segv)
void TryCatchInitHandlerSigSegv(
  // No arguments
  void);

#endif

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
