// ============ PBERR.C ================

// ================= Include =================

#include "pberr.h"

// ================= Define ==================

// Default PBErr
PBErr thePBErr = {._msg[0] = '\0', ._type = PBErrTypeUnknown, 
  ._stream = NULL, ._fatal = true};
// Declare a pointer for each repository, by default they are
// all pointing toward the default PBErr, but it allows the 
// user to manage separately the errors if necessary
PBErr* PBMathErr = &thePBErr;
PBErr* GSetErr = &thePBErr;
PBErr* ELORankErr = &thePBErr;
PBErr* ShapoidErr = &thePBErr;
PBErr* BCurveErr = &thePBErr;
PBErr* GenBrushErr = &thePBErr;
PBErr* FracNoiseErr = &thePBErr;
PBErr* GenAlgErr = &thePBErr;
PBErr* GradErr = &thePBErr;
PBErr* KnapSackErr = &thePBErr;
PBErr* NeuraNetErr = &thePBErr;
PBErr* PBPhysErr = &thePBErr;
PBErr* GenTreeErr = &thePBErr;
PBErr* JSONErr = &thePBErr;
PBErr* MiniFrameErr = &thePBErr;
PBErr* PixelToPosEstimatorErr = &thePBErr;
PBErr* PBDataAnalysisErr = &thePBErr;
PBErr* PBImgAnalysisErr = &thePBErr;
PBErr* PBFileSysErr = &thePBErr;
PBErr* SDSIAErr = &thePBErr;
PBErr* GDataSetErr = &thePBErr;
PBErr* ResPublishErr = &thePBErr;
PBErr* TheSquidErr = &thePBErr;
PBErr* CBoErr = &thePBErr;
PBErr* CrypticErr = &thePBErr;
PBErr* GradAutomatonErr = &thePBErr;
PBErr* SmallyErr = &thePBErr;
PBErr* BuzzyErr = &thePBErr;
PBErr* NeuraMorphErr = &thePBErr;

const char* PBErrTypeLbl[PBErrTypeNb] = {
  "unknown",
  "malloc failed",
  "null pointer",
  "invalid arguments",
  "unit test failed",
  "other",
  "invalid data",
  "I/O error",
  "not yet implemented",
  "runtime error"
};

// ================ Functions implementation ====================

// Static constructor
PBErr PBErrCreateStatic(void) {
  PBErr that = {._msg[0] = '\0', ._type = PBErrTypeUnknown, 
    ._stream = NULL, ._fatal = true};
  return that;
}

// Reset thePBErr
void PBErrReset(PBErr* const that) {
  if (that == NULL)
    return;
  that->_msg[0] = '\0';
  that->_type = PBErrTypeUnknown;
  that->_fatal = true;
}

// Hook for error handling
// Print the error type, the error message, the stack
// Exit if _fatal == true
// Reset the PBErr
void PBErrCatch(PBErr* const that) {
  if (that == NULL)
    return;
  FILE* stream = (that->_stream ? that->_stream : stderr);
  fprintf(stream, "---- PBErrCatch ----\n");
  PBErrPrintln(that, stream);
  fprintf(stream, "Stack:\n");
  void* stack[PBERR_MAXSTACKHEIGHT] = {NULL};
  int stackHeight = backtrace(stack, PBERR_MAXSTACKHEIGHT);
  backtrace_symbols_fd(stack, stackHeight, fileno(stream));
  if (errno != 0) {
    fprintf(stream, "errno: %s\n", strerror(errno));
    errno = 0;
  }
  if (that->_fatal) {
    fprintf(stream, "Exiting\n");
    fprintf(stream, "--------------------\n");
    exit(that->_type);
  }
  fprintf(stream, "--------------------\n");
  PBErrReset(that);
}

// Print the PBErr 'that' on 'stream'
void PBErrPrintln(const PBErr* const that, FILE* const stream) {
  // If the PBErr or stream is null
  if (that == NULL || stream == NULL)
    // Nothing to do
    return;
  if (that->_type > 0 && that->_type < PBErrTypeNb)
    fprintf(stream, "PBErrType: %s\n", PBErrTypeLbl[that->_type]);
  if (that->_msg[0] != '\0')
    fprintf(stream, "PBErrMsg: %s\n", that->_msg);
  if (that->_fatal)
    fprintf(stream, "PBErrFatal: true\n");
  else
    fprintf(stream, "PBErrFatal: false\n");
}

