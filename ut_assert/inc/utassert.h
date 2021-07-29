/*
 *  NASA Docket No. GSC-18,370-1, and identified as "Operating System Abstraction Layer"
 *
 *  Copyright (c) 2019 United States Government as represented by
 *  the Administrator of the National Aeronautics and Space Administration.
 *  All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**
 * \file
 *
 * Purpose: This code implements a standard set of asserts for use in unit tests.
 *
 * Design Notes:
 *    - All asserts evaluate a expression as true or false to determine if a unit test has
 *      passed or failed.  true means the test passed, false means the test failed.
 *    - All asserts return a boolen result to indicate the pass fail status.
 *    - All asserts are implemented as macros to hide the __LINE__ and __FILE__ macros.
 *    - All asserts must call the function UtAssert.
 */

#ifndef UTASSERT_H
#define UTASSERT_H

/*
 * Includes
 */

#include "common_types.h"
#include "uttools.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

/**
 * Define various types of messages that can be generated by a test.
 *
 * The order is significant for filtering; BSP's may elect to filter
 * messages based on the position within this list.  They should
 * be ordered in terms of relative importance from most to least severe.
 */
typedef enum
{
    UTASSERT_CASETYPE_NONE,    /**< Reserved value, No messages should be used with this */
    UTASSERT_CASETYPE_ABORT,   /**< Test sequence abort (major failure, cannot continue) */
    UTASSERT_CASETYPE_FAILURE, /**< Test case failure status messages */
    UTASSERT_CASETYPE_TSF,     /**< Test Setup Failure (TSF) status messages */
    UTASSERT_CASETYPE_TTF,     /**< Test Teardown Failure (TTF) status messages */
    UTASSERT_CASETYPE_MIR,     /**< Manual Inspection Required (MIR) status messages */
    UTASSERT_CASETYPE_NA,      /**< Test Not Applicable (NA) status messages */
    UTASSERT_CASETYPE_BEGIN,   /**< Beginning of test status messages */
    UTASSERT_CASETYPE_END,     /**< End of test status messages */
    UTASSERT_CASETYPE_INFO,    /**< All other informational status messages */
    UTASSERT_CASETYPE_PASS,    /**< Test case passed (normal) status messages */
    UTASSERT_CASETYPE_DEBUG,   /**< Debugging messages */
    UTASSERT_CASETYPE_MAX      /**< Reserved value, No messages should be used with this */
} UtAssert_CaseType_t;

/**
 * Test Counter object
 * Summarizes counters for all case types
 */
typedef struct
{
    uint32 TestSegmentCount;
    uint32 TotalTestCases;
    uint32 CaseCount[UTASSERT_CASETYPE_MAX];
} UtAssert_TestCounter_t;

/*
 * Macro Definitions
 */

/**
 * \brief A simplified UtAssert that uses the expression itself as the description
 */
#define UtAssert_Simple(Expression) UtAssert(Expression, #Expression, __FILE__, __LINE__)

/**
 * \brief Evaluates a expression as either true or false.
 *
 * true means the test passed, false means the test failed.
 */
#define UtAssert_True(Expression, ...) UtAssertEx(Expression, UtAssert_GetContext(), __FILE__, __LINE__, __VA_ARGS__)

/**
 * \copydoc UtAssert_True
 */
#define UtAssert_Bool(Expression, ...) UtAssertEx(Expression, UtAssert_GetContext(), __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief Asserts a test failure
 */
#define UtAssert_Failed(...) UtAssertEx(false, UtAssert_GetContext(), __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief Assert a test Not Applicable
 */
#define UtAssert_NA(...) UtAssertEx(false, UTASSERT_CASETYPE_NA, __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief Assert a test MIR (Manual Inspection Required)
 */
#define UtAssert_MIR(...) UtAssertEx(false, UTASSERT_CASETYPE_MIR, __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief Compares two integers and determines if they are equal within a specified absolute tolerance.
 */
