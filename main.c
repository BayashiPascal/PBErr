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

void fun(void) {
  double a = 0./0.;
  if (isnan(a)) PBErrRaise(PBErr_Exception_NaN); 
}

void UnitTestTryCatch() {

  // --------------

  PBErrTry:
    if (isnan(0./0.)) PBErrRaise(PBErr_Exception_NaN);

  PBErrCatchExc PBErr_Exception_NaN:
    printf("Catched exception NaN\n");

  PBErrEndTry;

  // --------------

  PBErrTry:

    PBErrTry:
      if (isnan(0./0.)) PBErrRaise(PBErr_Exception_NaN);

    PBErrEndTry;

  PBErrCatchExc PBErr_Exception_NaN:
    printf("Catched exception NaN at sublevel\n");

  PBErrEndTry;

  // --------------

  #define myUserDefinedException (PBErr_Exception_LastID + 1)
  PBErrTry:
    PBErrRaise(myUserDefinedException);

  PBErrCatchExc myUserDefinedException:
    printf("Catched user defined exception\n");

  PBErrEndTry;

  // --------------

  PBErrInitHandlerSigSegv();

  int* p = NULL;

  PBErrTry:
    *p = 1;

  PBErrCatchExc PBErr_Exception_NaN:
    printf("Catched exception NaN\n");

  PBErrCatchExc PBErr_Exception_Segv:
    printf("Catched exception Segv\n");

  PBErrCatchExc myUserDefinedException:
    printf("Catched user defined exception\n");

  PBErrEndTry;

  // --------------

  PBErrTry:
    fun();

  PBErrCatchExc PBErr_Exception_NaN:
    printf("Catched exception NaN in called function\n");

  PBErrEndTry;

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