// Secured malloc
#if defined(PBERRALL) || defined(PBERRSAFEMALLOC)
void* PBErrMalloc(PBErr* const that, const size_t size) {
  void* ret = malloc(size);
  if (ret == NULL) {
    that->_type = PBErrTypeMallocFailed;
    sprintf(that->_msg, "malloc of %ld bytes failed\n", 
      (unsigned long int)size);
    that->_fatal = true;
    PBErrCatch(that);
  }
  return ret;
}
#endif

// Secured I/O
#if defined(PBERRALL) || defined(PBERRSAFEIO)

FILE* PBErrOpenStreamIn(PBErr* const that, const char* const path) {
#if BUILDMODE == 0
  if (that == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'that' is null");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (path == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'path' is null");
    that->_fatal = true;
    PBErrCatch(that);
  }
#endif
  FILE* fd = fopen(path, "r");
  if (fd == NULL) {
    that->_type = PBErrTypeIOError;
    sprintf(that->_msg, "fopen failed for %s", path);
    that->_fatal = false;
    PBErrCatch(that);
  }
  return fd;
}

FILE* PBErrOpenStreamOut(PBErr* const that, const char* const path) {
#if BUILDMODE == 0
  if (that == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'that' is null");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (path == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'path' is null");
    that->_fatal = true;
    PBErrCatch(that);
  }
#endif
  FILE* fd = fopen(path, "w");
  if (fd == NULL) {
    that->_type = PBErrTypeIOError;
    sprintf(that->_msg, "fopen failed for %s", path);
    that->_fatal = false;
    PBErrCatch(that);
  }
  return fd;
}

void PBErrCloseStream(PBErr* const that, FILE* const fd) {
#if BUILDMODE == 0
  if (that == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'that' is null");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (fd == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'fd' is null");
    that->_fatal = true;
    PBErrCatch(that);
  }
#endif
  (void)that;
  fclose(fd);
}