#define UtAssert_IntegerCmpAbs(x, y, Tolerance, ...) \
    UtAssertEx((abs((x) - (y)) <= (Tolerance)), UtAssert_GetContext(), __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief Compares two floating point numbers and determines if they are equal within a specified absolute tolerance.
 */
#define UtAssert_DoubleCmpAbs(x, y, Tolerance, ...) \
    UtAssertEx((fabs((x) - (y)) <= (Tolerance)), UtAssert_GetContext(), __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief Compares two floating point numbers and determines if they are equal within a specified relative tolerance.
 */
#define UtAssert_DoubleCmpRel(x, y, Ratio, ...) \
    UtAssertEx((fabs((x) - (y)) / (x) <= (Ratio)), UtAssert_GetContext(), __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief  Compares two strings and determines if they are equal.
 */
#define UtAssert_StrCmp(String1, String2, ...) \
    UtAssertEx((strcmp(String1, String2) == 0), UtAssert_GetContext(), __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief Compares at most Length characters of two strings and determines if they are equal.
 */
#define UtAssert_StrnCmp(String1, String2, Length, ...) \
    UtAssertEx((strncmp(String1, String2, Length) == 0), UtAssert_GetContext(), __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief Compares two regions of memory and determines if they are equal.
 */
#define UtAssert_MemCmp(Memory1, Memory2, Length, ...) \
    UtAssertEx((memcmp(Memory1, Memory2, Length) == 0), UtAssert_GetContext(), __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief Compares a region of memory to a static pattern and determines if they are equal.
 *
 * \note Use standard memset() to fill a region of memory with a static pattern.
 */
#define UtAssert_MemCmpValue(Memory, Value, Length, ...) \
    UtAssertEx((UtMemCmpValue(Memory, Value, Length)), UtAssert_GetContext(), __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief Compares a region of memory to a byte count pattern and determines if they are equal.
 *
 * \note Use UtMemFill() to fill a region of memory with a byte count pattern.
 * \sa UtMemFill
 */
#define UtAssert_MemCmpCount(Memory, Length, ...) \
    UtAssertEx((UtMemCmpCount(Memory, Length)), UtAssert_GetContext(), __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief Compares a region of memory with the contents of a binary file and determines if they are equal.
 *
 * \note Use UtMem2BinFile() to copy a region of memory to a binary file.
 * \sa UtMem2BinFile
 */
#define UtAssert_Mem2BinFileCmp(Memory, Filename, ...) \
    UtAssertEx((UtMem2BinFileCmp(Memory, Filename)), UtAssert_GetContext(), __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief A wrapper around UtAssertEx that allows the user to specify the failure type and a more descriptive message
 */
#define UtAssert_Type(Type, Expression, ...) \
    UtAssertEx(Expression, UTASSERT_CASETYPE_##Type, __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief Compare addresses for equality with an auto-generated description message
 */
#define UtAssert_ADDRESS_EQ(actual, expect)                                          \
    do                                                                               \
    {                                                                                \
        void *exp = (void *)(expect);                                                \
        void *act = (void *)(actual);                                                \
        UtAssert_True(act == exp, "%s (%p) == %s (%p)", #actual, act, #expect, exp); \
    } while (0)

/**
 * \brief Compare two values for equality with an auto-generated description message
 * Values will be compared in an "int32" type context.
 */
#define UtAssert_INT32_EQ(actual, expect)                                                                  \
    do                                                                                                     \
    {                                                                                                      \
        int32 rcexp = (int32)(expect);                                                                     \
        int32 rcact = (int32)(actual);                                                                     \
        UtAssert_True(rcact == rcexp, "%s (%ld) == %s (%ld)", #actual, (long)rcact, #expect, (long)rcexp); \
    } while (0)

/**
 * \brief Compare two values for equality with an auto-generated description message
 * Values will be compared in an "uint32" type context.
 */
#define UtAssert_UINT32_EQ(actual, expect)                                                            \
    do                                                                                                \
    {                                                                                                 \
        uint32 rcexp = (uint32)(expect);                                                              \
        uint32 rcact = (uint32)(actual);                                                              \
        UtAssert_True(rcact == rcexp, "%s (%lu) == %s (%lu)", #actual, (unsigned long)rcact, #expect, \
                      (unsigned long)rcexp);                                                          \
    } while (0)

/**
 * \brief Confirm a pointer value is not NULL
 */
#define UtAssert_NOT_NULL(actual)                                    \
    do                                                               \
    {                                                                \
        void *ptr = (void *)(actual);                                \
        UtAssert_True(ptr != NULL, "%s (%p) != NULL", #actual, ptr); \
    } while (0)

/**
 * \brief Confirm a pointer value is NULL
 */
#define UtAssert_NULL(actual)                                        \
    do                                                               \
    {                                                                \
        void *ptr = (void *)(actual);                                \
        UtAssert_True(ptr == NULL, "%s (%p) == NULL", #actual, ptr); \
    } while (0)

/**
 * \brief Confirm an integer value is nonzero
 */
#define UtAssert_NONZERO(actual)                                \
    do                                                          \
    {                                                           \
        long val = (long)(actual);                              \
        UtAssert_True(val != 0, "%s (%ld) != 0", #actual, val); \
    } while (0)

/**
 * \brief Confirm an integer value is zero
 */
#define UtAssert_ZERO(actual)                                   \
    do                                                          \
    {                                                           \
        long val = (long)(actual);                              \
        UtAssert_True(val == 0, "%s (%ld) == 0", #actual, val); \
    } while (0)

/**
 * \brief Confirm that a stub function has been invoked the expected number of times
 */
#define UtAssert_STUB_COUNT(stub, expected)                                                                      \
    do                                                                                                           \
    {                                                                                                            \
        uint32 expval = (uint32)(expected);                                                                      \
        uint32 actval = UT_GetStubCount(UT_KEY(stub));                                                           \
        UtAssert_True(actval == expval, "%s() count (%lu) == %s (%lu)", #stub, (unsigned long)actval, #expected, \
                      (unsigned long)expval);                                                                    \
    } while (0)

/*
 * Exported Functions
 */

/**
 * \brief Returns the number of asserts that have passed.
 *
 * \returns Number of tests that have passed
 */
uint32 UtAssert_GetPassCount(void);

/**
 * \brief Returns the number of asserts that have failed.
 *
 * \returns Number of tests that have failed
 */
uint32 UtAssert_GetFailCount(void);

/**
 * \brief Returns the full status of UtAssert counters
 *
 * \returns Pointer to structure with all the current test counter values
 */
const UtAssert_TestCounter_t *UtAssert_GetCounters(void);

/**
 * \brief Begins a test Segment
 *
 * \param SegmentName the name of the test segment, for logging/output classification
 */
void UtAssert_BeginTest(const char *SegmentName);

/**
 * \brief Completes a test Segment
 *
 * Segment name and counters will be printed to the console
 */
void UtAssert_EndTest(void);

/**
 * \brief Sets the context of the current test, which is the default message type for UtAssert calls
 *
 * General assert statements for which do not directly specify a case type will inherit this case type
 * if an assert fails.  The framework automatically sets this to #UTASSERT_CASETYPE_TSF during setup
 * and #UTASSERT_CASETYPE_TTF during teardown.
 *
 * \sa UtAssert_GetContext
 *
 * \param Context the case type to use for general assert failures
 */
void UtAssert_SetContext(UtAssert_CaseType_t Context);

/**
 * \brief Sets the context of the current test, which is the default message type for UtAssert calls
 *
 * \sa UtAssert_SetContext
 *
 * \returns Case type that was set previously via UtAssert_SetContext
 */
UtAssert_CaseType_t UtAssert_GetContext(void);

/**
 * \brief Gets the segment name of the current test
 *
 * This is the name that was previously set via UtAssert_BeginTest()
 *
 * \note the appliction should not store this pointer, it may become
 * invalid after the next call to UtAssert_EndTest()
 *
 * \returns pointer to current segment name
 */
const char *UtAssert_GetSegmentName(void);

/**
 * \brief Base assert function
 *
 * This basic function is provided for backward compatibility with older
 * versions of Ut Assert.  It generates an assert case using the current
 * default context.
 *
 * Users should preferentially use one of the more type-specific assert macros,
 * if available.
 *
 * \param Expression  a boolean value which evaluates "true" if the test passes
 * \param Description a free form string describing the test case
 * \param File          The source file name in which the test case appears
 * \param Line          The source line number in which the test case appears
 */
bool UtAssert(bool Expression, const char *Description, const char *File, uint32 Line);

/**
 * \brief Assert function with specific CaseType (supports MIR, TSF, NA in addition to FAIL).
 *
 * This assert routine allows more consise description of the test case, as it supports
 * printf-style message strings to allow dynamic content in the messages.
 *
 * \param Expression    a boolean value which evaluates "true" if the test passes
 * \param CaseType      the case type to report if expression is "false" (TSF, TTF, FAIL, DEBUG, etc)
 * \param File          The source file name in which the test case appears
 * \param Line          The source line number in which the test case appears
 * \param MessageFormat a free form printf-style format string describing the test case
 */
bool UtAssertEx(bool Expression, UtAssert_CaseType_t CaseType, const char *File, uint32 Line, const char *MessageFormat,
                ...) OS_PRINTF(5, 6);

/**
 * \brief Aborts the entire test
 *
 * This may be used for fundamental failures, after which going on becomes meaningless or impossible.
 *
 * \param Message a free form string describing the failure
 */
void UtAssert_Abort(const char *Message);

/**
 * \brief Output an informational message to the console/log file
 *
 * Just like the standard printf except it will output to the given status channel (see utassert.h)
 *
 * This calls into the UT BSP function to actually write the message
 * to the current output device.  This may be the console or a log file
 * or something else depending on what BSP is in use.
 *
 * \param MessageType Message case type
 * \param File        File name containing the message
 * \param Line        Line number containing the message
 * \param Spec        printf style format followed by args of message
 *
 * \sa Helper macros: UtPrintf(), UtDebug()
 */
void UtAssert_Message(uint8 MessageType, const char *File, uint32 Line, const char *Spec, ...) OS_PRINTF(4, 5);

/**
 * The BSP single test case reporting function.
 *
 * Invokes the BSP-specific pass/fail reporting mechanism based on the MessageType.
 *
 * This is typically output as a message to the test log but may be fancier if the BSP requires it.
 * One example might be to toggle a GPIO bit or LED if the test is running on a separate processor board.
 *
 * \param File         File containing the test case
 * \param LineNum      Line number containing the test case
 * \param MessageType  Message case type
 * \param SubsysName   The subsystem under test (abbreviated name)
 * \param ShortDesc    Short description of the test case
 * \param SegmentNum   Sequence among the overall/global test Segments
 * \param TestDescr    Sequence within the current test Segment
 */
void UtAssert_DoReport(const char *File, uint32 LineNum, uint32 SegmentNum, uint32 SegmentSeq, uint8 MessageType,
                       const char *SubsysName, const char *ShortDesc);

/**
 * The BSP overall test reporting function.
 *
 * Invokes the BSP-specific overall pass/fail reporting mechanism based the subsystem pass/fail counters.
 *
 * Like the UtAssert_DoReport() function, this is typically done as a message on the console/log however
 * it might be different for embedded targets.
 *
 * \param Appname       The application under test
 * \param TestCounters  Counter object for the completed test
 */
void UtAssert_DoTestSegmentReport(const char *SegmentName, const UtAssert_TestCounter_t *TestCounters);

#endif /* UTASSERT_H */
