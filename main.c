#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "pberr.h"

void UnitTestCreateStatic() {
  printf("UnitTestCreateStatic\n");
  PBErr err = PBErrCreateStatic();
  PBErrPrintln(&err, stdout);
}

void UnitTestReset() {
  printf("UnitTestReset\n");
  PBErr err = PBErrCreateStatic();
  PBErr clone = err;
  memset(&err, 0, sizeof(PBErr));
  PBErrReset(&err);
    printf("Reset ");
  if (memcmp(&err, &clone, sizeof(PBErr)) == 0)
    printf("OK");
  else
    printf("NOK");
  printf("\n");
}

void UnitTestMalloc() {
  printf("UnitTestMalloc\n");
  char* arr = PBErrMalloc(&thePBErr, 2);
  printf("Malloc ");
  if (arr == NULL)
    printf("NOK");
  else
    printf("OK");
  printf("\n");
  arr[0] = 0;
  arr[1] = 1;
  free(arr);
}

void UnitTestIO() {
  FILE* fd = PBErrOpenStreamOut(&thePBErr, "./testio.txt");
  short a = 1;
  PBErrPrintf(&thePBErr, fd, "%hi\n", a);
  short b = 2;
  PBErrPrintf(&thePBErr, fd, "%i\n", b);
  float c = 3.0;
  PBErrPrintf(&thePBErr, fd, "%f\n", c);
  char* d = "string";
  PBErrPrintf(&thePBErr, fd, "%s\n", d);
  PBErrCloseStream(&thePBErr, fd);
  fd = PBErrOpenStreamIn(&thePBErr, "./testio.txt");
  short checka;
  PBErrScanf(&thePBErr, fd, "%hi", &checka);
  if (a != checka) {
    thePBErr._stream = stdout;
    thePBErr._type = PBErrTypeUnitTestFailed;
    sprintf(thePBErr._msg, "UnitTestIO failed");
    thePBErr._fatal = false;
    PBErrCatch(&thePBErr);
  }
  int checkb;
  PBErrScanf(&thePBErr, fd, "%i", &checkb);
  if (b != checkb) {
    thePBErr._stream = stdout;
    thePBErr._type = PBErrTypeUnitTestFailed;
    sprintf(thePBErr._msg, "UnitTestIO failed");
    thePBErr._fatal = false;
    PBErrCatch(&thePBErr);
  }
  float checkc;
  PBErrScanf(&thePBErr, fd, "%f", &checkc);
  if (fabs(c - checkc) > 0.0001) {
    thePBErr._stream = stdout;
    thePBErr._type = PBErrTypeUnitTestFailed;
    sprintf(thePBErr._msg, "UnitTestIO failed");
    thePBErr._fatal = false;
    PBErrCatch(&thePBErr);
  }
  char checkd[10];
  PBErrScanf(&thePBErr, fd, "%s", checkd);
  if (strcmp(d, checkd) != 0) {
    thePBErr._stream = stdout;
    thePBErr._type = PBErrTypeUnitTestFailed;
    sprintf(thePBErr._msg, "UnitTestIO failed");
    thePBErr._fatal = false;
    PBErrCatch(&thePBErr);
  }
  PBErrCloseStream(&thePBErr, fd);
  fd = PBErrOpenStreamIn(&thePBErr, "./missingfile");
  printf("UnitTestIO OK\n");
}

void UnitTestCatch() {
  printf("UnitTestCatch\n");
  thePBErr._stream = stdout;
  thePBErr._type = PBErrTypeInvalidArg;
  sprintf(thePBErr._msg, "UnitTestCatch: invalid arg");
  thePBErr._fatal = false;
  PBErrCatch(&thePBErr);
  thePBErr._type = PBErrTypeNullPointer;
  sprintf(thePBErr._msg, "UnitTestCatch: null pointer");
  thePBErr._fatal = true;
  PBErrCatch(&thePBErr);
}

void fun() {
  if (isnan(0. / 0.)) Raise(TryCatchException_NaN);
}

void UnitTestTryCatch() {

  // --------------
  // Simple example, raise an exception in a TryCatch block and catch it.

  Try {

    if (isnan(0. / 0.)) Raise(TryCatchException_NaN);

  } Catch(TryCatchException_NaN) {

    printf("Catched exception NaN\n");

  } EndTry;

  // Output:
  //
  // Catched exception NaN
  //

  // --------------
  // Example of TryCatch block inside another TryCatch block and exception
  // forwarded from the inner block to the outer block after being ignored
  // by the inner block.

  Try {

    Try {

      if (isnan(0. / 0.)) Raise(TryCatchException_NaN);

    } EndTry;

  } Catch (TryCatchException_NaN) {

    printf("Catched exception NaN at sublevel\n");

  } EndTry;

  // Output:
  //
  // Catched exception NaN at sublevel
  //

  // --------------
  // Example of user defined exception and multiple catch segments.

  enum UserDefinedExceptions {

    myUserExceptionA = TryCatchException_LastID,
    myUserExceptionB,
    myUserExceptionC

  };

  Try {

    Raise(myUserExceptionA);

  } Catch (myUserExceptionA) {

    printf("Catched user defined exception A\n");

  } Catch (myUserExceptionB) {

    printf("Catched user defined exception B\n");

  } Catch (myUserExceptionC) {

    printf("Catched user defined exception C\n");

  } EndTry;

  // Output:
  //
  // Catched user defined exception A
  //

// The struct siginfo_t used to handle the SIGSEV is not defined in
// ANSI C, guard against this.
#ifndef __STRICT_ANSI__

  // --------------
  // Example of handling exception raided by SIGSEV.

  // Init the SIGSEV signal handling by TryCatch.
  TryCatchInitHandlerSigSegv();

  Try {

    int *p = NULL;
    *p = 1;

  } Catch (TryCatchException_Segv) {

    printf("Catched exception Segv\n");

  } EndTry;

  // Output:
  //
  // Catched exception Segv
  //
#endif

  // --------------
  // Example of exception raised in called function and catched in calling
  // function.

  Try {

    fun();

  } Catch (TryCatchException_NaN) {

    printf("Catched exception NaN raised in called function\n");

  } EndTry;

  // Output:
  //
  // Catched exception NaN raised in called function
  //

  // --------------
  // Example of exception raised in called function and uncatched in calling
  // function.

  Try {

    fun();

  } EndTry;

  // Output:
  //
  // Unhandled exception (2).
  //

  // --------------
  // Example of exception raised outside a TryCatch block.

  Raise(TryCatchException_NaN);

  // Output:
  //
  // Unhandled exception (2).
  //

  // --------------
  // Example of overflow of recursive inclusion of TryCatch blocks.

  Try {

    Try {

      Try {

        Try {

          fun();

        } EndTry;

      } EndTry;

    } EndTry;

  } EndTry;

  // Output:
  //
  // TryCatch blocks recursive incursion overflow, exiting. (You can try
  // to raise the value of TryCatchMaxExcLvl in trycatch.c, it was: 3)
  //

  printf("UnitTestTryCatch OK\n");
}

void UnitTestAll() {
  PBErrPrintln(&thePBErr, stdout);
  UnitTestCreateStatic();
  UnitTestReset();
  UnitTestMalloc();
  UnitTestIO();
  UnitTestTryCatch();
  UnitTestCatch();
}

int main(void) {
  UnitTestAll();
  return 0;
}