bool _PBErrScanfShort(PBErr* const that, 
  FILE* const stream, const char* const format, short* const data) {
#if BUILDMODE == 0
  if (that == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'that' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (stream == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'stream' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (format == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'format' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (data == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'data' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
#endif
  // Read from the stream
  if (fscanf(stream, format, data) == EOF) {
    that->_type = PBErrTypeIOError;
    sprintf(that->_msg, "fscanf failed\n");
    that->_fatal = false;
    PBErrCatch(that);
    return false;
  }
  return true;
}

bool _PBErrScanfInt(PBErr* const that, 
  FILE* const stream, const char* const format, int* const data) {
#if BUILDMODE == 0
  if (that == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'that' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (stream == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'stream' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (format == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'format' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (data == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'data' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
#endif
  // Read from the stream
  if (fscanf(stream, format, data) == EOF) {
    that->_type = PBErrTypeIOError;
    sprintf(that->_msg, "fscanf failed\n");
    that->_fatal = false;
    PBErrCatch(that);
    return false;
  }
  return true;
}

bool _PBErrScanfFloat(PBErr* const that, 
  FILE* const stream, const char* const format, float* const data) {
#if BUILDMODE == 0
  if (that == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'that' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (stream == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'stream' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (format == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'format' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (data == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'data' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
#endif
  // Read from the stream
  if (fscanf(stream, format, data) == EOF) {
    that->_type = PBErrTypeIOError;
    sprintf(that->_msg, "fscanf failed\n");
    that->_fatal = false;
    PBErrCatch(that);
    return false;
  }
  return true;
}
  
bool _PBErrScanfStr(PBErr* const that, 
  FILE* const stream, const char* const format, char* const data) {
#if BUILDMODE == 0
  if (that == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'that' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (stream == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'stream' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (format == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'format' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (data == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'data' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
#endif
  // Read from the stream
  if (fscanf(stream, format, data) == EOF) {
    that->_type = PBErrTypeIOError;
    sprintf(that->_msg, "fscanf failed\n");
    that->_fatal = false;
    PBErrCatch(that);
    return false;
  }
  return true;
}
  
bool _PBErrPrintfShort(PBErr* const that, 
  FILE* const stream, const char* const format, const short data) {
#if BUILDMODE == 0
  if (that == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'that' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (stream == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'stream' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (format == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'format' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
#endif
  // Print to the stream
  if (fprintf(stream, format, data) < 0) {
    that->_type = PBErrTypeIOError;
    sprintf(that->_msg, "fprintf failed\n");
    that->_fatal = false;
    PBErrCatch(that);
    return false;
  }
  return true;
}

bool _PBErrPrintfLong(PBErr* const that, 
  FILE* const stream, const char* const format, const long data) {
#if BUILDMODE == 0
  if (that == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'that' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (stream == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'stream' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (format == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'format' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
#endif
  // Print to the stream
  if (fprintf(stream, format, data) < 0) {
    that->_type = PBErrTypeIOError;
    sprintf(that->_msg, "fprintf failed\n");
    that->_fatal = false;
    PBErrCatch(that);
    return false;
  }
  return true;
}

bool _PBErrPrintfInt(PBErr* const that, 
  FILE* const stream, const char* const format, const int data) {
#if BUILDMODE == 0
  if (that == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'that' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (stream == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'stream' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (format == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'format' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
#endif
  // Print to the stream
  if (fprintf(stream, format, data) < 0) {
    that->_type = PBErrTypeIOError;
    sprintf(that->_msg, "fprintf failed\n");
    that->_fatal = false;
    PBErrCatch(that);
    return false;
  }
  return true;
}

bool _PBErrPrintfFloat(PBErr* const that, 
  FILE* const stream, const char* const format, const float data) {
#if BUILDMODE == 0
  if (that == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'that' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (stream == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'stream' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (format == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'format' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
#endif
  // Print to the stream
  if (fprintf(stream, format, data) < 0) {
    that->_type = PBErrTypeIOError;
    sprintf(that->_msg, "fprintf failed\n");
    that->_fatal = false;
    PBErrCatch(that);
    return false;
  }
  return true;
}

bool _PBErrPrintfStr(PBErr* const that, 
  FILE* const stream, const char* const format, 
  const char* const data) {
#if BUILDMODE == 0
  if (that == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'that' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (stream == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'stream' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
  if (format == NULL) {
    that->_type = PBErrTypeNullPointer;
    sprintf(that->_msg, "'format' is null\n");
    that->_fatal = true;
    PBErrCatch(that);
  }
#endif
  // Print to the stream
  if (fprintf(stream, format, data) < 0) {
    that->_type = PBErrTypeIOError;
    sprintf(that->_msg, "fprintf failed\n");
    that->_fatal = false;
    PBErrCatch(that);
    return false;
  }
  return true;
}

#endif

// Try/catch

// Size of the stack of TryCatch blocks, define how many recursive incursion
// of TryCatch blocks can be done, overflow is checked at the beginning of
// each TryCatch blocks with TryCatchGuardOverflow()
// (Set delibarately low here to be able to test it in the example main.c)
#define TryCatchMaxExcLvl 3

// Stack of jmp_buf to memorise the TryCatch blocks
// To avoid exposing this variable to the user, implement any code using
// it as functions here instead of in the #define-s of trycatch.h
jmp_buf tryCatchExcJmp[TryCatchMaxExcLvl];

// Index of the next TryCatch block in the stack of jmp_buf
// To avoid exposing this variable to the user, implement any code using
// it as functions here instead of in the #define-s of trycatch.h
int tryCatchExcLvl = 0;

// ID of the last raised exception
// To avoid exposing this variable to the user, implement any code using
// it as functions here instead of in the #define-s of trycatch.h
// Do not use the type enum TryCatchException to allow the user to extend
// the list of exceptions with user defined exceptions outside of enum
// TryCatchException.
int tryCatchExc = 0;

// Function called at the beginning of a TryCatch block to guard against
// overflow of the stack of jump_buf
void TryCatchGuardOverflow(
  // No arguments
  void) {

  // If the max level of incursion is reached
  if (tryCatchExcLvl == TryCatchMaxExcLvl) {

    // Print a message on the standard error output and exit
    fprintf(
      stderr,
      "TryCatch blocks recursive incursion overflow, exiting. "
      "(You can try to raise the value of TryCatchMaxExcLvl in trycatch.c, "
      "it was: %d)\n",
      TryCatchMaxExcLvl);
    exit(EXIT_FAILURE);

  }

}

// Function called to get the jmp_buf on the top of the stack when
// starting a new TryCatch block
jmp_buf* TryCatchGetJmpBufOnStackTop(
  // No arguments
  void) {

  // Reset the last raised exception
  tryCatchExc = 0;

  // Memorise the current jmp_buf at the top of the stack
  jmp_buf* topStack = tryCatchExcJmp + tryCatchExcLvl;

  // Move the index of the top of the stack of jmp_buf to the upper level
  tryCatchExcLvl++;

  // Return the jmp_buf previously at the top of the stack
  return topStack;

}

// Function called to raise the TryCatchException 'exc'
void Raise(
  // The TryCatchException to raise. Do not use the type enum
  // TryCatchException to allow the user to extend the list of exceptions
  // with user defined exception outside of enum TryCatchException.
  int exc) {

  // If we are in a TryCatch block
  if (tryCatchExcLvl > 0) {

    // Memorise the last raised exception to be able to handle it if
    // it reaches the default case in the swith statement of the TryCatch
    // block
    tryCatchExc = exc;

    // Move to the top of the stack of jmp_buf to the lower level
    tryCatchExcLvl--;

    // Call longjmp with the appropriate jmp_buf in the stack and the
    // raised TryCatchException.
    longjmp(
      tryCatchExcJmp[tryCatchExcLvl],
      exc);

  // Else we are not in a TryCatch block
  } else {

    // Print a message on the standard error stream and ignore the
    // exception
    fprintf(
      stderr,
      "Unhandled exception (%d).\n",
      exc);

  }

}

// Function called when a raised TryCatchException has not been catched
// by a Catch segment
void TryCatchDefault(
  // No arguments
  void) {

  // If we are outside of a TryCatch block
  if (tryCatchExcLvl == 0) {

    // The exception has not been catched by a Catch segment,
    // print a message on the standard error stream and ignore it
    fprintf(
      stderr,
      "Unhandled exception (%d).\n",
      tryCatchExc);

  // Else, the exception has not been catched in the current
  // TryCatch block but may be catchable at lower level
  } else {

    // Move to the lower level in the stack of jmp_buf and raise the
    // exception again
    Raise(tryCatchExc);

  }

}

// Function called at the end of a TryCatch block
void TryCatchEnd(
  // No arguments
  void) {

  // The execution has reached the end of the current TryCatch block,
  // move back to the lower level in the stack of jmp_buf
  if (tryCatchExcLvl > 0) tryCatchExcLvl--;

}

// The struct siginfo_t used to handle the SIGSEV is not defined in
// ANSI C, guard against this.
#ifndef __STRICT_ANSI__

// Handler function to raise the exception TryCatchException_Segv when
// receiving the signal SIGSEV.
void TryCatchSigSegvHandler(
  // Received signal, will always be SIGSEV, unused
  int signal,
  // Info about the signal, unused
  siginfo_t *si,
  // Optional arguments, unused
  void *arg) {

  (void)signal; (void)si; (void)arg;

  // Raise the exception
  Raise(TryCatchException_Segv);

}

// Function to set the handler function of the signal SIGSEV and raise
// TryCatchException_Segv upon reception of this signal. Must have been
// called before using Catch(TryCatchException_Segv)
void TryCatchInitHandlerSigSegv(
  // No arugments
  void) {

  // Create a struct sigaction to set the handler
  struct sigaction sigActionSegv;
  memset(
    &sigActionSegv,
    0,
    sizeof(struct sigaction));
  sigemptyset(&(sigActionSegv.sa_mask));
  sigActionSegv.sa_sigaction = TryCatchSigSegvHandler;
  sigActionSegv.sa_flags = SA_SIGINFO;

  // Set the handler
  sigaction(
    SIGSEGV,
    &sigActionSegv,
    NULL);

}

#endif


