// ============ PBERR.C ================

// ================= Include =================

#include "pberr.h"

// ================= Define ==================

PBErr thePBErr = {._msg[0] = '\0', ._type = PBErrTypeUnknown, 
  ._stream = NULL, ._fatal = true};
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
PBErr* GTreeErr = &thePBErr;
PBErr* JSONErr = &thePBErr;

char* PBErrTypeLbl[PBErrTypeNb] = {
  "unknown",
  "malloc failed",
  "null pointer",
  "invalid arguments",
  "unit test failed",
  "other",
  "InvalidData",
  "I/O error"
};

// ================ Functions implementation ====================

// Static constructor
PBErr PBErrCreateStatic(void) {
  PBErr that = {._msg[0] = '\0', ._type = PBErrTypeUnknown, 
    ._stream = NULL, ._fatal = true};
  return that;
}

// Reset thePBErr
void PBErrReset(PBErr* that) {
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
void PBErrCatch(PBErr* that) {
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
void PBErrPrintln(PBErr* that, FILE* stream) {
  // If the PBErr or stream is null
  if (that == NULL || stream == NULL)
    // Nothing to do
    return;
  if (that->_type > 0 && that->_type < PBErrTypeNb)
    fprintf(stream, "PBErrType: %s\n", PBErrTypeLbl[that->_type]);
  if (that->_msg != NULL)
    fprintf(stream, "PBErrMsg: %s\n", that->_msg);
  if (that->_fatal)
    fprintf(stream, "PBErrFatal: true\n");
  else
    fprintf(stream, "PBErrFatal: false\n");
}

// Secured malloc
#if defined(PBERRALL) || defined(PBERRSAFEMALLOC)
void* PBErrMalloc(PBErr* that, size_t size) {
  void* ret = malloc(size);
  if (ret == NULL) {
    that->_type = PBErrTypeMallocFailed;
    sprintf(that->_msg, "malloc of %d bytes failed\n", size);
    that->_fatal = true;
    PBErrCatch(that);
  }
  return ret;
}
#endif

// Secured I/O
#if defined(PBERRALL) || defined(PBERRSAFEIO)

FILE* PBErrOpenStreamIn(PBErr* that, char* path) {
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

FILE* PBErrOpenStreamOut(PBErr* that, char* path) {
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

void PBErrCloseStream(PBErr* that, FILE* fd) {
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


bool _PBErrScanfShort(PBErr* that, 
  FILE* stream, char* format, short* data) {
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

bool _PBErrScanfInt(PBErr* that, 
  FILE* stream, char* format, int* data) {
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

bool _PBErrScanfFloat(PBErr* that, 
  FILE* stream, char* format, float* data) {
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
  
bool _PBErrScanfStr(PBErr* that, 
  FILE* stream, char* format, char* data) {
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
  
bool _PBErrPrintfShort(PBErr* that, 
  FILE* stream, char* format, short data) {
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

bool _PBErrPrintfInt(PBErr* that, 
  FILE* stream, char* format, int data) {
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

bool _PBErrPrintfFloat(PBErr* that, 
  FILE* stream, char* format, float data) {
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

bool _PBErrPrintfStr(PBErr* that, 
  FILE* stream, char* format, char* data) {
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
