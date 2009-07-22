// Copyright 2008, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: mheule@google.com (Markus Heule)
//
// Google C++ Testing Framework (Google Test)
//
// Sometimes it's desirable to build Google Test by compiling a single file.
// This file serves this purpose.
// Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: wan@google.com (Zhanyong Wan)
//
// The Google C++ Testing Framework (Google Test)

#include <gtest/gtest.h>
// Copyright 2007, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: wan@google.com (Zhanyong Wan)
//
// Utilities for testing Google Test itself and code that uses Google Test
// (e.g. frameworks built on top of Google Test).

#ifndef GTEST_INCLUDE_GTEST_GTEST_SPI_H_
#define GTEST_INCLUDE_GTEST_GTEST_SPI_H_


namespace testing {

// This helper class can be used to mock out Google Test failure reporting
// so that we can test Google Test or code that builds on Google Test.
//
// An object of this class appends a TestPartResult object to the
// TestPartResultArray object given in the constructor whenever a Google Test
// failure is reported. It can either intercept only failures that are
// generated in the same thread that created this object or it can intercept
// all generated failures. The scope of this mock object can be controlled with
// the second argument to the two arguments constructor.
class ScopedFakeTestPartResultReporter
    : public TestPartResultReporterInterface {
 public:
  // The two possible mocking modes of this object.
  enum InterceptMode {
    INTERCEPT_ONLY_CURRENT_THREAD,  // Intercepts only thread local failures.
    INTERCEPT_ALL_THREADS           // Intercepts all failures.
  };

  // The c'tor sets this object as the test part result reporter used
  // by Google Test.  The 'result' parameter specifies where to report the
  // results. This reporter will only catch failures generated in the current
  // thread. DEPRECATED
  explicit ScopedFakeTestPartResultReporter(TestPartResultArray* result);

  // Same as above, but you can choose the interception scope of this object.
  ScopedFakeTestPartResultReporter(InterceptMode intercept_mode,
                                   TestPartResultArray* result);

  // The d'tor restores the previous test part result reporter.
  virtual ~ScopedFakeTestPartResultReporter();

  // Appends the TestPartResult object to the TestPartResultArray
  // received in the constructor.
  //
  // This method is from the TestPartResultReporterInterface
  // interface.
  virtual void ReportTestPartResult(const TestPartResult& result);
 private:
  void Init();

  const InterceptMode intercept_mode_;
  TestPartResultReporterInterface* old_reporter_;
  TestPartResultArray* const result_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(ScopedFakeTestPartResultReporter);
};

namespace internal {

// A helper class for implementing EXPECT_FATAL_FAILURE() and
// EXPECT_NONFATAL_FAILURE().  Its destructor verifies that the given
// TestPartResultArray contains exactly one failure that has the given
// type and contains the given substring.  If that's not the case, a
// non-fatal failure will be generated.
class SingleFailureChecker {
 public:
  // The constructor remembers the arguments.
  SingleFailureChecker(const TestPartResultArray* results,
                       TestPartResultType type,
                       const char* substr);
  ~SingleFailureChecker();
 private:
  const TestPartResultArray* const results_;
  const TestPartResultType type_;
  const String substr_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(SingleFailureChecker);
};

}  // namespace internal

}  // namespace testing

// A set of macros for testing Google Test assertions or code that's expected
// to generate Google Test fatal failures.  It verifies that the given
// statement will cause exactly one fatal Google Test failure with 'substr'
// being part of the failure message.
//
// There are two different versions of this macro. EXPECT_FATAL_FAILURE only
// affects and considers failures generated in the current thread and
// EXPECT_FATAL_FAILURE_ON_ALL_THREADS does the same but for all threads.
//
// The verification of the assertion is done correctly even when the statement
// throws an exception or aborts the current function.
//
// Known restrictions:
//   - 'statement' cannot reference local non-static variables or
//     non-static members of the current object.
//   - 'statement' cannot return a value.
//   - You cannot stream a failure message to this macro.
//
// Note that even though the implementations of the following two
// macros are much alike, we cannot refactor them to use a common
// helper macro, due to some peculiarity in how the preprocessor
// works.  The AcceptsMacroThatExpandsToUnprotectedComma test in
// gtest_unittest.cc will fail to compile if we do that.
#define EXPECT_FATAL_FAILURE(statement, substr) \
  do { \
    class GTestExpectFatalFailureHelper {\
     public:\
      static void Execute() { statement; }\
    };\
    ::testing::TestPartResultArray gtest_failures;\
    ::testing::internal::SingleFailureChecker gtest_checker(\
        &gtest_failures, ::testing::TPRT_FATAL_FAILURE, (substr));\
    {\
      ::testing::ScopedFakeTestPartResultReporter gtest_reporter(\
          ::testing::ScopedFakeTestPartResultReporter:: \
          INTERCEPT_ONLY_CURRENT_THREAD, &gtest_failures);\
      GTestExpectFatalFailureHelper::Execute();\
    }\
  } while (false)

#define EXPECT_FATAL_FAILURE_ON_ALL_THREADS(statement, substr) \
  do { \
    class GTestExpectFatalFailureHelper {\
     public:\
      static void Execute() { statement; }\
    };\
    ::testing::TestPartResultArray gtest_failures;\
    ::testing::internal::SingleFailureChecker gtest_checker(\
        &gtest_failures, ::testing::TPRT_FATAL_FAILURE, (substr));\
    {\
      ::testing::ScopedFakeTestPartResultReporter gtest_reporter(\
          ::testing::ScopedFakeTestPartResultReporter:: \
          INTERCEPT_ALL_THREADS, &gtest_failures);\
      GTestExpectFatalFailureHelper::Execute();\
    }\
  } while (false)

// A macro for testing Google Test assertions or code that's expected to
// generate Google Test non-fatal failures.  It asserts that the given
// statement will cause exactly one non-fatal Google Test failure with 'substr'
// being part of the failure message.
//
// There are two different versions of this macro. EXPECT_NONFATAL_FAILURE only
// affects and considers failures generated in the current thread and
// EXPECT_NONFATAL_FAILURE_ON_ALL_THREADS does the same but for all threads.
//
// 'statement' is allowed to reference local variables and members of
// the current object.
//
// The verification of the assertion is done correctly even when the statement
// throws an exception or aborts the current function.
//
// Known restrictions:
//   - You cannot stream a failure message to this macro.
//
// Note that even though the implementations of the following two
// macros are much alike, we cannot refactor them to use a common
// helper macro, due to some peculiarity in how the preprocessor
// works.  The AcceptsMacroThatExpandsToUnprotectedComma test in
// gtest_unittest.cc will fail to compile if we do that.
#define EXPECT_NONFATAL_FAILURE(statement, substr) \
  do {\
    ::testing::TestPartResultArray gtest_failures;\
    ::testing::internal::SingleFailureChecker gtest_checker(\
        &gtest_failures, ::testing::TPRT_NONFATAL_FAILURE, (substr));\
    {\
      ::testing::ScopedFakeTestPartResultReporter gtest_reporter(\
          ::testing::ScopedFakeTestPartResultReporter:: \
          INTERCEPT_ONLY_CURRENT_THREAD, &gtest_failures);\
      statement;\
    }\
  } while (false)

#define EXPECT_NONFATAL_FAILURE_ON_ALL_THREADS(statement, substr) \
  do {\
    ::testing::TestPartResultArray gtest_failures;\
    ::testing::internal::SingleFailureChecker gtest_checker(\
        &gtest_failures, ::testing::TPRT_NONFATAL_FAILURE, (substr));\
    {\
      ::testing::ScopedFakeTestPartResultReporter gtest_reporter(\
          ::testing::ScopedFakeTestPartResultReporter::INTERCEPT_ALL_THREADS,\
          &gtest_failures);\
      statement;\
    }\
  } while (false)

#endif  // GTEST_INCLUDE_GTEST_GTEST_SPI_H_

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#if GTEST_OS_LINUX

// TODO(kenton@google.com): Use autoconf to detect availability of
// gettimeofday().
#define GTEST_HAS_GETTIMEOFDAY_ 1

#include <fcntl.h>
#include <limits.h>
#include <sched.h>
// Declares vsnprintf().  This header is not available on Windows.
#include <strings.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <string>
#include <vector>

#elif GTEST_OS_SYMBIAN
#define GTEST_HAS_GETTIMEOFDAY_ 1
#include <sys/time.h>  // NOLINT

#elif GTEST_OS_ZOS
#define GTEST_HAS_GETTIMEOFDAY_ 1
#include <sys/time.h>  // NOLINT

// On z/OS we additionally need strings.h for strcasecmp.
#include <strings.h>  // NOLINT

#elif defined(_WIN32_WCE)  // We are on Windows CE.

#include <windows.h>  // NOLINT

#elif GTEST_OS_WINDOWS  // We are on Windows proper.

#include <io.h>  // NOLINT
#include <sys/timeb.h>  // NOLINT
#include <sys/types.h>  // NOLINT
#include <sys/stat.h>  // NOLINT

#if defined(__MINGW__) || defined(__MINGW32__)
// MinGW has gettimeofday() but not _ftime64().
// TODO(kenton@google.com): Use autoconf to detect availability of
//   gettimeofday().
// TODO(kenton@google.com): There are other ways to get the time on
//   Windows, like GetTickCount() or GetSystemTimeAsFileTime().  MinGW
//   supports these.  consider using them instead.
#define GTEST_HAS_GETTIMEOFDAY_ 1
#include <sys/time.h>  // NOLINT
#endif  // defined(__MINGW__) || defined(__MINGW32__)

// cpplint thinks that the header is already included, so we want to
// silence it.
#include <windows.h>  // NOLINT

#else

// Assume other platforms have gettimeofday().
// TODO(kenton@google.com): Use autoconf to detect availability of
//   gettimeofday().
#define GTEST_HAS_GETTIMEOFDAY_ 1

// cpplint thinks that the header is already included, so we want to
// silence it.
#include <sys/time.h>  // NOLINT
#include <unistd.h>  // NOLINT

#endif  // GTEST_OS_LINUX

#if GTEST_HAS_EXCEPTIONS
#include <stdexcept>
#endif

// Indicates that this translation unit is part of Google Test's
// implementation.  It must come before gtest-internal-inl.h is
// included, or there will be a compiler error.  This trick is to
// prevent a user from accidentally including gtest-internal-inl.h in
// his code.
#define GTEST_IMPLEMENTATION_ 1
// Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Utility functions and classes used by the Google C++ testing framework.
//
// Author: wan@google.com (Zhanyong Wan)
//
// This file contains purely Google Test's internal implementation.  Please
// DO NOT #INCLUDE IT IN A USER PROGRAM.

#ifndef GTEST_SRC_GTEST_INTERNAL_INL_H_
#define GTEST_SRC_GTEST_INTERNAL_INL_H_

// GTEST_IMPLEMENTATION_ is defined to 1 iff the current translation unit is
// part of Google Test's implementation; otherwise it's undefined.
#if !GTEST_IMPLEMENTATION_
// A user is trying to include this from his code - just say no.
#error "gtest-internal-inl.h is part of Google Test's internal implementation."
#error "It must not be included except by Google Test itself."
#endif  // GTEST_IMPLEMENTATION_

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>   // For strtoll/_strtoul64.

#include <string>


#if GTEST_OS_WINDOWS
#include <windows.h>  // For DWORD.
#endif  // GTEST_OS_WINDOWS


namespace testing {

// Declares the flags.
//
// We don't want the users to modify this flag in the code, but want
// Google Test's own unit tests to be able to access it. Therefore we
// declare it here as opposed to in gtest.h.
GTEST_DECLARE_bool_(death_test_use_fork);

namespace internal {

// The value of GetTestTypeId() as seen from within the Google Test
// library.  This is solely for testing GetTestTypeId().
extern const TypeId kTestTypeIdInGoogleTest;

// Names of the flags (needed for parsing Google Test flags).
const char kAlsoRunDisabledTestsFlag[] = "also_run_disabled_tests";
const char kBreakOnFailureFlag[] = "break_on_failure";
const char kCatchExceptionsFlag[] = "catch_exceptions";
const char kColorFlag[] = "color";
const char kFilterFlag[] = "filter";
const char kListTestsFlag[] = "list_tests";
const char kOutputFlag[] = "output";
const char kPrintTimeFlag[] = "print_time";
const char kRepeatFlag[] = "repeat";
const char kThrowOnFailureFlag[] = "throw_on_failure";

// This class saves the values of all Google Test flags in its c'tor, and
// restores them in its d'tor.
class GTestFlagSaver {
 public:
  // The c'tor.
  GTestFlagSaver() {
    also_run_disabled_tests_ = GTEST_FLAG(also_run_disabled_tests);
    break_on_failure_ = GTEST_FLAG(break_on_failure);
    catch_exceptions_ = GTEST_FLAG(catch_exceptions);
    color_ = GTEST_FLAG(color);
    death_test_style_ = GTEST_FLAG(death_test_style);
    death_test_use_fork_ = GTEST_FLAG(death_test_use_fork);
    filter_ = GTEST_FLAG(filter);
    internal_run_death_test_ = GTEST_FLAG(internal_run_death_test);
    list_tests_ = GTEST_FLAG(list_tests);
    output_ = GTEST_FLAG(output);
    print_time_ = GTEST_FLAG(print_time);
    repeat_ = GTEST_FLAG(repeat);
    throw_on_failure_ = GTEST_FLAG(throw_on_failure);
  }

  // The d'tor is not virtual.  DO NOT INHERIT FROM THIS CLASS.
  ~GTestFlagSaver() {
    GTEST_FLAG(also_run_disabled_tests) = also_run_disabled_tests_;
    GTEST_FLAG(break_on_failure) = break_on_failure_;
    GTEST_FLAG(catch_exceptions) = catch_exceptions_;
    GTEST_FLAG(color) = color_;
    GTEST_FLAG(death_test_style) = death_test_style_;
    GTEST_FLAG(death_test_use_fork) = death_test_use_fork_;
    GTEST_FLAG(filter) = filter_;
    GTEST_FLAG(internal_run_death_test) = internal_run_death_test_;
    GTEST_FLAG(list_tests) = list_tests_;
    GTEST_FLAG(output) = output_;
    GTEST_FLAG(print_time) = print_time_;
    GTEST_FLAG(repeat) = repeat_;
    GTEST_FLAG(throw_on_failure) = throw_on_failure_;
  }
 private:
  // Fields for saving the original values of flags.
  bool also_run_disabled_tests_;
  bool break_on_failure_;
  bool catch_exceptions_;
  String color_;
  String death_test_style_;
  bool death_test_use_fork_;
  String filter_;
  String internal_run_death_test_;
  bool list_tests_;
  String output_;
  bool print_time_;
  bool pretty_;
  internal::Int32 repeat_;
  bool throw_on_failure_;
} GTEST_ATTRIBUTE_UNUSED_;

// Converts a Unicode code point to a narrow string in UTF-8 encoding.
// code_point parameter is of type UInt32 because wchar_t may not be
// wide enough to contain a code point.
// The output buffer str must containt at least 32 characters.
// The function returns the address of the output buffer.
// If the code_point is not a valid Unicode code point
// (i.e. outside of Unicode range U+0 to U+10FFFF) it will be output
// as '(Invalid Unicode 0xXXXXXXXX)'.
char* CodePointToUtf8(UInt32 code_point, char* str);

// Converts a wide string to a narrow string in UTF-8 encoding.
// The wide string is assumed to have the following encoding:
//   UTF-16 if sizeof(wchar_t) == 2 (on Windows, Cygwin, Symbian OS)
//   UTF-32 if sizeof(wchar_t) == 4 (on Linux)
// Parameter str points to a null-terminated wide string.
// Parameter num_chars may additionally limit the number
// of wchar_t characters processed. -1 is used when the entire string
// should be processed.
// If the string contains code points that are not valid Unicode code points
// (i.e. outside of Unicode range U+0 to U+10FFFF) they will be output
// as '(Invalid Unicode 0xXXXXXXXX)'. If the string is in UTF16 encoding
// and contains invalid UTF-16 surrogate pairs, values in those pairs
// will be encoded as individual Unicode characters from Basic Normal Plane.
String WideStringToUtf8(const wchar_t* str, int num_chars);

// Returns the number of active threads, or 0 when there is an error.
size_t GetThreadCount();

// Reads the GTEST_SHARD_STATUS_FILE environment variable, and creates the file
// if the variable is present. If a file already exists at this location, this
// function will write over it. If the variable is present, but the file cannot
// be created, prints an error and exits.
void WriteToShardStatusFileIfNeeded();

// Checks whether sharding is enabled by examining the relevant
// environment variable values. If the variables are present,
// but inconsistent (e.g., shard_index >= total_shards), prints
// an error and exits. If in_subprocess_for_death_test, sharding is
// disabled because it must only be applied to the original test
// process. Otherwise, we could filter out death tests we intended to execute.
bool ShouldShard(const char* total_shards_str, const char* shard_index_str,
                 bool in_subprocess_for_death_test);

// Parses the environment variable var as an Int32. If it is unset,
// returns default_val. If it is not an Int32, prints an error and
// and aborts.
Int32 Int32FromEnvOrDie(const char* env_var, Int32 default_val);

// Given the total number of shards, the shard index, and the test id,
// returns true iff the test should be run on this shard. The test id is
// some arbitrary but unique non-negative integer assigned to each test
// method. Assumes that 0 <= shard_index < total_shards.
bool ShouldRunTestOnShard(int total_shards, int shard_index, int test_id);

// List is a simple singly-linked list container.
//
// We cannot use std::list as Microsoft's implementation of STL has
// problems when exception is disabled.  There is a hack to work
// around this, but we've seen cases where the hack fails to work.
//
// TODO(wan): switch to std::list when we have a reliable fix for the
// STL problem, e.g. when we upgrade to the next version of Visual
// C++, or (more likely) switch to STLport.
//
// The element type must support copy constructor.

// Forward declare List
template <typename E>  // E is the element type.
class List;

// ListNode is a node in a singly-linked list.  It consists of an
// element and a pointer to the next node.  The last node in the list
// has a NULL value for its next pointer.
template <typename E>  // E is the element type.
class ListNode {
  friend class List<E>;

 private:

  E element_;
  ListNode * next_;

  // The c'tor is private s.t. only in the ListNode class and in its
  // friend class List we can create a ListNode object.
  //
  // Creates a node with a given element value.  The next pointer is
  // set to NULL.
  //
  // ListNode does NOT have a default constructor.  Always use this
  // constructor (with parameter) to create a ListNode object.
  explicit ListNode(const E & element) : element_(element), next_(NULL) {}

  // We disallow copying ListNode
  GTEST_DISALLOW_COPY_AND_ASSIGN_(ListNode);

 public:

  // Gets the element in this node.
  E & element() { return element_; }
  const E & element() const { return element_; }

  // Gets the next node in the list.
  ListNode * next() { return next_; }
  const ListNode * next() const { return next_; }
};


// List is a simple singly-linked list container.
template <typename E>  // E is the element type.
class List {
 public:

  // Creates an empty list.
  List() : head_(NULL), last_(NULL), size_(0) {}

  // D'tor.
  virtual ~List();

  // Clears the list.
  void Clear() {
    if ( size_ > 0 ) {
      // 1. Deletes every node.
      ListNode<E> * node = head_;
      ListNode<E> * next = node->next();
      for ( ; ; ) {
        delete node;
        node = next;
        if ( node == NULL ) break;
        next = node->next();
      }

      // 2. Resets the member variables.
      head_ = last_ = NULL;
      size_ = 0;
    }
  }

  // Gets the number of elements.
  int size() const { return size_; }

  // Returns true if the list is empty.
  bool IsEmpty() const { return size() == 0; }

  // Gets the first element of the list, or NULL if the list is empty.
  ListNode<E> * Head() { return head_; }
  const ListNode<E> * Head() const { return head_; }

  // Gets the last element of the list, or NULL if the list is empty.
  ListNode<E> * Last() { return last_; }
  const ListNode<E> * Last() const { return last_; }

  // Adds an element to the end of the list.  A copy of the element is
  // created using the copy constructor, and then stored in the list.
  // Changes made to the element in the list doesn't affect the source
  // object, and vice versa.
  void PushBack(const E & element) {
    ListNode<E> * new_node = new ListNode<E>(element);

    if ( size_ == 0 ) {
      head_ = last_ = new_node;
      size_ = 1;
    } else {
      last_->next_ = new_node;
      last_ = new_node;
      size_++;
    }
  }

  // Adds an element to the beginning of this list.
  void PushFront(const E& element) {
    ListNode<E>* const new_node = new ListNode<E>(element);

    if ( size_ == 0 ) {
      head_ = last_ = new_node;
      size_ = 1;
    } else {
      new_node->next_ = head_;
      head_ = new_node;
      size_++;
    }
  }

  // Removes an element from the beginning of this list.  If the
  // result argument is not NULL, the removed element is stored in the
  // memory it points to.  Otherwise the element is thrown away.
  // Returns true iff the list wasn't empty before the operation.
  bool PopFront(E* result) {
    if (size_ == 0) return false;

    if (result != NULL) {
      *result = head_->element_;
    }

    ListNode<E>* const old_head = head_;
    size_--;
    if (size_ == 0) {
      head_ = last_ = NULL;
    } else {
      head_ = head_->next_;
    }
    delete old_head;

    return true;
  }

  // Inserts an element after a given node in the list.  It's the
  // caller's responsibility to ensure that the given node is in the
  // list.  If the given node is NULL, inserts the element at the
  // front of the list.
  ListNode<E>* InsertAfter(ListNode<E>* node, const E& element) {
    if (node == NULL) {
      PushFront(element);
      return Head();
    }

    ListNode<E>* const new_node = new ListNode<E>(element);
    new_node->next_ = node->next_;
    node->next_ = new_node;
    size_++;
    if (node == last_) {
      last_ = new_node;
    }

    return new_node;
  }

  // Returns the number of elements that satisfy a given predicate.
  // The parameter 'predicate' is a Boolean function or functor that
  // accepts a 'const E &', where E is the element type.
  template <typename P>  // P is the type of the predicate function/functor
  int CountIf(P predicate) const {
    int count = 0;
    for ( const ListNode<E> * node = Head();
          node != NULL;
          node = node->next() ) {
      if ( predicate(node->element()) ) {
        count++;
      }
    }

    return count;
  }

  // Applies a function/functor to each element in the list.  The
  // parameter 'functor' is a function/functor that accepts a 'const
  // E &', where E is the element type.  This method does not change
  // the elements.
  template <typename F>  // F is the type of the function/functor
  void ForEach(F functor) const {
    for ( const ListNode<E> * node = Head();
          node != NULL;
          node = node->next() ) {
      functor(node->element());
    }
  }

  // Returns the first node whose element satisfies a given predicate,
  // or NULL if none is found.  The parameter 'predicate' is a
  // function/functor that accepts a 'const E &', where E is the
  // element type.  This method does not change the elements.
  template <typename P>  // P is the type of the predicate function/functor.
  const ListNode<E> * FindIf(P predicate) const {
    for ( const ListNode<E> * node = Head();
          node != NULL;
          node = node->next() ) {
      if ( predicate(node->element()) ) {
        return node;
      }
    }

    return NULL;
  }

  template <typename P>
  ListNode<E> * FindIf(P predicate) {
    for ( ListNode<E> * node = Head();
          node != NULL;
          node = node->next() ) {
      if ( predicate(node->element() ) ) {
        return node;
      }
    }

    return NULL;
  }

 private:
  ListNode<E>* head_;  // The first node of the list.
  ListNode<E>* last_;  // The last node of the list.
  int size_;           // The number of elements in the list.

  // We disallow copying List.
  GTEST_DISALLOW_COPY_AND_ASSIGN_(List);
};

// The virtual destructor of List.
template <typename E>
List<E>::~List() {
  Clear();
}

// A function for deleting an object.  Handy for being used as a
// functor.
template <typename T>
static void Delete(T * x) {
  delete x;
}

// A copyable object representing a user specified test property which can be
// output as a key/value string pair.
//
// Don't inherit from TestProperty as its destructor is not virtual.
class TestProperty {
 public:
  // C'tor.  TestProperty does NOT have a default constructor.
  // Always use this constructor (with parameters) to create a
  // TestProperty object.
  TestProperty(const char* key, const char* value) :
    key_(key), value_(value) {
  }

  // Gets the user supplied key.
  const char* key() const {
    return key_.c_str();
  }

  // Gets the user supplied value.
  const char* value() const {
    return value_.c_str();
  }

  // Sets a new value, overriding the one supplied in the constructor.
  void SetValue(const char* new_value) {
    value_ = new_value;
  }

 private:
  // The key supplied by the user.
  String key_;
  // The value supplied by the user.
  String value_;
};

// A predicate that checks the key of a TestProperty against a known key.
//
// TestPropertyKeyIs is copyable.
class TestPropertyKeyIs {
 public:
  // Constructor.
  //
  // TestPropertyKeyIs has NO default constructor.
  explicit TestPropertyKeyIs(const char* key)
      : key_(key) {}

  // Returns true iff the test name of test property matches on key_.
  bool operator()(const TestProperty& test_property) const {
    return String(test_property.key()).Compare(key_) == 0;
  }

 private:
  String key_;
};

// The result of a single Test.  This includes a list of
// TestPartResults, a list of TestProperties, a count of how many
// death tests there are in the Test, and how much time it took to run
// the Test.
//
// TestResult is not copyable.
class TestResult {
 public:
  // Creates an empty TestResult.
  TestResult();

  // D'tor.  Do not inherit from TestResult.
  ~TestResult();

  // Gets the list of TestPartResults.
  const internal::List<TestPartResult> & test_part_results() const {
    return test_part_results_;
  }

  // Gets the list of TestProperties.
  const internal::List<internal::TestProperty> & test_properties() const {
    return test_properties_;
  }

  // Gets the number of successful test parts.
  int successful_part_count() const;

  // Gets the number of failed test parts.
  int failed_part_count() const;

  // Gets the number of all test parts.  This is the sum of the number
  // of successful test parts and the number of failed test parts.
  int total_part_count() const;

  // Returns true iff the test passed (i.e. no test part failed).
  bool Passed() const { return !Failed(); }

  // Returns true iff the test failed.
  bool Failed() const { return failed_part_count() > 0; }

  // Returns true iff the test fatally failed.
  bool HasFatalFailure() const;

  // Returns the elapsed time, in milliseconds.
  TimeInMillis elapsed_time() const { return elapsed_time_; }

  // Sets the elapsed time.
  void set_elapsed_time(TimeInMillis elapsed) { elapsed_time_ = elapsed; }

  // Adds a test part result to the list.
  void AddTestPartResult(const TestPartResult& test_part_result);

  // Adds a test property to the list. The property is validated and may add
  // a non-fatal failure if invalid (e.g., if it conflicts with reserved
  // key names). If a property is already recorded for the same key, the
  // value will be updated, rather than storing multiple values for the same
  // key.
  void RecordProperty(const internal::TestProperty& test_property);

  // Adds a failure if the key is a reserved attribute of Google Test
  // testcase tags.  Returns true if the property is valid.
  // TODO(russr): Validate attribute names are legal and human readable.
  static bool ValidateTestProperty(const internal::TestProperty& test_property);

  // Returns the death test count.
  int death_test_count() const { return death_test_count_; }

  // Increments the death test count, returning the new count.
  int increment_death_test_count() { return ++death_test_count_; }

  // Clears the object.
  void Clear();
 private:
  // Protects mutable state of the property list and of owned properties, whose
  // values may be updated.
  internal::Mutex test_properites_mutex_;

  // The list of TestPartResults
  internal::List<TestPartResult> test_part_results_;
  // The list of TestProperties
  internal::List<internal::TestProperty> test_properties_;
  // Running count of death tests.
  int death_test_count_;
  // The elapsed time, in milliseconds.
  TimeInMillis elapsed_time_;

  // We disallow copying TestResult.
  GTEST_DISALLOW_COPY_AND_ASSIGN_(TestResult);
};  // class TestResult

class TestInfoImpl {
 public:
  TestInfoImpl(TestInfo* parent, const char* test_case_name,
               const char* name, const char* test_case_comment,
               const char* comment, TypeId fixture_class_id,
               internal::TestFactoryBase* factory);
  ~TestInfoImpl();

  // Returns true if this test should run.
  bool should_run() const { return should_run_; }

  // Sets the should_run member.
  void set_should_run(bool should) { should_run_ = should; }

  // Returns true if this test is disabled. Disabled tests are not run.
  bool is_disabled() const { return is_disabled_; }

  // Sets the is_disabled member.
  void set_is_disabled(bool is) { is_disabled_ = is; }

  // Returns the test case name.
  const char* test_case_name() const { return test_case_name_.c_str(); }

  // Returns the test name.
  const char* name() const { return name_.c_str(); }

  // Returns the test case comment.
  const char* test_case_comment() const { return test_case_comment_.c_str(); }

  // Returns the test comment.
  const char* comment() const { return comment_.c_str(); }

  // Returns the ID of the test fixture class.
  TypeId fixture_class_id() const { return fixture_class_id_; }

  // Returns the test result.
  internal::TestResult* result() { return &result_; }
  const internal::TestResult* result() const { return &result_; }

  // Creates the test object, runs it, records its result, and then
  // deletes it.
  void Run();

  // Calls the given TestInfo object's Run() method.
  static void RunTest(TestInfo * test_info) {
    test_info->impl()->Run();
  }

  // Clears the test result.
  void ClearResult() { result_.Clear(); }

  // Clears the test result in the given TestInfo object.
  static void ClearTestResult(TestInfo * test_info) {
    test_info->impl()->ClearResult();
  }

 private:
  // These fields are immutable properties of the test.
  TestInfo* const parent_;          // The owner of this object
  const String test_case_name_;     // Test case name
  const String name_;               // Test name
  const String test_case_comment_;  // Test case comment
  const String comment_;            // Test comment
  const TypeId fixture_class_id_;   // ID of the test fixture class
  bool should_run_;                 // True iff this test should run
  bool is_disabled_;                // True iff this test is disabled
  internal::TestFactoryBase* const factory_;  // The factory that creates
                                              // the test object

  // This field is mutable and needs to be reset before running the
  // test for the second time.
  internal::TestResult result_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(TestInfoImpl);
};

}  // namespace internal

// A test case, which consists of a list of TestInfos.
//
// TestCase is not copyable.
class TestCase {
 public:
  // Creates a TestCase with the given name.
  //
  // TestCase does NOT have a default constructor.  Always use this
  // constructor to create a TestCase object.
  //
  // Arguments:
  //
  //   name:         name of the test case
  //   set_up_tc:    pointer to the function that sets up the test case
  //   tear_down_tc: pointer to the function that tears down the test case
  TestCase(const char* name, const char* comment,
           Test::SetUpTestCaseFunc set_up_tc,
           Test::TearDownTestCaseFunc tear_down_tc);

  // Destructor of TestCase.
  virtual ~TestCase();

  // Gets the name of the TestCase.
  const char* name() const { return name_.c_str(); }

  // Returns the test case comment.
  const char* comment() const { return comment_.c_str(); }

  // Returns true if any test in this test case should run.
  bool should_run() const { return should_run_; }

  // Sets the should_run member.
  void set_should_run(bool should) { should_run_ = should; }

  // Gets the (mutable) list of TestInfos in this TestCase.
  internal::List<TestInfo*>& test_info_list() { return *test_info_list_; }

  // Gets the (immutable) list of TestInfos in this TestCase.
  const internal::List<TestInfo *> & test_info_list() const {
    return *test_info_list_;
  }

  // Gets the number of successful tests in this test case.
  int successful_test_count() const;

  // Gets the number of failed tests in this test case.
  int failed_test_count() const;

  // Gets the number of disabled tests in this test case.
  int disabled_test_count() const;

  // Get the number of tests in this test case that should run.
  int test_to_run_count() const;

  // Gets the number of all tests in this test case.
  int total_test_count() const;

  // Returns true iff the test case passed.
  bool Passed() const { return !Failed(); }

  // Returns true iff the test case failed.
  bool Failed() const { return failed_test_count() > 0; }

  // Returns the elapsed time, in milliseconds.
  internal::TimeInMillis elapsed_time() const { return elapsed_time_; }

  // Adds a TestInfo to this test case.  Will delete the TestInfo upon
  // destruction of the TestCase object.
  void AddTestInfo(TestInfo * test_info);

  // Finds and returns a TestInfo with the given name.  If one doesn't
  // exist, returns NULL.
  TestInfo* GetTestInfo(const char* test_name);

  // Clears the results of all tests in this test case.
  void ClearResult();

  // Clears the results of all tests in the given test case.
  static void ClearTestCaseResult(TestCase* test_case) {
    test_case->ClearResult();
  }

  // Runs every test in this TestCase.
  void Run();

  // Runs every test in the given TestCase.
  static void RunTestCase(TestCase * test_case) { test_case->Run(); }

  // Returns true iff test passed.
  static bool TestPassed(const TestInfo * test_info) {
    const internal::TestInfoImpl* const impl = test_info->impl();
    return impl->should_run() && impl->result()->Passed();
  }

  // Returns true iff test failed.
  static bool TestFailed(const TestInfo * test_info) {
    const internal::TestInfoImpl* const impl = test_info->impl();
    return impl->should_run() && impl->result()->Failed();
  }

  // Returns true iff test is disabled.
  static bool TestDisabled(const TestInfo * test_info) {
    return test_info->impl()->is_disabled();
  }

  // Returns true if the given test should run.
  static bool ShouldRunTest(const TestInfo *test_info) {
    return test_info->impl()->should_run();
  }

 private:
  // Name of the test case.
  internal::String name_;
  // Comment on the test case.
  internal::String comment_;
  // List of TestInfos.
  internal::List<TestInfo*>* test_info_list_;
  // Pointer to the function that sets up the test case.
  Test::SetUpTestCaseFunc set_up_tc_;
  // Pointer to the function that tears down the test case.
  Test::TearDownTestCaseFunc tear_down_tc_;
  // True iff any test in this test case should run.
  bool should_run_;
  // Elapsed time, in milliseconds.
  internal::TimeInMillis elapsed_time_;

  // We disallow copying TestCases.
  GTEST_DISALLOW_COPY_AND_ASSIGN_(TestCase);
};

namespace internal {

// Class UnitTestOptions.
//
// This class contains functions for processing options the user
// specifies when running the tests.  It has only static members.
//
// In most cases, the user can specify an option using either an
// environment variable or a command line flag.  E.g. you can set the
// test filter using either GTEST_FILTER or --gtest_filter.  If both
// the variable and the flag are present, the latter overrides the
// former.
class UnitTestOptions {
 public:
  // Functions for processing the gtest_output flag.

  // Returns the output format, or "" for normal printed output.
  static String GetOutputFormat();

  // Returns the absolute path of the requested output file, or the
  // default (test_detail.xml in the original working directory) if
  // none was explicitly specified.
  static String GetAbsolutePathToOutputFile();

  // Functions for processing the gtest_filter flag.

  // Returns true iff the wildcard pattern matches the string.  The
  // first ':' or '\0' character in pattern marks the end of it.
  //
  // This recursive algorithm isn't very efficient, but is clear and
  // works well enough for matching test names, which are short.
  static bool PatternMatchesString(const char *pattern, const char *str);

  // Returns true iff the user-specified filter matches the test case
  // name and the test name.
  static bool FilterMatchesTest(const String &test_case_name,
                                const String &test_name);

#if GTEST_OS_WINDOWS
  // Function for supporting the gtest_catch_exception flag.

  // Returns EXCEPTION_EXECUTE_HANDLER if Google Test should handle the
  // given SEH exception, or EXCEPTION_CONTINUE_SEARCH otherwise.
  // This function is useful as an __except condition.
  static int GTestShouldProcessSEH(DWORD exception_code);
#endif  // GTEST_OS_WINDOWS

  // Returns true if "name" matches the ':' separated list of glob-style
  // filters in "filter".
  static bool MatchesFilter(const String& name, const char* filter);
};

// Returns the current application's name, removing directory path if that
// is present.  Used by UnitTestOptions::GetOutputFile.
FilePath GetCurrentExecutableName();

// The role interface for getting the OS stack trace as a string.
class OsStackTraceGetterInterface {
 public:
  OsStackTraceGetterInterface() {}
  virtual ~OsStackTraceGetterInterface() {}

  // Returns the current OS stack trace as a String.  Parameters:
  //
  //   max_depth  - the maximum number of stack frames to be included
  //                in the trace.
  //   skip_count - the number of top frames to be skipped; doesn't count
  //                against max_depth.
  virtual String CurrentStackTrace(int max_depth, int skip_count) = 0;

  // UponLeavingGTest() should be called immediately before Google Test calls
  // user code. It saves some information about the current stack that
  // CurrentStackTrace() will use to find and hide Google Test stack frames.
  virtual void UponLeavingGTest() = 0;

 private:
  GTEST_DISALLOW_COPY_AND_ASSIGN_(OsStackTraceGetterInterface);
};

// A working implementation of the OsStackTraceGetterInterface interface.
class OsStackTraceGetter : public OsStackTraceGetterInterface {
 public:
  OsStackTraceGetter() {}
  virtual String CurrentStackTrace(int max_depth, int skip_count);
  virtual void UponLeavingGTest();

  // This string is inserted in place of stack frames that are part of
  // Google Test's implementation.
  static const char* const kElidedFramesMarker;

 private:
  Mutex mutex_;  // protects all internal state

  // We save the stack frame below the frame that calls user code.
  // We do this because the address of the frame immediately below
  // the user code changes between the call to UponLeavingGTest()
  // and any calls to CurrentStackTrace() from within the user code.
  void* caller_frame_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(OsStackTraceGetter);
};

// Information about a Google Test trace point.
struct TraceInfo {
  const char* file;
  int line;
  String message;
};

// This is the default global test part result reporter used in UnitTestImpl.
// This class should only be used by UnitTestImpl.
class DefaultGlobalTestPartResultReporter
  : public TestPartResultReporterInterface {
 public:
  explicit DefaultGlobalTestPartResultReporter(UnitTestImpl* unit_test);
  // Implements the TestPartResultReporterInterface. Reports the test part
  // result in the current test.
  virtual void ReportTestPartResult(const TestPartResult& result);

 private:
  UnitTestImpl* const unit_test_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(DefaultGlobalTestPartResultReporter);
};

// This is the default per thread test part result reporter used in
// UnitTestImpl. This class should only be used by UnitTestImpl.
class DefaultPerThreadTestPartResultReporter
    : public TestPartResultReporterInterface {
 public:
  explicit DefaultPerThreadTestPartResultReporter(UnitTestImpl* unit_test);
  // Implements the TestPartResultReporterInterface. The implementation just
  // delegates to the current global test part result reporter of *unit_test_.
  virtual void ReportTestPartResult(const TestPartResult& result);

 private:
  UnitTestImpl* const unit_test_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(DefaultPerThreadTestPartResultReporter);
};

// The private implementation of the UnitTest class.  We don't protect
// the methods under a mutex, as this class is not accessible by a
// user and the UnitTest class that delegates work to this class does
// proper locking.
class UnitTestImpl {
 public:
  explicit UnitTestImpl(UnitTest* parent);
  virtual ~UnitTestImpl();

  // There are two different ways to register your own TestPartResultReporter.
  // You can register your own repoter to listen either only for test results
  // from the current thread or for results from all threads.
  // By default, each per-thread test result repoter just passes a new
  // TestPartResult to the global test result reporter, which registers the
  // test part result for the currently running test.

  // Returns the global test part result reporter.
  TestPartResultReporterInterface* GetGlobalTestPartResultReporter();

  // Sets the global test part result reporter.
  void SetGlobalTestPartResultReporter(
      TestPartResultReporterInterface* reporter);

  // Returns the test part result reporter for the current thread.
  TestPartResultReporterInterface* GetTestPartResultReporterForCurrentThread();

  // Sets the test part result reporter for the current thread.
  void SetTestPartResultReporterForCurrentThread(
      TestPartResultReporterInterface* reporter);

  // Gets the number of successful test cases.
  int successful_test_case_count() const;

  // Gets the number of failed test cases.
  int failed_test_case_count() const;

  // Gets the number of all test cases.
  int total_test_case_count() const;

  // Gets the number of all test cases that contain at least one test
  // that should run.
  int test_case_to_run_count() const;

  // Gets the number of successful tests.
  int successful_test_count() const;

  // Gets the number of failed tests.
  int failed_test_count() const;

  // Gets the number of disabled tests.
  int disabled_test_count() const;

  // Gets the number of all tests.
  int total_test_count() const;

  // Gets the number of tests that should run.
  int test_to_run_count() const;

  // Gets the elapsed time, in milliseconds.
  TimeInMillis elapsed_time() const { return elapsed_time_; }

  // Returns true iff the unit test passed (i.e. all test cases passed).
  bool Passed() const { return !Failed(); }

  // Returns true iff the unit test failed (i.e. some test case failed
  // or something outside of all tests failed).
  bool Failed() const {
    return failed_test_case_count() > 0 || ad_hoc_test_result()->Failed();
  }

  // Returns the TestResult for the test that's currently running, or
  // the TestResult for the ad hoc test if no test is running.
  internal::TestResult* current_test_result();

  // Returns the TestResult for the ad hoc test.
  const internal::TestResult* ad_hoc_test_result() const {
    return &ad_hoc_test_result_;
  }

  // Sets the unit test result printer.
  //
  // Does nothing if the input and the current printer object are the
  // same; otherwise, deletes the old printer object and makes the
  // input the current printer.
  void set_result_printer(UnitTestEventListenerInterface * result_printer);

  // Returns the current unit test result printer if it is not NULL;
  // otherwise, creates an appropriate result printer, makes it the
  // current printer, and returns it.
  UnitTestEventListenerInterface* result_printer();

  // Sets the OS stack trace getter.
  //
  // Does nothing if the input and the current OS stack trace getter
  // are the same; otherwise, deletes the old getter and makes the
  // input the current getter.
  void set_os_stack_trace_getter(OsStackTraceGetterInterface* getter);

  // Returns the current OS stack trace getter if it is not NULL;
  // otherwise, creates an OsStackTraceGetter, makes it the current
  // getter, and returns it.
  OsStackTraceGetterInterface* os_stack_trace_getter();

  // Returns the current OS stack trace as a String.
  //
  // The maximum number of stack frames to be included is specified by
  // the gtest_stack_trace_depth flag.  The skip_count parameter
  // specifies the number of top frames to be skipped, which doesn't
  // count against the number of frames to be included.
  //
  // For example, if Foo() calls Bar(), which in turn calls
  // CurrentOsStackTraceExceptTop(1), Foo() will be included in the
  // trace but Bar() and CurrentOsStackTraceExceptTop() won't.
  String CurrentOsStackTraceExceptTop(int skip_count);

  // Finds and returns a TestCase with the given name.  If one doesn't
  // exist, creates one and returns it.
  //
  // Arguments:
  //
  //   test_case_name: name of the test case
  //   set_up_tc:      pointer to the function that sets up the test case
  //   tear_down_tc:   pointer to the function that tears down the test case
  TestCase* GetTestCase(const char* test_case_name,
                        const char* comment,
                        Test::SetUpTestCaseFunc set_up_tc,
                        Test::TearDownTestCaseFunc tear_down_tc);

  // Adds a TestInfo to the unit test.
  //
  // Arguments:
  //
  //   set_up_tc:    pointer to the function that sets up the test case
  //   tear_down_tc: pointer to the function that tears down the test case
  //   test_info:    the TestInfo object
  void AddTestInfo(Test::SetUpTestCaseFunc set_up_tc,
                   Test::TearDownTestCaseFunc tear_down_tc,
                   TestInfo * test_info) {
    // In order to support thread-safe death tests, we need to
    // remember the original working directory when the test program
    // was first invoked.  We cannot do this in RUN_ALL_TESTS(), as
    // the user may have changed the current directory before calling
    // RUN_ALL_TESTS().  Therefore we capture the current directory in
    // AddTestInfo(), which is called to register a TEST or TEST_F
    // before main() is reached.
    if (original_working_dir_.IsEmpty()) {
      original_working_dir_.Set(FilePath::GetCurrentDir());
      if (original_working_dir_.IsEmpty()) {
        printf("%s\n", "Failed to get the current working directory.");
        abort();
      }
    }

    GetTestCase(test_info->test_case_name(),
                test_info->test_case_comment(),
                set_up_tc,
                tear_down_tc)->AddTestInfo(test_info);
  }

#if GTEST_HAS_PARAM_TEST
  // Returns ParameterizedTestCaseRegistry object used to keep track of
  // value-parameterized tests and instantiate and register them.
  internal::ParameterizedTestCaseRegistry& parameterized_test_registry() {
    return parameterized_test_registry_;
  }
#endif  // GTEST_HAS_PARAM_TEST

  // Sets the TestCase object for the test that's currently running.
  void set_current_test_case(TestCase* current_test_case) {
    current_test_case_ = current_test_case;
  }

  // Sets the TestInfo object for the test that's currently running.  If
  // current_test_info is NULL, the assertion results will be stored in
  // ad_hoc_test_result_.
  void set_current_test_info(TestInfo* current_test_info) {
    current_test_info_ = current_test_info;
  }

  // Registers all parameterized tests defined using TEST_P and
  // INSTANTIATE_TEST_P, creating regular tests for each test/parameter
  // combination. This method can be called more then once; it has
  // guards protecting from registering the tests more then once.
  // If value-parameterized tests are disabled, RegisterParameterizedTests
  // is present but does nothing.
  void RegisterParameterizedTests();

  // Runs all tests in this UnitTest object, prints the result, and
  // returns 0 if all tests are successful, or 1 otherwise.  If any
  // exception is thrown during a test on Windows, this test is
  // considered to be failed, but the rest of the tests will still be
  // run.  (We disable exceptions on Linux and Mac OS X, so the issue
  // doesn't apply there.)
  int RunAllTests();

  // Clears the results of all tests, including the ad hoc test.
  void ClearResult() {
    test_cases_.ForEach(TestCase::ClearTestCaseResult);
    ad_hoc_test_result_.Clear();
  }

  enum ReactionToSharding {
    HONOR_SHARDING_PROTOCOL,
    IGNORE_SHARDING_PROTOCOL
  };

  // Matches the full name of each test against the user-specified
  // filter to decide whether the test should run, then records the
  // result in each TestCase and TestInfo object.
  // If shard_tests == HONOR_SHARDING_PROTOCOL, further filters tests
  // based on sharding variables in the environment.
  // Returns the number of tests that should run.
  int FilterTests(ReactionToSharding shard_tests);

  // Lists all the tests by name.
  void ListAllTests();

  const TestCase* current_test_case() const { return current_test_case_; }
  TestInfo* current_test_info() { return current_test_info_; }
  const TestInfo* current_test_info() const { return current_test_info_; }

  // Returns the list of environments that need to be set-up/torn-down
  // before/after the tests are run.
  internal::List<Environment*>* environments() { return &environments_; }
  internal::List<Environment*>* environments_in_reverse_order() {
    return &environments_in_reverse_order_;
  }

  internal::List<TestCase*>* test_cases() { return &test_cases_; }
  const internal::List<TestCase*>* test_cases() const { return &test_cases_; }

  // Getters for the per-thread Google Test trace stack.
  internal::List<TraceInfo>* gtest_trace_stack() {
    return gtest_trace_stack_.pointer();
  }
  const internal::List<TraceInfo>* gtest_trace_stack() const {
    return gtest_trace_stack_.pointer();
  }

#if GTEST_HAS_DEATH_TEST
  // Returns a pointer to the parsed --gtest_internal_run_death_test
  // flag, or NULL if that flag was not specified.
  // This information is useful only in a death test child process.
  const InternalRunDeathTestFlag* internal_run_death_test_flag() const {
    return internal_run_death_test_flag_.get();
  }

  // Returns a pointer to the current death test factory.
  internal::DeathTestFactory* death_test_factory() {
    return death_test_factory_.get();
  }

  friend class ReplaceDeathTestFactory;
#endif  // GTEST_HAS_DEATH_TEST

 private:
  friend class ::testing::UnitTest;

  // The UnitTest object that owns this implementation object.
  UnitTest* const parent_;

  // The working directory when the first TEST() or TEST_F() was
  // executed.
  internal::FilePath original_working_dir_;

  // The default test part result reporters.
  DefaultGlobalTestPartResultReporter default_global_test_part_result_reporter_;
  DefaultPerThreadTestPartResultReporter
      default_per_thread_test_part_result_reporter_;

  // Points to (but doesn't own) the global test part result reporter.
  TestPartResultReporterInterface* global_test_part_result_repoter_;

  // Protects read and write access to global_test_part_result_reporter_.
  internal::Mutex global_test_part_result_reporter_mutex_;

  // Points to (but doesn't own) the per-thread test part result reporter.
  internal::ThreadLocal<TestPartResultReporterInterface*>
      per_thread_test_part_result_reporter_;

  // The list of environments that need to be set-up/torn-down
  // before/after the tests are run.  environments_in_reverse_order_
  // simply mirrors environments_ in reverse order.
  internal::List<Environment*> environments_;
  internal::List<Environment*> environments_in_reverse_order_;

  internal::List<TestCase*> test_cases_;  // The list of TestCases.

#if GTEST_HAS_PARAM_TEST
  // ParameterizedTestRegistry object used to register value-parameterized
  // tests.
  internal::ParameterizedTestCaseRegistry parameterized_test_registry_;

  // Indicates whether RegisterParameterizedTests() has been called already.
  bool parameterized_tests_registered_;
#endif  // GTEST_HAS_PARAM_TEST

  // Points to the last death test case registered.  Initially NULL.
  internal::ListNode<TestCase*>* last_death_test_case_;

  // This points to the TestCase for the currently running test.  It
  // changes as Google Test goes through one test case after another.
  // When no test is running, this is set to NULL and Google Test
  // stores assertion results in ad_hoc_test_result_.  Initally NULL.
  TestCase* current_test_case_;

  // This points to the TestInfo for the currently running test.  It
  // changes as Google Test goes through one test after another.  When
  // no test is running, this is set to NULL and Google Test stores
  // assertion results in ad_hoc_test_result_.  Initially NULL.
  TestInfo* current_test_info_;

  // Normally, a user only writes assertions inside a TEST or TEST_F,
  // or inside a function called by a TEST or TEST_F.  Since Google
  // Test keeps track of which test is current running, it can
  // associate such an assertion with the test it belongs to.
  //
  // If an assertion is encountered when no TEST or TEST_F is running,
  // Google Test attributes the assertion result to an imaginary "ad hoc"
  // test, and records the result in ad_hoc_test_result_.
  internal::TestResult ad_hoc_test_result_;

  // The unit test result printer.  Will be deleted when the UnitTest
  // object is destructed.  By default, a plain text printer is used,
  // but the user can set this field to use a custom printer if that
  // is desired.
  UnitTestEventListenerInterface* result_printer_;

  // The OS stack trace getter.  Will be deleted when the UnitTest
  // object is destructed.  By default, an OsStackTraceGetter is used,
  // but the user can set this field to use a custom getter if that is
  // desired.
  OsStackTraceGetterInterface* os_stack_trace_getter_;

  // How long the test took to run, in milliseconds.
  TimeInMillis elapsed_time_;

#if GTEST_HAS_DEATH_TEST
  // The decomposed components of the gtest_internal_run_death_test flag,
  // parsed when RUN_ALL_TESTS is called.
  internal::scoped_ptr<InternalRunDeathTestFlag> internal_run_death_test_flag_;
  internal::scoped_ptr<internal::DeathTestFactory> death_test_factory_;
#endif  // GTEST_HAS_DEATH_TEST

  // A per-thread stack of traces created by the SCOPED_TRACE() macro.
  internal::ThreadLocal<internal::List<TraceInfo> > gtest_trace_stack_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(UnitTestImpl);
};  // class UnitTestImpl

// Convenience function for accessing the global UnitTest
// implementation object.
inline UnitTestImpl* GetUnitTestImpl() {
  return UnitTest::GetInstance()->impl();
}

// Internal helper functions for implementing the simple regular
// expression matcher.
bool IsInSet(char ch, const char* str);
bool IsDigit(char ch);
bool IsPunct(char ch);
bool IsRepeat(char ch);
bool IsWhiteSpace(char ch);
bool IsWordChar(char ch);
bool IsValidEscape(char ch);
bool AtomMatchesChar(bool escaped, char pattern, char ch);
bool ValidateRegex(const char* regex);
bool MatchRegexAtHead(const char* regex, const char* str);
bool MatchRepetitionAndRegexAtHead(
    bool escaped, char ch, char repeat, const char* regex, const char* str);
bool MatchRegexAnywhere(const char* regex, const char* str);

// Parses the command line for Google Test flags, without initializing
// other parts of Google Test.
void ParseGoogleTestFlagsOnly(int* argc, char** argv);
void ParseGoogleTestFlagsOnly(int* argc, wchar_t** argv);

#if GTEST_HAS_DEATH_TEST

// Returns the message describing the last system error, regardless of the
// platform.
String GetLastSystemErrorMessage();

#if GTEST_OS_WINDOWS
// Provides leak-safe Windows kernel handle ownership.
class AutoHandle {
 public:
  AutoHandle() : handle_(INVALID_HANDLE_VALUE) {}
  explicit AutoHandle(HANDLE handle) : handle_(handle) {}

  ~AutoHandle() { Reset(); }

  HANDLE Get() const { return handle_; }
  void Reset() { Reset(INVALID_HANDLE_VALUE); }
  void Reset(HANDLE handle) {
    if (handle != handle_) {
      if (handle_ != INVALID_HANDLE_VALUE)
        ::CloseHandle(handle_);
      handle_ = handle;
    }
  }

 private:
  HANDLE handle_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(AutoHandle);
};
#endif  // GTEST_OS_WINDOWS

// Attempts to parse a string into a positive integer pointed to by the
// number parameter.  Returns true if that is possible.
// GTEST_HAS_DEATH_TEST implies that we have ::std::string, so we can use
// it here.
template <typename Integer>
bool ParseNaturalNumber(const ::std::string& str, Integer* number) {
  // Fail fast if the given string does not begin with a digit;
  // this bypasses strtoXXX's "optional leading whitespace and plus
  // or minus sign" semantics, which are undesirable here.
  if (str.empty() || !isdigit(str[0])) {
    return false;
  }
  errno = 0;

  char* end;
  // BiggestConvertible is the largest integer type that system-provided
  // string-to-number conversion routines can return.
#if GTEST_OS_WINDOWS
  typedef unsigned __int64 BiggestConvertible;
  const BiggestConvertible parsed = _strtoui64(str.c_str(), &end, 10);
#else
  typedef unsigned long long BiggestConvertible;  // NOLINT
  const BiggestConvertible parsed = strtoull(str.c_str(), &end, 10);
#endif  // GTEST_OS_WINDOWS
  const bool parse_success = *end == '\0' && errno == 0;

  // TODO(vladl@google.com): Convert this to compile time assertion when it is
  // available.
  GTEST_CHECK_(sizeof(Integer) <= sizeof(parsed));

  const Integer result = static_cast<Integer>(parsed);
  if (parse_success && static_cast<BiggestConvertible>(result) == parsed) {
    *number = result;
    return true;
  }
  return false;
}
#endif  // GTEST_HAS_DEATH_TEST

}  // namespace internal
}  // namespace testing

#endif  // GTEST_SRC_GTEST_INTERNAL_INL_H_
#undef GTEST_IMPLEMENTATION_

#if GTEST_OS_WINDOWS
#define fileno _fileno
#define isatty _isatty
#define vsnprintf _vsnprintf
#endif  // GTEST_OS_WINDOWS

namespace testing {

// Constants.

// A test whose test case name or test name matches this filter is
// disabled and not run.
static const char kDisableTestFilter[] = "DISABLED_*:*/DISABLED_*";

// A test case whose name matches this filter is considered a death
// test case and will be run before test cases whose name doesn't
// match this filter.
static const char kDeathTestCaseFilter[] = "*DeathTest:*DeathTest/*";

// A test filter that matches everything.
static const char kUniversalFilter[] = "*";

// The default output file for XML output.
static const char kDefaultOutputFile[] = "test_detail.xml";

// The environment variable name for the test shard index.
static const char kTestShardIndex[] = "GTEST_SHARD_INDEX";
// The environment variable name for the total number of test shards.
static const char kTestTotalShards[] = "GTEST_TOTAL_SHARDS";
// The environment variable name for the test shard status file.
static const char kTestShardStatusFile[] = "GTEST_SHARD_STATUS_FILE";

namespace internal {

// The text used in failure messages to indicate the start of the
// stack trace.
const char kStackTraceMarker[] = "\nStack trace:\n";

}  // namespace internal

GTEST_DEFINE_bool_(
    also_run_disabled_tests,
    internal::BoolFromGTestEnv("also_run_disabled_tests", false),
    "Run disabled tests too, in addition to the tests normally being run.");

GTEST_DEFINE_bool_(
    break_on_failure,
    internal::BoolFromGTestEnv("break_on_failure", false),
    "True iff a failed assertion should be a debugger break-point.");

GTEST_DEFINE_bool_(
    catch_exceptions,
    internal::BoolFromGTestEnv("catch_exceptions", false),
    "True iff " GTEST_NAME_
    " should catch exceptions and treat them as test failures.");

GTEST_DEFINE_string_(
    color,
    internal::StringFromGTestEnv("color", "auto"),
    "Whether to use colors in the output.  Valid values: yes, no, "
    "and auto.  'auto' means to use colors if the output is "
    "being sent to a terminal and the TERM environment variable "
    "is set to xterm or xterm-color.");

GTEST_DEFINE_string_(
    filter,
    internal::StringFromGTestEnv("filter", kUniversalFilter),
    "A colon-separated list of glob (not regex) patterns "
    "for filtering the tests to run, optionally followed by a "
    "'-' and a : separated list of negative patterns (tests to "
    "exclude).  A test is run if it matches one of the positive "
    "patterns and does not match any of the negative patterns.");

GTEST_DEFINE_bool_(list_tests, false,
                   "List all tests without running them.");

GTEST_DEFINE_string_(
    output,
    internal::StringFromGTestEnv("output", ""),
    "A format (currently must be \"xml\"), optionally followed "
    "by a colon and an output file name or directory. A directory "
    "is indicated by a trailing pathname separator. "
    "Examples: \"xml:filename.xml\", \"xml::directoryname/\". "
    "If a directory is specified, output files will be created "
    "within that directory, with file-names based on the test "
    "executable's name and, if necessary, made unique by adding "
    "digits.");

GTEST_DEFINE_bool_(
    print_time,
    internal::BoolFromGTestEnv("print_time", false),
    "True iff " GTEST_NAME_
    " should display elapsed time in text output.");

GTEST_DEFINE_int32_(
    repeat,
    internal::Int32FromGTestEnv("repeat", 1),
    "How many times to repeat each test.  Specify a negative number "
    "for repeating forever.  Useful for shaking out flaky tests.");

GTEST_DEFINE_int32_(
    stack_trace_depth,
        internal::Int32FromGTestEnv("stack_trace_depth", kMaxStackTraceDepth),
    "The maximum number of stack frames to print when an "
    "assertion fails.  The valid range is 0 through 100, inclusive.");

GTEST_DEFINE_bool_(
    show_internal_stack_frames, false,
    "True iff " GTEST_NAME_ " should include internal stack frames when "
    "printing test failure stack traces.");

GTEST_DEFINE_bool_(
    throw_on_failure,
    internal::BoolFromGTestEnv("throw_on_failure", false),
    "When this flag is specified, a failed assertion will throw an exception "
    "if exceptions are enabled or exit the program with a non-zero code "
    "otherwise.");

namespace internal {

// g_help_flag is true iff the --help flag or an equivalent form is
// specified on the command line.
static bool g_help_flag = false;

// GTestIsInitialized() returns true iff the user has initialized
// Google Test.  Useful for catching the user mistake of not initializing
// Google Test before calling RUN_ALL_TESTS().
//
// A user must call testing::InitGoogleTest() to initialize Google
// Test.  g_init_gtest_count is set to the number of times
// InitGoogleTest() has been called.  We don't protect this variable
// under a mutex as it is only accessed in the main thread.
int g_init_gtest_count = 0;
static bool GTestIsInitialized() { return g_init_gtest_count != 0; }

// Iterates over a list of TestCases, keeping a running sum of the
// results of calling a given int-returning method on each.
// Returns the sum.
static int SumOverTestCaseList(const internal::List<TestCase*>& case_list,
                               int (TestCase::*method)() const) {
  int sum = 0;
  for (const internal::ListNode<TestCase*>* node = case_list.Head();
       node != NULL;
       node = node->next()) {
    sum += (node->element()->*method)();
  }
  return sum;
}

// Returns true iff the test case passed.
static bool TestCasePassed(const TestCase* test_case) {
  return test_case->should_run() && test_case->Passed();
}

// Returns true iff the test case failed.
static bool TestCaseFailed(const TestCase* test_case) {
  return test_case->should_run() && test_case->Failed();
}

// Returns true iff test_case contains at least one test that should
// run.
static bool ShouldRunTestCase(const TestCase* test_case) {
  return test_case->should_run();
}

// AssertHelper constructor.
AssertHelper::AssertHelper(TestPartResultType type, const char* file,
                           int line, const char* message)
    : type_(type), file_(file), line_(line), message_(message) {
}

// Message assignment, for assertion streaming support.
void AssertHelper::operator=(const Message& message) const {
  UnitTest::GetInstance()->
    AddTestPartResult(type_, file_, line_,
                      AppendUserMessage(message_, message),
                      UnitTest::GetInstance()->impl()
                      ->CurrentOsStackTraceExceptTop(1)
                      // Skips the stack frame for this function itself.
                      );  // NOLINT
}

// Mutex for linked pointers.
Mutex g_linked_ptr_mutex(Mutex::NO_CONSTRUCTOR_NEEDED_FOR_STATIC_MUTEX);

// Application pathname gotten in InitGoogleTest.
String g_executable_path;

// Returns the current application's name, removing directory path if that
// is present.
FilePath GetCurrentExecutableName() {
  FilePath result;

#if defined(_WIN32_WCE) || GTEST_OS_WINDOWS
  result.Set(FilePath(g_executable_path).RemoveExtension("exe"));
#else
  result.Set(FilePath(g_executable_path));
#endif  // _WIN32_WCE || GTEST_OS_WINDOWS

  return result.RemoveDirectoryName();
}

// Functions for processing the gtest_output flag.

// Returns the output format, or "" for normal printed output.
String UnitTestOptions::GetOutputFormat() {
  const char* const gtest_output_flag = GTEST_FLAG(output).c_str();
  if (gtest_output_flag == NULL) return String("");

  const char* const colon = strchr(gtest_output_flag, ':');
  return (colon == NULL) ?
      String(gtest_output_flag) :
      String(gtest_output_flag, colon - gtest_output_flag);
}

// Returns the name of the requested output file, or the default if none
// was explicitly specified.
String UnitTestOptions::GetAbsolutePathToOutputFile() {
  const char* const gtest_output_flag = GTEST_FLAG(output).c_str();
  if (gtest_output_flag == NULL)
    return String("");

  const char* const colon = strchr(gtest_output_flag, ':');
  if (colon == NULL)
    return String(internal::FilePath::ConcatPaths(
               internal::FilePath(
                   UnitTest::GetInstance()->original_working_dir()),
               internal::FilePath(kDefaultOutputFile)).ToString() );

  internal::FilePath output_name(colon + 1);
  if (!output_name.IsAbsolutePath())
    // TODO(wan@google.com): on Windows \some\path is not an absolute
    // path (as its meaning depends on the current drive), yet the
    // following logic for turning it into an absolute path is wrong.
    // Fix it.
    output_name = internal::FilePath::ConcatPaths(
        internal::FilePath(UnitTest::GetInstance()->original_working_dir()),
        internal::FilePath(colon + 1));

  if (!output_name.IsDirectory())
    return output_name.ToString();

  internal::FilePath result(internal::FilePath::GenerateUniqueFileName(
      output_name, internal::GetCurrentExecutableName(),
      GetOutputFormat().c_str()));
  return result.ToString();
}

// Returns true iff the wildcard pattern matches the string.  The
// first ':' or '\0' character in pattern marks the end of it.
//
// This recursive algorithm isn't very efficient, but is clear and
// works well enough for matching test names, which are short.
bool UnitTestOptions::PatternMatchesString(const char *pattern,
                                           const char *str) {
  switch (*pattern) {
    case '\0':
    case ':':  // Either ':' or '\0' marks the end of the pattern.
      return *str == '\0';
    case '?':  // Matches any single character.
      return *str != '\0' && PatternMatchesString(pattern + 1, str + 1);
    case '*':  // Matches any string (possibly empty) of characters.
      return (*str != '\0' && PatternMatchesString(pattern, str + 1)) ||
          PatternMatchesString(pattern + 1, str);
    default:  // Non-special character.  Matches itself.
      return *pattern == *str &&
          PatternMatchesString(pattern + 1, str + 1);
  }
}

bool UnitTestOptions::MatchesFilter(const String& name, const char* filter) {
  const char *cur_pattern = filter;
  for (;;) {
    if (PatternMatchesString(cur_pattern, name.c_str())) {
      return true;
    }

    // Finds the next pattern in the filter.
    cur_pattern = strchr(cur_pattern, ':');

    // Returns if no more pattern can be found.
    if (cur_pattern == NULL) {
      return false;
    }

    // Skips the pattern separater (the ':' character).
    cur_pattern++;
  }
}

// TODO(keithray): move String function implementations to gtest-string.cc.

// Returns true iff the user-specified filter matches the test case
// name and the test name.
bool UnitTestOptions::FilterMatchesTest(const String &test_case_name,
                                        const String &test_name) {
  const String& full_name = String::Format("%s.%s",
                                           test_case_name.c_str(),
                                           test_name.c_str());

  // Split --gtest_filter at '-', if there is one, to separate into
  // positive filter and negative filter portions
  const char* const p = GTEST_FLAG(filter).c_str();
  const char* const dash = strchr(p, '-');
  String positive;
  String negative;
  if (dash == NULL) {
    positive = GTEST_FLAG(filter).c_str();  // Whole string is a positive filter
    negative = String("");
  } else {
    positive.Set(p, dash - p);       // Everything up to the dash
    negative = String(dash+1);       // Everything after the dash
    if (positive.empty()) {
      // Treat '-test1' as the same as '*-test1'
      positive = kUniversalFilter;
    }
  }

  // A filter is a colon-separated list of patterns.  It matches a
  // test if any pattern in it matches the test.
  return (MatchesFilter(full_name, positive.c_str()) &&
          !MatchesFilter(full_name, negative.c_str()));
}

#if GTEST_OS_WINDOWS
// Returns EXCEPTION_EXECUTE_HANDLER if Google Test should handle the
// given SEH exception, or EXCEPTION_CONTINUE_SEARCH otherwise.
// This function is useful as an __except condition.
int UnitTestOptions::GTestShouldProcessSEH(DWORD exception_code) {
  // Google Test should handle an exception if:
  //   1. the user wants it to, AND
  //   2. this is not a breakpoint exception.
  return (GTEST_FLAG(catch_exceptions) &&
          exception_code != EXCEPTION_BREAKPOINT) ?
      EXCEPTION_EXECUTE_HANDLER :
      EXCEPTION_CONTINUE_SEARCH;
}
#endif  // GTEST_OS_WINDOWS

}  // namespace internal

// The interface for printing the result of a UnitTest
class UnitTestEventListenerInterface {
 public:
  // The d'tor is pure virtual as this is an abstract class.
  virtual ~UnitTestEventListenerInterface() = 0;

  // Called before the unit test starts.
  virtual void OnUnitTestStart(const UnitTest*) {}

  // Called after the unit test ends.
  virtual void OnUnitTestEnd(const UnitTest*) {}

  // Called before the test case starts.
  virtual void OnTestCaseStart(const TestCase*) {}

  // Called after the test case ends.
  virtual void OnTestCaseEnd(const TestCase*) {}

  // Called before the global set-up starts.
  virtual void OnGlobalSetUpStart(const UnitTest*) {}

  // Called after the global set-up ends.
  virtual void OnGlobalSetUpEnd(const UnitTest*) {}

  // Called before the global tear-down starts.
  virtual void OnGlobalTearDownStart(const UnitTest*) {}

  // Called after the global tear-down ends.
  virtual void OnGlobalTearDownEnd(const UnitTest*) {}

  // Called before the test starts.
  virtual void OnTestStart(const TestInfo*) {}

  // Called after the test ends.
  virtual void OnTestEnd(const TestInfo*) {}

  // Called after an assertion.
  virtual void OnNewTestPartResult(const TestPartResult*) {}
};

// The c'tor sets this object as the test part result reporter used by
// Google Test.  The 'result' parameter specifies where to report the
// results. Intercepts only failures from the current thread.
ScopedFakeTestPartResultReporter::ScopedFakeTestPartResultReporter(
    TestPartResultArray* result)
    : intercept_mode_(INTERCEPT_ONLY_CURRENT_THREAD),
      result_(result) {
  Init();
}

// The c'tor sets this object as the test part result reporter used by
// Google Test.  The 'result' parameter specifies where to report the
// results.
ScopedFakeTestPartResultReporter::ScopedFakeTestPartResultReporter(
    InterceptMode intercept_mode, TestPartResultArray* result)
    : intercept_mode_(intercept_mode),
      result_(result) {
  Init();
}

void ScopedFakeTestPartResultReporter::Init() {
  internal::UnitTestImpl* const impl = UnitTest::GetInstance()->impl();
  if (intercept_mode_ == INTERCEPT_ALL_THREADS) {
    old_reporter_ = impl->GetGlobalTestPartResultReporter();
    impl->SetGlobalTestPartResultReporter(this);
  } else {
    old_reporter_ = impl->GetTestPartResultReporterForCurrentThread();
    impl->SetTestPartResultReporterForCurrentThread(this);
  }
}

// The d'tor restores the test part result reporter used by Google Test
// before.
ScopedFakeTestPartResultReporter::~ScopedFakeTestPartResultReporter() {
  internal::UnitTestImpl* const impl = UnitTest::GetInstance()->impl();
  if (intercept_mode_ == INTERCEPT_ALL_THREADS) {
    impl->SetGlobalTestPartResultReporter(old_reporter_);
  } else {
    impl->SetTestPartResultReporterForCurrentThread(old_reporter_);
  }
}

// Increments the test part result count and remembers the result.
// This method is from the TestPartResultReporterInterface interface.
void ScopedFakeTestPartResultReporter::ReportTestPartResult(
    const TestPartResult& result) {
  result_->Append(result);
}

namespace internal {

// Returns the type ID of ::testing::Test.  We should always call this
// instead of GetTypeId< ::testing::Test>() to get the type ID of
// testing::Test.  This is to work around a suspected linker bug when
// using Google Test as a framework on Mac OS X.  The bug causes
// GetTypeId< ::testing::Test>() to return different values depending
// on whether the call is from the Google Test framework itself or
// from user test code.  GetTestTypeId() is guaranteed to always
// return the same value, as it always calls GetTypeId<>() from the
// gtest.cc, which is within the Google Test framework.
TypeId GetTestTypeId() {
  return GetTypeId<Test>();
}

// The value of GetTestTypeId() as seen from within the Google Test
// library.  This is solely for testing GetTestTypeId().
extern const TypeId kTestTypeIdInGoogleTest = GetTestTypeId();

// This predicate-formatter checks that 'results' contains a test part
// failure of the given type and that the failure message contains the
// given substring.
AssertionResult HasOneFailure(const char* /* results_expr */,
                              const char* /* type_expr */,
                              const char* /* substr_expr */,
                              const TestPartResultArray& results,
                              TestPartResultType type,
                              const char* substr) {
  const String expected(
      type == TPRT_FATAL_FAILURE ? "1 fatal failure" :
      "1 non-fatal failure");
  Message msg;
  if (results.size() != 1) {
    msg << "Expected: " << expected << "\n"
        << "  Actual: " << results.size() << " failures";
    for (int i = 0; i < results.size(); i++) {
      msg << "\n" << results.GetTestPartResult(i);
    }
    return AssertionFailure(msg);
  }

  const TestPartResult& r = results.GetTestPartResult(0);
  if (r.type() != type) {
    msg << "Expected: " << expected << "\n"
        << "  Actual:\n"
        << r;
    return AssertionFailure(msg);
  }

  if (strstr(r.message(), substr) == NULL) {
    msg << "Expected: " << expected << " containing \""
        << substr << "\"\n"
        << "  Actual:\n"
        << r;
    return AssertionFailure(msg);
  }

  return AssertionSuccess();
}

// The constructor of SingleFailureChecker remembers where to look up
// test part results, what type of failure we expect, and what
// substring the failure message should contain.
SingleFailureChecker:: SingleFailureChecker(
    const TestPartResultArray* results,
    TestPartResultType type,
    const char* substr)
    : results_(results),
      type_(type),
      substr_(substr) {}

// The destructor of SingleFailureChecker verifies that the given
// TestPartResultArray contains exactly one failure that has the given
// type and contains the given substring.  If that's not the case, a
// non-fatal failure will be generated.
SingleFailureChecker::~SingleFailureChecker() {
  EXPECT_PRED_FORMAT3(HasOneFailure, *results_, type_, substr_.c_str());
}

DefaultGlobalTestPartResultReporter::DefaultGlobalTestPartResultReporter(
    UnitTestImpl* unit_test) : unit_test_(unit_test) {}

void DefaultGlobalTestPartResultReporter::ReportTestPartResult(
    const TestPartResult& result) {
  unit_test_->current_test_result()->AddTestPartResult(result);
  unit_test_->result_printer()->OnNewTestPartResult(&result);
}

DefaultPerThreadTestPartResultReporter::DefaultPerThreadTestPartResultReporter(
    UnitTestImpl* unit_test) : unit_test_(unit_test) {}

void DefaultPerThreadTestPartResultReporter::ReportTestPartResult(
    const TestPartResult& result) {
  unit_test_->GetGlobalTestPartResultReporter()->ReportTestPartResult(result);
}

// Returns the global test part result reporter.
TestPartResultReporterInterface*
UnitTestImpl::GetGlobalTestPartResultReporter() {
  internal::MutexLock lock(&global_test_part_result_reporter_mutex_);
  return global_test_part_result_repoter_;
}

// Sets the global test part result reporter.
void UnitTestImpl::SetGlobalTestPartResultReporter(
    TestPartResultReporterInterface* reporter) {
  internal::MutexLock lock(&global_test_part_result_reporter_mutex_);
  global_test_part_result_repoter_ = reporter;
}

// Returns the test part result reporter for the current thread.
TestPartResultReporterInterface*
UnitTestImpl::GetTestPartResultReporterForCurrentThread() {
  return per_thread_test_part_result_reporter_.get();
}

// Sets the test part result reporter for the current thread.
void UnitTestImpl::SetTestPartResultReporterForCurrentThread(
    TestPartResultReporterInterface* reporter) {
  per_thread_test_part_result_reporter_.set(reporter);
}

// Gets the number of successful test cases.
int UnitTestImpl::successful_test_case_count() const {
  return test_cases_.CountIf(TestCasePassed);
}

// Gets the number of failed test cases.
int UnitTestImpl::failed_test_case_count() const {
  return test_cases_.CountIf(TestCaseFailed);
}

// Gets the number of all test cases.
int UnitTestImpl::total_test_case_count() const {
  return test_cases_.size();
}

// Gets the number of all test cases that contain at least one test
// that should run.
int UnitTestImpl::test_case_to_run_count() const {
  return test_cases_.CountIf(ShouldRunTestCase);
}

// Gets the number of successful tests.
int UnitTestImpl::successful_test_count() const {
  return SumOverTestCaseList(test_cases_, &TestCase::successful_test_count);
}

// Gets the number of failed tests.
int UnitTestImpl::failed_test_count() const {
  return SumOverTestCaseList(test_cases_, &TestCase::failed_test_count);
}

// Gets the number of disabled tests.
int UnitTestImpl::disabled_test_count() const {
  return SumOverTestCaseList(test_cases_, &TestCase::disabled_test_count);
}

// Gets the number of all tests.
int UnitTestImpl::total_test_count() const {
  return SumOverTestCaseList(test_cases_, &TestCase::total_test_count);
}

// Gets the number of tests that should run.
int UnitTestImpl::test_to_run_count() const {
  return SumOverTestCaseList(test_cases_, &TestCase::test_to_run_count);
}

// Returns the current OS stack trace as a String.
//
// The maximum number of stack frames to be included is specified by
// the gtest_stack_trace_depth flag.  The skip_count parameter
// specifies the number of top frames to be skipped, which doesn't
// count against the number of frames to be included.
//
// For example, if Foo() calls Bar(), which in turn calls
// CurrentOsStackTraceExceptTop(1), Foo() will be included in the
// trace but Bar() and CurrentOsStackTraceExceptTop() won't.
String UnitTestImpl::CurrentOsStackTraceExceptTop(int skip_count) {
  (void)skip_count;
  return String("");
}

static TimeInMillis GetTimeInMillis() {
#ifdef _WIN32_WCE  // We are on Windows CE
  // Difference between 1970-01-01 and 1601-01-01 in miliseconds.
  // http://analogous.blogspot.com/2005/04/epoch.html
  const TimeInMillis kJavaEpochToWinFileTimeDelta = 11644473600000UL;
  const DWORD kTenthMicrosInMilliSecond = 10000;

  SYSTEMTIME now_systime;
  FILETIME now_filetime;
  ULARGE_INTEGER now_int64;
  // TODO(kenton@google.com): Shouldn't this just use
  //   GetSystemTimeAsFileTime()?
  GetSystemTime(&now_systime);
  if (SystemTimeToFileTime(&now_systime, &now_filetime)) {
    now_int64.LowPart = now_filetime.dwLowDateTime;
    now_int64.HighPart = now_filetime.dwHighDateTime;
    now_int64.QuadPart = (now_int64.QuadPart / kTenthMicrosInMilliSecond) -
      kJavaEpochToWinFileTimeDelta;
    return now_int64.QuadPart;
  }
  return 0;
#elif GTEST_OS_WINDOWS && !GTEST_HAS_GETTIMEOFDAY_
  __timeb64 now;
#ifdef _MSC_VER
  // MSVC 8 deprecates _ftime64(), so we want to suppress warning 4996
  // (deprecated function) there.
  // TODO(kenton@google.com): Use GetTickCount()?  Or use
  //   SystemTimeToFileTime()
#pragma warning(push)          // Saves the current warning state.
#pragma warning(disable:4996)  // Temporarily disables warning 4996.
  _ftime64(&now);
#pragma warning(pop)           // Restores the warning state.
#else
  _ftime64(&now);
#endif  // _MSC_VER
  return static_cast<TimeInMillis>(now.time) * 1000 + now.millitm;
#elif GTEST_HAS_GETTIMEOFDAY_
  struct timeval now;
  gettimeofday(&now, NULL);
  return static_cast<TimeInMillis>(now.tv_sec) * 1000 + now.tv_usec / 1000;
#else
#error "Don't know how to get the current time on your system."
#endif
}

// Utilities

// class String

// Returns the input enclosed in double quotes if it's not NULL;
// otherwise returns "(null)".  For example, "\"Hello\"" is returned
// for input "Hello".
//
// This is useful for printing a C string in the syntax of a literal.
//
// Known issue: escape sequences are not handled yet.
String String::ShowCStringQuoted(const char* c_str) {
  return c_str ? String::Format("\"%s\"", c_str) : String("(null)");
}

// Copies at most length characters from str into a newly-allocated
// piece of memory of size length+1.  The memory is allocated with new[].
// A terminating null byte is written to the memory, and a pointer to it
// is returned.  If str is NULL, NULL is returned.
static char* CloneString(const char* str, size_t length) {
  if (str == NULL) {
    return NULL;
  } else {
    char* const clone = new char[length + 1];
    // MSVC 8 deprecates strncpy(), so we want to suppress warning
    // 4996 (deprecated function) there.
#if GTEST_OS_WINDOWS  // We are on Windows.
#pragma warning(push)          // Saves the current warning state.
#pragma warning(disable:4996)  // Temporarily disables warning 4996.
    strncpy(clone, str, length);
#pragma warning(pop)           // Restores the warning state.
#else  // We are on Linux or Mac OS.
    strncpy(clone, str, length);
#endif  // GTEST_OS_WINDOWS
    clone[length] = '\0';
    return clone;
  }
}

// Clones a 0-terminated C string, allocating memory using new.  The
// caller is responsible for deleting[] the return value.  Returns the
// cloned string, or NULL if the input is NULL.
const char * String::CloneCString(const char* c_str) {
  return (c_str == NULL) ?
                    NULL : CloneString(c_str, strlen(c_str));
}

#ifdef _WIN32_WCE
// Creates a UTF-16 wide string from the given ANSI string, allocating
// memory using new. The caller is responsible for deleting the return
// value using delete[]. Returns the wide string, or NULL if the
// input is NULL.
LPCWSTR String::AnsiToUtf16(const char* ansi) {
  if (!ansi) return NULL;
  const int length = strlen(ansi);
  const int unicode_length =
      MultiByteToWideChar(CP_ACP, 0, ansi, length,
                          NULL, 0);
  WCHAR* unicode = new WCHAR[unicode_length + 1];
  MultiByteToWideChar(CP_ACP, 0, ansi, length,
                      unicode, unicode_length);
  unicode[unicode_length] = 0;
  return unicode;
}

// Creates an ANSI string from the given wide string, allocating
// memory using new. The caller is responsible for deleting the return
// value using delete[]. Returns the ANSI string, or NULL if the
// input is NULL.
const char* String::Utf16ToAnsi(LPCWSTR utf16_str)  {
  if (!utf16_str) return NULL;
  const int ansi_length =
      WideCharToMultiByte(CP_ACP, 0, utf16_str, -1,
                          NULL, 0, NULL, NULL);
  char* ansi = new char[ansi_length + 1];
  WideCharToMultiByte(CP_ACP, 0, utf16_str, -1,
                      ansi, ansi_length, NULL, NULL);
  ansi[ansi_length] = 0;
  return ansi;
}

#endif  // _WIN32_WCE

// Compares two C strings.  Returns true iff they have the same content.
//
// Unlike strcmp(), this function can handle NULL argument(s).  A NULL
// C string is considered different to any non-NULL C string,
// including the empty string.
bool String::CStringEquals(const char * lhs, const char * rhs) {
  if ( lhs == NULL ) return rhs == NULL;

  if ( rhs == NULL ) return false;

  return strcmp(lhs, rhs) == 0;
}

#if GTEST_HAS_STD_WSTRING || GTEST_HAS_GLOBAL_WSTRING

// Converts an array of wide chars to a narrow string using the UTF-8
// encoding, and streams the result to the given Message object.
static void StreamWideCharsToMessage(const wchar_t* wstr, size_t len,
                                     Message* msg) {
  // TODO(wan): consider allowing a testing::String object to
  // contain '\0'.  This will make it behave more like std::string,
  // and will allow ToUtf8String() to return the correct encoding
  // for '\0' s.t. we can get rid of the conditional here (and in
  // several other places).
  for (size_t i = 0; i != len; ) {  // NOLINT
    if (wstr[i] != L'\0') {
      *msg << WideStringToUtf8(wstr + i, static_cast<int>(len - i));
      while (i != len && wstr[i] != L'\0')
        i++;
    } else {
      *msg << '\0';
      i++;
    }
  }
}

#endif  // GTEST_HAS_STD_WSTRING || GTEST_HAS_GLOBAL_WSTRING

}  // namespace internal

#if GTEST_HAS_STD_WSTRING
// Converts the given wide string to a narrow string using the UTF-8
// encoding, and streams the result to this Message object.
Message& Message::operator <<(const ::std::wstring& wstr) {
  internal::StreamWideCharsToMessage(wstr.c_str(), wstr.length(), this);
  return *this;
}
#endif  // GTEST_HAS_STD_WSTRING

#if GTEST_HAS_GLOBAL_WSTRING
// Converts the given wide string to a narrow string using the UTF-8
// encoding, and streams the result to this Message object.
Message& Message::operator <<(const ::wstring& wstr) {
  internal::StreamWideCharsToMessage(wstr.c_str(), wstr.length(), this);
  return *this;
}
#endif  // GTEST_HAS_GLOBAL_WSTRING

namespace internal {

// Formats a value to be used in a failure message.

// For a char value, we print it as a C++ char literal and as an
// unsigned integer (both in decimal and in hexadecimal).
String FormatForFailureMessage(char ch) {
  const unsigned int ch_as_uint = ch;
  // A String object cannot contain '\0', so we print "\\0" when ch is
  // '\0'.
  return String::Format("'%s' (%u, 0x%X)",
                        ch ? String::Format("%c", ch).c_str() : "\\0",
                        ch_as_uint, ch_as_uint);
}

// For a wchar_t value, we print it as a C++ wchar_t literal and as an
// unsigned integer (both in decimal and in hexidecimal).
String FormatForFailureMessage(wchar_t wchar) {
  // The C++ standard doesn't specify the exact size of the wchar_t
  // type.  It just says that it shall have the same size as another
  // integral type, called its underlying type.
  //
  // Therefore, in order to print a wchar_t value in the numeric form,
  // we first convert it to the largest integral type (UInt64) and
  // then print the converted value.
  //
  // We use streaming to print the value as "%llu" doesn't work
  // correctly with MSVC 7.1.
  const UInt64 wchar_as_uint64 = wchar;
  Message msg;
  // A String object cannot contain '\0', so we print "\\0" when wchar is
  // L'\0'.
  char buffer[32];  // CodePointToUtf8 requires a buffer that big.
  msg << "L'"
      << (wchar ? CodePointToUtf8(static_cast<UInt32>(wchar), buffer) : "\\0")
      << "' (" << wchar_as_uint64 << ", 0x" << ::std::setbase(16)
      << wchar_as_uint64 << ")";
  return msg.GetString();
}

}  // namespace internal

// AssertionResult constructor.
AssertionResult::AssertionResult(const internal::String& failure_message)
    : failure_message_(failure_message) {
}


// Makes a successful assertion result.
AssertionResult AssertionSuccess() {
  return AssertionResult();
}


// Makes a failed assertion result with the given failure message.
AssertionResult AssertionFailure(const Message& message) {
  return AssertionResult(message.GetString());
}

namespace internal {

// Constructs and returns the message for an equality assertion
// (e.g. ASSERT_EQ, EXPECT_STREQ, etc) failure.
//
// The first four parameters are the expressions used in the assertion
// and their values, as strings.  For example, for ASSERT_EQ(foo, bar)
// where foo is 5 and bar is 6, we have:
//
//   expected_expression: "foo"
//   actual_expression:   "bar"
//   expected_value:      "5"
//   actual_value:        "6"
//
// The ignoring_case parameter is true iff the assertion is a
// *_STRCASEEQ*.  When it's true, the string " (ignoring case)" will
// be inserted into the message.
AssertionResult EqFailure(const char* expected_expression,
                          const char* actual_expression,
                          const String& expected_value,
                          const String& actual_value,
                          bool ignoring_case) {
  Message msg;
  msg << "Value of: " << actual_expression;
  if (actual_value != actual_expression) {
    msg << "\n  Actual: " << actual_value;
  }

  msg << "\nExpected: " << expected_expression;
  if (ignoring_case) {
    msg << " (ignoring case)";
  }
  if (expected_value != expected_expression) {
    msg << "\nWhich is: " << expected_value;
  }

  return AssertionFailure(msg);
}


// Helper function for implementing ASSERT_NEAR.
AssertionResult DoubleNearPredFormat(const char* expr1,
                                     const char* expr2,
                                     const char* abs_error_expr,
                                     double val1,
                                     double val2,
                                     double abs_error) {
  const double diff = fabs(val1 - val2);
  if (diff <= abs_error) return AssertionSuccess();

  // TODO(wan): do not print the value of an expression if it's
  // already a literal.
  Message msg;
  msg << "The difference between " << expr1 << " and " << expr2
      << " is " << diff << ", which exceeds " << abs_error_expr << ", where\n"
      << expr1 << " evaluates to " << val1 << ",\n"
      << expr2 << " evaluates to " << val2 << ", and\n"
      << abs_error_expr << " evaluates to " << abs_error << ".";
  return AssertionFailure(msg);
}


// Helper template for implementing FloatLE() and DoubleLE().
template <typename RawType>
AssertionResult FloatingPointLE(const char* expr1,
                                const char* expr2,
                                RawType val1,
                                RawType val2) {
  // Returns success if val1 is less than val2,
  if (val1 < val2) {
    return AssertionSuccess();
  }

  // or if val1 is almost equal to val2.
  const FloatingPoint<RawType> lhs(val1), rhs(val2);
  if (lhs.AlmostEquals(rhs)) {
    return AssertionSuccess();
  }

  // Note that the above two checks will both fail if either val1 or
  // val2 is NaN, as the IEEE floating-point standard requires that
  // any predicate involving a NaN must return false.

  StrStream val1_ss;
  val1_ss << std::setprecision(std::numeric_limits<RawType>::digits10 + 2)
          << val1;

  StrStream val2_ss;
  val2_ss << std::setprecision(std::numeric_limits<RawType>::digits10 + 2)
          << val2;

  Message msg;
  msg << "Expected: (" << expr1 << ") <= (" << expr2 << ")\n"
      << "  Actual: " << StrStreamToString(&val1_ss) << " vs "
      << StrStreamToString(&val2_ss);

  return AssertionFailure(msg);
}

}  // namespace internal

// Asserts that val1 is less than, or almost equal to, val2.  Fails
// otherwise.  In particular, it fails if either val1 or val2 is NaN.
AssertionResult FloatLE(const char* expr1, const char* expr2,
                        float val1, float val2) {
  return internal::FloatingPointLE<float>(expr1, expr2, val1, val2);
}

// Asserts that val1 is less than, or almost equal to, val2.  Fails
// otherwise.  In particular, it fails if either val1 or val2 is NaN.
AssertionResult DoubleLE(const char* expr1, const char* expr2,
                         double val1, double val2) {
  return internal::FloatingPointLE<double>(expr1, expr2, val1, val2);
}

namespace internal {

// The helper function for {ASSERT|EXPECT}_EQ with int or enum
// arguments.
AssertionResult CmpHelperEQ(const char* expected_expression,
                            const char* actual_expression,
                            BiggestInt expected,
                            BiggestInt actual) {
  if (expected == actual) {
    return AssertionSuccess();
  }

  return EqFailure(expected_expression,
                   actual_expression,
                   FormatForComparisonFailureMessage(expected, actual),
                   FormatForComparisonFailureMessage(actual, expected),
                   false);
}

// A macro for implementing the helper functions needed to implement
// ASSERT_?? and EXPECT_?? with integer or enum arguments.  It is here
// just to avoid copy-and-paste of similar code.
#define GTEST_IMPL_CMP_HELPER_(op_name, op)\
AssertionResult CmpHelper##op_name(const char* expr1, const char* expr2, \
                                   BiggestInt val1, BiggestInt val2) {\
  if (val1 op val2) {\
    return AssertionSuccess();\
  } else {\
    Message msg;\
    msg << "Expected: (" << expr1 << ") " #op " (" << expr2\
        << "), actual: " << FormatForComparisonFailureMessage(val1, val2)\
        << " vs " << FormatForComparisonFailureMessage(val2, val1);\
    return AssertionFailure(msg);\
  }\
}

// Implements the helper function for {ASSERT|EXPECT}_NE with int or
// enum arguments.
GTEST_IMPL_CMP_HELPER_(NE, !=)
// Implements the helper function for {ASSERT|EXPECT}_LE with int or
// enum arguments.
GTEST_IMPL_CMP_HELPER_(LE, <=)
// Implements the helper function for {ASSERT|EXPECT}_LT with int or
// enum arguments.
GTEST_IMPL_CMP_HELPER_(LT, < )
// Implements the helper function for {ASSERT|EXPECT}_GE with int or
// enum arguments.
GTEST_IMPL_CMP_HELPER_(GE, >=)
// Implements the helper function for {ASSERT|EXPECT}_GT with int or
// enum arguments.
GTEST_IMPL_CMP_HELPER_(GT, > )

#undef GTEST_IMPL_CMP_HELPER_

// The helper function for {ASSERT|EXPECT}_STREQ.
AssertionResult CmpHelperSTREQ(const char* expected_expression,
                               const char* actual_expression,
                               const char* expected,
                               const char* actual) {
  if (String::CStringEquals(expected, actual)) {
    return AssertionSuccess();
  }

  return EqFailure(expected_expression,
                   actual_expression,
                   String::ShowCStringQuoted(expected),
                   String::ShowCStringQuoted(actual),
                   false);
}

// The helper function for {ASSERT|EXPECT}_STRCASEEQ.
AssertionResult CmpHelperSTRCASEEQ(const char* expected_expression,
                                   const char* actual_expression,
                                   const char* expected,
                                   const char* actual) {
  if (String::CaseInsensitiveCStringEquals(expected, actual)) {
    return AssertionSuccess();
  }

  return EqFailure(expected_expression,
                   actual_expression,
                   String::ShowCStringQuoted(expected),
                   String::ShowCStringQuoted(actual),
                   true);
}

// The helper function for {ASSERT|EXPECT}_STRNE.
AssertionResult CmpHelperSTRNE(const char* s1_expression,
                               const char* s2_expression,
                               const char* s1,
                               const char* s2) {
  if (!String::CStringEquals(s1, s2)) {
    return AssertionSuccess();
  } else {
    Message msg;
    msg << "Expected: (" << s1_expression << ") != ("
        << s2_expression << "), actual: \""
        << s1 << "\" vs \"" << s2 << "\"";
    return AssertionFailure(msg);
  }
}

// The helper function for {ASSERT|EXPECT}_STRCASENE.
AssertionResult CmpHelperSTRCASENE(const char* s1_expression,
                                   const char* s2_expression,
                                   const char* s1,
                                   const char* s2) {
  if (!String::CaseInsensitiveCStringEquals(s1, s2)) {
    return AssertionSuccess();
  } else {
    Message msg;
    msg << "Expected: (" << s1_expression << ") != ("
        << s2_expression << ") (ignoring case), actual: \""
        << s1 << "\" vs \"" << s2 << "\"";
    return AssertionFailure(msg);
  }
}

}  // namespace internal

namespace {

// Helper functions for implementing IsSubString() and IsNotSubstring().

// This group of overloaded functions return true iff needle is a
// substring of haystack.  NULL is considered a substring of itself
// only.

bool IsSubstringPred(const char* needle, const char* haystack) {
  if (needle == NULL || haystack == NULL)
    return needle == haystack;

  return strstr(haystack, needle) != NULL;
}

bool IsSubstringPred(const wchar_t* needle, const wchar_t* haystack) {
  if (needle == NULL || haystack == NULL)
    return needle == haystack;

  return wcsstr(haystack, needle) != NULL;
}

// StringType here can be either ::std::string or ::std::wstring.
template <typename StringType>
bool IsSubstringPred(const StringType& needle,
                     const StringType& haystack) {
  return haystack.find(needle) != StringType::npos;
}

// This function implements either IsSubstring() or IsNotSubstring(),
// depending on the value of the expected_to_be_substring parameter.
// StringType here can be const char*, const wchar_t*, ::std::string,
// or ::std::wstring.
template <typename StringType>
AssertionResult IsSubstringImpl(
    bool expected_to_be_substring,
    const char* needle_expr, const char* haystack_expr,
    const StringType& needle, const StringType& haystack) {
  if (IsSubstringPred(needle, haystack) == expected_to_be_substring)
    return AssertionSuccess();

  const bool is_wide_string = sizeof(needle[0]) > 1;
  const char* const begin_string_quote = is_wide_string ? "L\"" : "\"";
  return AssertionFailure(
      Message()
      << "Value of: " << needle_expr << "\n"
      << "  Actual: " << begin_string_quote << needle << "\"\n"
      << "Expected: " << (expected_to_be_substring ? "" : "not ")
      << "a substring of " << haystack_expr << "\n"
      << "Which is: " << begin_string_quote << haystack << "\"");
}

}  // namespace

// IsSubstring() and IsNotSubstring() check whether needle is a
// substring of haystack (NULL is considered a substring of itself
// only), and return an appropriate error message when they fail.

AssertionResult IsSubstring(
    const char* needle_expr, const char* haystack_expr,
    const char* needle, const char* haystack) {
  return IsSubstringImpl(true, needle_expr, haystack_expr, needle, haystack);
}

AssertionResult IsSubstring(
    const char* needle_expr, const char* haystack_expr,
    const wchar_t* needle, const wchar_t* haystack) {
  return IsSubstringImpl(true, needle_expr, haystack_expr, needle, haystack);
}

AssertionResult IsNotSubstring(
    const char* needle_expr, const char* haystack_expr,
    const char* needle, const char* haystack) {
  return IsSubstringImpl(false, needle_expr, haystack_expr, needle, haystack);
}

AssertionResult IsNotSubstring(
    const char* needle_expr, const char* haystack_expr,
    const wchar_t* needle, const wchar_t* haystack) {
  return IsSubstringImpl(false, needle_expr, haystack_expr, needle, haystack);
}

#if GTEST_HAS_STD_STRING
AssertionResult IsSubstring(
    const char* needle_expr, const char* haystack_expr,
    const ::std::string& needle, const ::std::string& haystack) {
  return IsSubstringImpl(true, needle_expr, haystack_expr, needle, haystack);
}

AssertionResult IsNotSubstring(
    const char* needle_expr, const char* haystack_expr,
    const ::std::string& needle, const ::std::string& haystack) {
  return IsSubstringImpl(false, needle_expr, haystack_expr, needle, haystack);
}
#endif  // GTEST_HAS_STD_STRING

#if GTEST_HAS_STD_WSTRING
AssertionResult IsSubstring(
    const char* needle_expr, const char* haystack_expr,
    const ::std::wstring& needle, const ::std::wstring& haystack) {
  return IsSubstringImpl(true, needle_expr, haystack_expr, needle, haystack);
}

AssertionResult IsNotSubstring(
    const char* needle_expr, const char* haystack_expr,
    const ::std::wstring& needle, const ::std::wstring& haystack) {
  return IsSubstringImpl(false, needle_expr, haystack_expr, needle, haystack);
}
#endif  // GTEST_HAS_STD_WSTRING

namespace internal {

#if GTEST_OS_WINDOWS

namespace {

// Helper function for IsHRESULT{SuccessFailure} predicates
AssertionResult HRESULTFailureHelper(const char* expr,
                                     const char* expected,
                                     long hr) {  // NOLINT
#ifdef _WIN32_WCE
  // Windows CE doesn't support FormatMessage.
  const char error_text[] = "";
#else
  // Looks up the human-readable system message for the HRESULT code
  // and since we're not passing any params to FormatMessage, we don't
  // want inserts expanded.
  const DWORD kFlags = FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS;
  const DWORD kBufSize = 4096;  // String::Format can't exceed this length.
  // Gets the system's human readable message string for this HRESULT.
  char error_text[kBufSize] = { '\0' };
  DWORD message_length = ::FormatMessageA(kFlags,
                                          0,  // no source, we're asking system
                                          hr,  // the error
                                          0,  // no line width restrictions
                                          error_text,  // output buffer
                                          kBufSize,  // buf size
                                          NULL);  // no arguments for inserts
  // Trims tailing white space (FormatMessage leaves a trailing cr-lf)
  for (; message_length && isspace(error_text[message_length - 1]);
          --message_length) {
    error_text[message_length - 1] = '\0';
  }
#endif  // _WIN32_WCE

  const String error_hex(String::Format("0x%08X ", hr));
  Message msg;
  msg << "Expected: " << expr << " " << expected << ".\n"
      << "  Actual: " << error_hex << error_text << "\n";

  return ::testing::AssertionFailure(msg);
}

}  // namespace

AssertionResult IsHRESULTSuccess(const char* expr, long hr) {  // NOLINT
  if (SUCCEEDED(hr)) {
    return AssertionSuccess();
  }
  return HRESULTFailureHelper(expr, "succeeds", hr);
}

AssertionResult IsHRESULTFailure(const char* expr, long hr) {  // NOLINT
  if (FAILED(hr)) {
    return AssertionSuccess();
  }
  return HRESULTFailureHelper(expr, "fails", hr);
}

#endif  // GTEST_OS_WINDOWS

// Utility functions for encoding Unicode text (wide strings) in
// UTF-8.

// A Unicode code-point can have upto 21 bits, and is encoded in UTF-8
// like this:
//
// Code-point length   Encoding
//   0 -  7 bits       0xxxxxxx
//   8 - 11 bits       110xxxxx 10xxxxxx
//  12 - 16 bits       1110xxxx 10xxxxxx 10xxxxxx
//  17 - 21 bits       11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

// The maximum code-point a one-byte UTF-8 sequence can represent.
const UInt32 kMaxCodePoint1 = (static_cast<UInt32>(1) <<  7) - 1;

// The maximum code-point a two-byte UTF-8 sequence can represent.
const UInt32 kMaxCodePoint2 = (static_cast<UInt32>(1) << (5 + 6)) - 1;

// The maximum code-point a three-byte UTF-8 sequence can represent.
const UInt32 kMaxCodePoint3 = (static_cast<UInt32>(1) << (4 + 2*6)) - 1;

// The maximum code-point a four-byte UTF-8 sequence can represent.
const UInt32 kMaxCodePoint4 = (static_cast<UInt32>(1) << (3 + 3*6)) - 1;

// Chops off the n lowest bits from a bit pattern.  Returns the n
// lowest bits.  As a side effect, the original bit pattern will be
// shifted to the right by n bits.
inline UInt32 ChopLowBits(UInt32* bits, int n) {
  const UInt32 low_bits = *bits & ((static_cast<UInt32>(1) << n) - 1);
  *bits >>= n;
  return low_bits;
}

// Converts a Unicode code point to a narrow string in UTF-8 encoding.
// code_point parameter is of type UInt32 because wchar_t may not be
// wide enough to contain a code point.
// The output buffer str must containt at least 32 characters.
// The function returns the address of the output buffer.
// If the code_point is not a valid Unicode code point
// (i.e. outside of Unicode range U+0 to U+10FFFF) it will be output
// as '(Invalid Unicode 0xXXXXXXXX)'.
char* CodePointToUtf8(UInt32 code_point, char* str) {
  if (code_point <= kMaxCodePoint1) {
    str[1] = '\0';
    str[0] = static_cast<char>(code_point);                          // 0xxxxxxx
  } else if (code_point <= kMaxCodePoint2) {
    str[2] = '\0';
    str[1] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
    str[0] = static_cast<char>(0xC0 | code_point);                   // 110xxxxx
  } else if (code_point <= kMaxCodePoint3) {
    str[3] = '\0';
    str[2] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
    str[1] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
    str[0] = static_cast<char>(0xE0 | code_point);                   // 1110xxxx
  } else if (code_point <= kMaxCodePoint4) {
    str[4] = '\0';
    str[3] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
    str[2] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
    str[1] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
    str[0] = static_cast<char>(0xF0 | code_point);                   // 11110xxx
  } else {
    // The longest string String::Format can produce when invoked
    // with these parameters is 28 character long (not including
    // the terminating nul character). We are asking for 32 character
    // buffer just in case. This is also enough for strncpy to
    // null-terminate the destination string.
    // MSVC 8 deprecates strncpy(), so we want to suppress warning
    // 4996 (deprecated function) there.
#if GTEST_OS_WINDOWS  // We are on Windows.
#pragma warning(push)          // Saves the current warning state.
#pragma warning(disable:4996)  // Temporarily disables warning 4996.
#endif
    strncpy(str, String::Format("(Invalid Unicode 0x%X)", code_point).c_str(),
            32);
#if GTEST_OS_WINDOWS  // We are on Windows.
#pragma warning(pop)  // Restores the warning state.
#endif
    str[31] = '\0';  // Makes sure no change in the format to strncpy leaves
                     // the result unterminated.
  }
  return str;
}

// The following two functions only make sense if the the system
// uses UTF-16 for wide string encoding. All supported systems
// with 16 bit wchar_t (Windows, Cygwin, Symbian OS) do use UTF-16.

// Determines if the arguments constitute UTF-16 surrogate pair
// and thus should be combined into a single Unicode code point
// using CreateCodePointFromUtf16SurrogatePair.
inline bool IsUtf16SurrogatePair(wchar_t first, wchar_t second) {
  return sizeof(wchar_t) == 2 &&
      (first & 0xFC00) == 0xD800 && (second & 0xFC00) == 0xDC00;
}

// Creates a Unicode code point from UTF16 surrogate pair.
inline UInt32 CreateCodePointFromUtf16SurrogatePair(wchar_t first,
                                                    wchar_t second) {
  const UInt32 mask = (1 << 10) - 1;
  return (sizeof(wchar_t) == 2) ?
      (((first & mask) << 10) | (second & mask)) + 0x10000 :
      // This function should not be called when the condition is
      // false, but we provide a sensible default in case it is.
      static_cast<UInt32>(first);
}

// Converts a wide string to a narrow string in UTF-8 encoding.
// The wide string is assumed to have the following encoding:
//   UTF-16 if sizeof(wchar_t) == 2 (on Windows, Cygwin, Symbian OS)
//   UTF-32 if sizeof(wchar_t) == 4 (on Linux)
// Parameter str points to a null-terminated wide string.
// Parameter num_chars may additionally limit the number
// of wchar_t characters processed. -1 is used when the entire string
// should be processed.
// If the string contains code points that are not valid Unicode code points
// (i.e. outside of Unicode range U+0 to U+10FFFF) they will be output
// as '(Invalid Unicode 0xXXXXXXXX)'. If the string is in UTF16 encoding
// and contains invalid UTF-16 surrogate pairs, values in those pairs
// will be encoded as individual Unicode characters from Basic Normal Plane.
String WideStringToUtf8(const wchar_t* str, int num_chars) {
  if (num_chars == -1)
    num_chars = static_cast<int>(wcslen(str));

  StrStream stream;
  for (int i = 0; i < num_chars; ++i) {
    UInt32 unicode_code_point;

    if (str[i] == L'\0') {
      break;
    } else if (i + 1 < num_chars && IsUtf16SurrogatePair(str[i], str[i + 1])) {
      unicode_code_point = CreateCodePointFromUtf16SurrogatePair(str[i],
                                                                 str[i + 1]);
      i++;
    } else {
      unicode_code_point = static_cast<UInt32>(str[i]);
    }

    char buffer[32];  // CodePointToUtf8 requires a buffer this big.
    stream << CodePointToUtf8(unicode_code_point, buffer);
  }
  return StrStreamToString(&stream);
}

// Converts a wide C string to a String using the UTF-8 encoding.
// NULL will be converted to "(null)".
String String::ShowWideCString(const wchar_t * wide_c_str) {
  if (wide_c_str == NULL) return String("(null)");

  return String(internal::WideStringToUtf8(wide_c_str, -1).c_str());
}

// Similar to ShowWideCString(), except that this function encloses
// the converted string in double quotes.
String String::ShowWideCStringQuoted(const wchar_t* wide_c_str) {
  if (wide_c_str == NULL) return String("(null)");

  return String::Format("L\"%s\"",
                        String::ShowWideCString(wide_c_str).c_str());
}

// Compares two wide C strings.  Returns true iff they have the same
// content.
//
// Unlike wcscmp(), this function can handle NULL argument(s).  A NULL
// C string is considered different to any non-NULL C string,
// including the empty string.
bool String::WideCStringEquals(const wchar_t * lhs, const wchar_t * rhs) {
  if (lhs == NULL) return rhs == NULL;

  if (rhs == NULL) return false;

  return wcscmp(lhs, rhs) == 0;
}

// Helper function for *_STREQ on wide strings.
AssertionResult CmpHelperSTREQ(const char* expected_expression,
                               const char* actual_expression,
                               const wchar_t* expected,
                               const wchar_t* actual) {
  if (String::WideCStringEquals(expected, actual)) {
    return AssertionSuccess();
  }

  return EqFailure(expected_expression,
                   actual_expression,
                   String::ShowWideCStringQuoted(expected),
                   String::ShowWideCStringQuoted(actual),
                   false);
}

// Helper function for *_STRNE on wide strings.
AssertionResult CmpHelperSTRNE(const char* s1_expression,
                               const char* s2_expression,
                               const wchar_t* s1,
                               const wchar_t* s2) {
  if (!String::WideCStringEquals(s1, s2)) {
    return AssertionSuccess();
  }

  Message msg;
  msg << "Expected: (" << s1_expression << ") != ("
      << s2_expression << "), actual: "
      << String::ShowWideCStringQuoted(s1)
      << " vs " << String::ShowWideCStringQuoted(s2);
  return AssertionFailure(msg);
}

// Compares two C strings, ignoring case.  Returns true iff they have
// the same content.
//
// Unlike strcasecmp(), this function can handle NULL argument(s).  A
// NULL C string is considered different to any non-NULL C string,
// including the empty string.
bool String::CaseInsensitiveCStringEquals(const char * lhs, const char * rhs) {
  if ( lhs == NULL ) return rhs == NULL;

  if ( rhs == NULL ) return false;

#if GTEST_OS_WINDOWS
  return _stricmp(lhs, rhs) == 0;
#else  // GTEST_OS_WINDOWS
  return strcasecmp(lhs, rhs) == 0;
#endif  // GTEST_OS_WINDOWS
}

  // Compares two wide C strings, ignoring case.  Returns true iff they
  // have the same content.
  //
  // Unlike wcscasecmp(), this function can handle NULL argument(s).
  // A NULL C string is considered different to any non-NULL wide C string,
  // including the empty string.
  // NB: The implementations on different platforms slightly differ.
  // On windows, this method uses _wcsicmp which compares according to LC_CTYPE
  // environment variable. On GNU platform this method uses wcscasecmp
  // which compares according to LC_CTYPE category of the current locale.
  // On MacOS X, it uses towlower, which also uses LC_CTYPE category of the
  // current locale.
bool String::CaseInsensitiveWideCStringEquals(const wchar_t* lhs,
                                              const wchar_t* rhs) {
  if ( lhs == NULL ) return rhs == NULL;

  if ( rhs == NULL ) return false;

#if GTEST_OS_WINDOWS
  return _wcsicmp(lhs, rhs) == 0;
#elif GTEST_OS_LINUX
  return wcscasecmp(lhs, rhs) == 0;
#else
  // Mac OS X and Cygwin don't define wcscasecmp.  Other unknown OSes
  // may not define it either.
  wint_t left, right;
  do {
    left = towlower(*lhs++);
    right = towlower(*rhs++);
  } while (left && left == right);
  return left == right;
#endif  // OS selector
}

// Constructs a String by copying a given number of chars from a
// buffer.  E.g. String("hello", 3) will create the string "hel".
String::String(const char * buffer, size_t len) {
  char * const temp = new char[ len + 1 ];
  memcpy(temp, buffer, len);
  temp[ len ] = '\0';
  c_str_ = temp;
}

// Compares this with another String.
// Returns < 0 if this is less than rhs, 0 if this is equal to rhs, or > 0
// if this is greater than rhs.
int String::Compare(const String & rhs) const {
  if ( c_str_ == NULL ) {
    return rhs.c_str_ == NULL ? 0 : -1;  // NULL < anything except NULL
  }

  return rhs.c_str_ == NULL ? 1 : strcmp(c_str_, rhs.c_str_);
}

// Returns true iff this String ends with the given suffix.  *Any*
// String is considered to end with a NULL or empty suffix.
bool String::EndsWith(const char* suffix) const {
  if (suffix == NULL || CStringEquals(suffix, "")) return true;

  if (c_str_ == NULL) return false;

  const size_t this_len = strlen(c_str_);
  const size_t suffix_len = strlen(suffix);
  return (this_len >= suffix_len) &&
         CStringEquals(c_str_ + this_len - suffix_len, suffix);
}

// Returns true iff this String ends with the given suffix, ignoring case.
// Any String is considered to end with a NULL or empty suffix.
bool String::EndsWithCaseInsensitive(const char* suffix) const {
  if (suffix == NULL || CStringEquals(suffix, "")) return true;

  if (c_str_ == NULL) return false;

  const size_t this_len = strlen(c_str_);
  const size_t suffix_len = strlen(suffix);
  return (this_len >= suffix_len) &&
         CaseInsensitiveCStringEquals(c_str_ + this_len - suffix_len, suffix);
}

// Sets the 0-terminated C string this String object represents.  The
// old string in this object is deleted, and this object will own a
// clone of the input string.  This function copies only up to length
// bytes (plus a terminating null byte), or until the first null byte,
// whichever comes first.
//
// This function works even when the c_str parameter has the same
// value as that of the c_str_ field.
void String::Set(const char * c_str, size_t length) {
  // Makes sure this works when c_str == c_str_
  const char* const temp = CloneString(c_str, length);
  delete[] c_str_;
  c_str_ = temp;
}

// Assigns a C string to this object.  Self-assignment works.
const String& String::operator=(const char* c_str) {
  // Makes sure this works when c_str == c_str_
  if (c_str != c_str_) {
    delete[] c_str_;
    c_str_ = CloneCString(c_str);
  }
  return *this;
}

// Formats a list of arguments to a String, using the same format
// spec string as for printf.
//
// We do not use the StringPrintf class as it is not universally
// available.
//
// The result is limited to 4096 characters (including the tailing 0).
// If 4096 characters are not enough to format the input,
// "<buffer exceeded>" is returned.
String String::Format(const char * format, ...) {
  va_list args;
  va_start(args, format);

  char buffer[4096];
  // MSVC 8 deprecates vsnprintf(), so we want to suppress warning
  // 4996 (deprecated function) there.
#if GTEST_OS_WINDOWS  // We are on Windows.
#pragma warning(push)          // Saves the current warning state.
#pragma warning(disable:4996)  // Temporarily disables warning 4996.
  const int size =
    vsnprintf(buffer, sizeof(buffer)/sizeof(buffer[0]) - 1, format, args);
#pragma warning(pop)           // Restores the warning state.
#else  // We are on Linux or Mac OS.
  const int size =
    vsnprintf(buffer, sizeof(buffer)/sizeof(buffer[0]) - 1, format, args);
#endif  // GTEST_OS_WINDOWS
  va_end(args);

  return String(size >= 0 ? buffer : "<buffer exceeded>");
}

// Converts the buffer in a StrStream to a String, converting NUL
// bytes to "\\0" along the way.
String StrStreamToString(StrStream* ss) {
#if GTEST_HAS_STD_STRING
  const ::std::string& str = ss->str();
  const char* const start = str.c_str();
  const char* const end = start + str.length();
#else
  const char* const start = ss->str();
  const char* const end = start + ss->pcount();
#endif  // GTEST_HAS_STD_STRING

  // We need to use a helper StrStream to do this transformation
  // because String doesn't support push_back().
  StrStream helper;
  for (const char* ch = start; ch != end; ++ch) {
    if (*ch == '\0') {
      helper << "\\0";  // Replaces NUL with "\\0";
    } else {
      helper.put(*ch);
    }
  }

#if GTEST_HAS_STD_STRING
  return String(helper.str().c_str());
#else
  const String str(helper.str(), helper.pcount());
  helper.freeze(false);
  ss->freeze(false);
  return str;
#endif  // GTEST_HAS_STD_STRING
}

// Appends the user-supplied message to the Google-Test-generated message.
String AppendUserMessage(const String& gtest_msg,
                         const Message& user_msg) {
  // Appends the user message if it's non-empty.
  const String user_msg_string = user_msg.GetString();
  if (user_msg_string.empty()) {
    return gtest_msg;
  }

  Message msg;
  msg << gtest_msg << "\n" << user_msg_string;

  return msg.GetString();
}

// class TestResult

// Creates an empty TestResult.
TestResult::TestResult()
    : death_test_count_(0),
      elapsed_time_(0) {
}

// D'tor.
TestResult::~TestResult() {
}

// Adds a test part result to the list.
void TestResult::AddTestPartResult(const TestPartResult& test_part_result) {
  test_part_results_.PushBack(test_part_result);
}

// Adds a test property to the list. If a property with the same key as the
// supplied property is already represented, the value of this test_property
// replaces the old value for that key.
void TestResult::RecordProperty(const TestProperty& test_property) {
  if (!ValidateTestProperty(test_property)) {
    return;
  }
  MutexLock lock(&test_properites_mutex_);
  ListNode<TestProperty>* const node_with_matching_key =
      test_properties_.FindIf(TestPropertyKeyIs(test_property.key()));
  if (node_with_matching_key == NULL) {
    test_properties_.PushBack(test_property);
    return;
  }
  TestProperty& property_with_matching_key = node_with_matching_key->element();
  property_with_matching_key.SetValue(test_property.value());
}

// Adds a failure if the key is a reserved attribute of Google Test
// testcase tags.  Returns true if the property is valid.
bool TestResult::ValidateTestProperty(const TestProperty& test_property) {
  String key(test_property.key());
  if (key == "name" || key == "status" || key == "time" || key == "classname") {
    ADD_FAILURE()
        << "Reserved key used in RecordProperty(): "
        << key
        << " ('name', 'status', 'time', and 'classname' are reserved by "
        << GTEST_NAME_ << ")";
    return false;
  }
  return true;
}

// Clears the object.
void TestResult::Clear() {
  test_part_results_.Clear();
  test_properties_.Clear();
  death_test_count_ = 0;
  elapsed_time_ = 0;
}

// Returns true iff the test part passed.
static bool TestPartPassed(const TestPartResult & result) {
  return result.passed();
}

// Gets the number of successful test parts.
int TestResult::successful_part_count() const {
  return test_part_results_.CountIf(TestPartPassed);
}

// Returns true iff the test part failed.
static bool TestPartFailed(const TestPartResult & result) {
  return result.failed();
}

// Gets the number of failed test parts.
int TestResult::failed_part_count() const {
  return test_part_results_.CountIf(TestPartFailed);
}

// Returns true iff the test part fatally failed.
static bool TestPartFatallyFailed(const TestPartResult & result) {
  return result.fatally_failed();
}

// Returns true iff the test fatally failed.
bool TestResult::HasFatalFailure() const {
  return test_part_results_.CountIf(TestPartFatallyFailed) > 0;
}

// Gets the number of all test parts.  This is the sum of the number
// of successful test parts and the number of failed test parts.
int TestResult::total_part_count() const {
  return test_part_results_.size();
}

}  // namespace internal

// class Test

// Creates a Test object.

// The c'tor saves the values of all Google Test flags.
Test::Test()
    : gtest_flag_saver_(new internal::GTestFlagSaver) {
}

// The d'tor restores the values of all Google Test flags.
Test::~Test() {
  delete gtest_flag_saver_;
}

// Sets up the test fixture.
//
// A sub-class may override this.
void Test::SetUp() {
}

// Tears down the test fixture.
//
// A sub-class may override this.
void Test::TearDown() {
}

// Allows user supplied key value pairs to be recorded for later output.
void Test::RecordProperty(const char* key, const char* value) {
  UnitTest::GetInstance()->RecordPropertyForCurrentTest(key, value);
}

// Allows user supplied key value pairs to be recorded for later output.
void Test::RecordProperty(const char* key, int value) {
  Message value_message;
  value_message << value;
  RecordProperty(key, value_message.GetString().c_str());
}

#if GTEST_OS_WINDOWS
// We are on Windows.

// Adds an "exception thrown" fatal failure to the current test.
static void AddExceptionThrownFailure(DWORD exception_code,
                                      const char* location) {
  Message message;
  message << "Exception thrown with code 0x" << std::setbase(16) <<
    exception_code << std::setbase(10) << " in " << location << ".";

  UnitTest* const unit_test = UnitTest::GetInstance();
  unit_test->AddTestPartResult(
      TPRT_FATAL_FAILURE,
      static_cast<const char *>(NULL),
           // We have no info about the source file where the exception
           // occurred.
      -1,  // We have no info on which line caused the exception.
      message.GetString(),
      internal::String(""));
}

#endif  // GTEST_OS_WINDOWS

// Google Test requires all tests in the same test case to use the same test
// fixture class.  This function checks if the current test has the
// same fixture class as the first test in the current test case.  If
// yes, it returns true; otherwise it generates a Google Test failure and
// returns false.
bool Test::HasSameFixtureClass() {
  internal::UnitTestImpl* const impl = internal::GetUnitTestImpl();
  const TestCase* const test_case = impl->current_test_case();

  // Info about the first test in the current test case.
  const internal::TestInfoImpl* const first_test_info =
      test_case->test_info_list().Head()->element()->impl();
  const internal::TypeId first_fixture_id = first_test_info->fixture_class_id();
  const char* const first_test_name = first_test_info->name();

  // Info about the current test.
  const internal::TestInfoImpl* const this_test_info =
      impl->current_test_info()->impl();
  const internal::TypeId this_fixture_id = this_test_info->fixture_class_id();
  const char* const this_test_name = this_test_info->name();

  if (this_fixture_id != first_fixture_id) {
    // Is the first test defined using TEST?
    const bool first_is_TEST = first_fixture_id == internal::GetTestTypeId();
    // Is this test defined using TEST?
    const bool this_is_TEST = this_fixture_id == internal::GetTestTypeId();

    if (first_is_TEST || this_is_TEST) {
      // The user mixed TEST and TEST_F in this test case - we'll tell
      // him/her how to fix it.

      // Gets the name of the TEST and the name of the TEST_F.  Note
      // that first_is_TEST and this_is_TEST cannot both be true, as
      // the fixture IDs are different for the two tests.
      const char* const TEST_name =
          first_is_TEST ? first_test_name : this_test_name;
      const char* const TEST_F_name =
          first_is_TEST ? this_test_name : first_test_name;

      ADD_FAILURE()
          << "All tests in the same test case must use the same test fixture\n"
          << "class, so mixing TEST_F and TEST in the same test case is\n"
          << "illegal.  In test case " << this_test_info->test_case_name()
          << ",\n"
          << "test " << TEST_F_name << " is defined using TEST_F but\n"
          << "test " << TEST_name << " is defined using TEST.  You probably\n"
          << "want to change the TEST to TEST_F or move it to another test\n"
          << "case.";
    } else {
      // The user defined two fixture classes with the same name in
      // two namespaces - we'll tell him/her how to fix it.
      ADD_FAILURE()
          << "All tests in the same test case must use the same test fixture\n"
          << "class.  However, in test case "
          << this_test_info->test_case_name() << ",\n"
          << "you defined test " << first_test_name
          << " and test " << this_test_name << "\n"
          << "using two different test fixture classes.  This can happen if\n"
          << "the two classes are from different namespaces or translation\n"
          << "units and have the same name.  You should probably rename one\n"
          << "of the classes to put the tests into different test cases.";
    }
    return false;
  }

  return true;
}

// Runs the test and updates the test result.
void Test::Run() {
  if (!HasSameFixtureClass()) return;

  internal::UnitTestImpl* const impl = internal::GetUnitTestImpl();
#if GTEST_OS_WINDOWS
  // We are on Windows.
  impl->os_stack_trace_getter()->UponLeavingGTest();
  __try {
    SetUp();
  } __except(internal::UnitTestOptions::GTestShouldProcessSEH(
      GetExceptionCode())) {
    AddExceptionThrownFailure(GetExceptionCode(), "SetUp()");
  }

  // We will run the test only if SetUp() had no fatal failure.
  if (!HasFatalFailure()) {
    impl->os_stack_trace_getter()->UponLeavingGTest();
    __try {
      TestBody();
    } __except(internal::UnitTestOptions::GTestShouldProcessSEH(
        GetExceptionCode())) {
      AddExceptionThrownFailure(GetExceptionCode(), "the test body");
    }
  }

  // However, we want to clean up as much as possible.  Hence we will
  // always call TearDown(), even if SetUp() or the test body has
  // failed.
  impl->os_stack_trace_getter()->UponLeavingGTest();
  __try {
    TearDown();
  } __except(internal::UnitTestOptions::GTestShouldProcessSEH(
      GetExceptionCode())) {
    AddExceptionThrownFailure(GetExceptionCode(), "TearDown()");
  }

#else  // We are on Linux or Mac - exceptions are disabled.
  impl->os_stack_trace_getter()->UponLeavingGTest();
  SetUp();

  // We will run the test only if SetUp() was successful.
  if (!HasFatalFailure()) {
    impl->os_stack_trace_getter()->UponLeavingGTest();
    TestBody();
  }

  // However, we want to clean up as much as possible.  Hence we will
  // always call TearDown(), even if SetUp() or the test body has
  // failed.
  impl->os_stack_trace_getter()->UponLeavingGTest();
  TearDown();
#endif  // GTEST_OS_WINDOWS
}


// Returns true iff the current test has a fatal failure.
bool Test::HasFatalFailure() {
  return internal::GetUnitTestImpl()->current_test_result()->HasFatalFailure();
}

// class TestInfo

// Constructs a TestInfo object. It assumes ownership of the test factory
// object via impl_.
TestInfo::TestInfo(const char* test_case_name,
                   const char* name,
                   const char* test_case_comment,
                   const char* comment,
                   internal::TypeId fixture_class_id,
                   internal::TestFactoryBase* factory) {
  impl_ = new internal::TestInfoImpl(this, test_case_name, name,
                                     test_case_comment, comment,
                                     fixture_class_id, factory);
}

// Destructs a TestInfo object.
TestInfo::~TestInfo() {
  delete impl_;
}

namespace internal {

// Creates a new TestInfo object and registers it with Google Test;
// returns the created object.
//
// Arguments:
//
//   test_case_name:   name of the test case
//   name:             name of the test
//   test_case_comment: a comment on the test case that will be included in
//                      the test output
//   comment:          a comment on the test that will be included in the
//                     test output
//   fixture_class_id: ID of the test fixture class
//   set_up_tc:        pointer to the function that sets up the test case
//   tear_down_tc:     pointer to the function that tears down the test case
//   factory:          pointer to the factory that creates a test object.
//                     The newly created TestInfo instance will assume
//                     ownership of the factory object.
TestInfo* MakeAndRegisterTestInfo(
    const char* test_case_name, const char* name,
    const char* test_case_comment, const char* comment,
    TypeId fixture_class_id,
    SetUpTestCaseFunc set_up_tc,
    TearDownTestCaseFunc tear_down_tc,
    TestFactoryBase* factory) {
  TestInfo* const test_info =
      new TestInfo(test_case_name, name, test_case_comment, comment,
                   fixture_class_id, factory);
  GetUnitTestImpl()->AddTestInfo(set_up_tc, tear_down_tc, test_info);
  return test_info;
}

#if GTEST_HAS_PARAM_TEST
void ReportInvalidTestCaseType(const char* test_case_name,
                               const char* file, int line) {
  Message errors;
  errors
      << "Attempted redefinition of test case " << test_case_name << ".\n"
      << "All tests in the same test case must use the same test fixture\n"
      << "class.  However, in test case " << test_case_name << ", you tried\n"
      << "to define a test using a fixture class different from the one\n"
      << "used earlier. This can happen if the two fixture classes are\n"
      << "from different namespaces and have the same name. You should\n"
      << "probably rename one of the classes to put the tests into different\n"
      << "test cases.";

  fprintf(stderr, "%s %s", FormatFileLocation(file, line).c_str(),
          errors.GetString().c_str());
}
#endif  // GTEST_HAS_PARAM_TEST

}  // namespace internal

// Returns the test case name.
const char* TestInfo::test_case_name() const {
  return impl_->test_case_name();
}

// Returns the test name.
const char* TestInfo::name() const {
  return impl_->name();
}

// Returns the test case comment.
const char* TestInfo::test_case_comment() const {
  return impl_->test_case_comment();
}

// Returns the test comment.
const char* TestInfo::comment() const {
  return impl_->comment();
}

// Returns true if this test should run.
bool TestInfo::should_run() const { return impl_->should_run(); }

// Returns the result of the test.
const internal::TestResult* TestInfo::result() const { return impl_->result(); }

// Increments the number of death tests encountered in this test so
// far.
int TestInfo::increment_death_test_count() {
  return impl_->result()->increment_death_test_count();
}

namespace {

// A predicate that checks the test name of a TestInfo against a known
// value.
//
// This is used for implementation of the TestCase class only.  We put
// it in the anonymous namespace to prevent polluting the outer
// namespace.
//
// TestNameIs is copyable.
class TestNameIs {
 public:
  // Constructor.
  //
  // TestNameIs has NO default constructor.
  explicit TestNameIs(const char* name)
      : name_(name) {}

  // Returns true iff the test name of test_info matches name_.
  bool operator()(const TestInfo * test_info) const {
    return test_info && internal::String(test_info->name()).Compare(name_) == 0;
  }

 private:
  internal::String name_;
};

}  // namespace

// Finds and returns a TestInfo with the given name.  If one doesn't
// exist, returns NULL.
TestInfo * TestCase::GetTestInfo(const char* test_name) {
  // Can we find a TestInfo with the given name?
  internal::ListNode<TestInfo *> * const node = test_info_list_->FindIf(
      TestNameIs(test_name));

  // Returns the TestInfo found.
  return node ? node->element() : NULL;
}

namespace internal {

// This method expands all parameterized tests registered with macros TEST_P
// and INSTANTIATE_TEST_CASE_P into regular tests and registers those.
// This will be done just once during the program runtime.
void UnitTestImpl::RegisterParameterizedTests() {
#if GTEST_HAS_PARAM_TEST
  if (!parameterized_tests_registered_) {
    parameterized_test_registry_.RegisterTests();
    parameterized_tests_registered_ = true;
  }
#endif
}

// Creates the test object, runs it, records its result, and then
// deletes it.
void TestInfoImpl::Run() {
  if (!should_run_) return;

  // Tells UnitTest where to store test result.
  UnitTestImpl* const impl = internal::GetUnitTestImpl();
  impl->set_current_test_info(parent_);

  // Notifies the unit test event listener that a test is about to
  // start.
  UnitTestEventListenerInterface* const result_printer =
    impl->result_printer();
  result_printer->OnTestStart(parent_);

  const TimeInMillis start = GetTimeInMillis();

  impl->os_stack_trace_getter()->UponLeavingGTest();
#if GTEST_OS_WINDOWS
  // We are on Windows.
  Test* test = NULL;

  __try {
    // Creates the test object.
    test = factory_->CreateTest();
  } __except(internal::UnitTestOptions::GTestShouldProcessSEH(
      GetExceptionCode())) {
    AddExceptionThrownFailure(GetExceptionCode(),
                              "the test fixture's constructor");
    return;
  }
#else  // We are on Linux or Mac OS - exceptions are disabled.

  // TODO(wan): If test->Run() throws, test won't be deleted.  This is
  // not a problem now as we don't use exceptions.  If we were to
  // enable exceptions, we should revise the following to be
  // exception-safe.

  // Creates the test object.
  Test* test = factory_->CreateTest();
#endif  // GTEST_OS_WINDOWS

  // Runs the test only if the constructor of the test fixture didn't
  // generate a fatal failure.
  if (!Test::HasFatalFailure()) {
    test->Run();
  }

  // Deletes the test object.
  impl->os_stack_trace_getter()->UponLeavingGTest();
  delete test;
  test = NULL;

  result_.set_elapsed_time(GetTimeInMillis() - start);

  // Notifies the unit test event listener that a test has just finished.
  result_printer->OnTestEnd(parent_);

  // Tells UnitTest to stop associating assertion results to this
  // test.
  impl->set_current_test_info(NULL);
}

}  // namespace internal

// class TestCase

// Gets the number of successful tests in this test case.
int TestCase::successful_test_count() const {
  return test_info_list_->CountIf(TestPassed);
}

// Gets the number of failed tests in this test case.
int TestCase::failed_test_count() const {
  return test_info_list_->CountIf(TestFailed);
}

int TestCase::disabled_test_count() const {
  return test_info_list_->CountIf(TestDisabled);
}

// Get the number of tests in this test case that should run.
int TestCase::test_to_run_count() const {
  return test_info_list_->CountIf(ShouldRunTest);
}

// Gets the number of all tests.
int TestCase::total_test_count() const {
  return test_info_list_->size();
}

// Creates a TestCase with the given name.
//
// Arguments:
//
//   name:         name of the test case
//   set_up_tc:    pointer to the function that sets up the test case
//   tear_down_tc: pointer to the function that tears down the test case
TestCase::TestCase(const char* name, const char* comment,
                   Test::SetUpTestCaseFunc set_up_tc,
                   Test::TearDownTestCaseFunc tear_down_tc)
    : name_(name),
      comment_(comment),
      set_up_tc_(set_up_tc),
      tear_down_tc_(tear_down_tc),
      should_run_(false),
      elapsed_time_(0) {
  test_info_list_ = new internal::List<TestInfo *>;
}

// Destructor of TestCase.
TestCase::~TestCase() {
  // Deletes every Test in the collection.
  test_info_list_->ForEach(internal::Delete<TestInfo>);

  // Then deletes the Test collection.
  delete test_info_list_;
  test_info_list_ = NULL;
}

// Adds a test to this test case.  Will delete the test upon
// destruction of the TestCase object.
void TestCase::AddTestInfo(TestInfo * test_info) {
  test_info_list_->PushBack(test_info);
}

// Runs every test in this TestCase.
void TestCase::Run() {
  if (!should_run_) return;

  internal::UnitTestImpl* const impl = internal::GetUnitTestImpl();
  impl->set_current_test_case(this);

  UnitTestEventListenerInterface * const result_printer =
      impl->result_printer();

  result_printer->OnTestCaseStart(this);
  impl->os_stack_trace_getter()->UponLeavingGTest();
  set_up_tc_();

  const internal::TimeInMillis start = internal::GetTimeInMillis();
  test_info_list_->ForEach(internal::TestInfoImpl::RunTest);
  elapsed_time_ = internal::GetTimeInMillis() - start;

  impl->os_stack_trace_getter()->UponLeavingGTest();
  tear_down_tc_();
  result_printer->OnTestCaseEnd(this);
  impl->set_current_test_case(NULL);
}

// Clears the results of all tests in this test case.
void TestCase::ClearResult() {
  test_info_list_->ForEach(internal::TestInfoImpl::ClearTestResult);
}


// class UnitTestEventListenerInterface

// The virtual d'tor.
UnitTestEventListenerInterface::~UnitTestEventListenerInterface() {
}

// A result printer that never prints anything.  Used in the child process
// of an exec-style death test to avoid needless output clutter.
class NullUnitTestResultPrinter : public UnitTestEventListenerInterface {};

// Formats a countable noun.  Depending on its quantity, either the
// singular form or the plural form is used. e.g.
//
// FormatCountableNoun(1, "formula", "formuli") returns "1 formula".
// FormatCountableNoun(5, "book", "books") returns "5 books".
static internal::String FormatCountableNoun(int count,
                                            const char * singular_form,
                                            const char * plural_form) {
  return internal::String::Format("%d %s", count,
                                  count == 1 ? singular_form : plural_form);
}

// Formats the count of tests.
static internal::String FormatTestCount(int test_count) {
  return FormatCountableNoun(test_count, "test", "tests");
}

// Formats the count of test cases.
static internal::String FormatTestCaseCount(int test_case_count) {
  return FormatCountableNoun(test_case_count, "test case", "test cases");
}

// Converts a TestPartResultType enum to human-friendly string
// representation.  Both TPRT_NONFATAL_FAILURE and TPRT_FATAL_FAILURE
// are translated to "Failure", as the user usually doesn't care about
// the difference between the two when viewing the test result.
static const char * TestPartResultTypeToString(TestPartResultType type) {
  switch (type) {
    case TPRT_SUCCESS:
      return "Success";

    case TPRT_NONFATAL_FAILURE:
    case TPRT_FATAL_FAILURE:
#ifdef _MSC_VER
      return "error: ";
#else
      return "Failure\n";
#endif
  }

  return "Unknown result type";
}

// Prints a TestPartResult to a String.
static internal::String PrintTestPartResultToString(
    const TestPartResult& test_part_result) {
  return (Message()
          << internal::FormatFileLocation(test_part_result.file_name(),
                                          test_part_result.line_number())
          << " " << TestPartResultTypeToString(test_part_result.type())
          << test_part_result.message()).GetString();
}

// Prints a TestPartResult.
static void PrintTestPartResult(
    const TestPartResult& test_part_result) {
  printf("%s\n", PrintTestPartResultToString(test_part_result).c_str());
  fflush(stdout);
}

// class PrettyUnitTestResultPrinter

namespace internal {

enum GTestColor {
  COLOR_DEFAULT,
  COLOR_RED,
  COLOR_GREEN,
  COLOR_YELLOW
};

#if GTEST_OS_WINDOWS && !defined(_WIN32_WCE)

// Returns the character attribute for the given color.
WORD GetColorAttribute(GTestColor color) {
  switch (color) {
    case COLOR_RED:    return FOREGROUND_RED;
    case COLOR_GREEN:  return FOREGROUND_GREEN;
    case COLOR_YELLOW: return FOREGROUND_RED | FOREGROUND_GREEN;
    default:           return 0;
  }
}

#else

// Returns the ANSI color code for the given color.  COLOR_DEFAULT is
// an invalid input.
const char* GetAnsiColorCode(GTestColor color) {
  switch (color) {
    case COLOR_RED:     return "1";
    case COLOR_GREEN:   return "2";
    case COLOR_YELLOW:  return "3";
    default:            return NULL;
  };
}

#endif  // GTEST_OS_WINDOWS && !_WIN32_WCE

// Returns true iff Google Test should use colors in the output.
bool ShouldUseColor(bool stdout_is_tty) {
  const char* const gtest_color = GTEST_FLAG(color).c_str();

  if (String::CaseInsensitiveCStringEquals(gtest_color, "auto")) {
#if GTEST_OS_WINDOWS
    // On Windows the TERM variable is usually not set, but the
    // console there does support colors.
    return stdout_is_tty;
#else
    // On non-Windows platforms, we rely on the TERM variable.
    const char* const term = GetEnv("TERM");
    const bool term_supports_color =
        String::CStringEquals(term, "xterm") ||
        String::CStringEquals(term, "xterm-color") ||
        String::CStringEquals(term, "cygwin");
    return stdout_is_tty && term_supports_color;
#endif  // GTEST_OS_WINDOWS
  }

  return String::CaseInsensitiveCStringEquals(gtest_color, "yes") ||
      String::CaseInsensitiveCStringEquals(gtest_color, "true") ||
      String::CaseInsensitiveCStringEquals(gtest_color, "t") ||
      String::CStringEquals(gtest_color, "1");
  // We take "yes", "true", "t", and "1" as meaning "yes".  If the
  // value is neither one of these nor "auto", we treat it as "no" to
  // be conservative.
}

// Helpers for printing colored strings to stdout. Note that on Windows, we
// cannot simply emit special characters and have the terminal change colors.
// This routine must actually emit the characters rather than return a string
// that would be colored when printed, as can be done on Linux.
void ColoredPrintf(GTestColor color, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

#if defined(_WIN32_WCE) || GTEST_OS_SYMBIAN || GTEST_OS_ZOS
  const bool use_color = false;
#else
  static const bool in_color_mode =
      ShouldUseColor(isatty(fileno(stdout)) != 0);
  const bool use_color = in_color_mode && (color != COLOR_DEFAULT);
#endif  // defined(_WIN32_WCE) || GTEST_OS_SYMBIAN || GTEST_OS_ZOS
  // The '!= 0' comparison is necessary to satisfy MSVC 7.1.

  if (!use_color) {
    vprintf(fmt, args);
    va_end(args);
    return;
  }

#if GTEST_OS_WINDOWS && !defined(_WIN32_WCE)
  const HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

  // Gets the current text color.
  CONSOLE_SCREEN_BUFFER_INFO buffer_info;
  GetConsoleScreenBufferInfo(stdout_handle, &buffer_info);
  const WORD old_color_attrs = buffer_info.wAttributes;

  SetConsoleTextAttribute(stdout_handle,
                          GetColorAttribute(color) | FOREGROUND_INTENSITY);
  vprintf(fmt, args);

  // Restores the text color.
  SetConsoleTextAttribute(stdout_handle, old_color_attrs);
#else
  printf("\033[0;3%sm", GetAnsiColorCode(color));
  vprintf(fmt, args);
  printf("\033[m");  // Resets the terminal to default.
#endif  // GTEST_OS_WINDOWS && !defined(_WIN32_WCE)
  va_end(args);
}

}  // namespace internal

using internal::ColoredPrintf;
using internal::COLOR_DEFAULT;
using internal::COLOR_RED;
using internal::COLOR_GREEN;
using internal::COLOR_YELLOW;

// This class implements the UnitTestEventListenerInterface interface.
//
// Class PrettyUnitTestResultPrinter is copyable.
class PrettyUnitTestResultPrinter : public UnitTestEventListenerInterface {
 public:
  PrettyUnitTestResultPrinter() {}
  static void PrintTestName(const char * test_case, const char * test) {
    printf("%s.%s", test_case, test);
  }

  // The following methods override what's in the
  // UnitTestEventListenerInterface class.
  virtual void OnUnitTestStart(const UnitTest * unit_test);
  virtual void OnGlobalSetUpStart(const UnitTest*);
  virtual void OnTestCaseStart(const TestCase * test_case);
  virtual void OnTestCaseEnd(const TestCase * test_case);
  virtual void OnTestStart(const TestInfo * test_info);
  virtual void OnNewTestPartResult(const TestPartResult * result);
  virtual void OnTestEnd(const TestInfo * test_info);
  virtual void OnGlobalTearDownStart(const UnitTest*);
  virtual void OnUnitTestEnd(const UnitTest * unit_test);

 private:
  internal::String test_case_name_;
};

// Called before the unit test starts.
void PrettyUnitTestResultPrinter::OnUnitTestStart(
    const UnitTest * unit_test) {
  const char * const filter = GTEST_FLAG(filter).c_str();

  // Prints the filter if it's not *.  This reminds the user that some
  // tests may be skipped.
  if (!internal::String::CStringEquals(filter, kUniversalFilter)) {
    ColoredPrintf(COLOR_YELLOW,
                  "Note: %s filter = %s\n", GTEST_NAME_, filter);
  }

  if (internal::ShouldShard(kTestTotalShards, kTestShardIndex, false)) {
    ColoredPrintf(COLOR_YELLOW,
                  "Note: This is test shard %s of %s.\n",
                  internal::GetEnv(kTestShardIndex),
                  internal::GetEnv(kTestTotalShards));
  }

  const internal::UnitTestImpl* const impl = unit_test->impl();
  ColoredPrintf(COLOR_GREEN,  "[==========] ");
  printf("Running %s from %s.\n",
         FormatTestCount(impl->test_to_run_count()).c_str(),
         FormatTestCaseCount(impl->test_case_to_run_count()).c_str());
  fflush(stdout);
}

void PrettyUnitTestResultPrinter::OnGlobalSetUpStart(const UnitTest*) {
  ColoredPrintf(COLOR_GREEN,  "[----------] ");
  printf("Global test environment set-up.\n");
  fflush(stdout);
}

void PrettyUnitTestResultPrinter::OnTestCaseStart(
    const TestCase * test_case) {
  test_case_name_ = test_case->name();
  const internal::String counts =
      FormatCountableNoun(test_case->test_to_run_count(), "test", "tests");
  ColoredPrintf(COLOR_GREEN, "[----------] ");
  printf("%s from %s", counts.c_str(), test_case_name_.c_str());
  if (test_case->comment()[0] == '\0') {
    printf("\n");
  } else {
    printf(", where %s\n", test_case->comment());
  }
  fflush(stdout);
}

void PrettyUnitTestResultPrinter::OnTestCaseEnd(
    const TestCase * test_case) {
  if (!GTEST_FLAG(print_time)) return;

  test_case_name_ = test_case->name();
  const internal::String counts =
      FormatCountableNoun(test_case->test_to_run_count(), "test", "tests");
  ColoredPrintf(COLOR_GREEN, "[----------] ");
  printf("%s from %s (%s ms total)\n\n",
         counts.c_str(), test_case_name_.c_str(),
         internal::StreamableToString(test_case->elapsed_time()).c_str());
  fflush(stdout);
}

void PrettyUnitTestResultPrinter::OnTestStart(const TestInfo * test_info) {
  ColoredPrintf(COLOR_GREEN,  "[ RUN      ] ");
  PrintTestName(test_case_name_.c_str(), test_info->name());
  if (test_info->comment()[0] == '\0') {
    printf("\n");
  } else {
    printf(", where %s\n", test_info->comment());
  }
  fflush(stdout);
}

void PrettyUnitTestResultPrinter::OnTestEnd(const TestInfo * test_info) {
  if (test_info->result()->Passed()) {
    ColoredPrintf(COLOR_GREEN, "[       OK ] ");
  } else {
    ColoredPrintf(COLOR_RED, "[  FAILED  ] ");
  }
  PrintTestName(test_case_name_.c_str(), test_info->name());
  if (GTEST_FLAG(print_time)) {
    printf(" (%s ms)\n", internal::StreamableToString(
           test_info->result()->elapsed_time()).c_str());
  } else {
    printf("\n");
  }
  fflush(stdout);
}

// Called after an assertion failure.
void PrettyUnitTestResultPrinter::OnNewTestPartResult(
    const TestPartResult * result) {
  // If the test part succeeded, we don't need to do anything.
  if (result->type() == TPRT_SUCCESS)
    return;

  // Print failure message from the assertion (e.g. expected this and got that).
  PrintTestPartResult(*result);
  fflush(stdout);
}

void PrettyUnitTestResultPrinter::OnGlobalTearDownStart(const UnitTest*) {
  ColoredPrintf(COLOR_GREEN,  "[----------] ");
  printf("Global test environment tear-down\n");
  fflush(stdout);
}

namespace internal {

// Internal helper for printing the list of failed tests.
static void PrintFailedTestsPretty(const UnitTestImpl* impl) {
  const int failed_test_count = impl->failed_test_count();
  if (failed_test_count == 0) {
    return;
  }

  for (const internal::ListNode<TestCase*>* node = impl->test_cases()->Head();
       node != NULL; node = node->next()) {
    const TestCase* const tc = node->element();
    if (!tc->should_run() || (tc->failed_test_count() == 0)) {
      continue;
    }
    for (const internal::ListNode<TestInfo*>* tinode =
         tc->test_info_list().Head();
         tinode != NULL; tinode = tinode->next()) {
      const TestInfo* const ti = tinode->element();
      if (!tc->ShouldRunTest(ti) || tc->TestPassed(ti)) {
        continue;
      }
      ColoredPrintf(COLOR_RED, "[  FAILED  ] ");
      printf("%s.%s", ti->test_case_name(), ti->name());
      if (ti->test_case_comment()[0] != '\0' ||
          ti->comment()[0] != '\0') {
        printf(", where %s", ti->test_case_comment());
        if (ti->test_case_comment()[0] != '\0' &&
            ti->comment()[0] != '\0') {
          printf(" and ");
        }
      }
      printf("%s\n", ti->comment());
    }
  }
}

}  // namespace internal

void PrettyUnitTestResultPrinter::OnUnitTestEnd(
    const UnitTest * unit_test) {
  const internal::UnitTestImpl* const impl = unit_test->impl();

  ColoredPrintf(COLOR_GREEN,  "[==========] ");
  printf("%s from %s ran.",
         FormatTestCount(impl->test_to_run_count()).c_str(),
         FormatTestCaseCount(impl->test_case_to_run_count()).c_str());
  if (GTEST_FLAG(print_time)) {
    printf(" (%s ms total)",
           internal::StreamableToString(impl->elapsed_time()).c_str());
  }
  printf("\n");
  ColoredPrintf(COLOR_GREEN,  "[  PASSED  ] ");
  printf("%s.\n", FormatTestCount(impl->successful_test_count()).c_str());

  int num_failures = impl->failed_test_count();
  if (!impl->Passed()) {
    const int failed_test_count = impl->failed_test_count();
    ColoredPrintf(COLOR_RED,  "[  FAILED  ] ");
    printf("%s, listed below:\n", FormatTestCount(failed_test_count).c_str());
    internal::PrintFailedTestsPretty(impl);
    printf("\n%2d FAILED %s\n", num_failures,
                        num_failures == 1 ? "TEST" : "TESTS");
  }

  int num_disabled = impl->disabled_test_count();
  if (num_disabled && !GTEST_FLAG(also_run_disabled_tests)) {
    if (!num_failures) {
      printf("\n");  // Add a spacer if no FAILURE banner is displayed.
    }
    ColoredPrintf(COLOR_YELLOW,
                  "  YOU HAVE %d DISABLED %s\n\n",
                  num_disabled,
                  num_disabled == 1 ? "TEST" : "TESTS");
  }
  // Ensure that Google Test output is printed before, e.g., heapchecker output.
  fflush(stdout);
}

// End PrettyUnitTestResultPrinter

// class UnitTestEventsRepeater
//
// This class forwards events to other event listeners.
class UnitTestEventsRepeater : public UnitTestEventListenerInterface {
 public:
  typedef internal::List<UnitTestEventListenerInterface *> Listeners;
  typedef internal::ListNode<UnitTestEventListenerInterface *> ListenersNode;
  UnitTestEventsRepeater() {}
  virtual ~UnitTestEventsRepeater();
  void AddListener(UnitTestEventListenerInterface *listener);

  virtual void OnUnitTestStart(const UnitTest* unit_test);
  virtual void OnUnitTestEnd(const UnitTest* unit_test);
  virtual void OnGlobalSetUpStart(const UnitTest* unit_test);
  virtual void OnGlobalSetUpEnd(const UnitTest* unit_test);
  virtual void OnGlobalTearDownStart(const UnitTest* unit_test);
  virtual void OnGlobalTearDownEnd(const UnitTest* unit_test);
  virtual void OnTestCaseStart(const TestCase* test_case);
  virtual void OnTestCaseEnd(const TestCase* test_case);
  virtual void OnTestStart(const TestInfo* test_info);
  virtual void OnTestEnd(const TestInfo* test_info);
  virtual void OnNewTestPartResult(const TestPartResult* result);

 private:
  Listeners listeners_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(UnitTestEventsRepeater);
};

UnitTestEventsRepeater::~UnitTestEventsRepeater() {
  for (ListenersNode* listener = listeners_.Head();
       listener != NULL;
       listener = listener->next()) {
    delete listener->element();
  }
}

void UnitTestEventsRepeater::AddListener(
    UnitTestEventListenerInterface *listener) {
  listeners_.PushBack(listener);
}

// Since the methods are identical, use a macro to reduce boilerplate.
// This defines a member that repeats the call to all listeners.
#define GTEST_REPEATER_METHOD_(Name, Type) \
void UnitTestEventsRepeater::Name(const Type* parameter) { \
  for (ListenersNode* listener = listeners_.Head(); \
       listener != NULL; \
       listener = listener->next()) { \
    listener->element()->Name(parameter); \
  } \
}

GTEST_REPEATER_METHOD_(OnUnitTestStart, UnitTest)
GTEST_REPEATER_METHOD_(OnUnitTestEnd, UnitTest)
GTEST_REPEATER_METHOD_(OnGlobalSetUpStart, UnitTest)
GTEST_REPEATER_METHOD_(OnGlobalSetUpEnd, UnitTest)
GTEST_REPEATER_METHOD_(OnGlobalTearDownStart, UnitTest)
GTEST_REPEATER_METHOD_(OnGlobalTearDownEnd, UnitTest)
GTEST_REPEATER_METHOD_(OnTestCaseStart, TestCase)
GTEST_REPEATER_METHOD_(OnTestCaseEnd, TestCase)
GTEST_REPEATER_METHOD_(OnTestStart, TestInfo)
GTEST_REPEATER_METHOD_(OnTestEnd, TestInfo)
GTEST_REPEATER_METHOD_(OnNewTestPartResult, TestPartResult)

#undef GTEST_REPEATER_METHOD_

// End PrettyUnitTestResultPrinter

// This class generates an XML output file.
class XmlUnitTestResultPrinter : public UnitTestEventListenerInterface {
 public:
  explicit XmlUnitTestResultPrinter(const char* output_file);

  virtual void OnUnitTestEnd(const UnitTest* unit_test);

 private:
  // Is c a whitespace character that is normalized to a space character
  // when it appears in an XML attribute value?
  static bool IsNormalizableWhitespace(char c) {
    return c == 0x9 || c == 0xA || c == 0xD;
  }

  // May c appear in a well-formed XML document?
  static bool IsValidXmlCharacter(char c) {
    return IsNormalizableWhitespace(c) || c >= 0x20;
  }

  // Returns an XML-escaped copy of the input string str.  If
  // is_attribute is true, the text is meant to appear as an attribute
  // value, and normalizable whitespace is preserved by replacing it
  // with character references.
  static internal::String EscapeXml(const char* str,
                                    bool is_attribute);

  // Convenience wrapper around EscapeXml when str is an attribute value.
  static internal::String EscapeXmlAttribute(const char* str) {
    return EscapeXml(str, true);
  }

  // Convenience wrapper around EscapeXml when str is not an attribute value.
  static internal::String EscapeXmlText(const char* str) {
    return EscapeXml(str, false);
  }

  // Prints an XML representation of a TestInfo object.
  static void PrintXmlTestInfo(FILE* out,
                               const char* test_case_name,
                               const TestInfo* test_info);

  // Prints an XML representation of a TestCase object
  static void PrintXmlTestCase(FILE* out, const TestCase* test_case);

  // Prints an XML summary of unit_test to output stream out.
  static void PrintXmlUnitTest(FILE* out, const UnitTest* unit_test);

  // Produces a string representing the test properties in a result as space
  // delimited XML attributes based on the property key="value" pairs.
  // When the String is not empty, it includes a space at the beginning,
  // to delimit this attribute from prior attributes.
  static internal::String TestPropertiesAsXmlAttributes(
      const internal::TestResult* result);

  // The output file.
  const internal::String output_file_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(XmlUnitTestResultPrinter);
};

// Creates a new XmlUnitTestResultPrinter.
XmlUnitTestResultPrinter::XmlUnitTestResultPrinter(const char* output_file)
    : output_file_(output_file) {
  if (output_file_.c_str() == NULL || output_file_.empty()) {
    fprintf(stderr, "XML output file may not be null\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
}

// Called after the unit test ends.
void XmlUnitTestResultPrinter::OnUnitTestEnd(const UnitTest* unit_test) {
  FILE* xmlout = NULL;
  internal::FilePath output_file(output_file_);
  internal::FilePath output_dir(output_file.RemoveFileName());

  if (output_dir.CreateDirectoriesRecursively()) {
  // MSVC 8 deprecates fopen(), so we want to suppress warning 4996
  // (deprecated function) there.
#if GTEST_OS_WINDOWS
  // We are on Windows.
#pragma warning(push)          // Saves the current warning state.
#pragma warning(disable:4996)  // Temporarily disables warning 4996.
    xmlout = fopen(output_file_.c_str(), "w");
#pragma warning(pop)           // Restores the warning state.
#else  // We are on Linux or Mac OS.
    xmlout = fopen(output_file_.c_str(), "w");
#endif  // GTEST_OS_WINDOWS
  }
  if (xmlout == NULL) {
    // TODO(wan): report the reason of the failure.
    //
    // We don't do it for now as:
    //
    //   1. There is no urgent need for it.
    //   2. It's a bit involved to make the errno variable thread-safe on
    //      all three operating systems (Linux, Windows, and Mac OS).
    //   3. To interpret the meaning of errno in a thread-safe way,
    //      we need the strerror_r() function, which is not available on
    //      Windows.
    fprintf(stderr,
            "Unable to open file \"%s\"\n",
            output_file_.c_str());
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  PrintXmlUnitTest(xmlout, unit_test);
  fclose(xmlout);
}

// Returns an XML-escaped copy of the input string str.  If is_attribute
// is true, the text is meant to appear as an attribute value, and
// normalizable whitespace is preserved by replacing it with character
// references.
//
// Invalid XML characters in str, if any, are stripped from the output.
// It is expected that most, if not all, of the text processed by this
// module will consist of ordinary English text.
// If this module is ever modified to produce version 1.1 XML output,
// most invalid characters can be retained using character references.
// TODO(wan): It might be nice to have a minimally invasive, human-readable
// escaping scheme for invalid characters, rather than dropping them.
internal::String XmlUnitTestResultPrinter::EscapeXml(const char* str,
                                                     bool is_attribute) {
  Message m;

  if (str != NULL) {
    for (const char* src = str; *src; ++src) {
      switch (*src) {
        case '<':
          m << "&lt;";
          break;
        case '>':
          m << "&gt;";
          break;
        case '&':
          m << "&amp;";
          break;
        case '\'':
          if (is_attribute)
            m << "&apos;";
          else
            m << '\'';
          break;
        case '"':
          if (is_attribute)
            m << "&quot;";
          else
            m << '"';
          break;
        default:
          if (IsValidXmlCharacter(*src)) {
            if (is_attribute && IsNormalizableWhitespace(*src))
              m << internal::String::Format("&#x%02X;", unsigned(*src));
            else
              m << *src;
          }
          break;
      }
    }
  }

  return m.GetString();
}


// The following routines generate an XML representation of a UnitTest
// object.
//
// This is how Google Test concepts map to the DTD:
//
// <testsuite name="AllTests">         <-- corresponds to a UnitTest object
//   <testsuite name="testcase-name">  <-- corresponds to a TestCase object
//     <testcase name="test-name">     <-- corresponds to a TestInfo object
//       <failure message="...">...</failure>
//       <failure message="...">...</failure>
//       <failure message="...">...</failure>
//                                     <-- individual assertion failures
//     </testcase>
//   </testsuite>
// </testsuite>

namespace internal {

// Formats the given time in milliseconds as seconds.  The returned
// C-string is owned by this function and cannot be released by the
// caller.  Calling the function again invalidates the previous
// result.
const char* FormatTimeInMillisAsSeconds(TimeInMillis ms) {
  static String str;
  str = (Message() << (ms/1000.0)).GetString();
  return str.c_str();
}

}  // namespace internal

// Prints an XML representation of a TestInfo object.
// TODO(wan): There is also value in printing properties with the plain printer.
void XmlUnitTestResultPrinter::PrintXmlTestInfo(FILE* out,
                                                const char* test_case_name,
                                                const TestInfo* test_info) {
  const internal::TestResult * const result = test_info->result();
  const internal::List<TestPartResult> &results = result->test_part_results();
  fprintf(out,
          "    <testcase name=\"%s\" status=\"%s\" time=\"%s\" "
          "classname=\"%s\"%s",
          EscapeXmlAttribute(test_info->name()).c_str(),
          test_info->should_run() ? "run" : "notrun",
          internal::FormatTimeInMillisAsSeconds(result->elapsed_time()),
          EscapeXmlAttribute(test_case_name).c_str(),
          TestPropertiesAsXmlAttributes(result).c_str());

  int failures = 0;
  for (const internal::ListNode<TestPartResult>* part_node = results.Head();
       part_node != NULL;
       part_node = part_node->next()) {
    const TestPartResult& part = part_node->element();
    if (part.failed()) {
      const internal::String message =
          internal::String::Format("%s:%d\n%s", part.file_name(),
                                   part.line_number(), part.message());
      if (++failures == 1)
        fprintf(out, ">\n");
      fprintf(out,
              "      <failure message=\"%s\" type=\"\"><![CDATA[%s]]>"
              "</failure>\n",
              EscapeXmlAttribute(part.summary()).c_str(), message.c_str());
    }
  }

  if (failures == 0)
    fprintf(out, " />\n");
  else
    fprintf(out, "    </testcase>\n");
}

// Prints an XML representation of a TestCase object
void XmlUnitTestResultPrinter::PrintXmlTestCase(FILE* out,
                                                const TestCase* test_case) {
  fprintf(out,
          "  <testsuite name=\"%s\" tests=\"%d\" failures=\"%d\" "
          "disabled=\"%d\" ",
          EscapeXmlAttribute(test_case->name()).c_str(),
          test_case->total_test_count(),
          test_case->failed_test_count(),
          test_case->disabled_test_count());
  fprintf(out,
          "errors=\"0\" time=\"%s\">\n",
          internal::FormatTimeInMillisAsSeconds(test_case->elapsed_time()));
  for (const internal::ListNode<TestInfo*>* info_node =
         test_case->test_info_list().Head();
       info_node != NULL;
       info_node = info_node->next()) {
    PrintXmlTestInfo(out, test_case->name(), info_node->element());
  }
  fprintf(out, "  </testsuite>\n");
}

// Prints an XML summary of unit_test to output stream out.
void XmlUnitTestResultPrinter::PrintXmlUnitTest(FILE* out,
                                                const UnitTest* unit_test) {
  const internal::UnitTestImpl* const impl = unit_test->impl();
  fprintf(out, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  fprintf(out,
          "<testsuite tests=\"%d\" failures=\"%d\" disabled=\"%d\" "
          "errors=\"0\" time=\"%s\" ",
          impl->total_test_count(),
          impl->failed_test_count(),
          impl->disabled_test_count(),
          internal::FormatTimeInMillisAsSeconds(impl->elapsed_time()));
  fprintf(out, "name=\"AllTests\">\n");
  for (const internal::ListNode<TestCase*>* case_node =
       impl->test_cases()->Head();
       case_node != NULL;
       case_node = case_node->next()) {
    PrintXmlTestCase(out, case_node->element());
  }
  fprintf(out, "</testsuite>\n");
}

// Produces a string representing the test properties in a result as space
// delimited XML attributes based on the property key="value" pairs.
internal::String XmlUnitTestResultPrinter::TestPropertiesAsXmlAttributes(
    const internal::TestResult* result) {
  using internal::TestProperty;
  Message attributes;
  const internal::List<TestProperty>& properties = result->test_properties();
  for (const internal::ListNode<TestProperty>* property_node =
       properties.Head();
       property_node != NULL;
       property_node = property_node->next()) {
    const TestProperty& property = property_node->element();
    attributes << " " << property.key() << "="
        << "\"" << EscapeXmlAttribute(property.value()) << "\"";
  }
  return attributes.GetString();
}

// End XmlUnitTestResultPrinter

namespace internal {

// Class ScopedTrace

// Pushes the given source file location and message onto a per-thread
// trace stack maintained by Google Test.
// L < UnitTest::mutex_
ScopedTrace::ScopedTrace(const char* file, int line, const Message& message) {
  TraceInfo trace;
  trace.file = file;
  trace.line = line;
  trace.message = message.GetString();

  UnitTest::GetInstance()->PushGTestTrace(trace);
}

// Pops the info pushed by the c'tor.
// L < UnitTest::mutex_
ScopedTrace::~ScopedTrace() {
  UnitTest::GetInstance()->PopGTestTrace();
}


// class OsStackTraceGetter

// Returns the current OS stack trace as a String.  Parameters:
//
//   max_depth  - the maximum number of stack frames to be included
//                in the trace.
//   skip_count - the number of top frames to be skipped; doesn't count
//                against max_depth.
//
// L < mutex_
// We use "L < mutex_" to denote that the function may acquire mutex_.
String OsStackTraceGetter::CurrentStackTrace(int, int) {
  return String("");
}

// L < mutex_
void OsStackTraceGetter::UponLeavingGTest() {
}

const char* const
OsStackTraceGetter::kElidedFramesMarker =
    "... " GTEST_NAME_ " internal frames ...";

}  // namespace internal

// class UnitTest

// Gets the singleton UnitTest object.  The first time this method is
// called, a UnitTest object is constructed and returned.  Consecutive
// calls will return the same object.
//
// We don't protect this under mutex_ as a user is not supposed to
// call this before main() starts, from which point on the return
// value will never change.
UnitTest * UnitTest::GetInstance() {
  // When compiled with MSVC 7.1 in optimized mode, destroying the
  // UnitTest object upon exiting the program messes up the exit code,
  // causing successful tests to appear failed.  We have to use a
  // different implementation in this case to bypass the compiler bug.
  // This implementation makes the compiler happy, at the cost of
  // leaking the UnitTest object.
#if _MSC_VER == 1310 && !defined(_DEBUG)  // MSVC 7.1 and optimized build.
  static UnitTest* const instance = new UnitTest;
  return instance;
#else
  static UnitTest instance;
  return &instance;
#endif  // _MSC_VER==1310 && !defined(_DEBUG)
}

// Registers and returns a global test environment.  When a test
// program is run, all global test environments will be set-up in the
// order they were registered.  After all tests in the program have
// finished, all global test environments will be torn-down in the
// *reverse* order they were registered.
//
// The UnitTest object takes ownership of the given environment.
//
// We don't protect this under mutex_, as we only support calling it
// from the main thread.
Environment* UnitTest::AddEnvironment(Environment* env) {
  if (env == NULL) {
    return NULL;
  }

  impl_->environments()->PushBack(env);
  impl_->environments_in_reverse_order()->PushFront(env);
  return env;
}

#if GTEST_HAS_EXCEPTIONS
// A failed Google Test assertion will throw an exception of this type
// when exceptions are enabled.  We derive it from std::runtime_error,
// which is for errors presumably detectable only at run time.  Since
// std::runtime_error inherits from std::exception, many testing
// frameworks know how to extract and print the message inside it.
class GoogleTestFailureException : public ::std::runtime_error {
 public:
  explicit GoogleTestFailureException(const TestPartResult& failure)
      : runtime_error(PrintTestPartResultToString(failure).c_str()) {}
};
#endif

// Adds a TestPartResult to the current TestResult object.  All Google Test
// assertion macros (e.g. ASSERT_TRUE, EXPECT_EQ, etc) eventually call
// this to report their results.  The user code should use the
// assertion macros instead of calling this directly.
// L < mutex_
void UnitTest::AddTestPartResult(TestPartResultType result_type,
                                 const char* file_name,
                                 int line_number,
                                 const internal::String& message,
                                 const internal::String& os_stack_trace) {
  Message msg;
  msg << message;

  internal::MutexLock lock(&mutex_);
  if (impl_->gtest_trace_stack()->size() > 0) {
    msg << "\n" << GTEST_NAME_ << " trace:";

    for (internal::ListNode<internal::TraceInfo>* node =
         impl_->gtest_trace_stack()->Head();
         node != NULL;
         node = node->next()) {
      const internal::TraceInfo& trace = node->element();
      msg << "\n" << trace.file << ":" << trace.line << ": " << trace.message;
    }
  }

  if (os_stack_trace.c_str() != NULL && !os_stack_trace.empty()) {
    msg << internal::kStackTraceMarker << os_stack_trace;
  }

  const TestPartResult result =
    TestPartResult(result_type, file_name, line_number,
                   msg.GetString().c_str());
  impl_->GetTestPartResultReporterForCurrentThread()->
      ReportTestPartResult(result);

  if (result_type != TPRT_SUCCESS) {
    // gunit_break_on_failure takes precedence over
    // gunit_throw_on_failure.  This allows a user to set the latter
    // in the code (perhaps in order to use Google Test assertions
    // with another testing framework) and specify the former on the
    // command line for debugging.
    if (GTEST_FLAG(break_on_failure)) {
      *static_cast<int*>(NULL) = 1;
    } else if (GTEST_FLAG(throw_on_failure)) {
#if GTEST_HAS_EXCEPTIONS
      throw GoogleTestFailureException(result);
#else
      // We cannot call abort() as it generates a pop-up in debug mode
      // that cannot be suppressed in VC 7.1 or below.
      exit(1);
#endif
    }
  }
}

// Creates and adds a property to the current TestResult. If a property matching
// the supplied value already exists, updates its value instead.
void UnitTest::RecordPropertyForCurrentTest(const char* key,
                                            const char* value) {
  const internal::TestProperty test_property(key, value);
  impl_->current_test_result()->RecordProperty(test_property);
}

// Runs all tests in this UnitTest object and prints the result.
// Returns 0 if successful, or 1 otherwise.
//
// We don't protect this under mutex_, as we only support calling it
// from the main thread.
int UnitTest::Run() {
#if GTEST_OS_WINDOWS

  const bool in_death_test_child_process =
      internal::GTEST_FLAG(internal_run_death_test).GetLength() > 0;

  // Either the user wants Google Test to catch exceptions thrown by the
  // tests or this is executing in the context of death test child
  // process. In either case the user does not want to see pop-up dialogs
  // about crashes - they are expected..
  if (GTEST_FLAG(catch_exceptions) || in_death_test_child_process) {
#if !defined(_WIN32_WCE)
    // SetErrorMode doesn't exist on CE.
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT |
                 SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
#endif  // _WIN32_WCE

    // Death test children can be terminated with _abort().  On Windows,
    // _abort() can show a dialog with a warning message.  This forces the
    // abort message to go to stderr instead.
    _set_error_mode(_OUT_TO_STDERR);

    // In the debug version, Visual Studio pops up a separate dialog
    // offering a choice to debug the aborted program. We need to suppress
    // this dialog or it will pop up for every EXPECT/ASSERT_DEATH statement
    // executed. Google Test will notify the user of any unexpected
    // failure via stderr.
#if _MSC_VER >= 1400
    // VC++ doesn't define _set_abort_behavior() prior to the version 8.0.
    // Users of prior VC versions shall suffer the agony and pain of
    // clicking through the countless debug dialogs.
    // TODO(vladl@google.com): find a way to suppress the abort dialog() in the
    // debug mode when compiled with VC 7.1 or lower.
    if (!GTEST_FLAG(break_on_failure))
      _set_abort_behavior(
          0x0,                                    // Clear the following flags:
          _WRITE_ABORT_MSG | _CALL_REPORTFAULT);  // pop-up window, core dump.
#endif  // _MSC_VER >= 1400
  }

  __try {
    return impl_->RunAllTests();
  } __except(internal::UnitTestOptions::GTestShouldProcessSEH(
      GetExceptionCode())) {
    printf("Exception thrown with code 0x%x.\nFAIL\n", GetExceptionCode());
    fflush(stdout);
    return 1;
  }

#else
  // We are on Linux or Mac OS.  There is no exception of any kind.

  return impl_->RunAllTests();
#endif  // GTEST_OS_WINDOWS
}

// Returns the working directory when the first TEST() or TEST_F() was
// executed.
const char* UnitTest::original_working_dir() const {
  return impl_->original_working_dir_.c_str();
}

// Returns the TestCase object for the test that's currently running,
// or NULL if no test is running.
// L < mutex_
const TestCase* UnitTest::current_test_case() const {
  internal::MutexLock lock(&mutex_);
  return impl_->current_test_case();
}

// Returns the TestInfo object for the test that's currently running,
// or NULL if no test is running.
// L < mutex_
const TestInfo* UnitTest::current_test_info() const {
  internal::MutexLock lock(&mutex_);
  return impl_->current_test_info();
}

#if GTEST_HAS_PARAM_TEST
// Returns ParameterizedTestCaseRegistry object used to keep track of
// value-parameterized tests and instantiate and register them.
// L < mutex_
internal::ParameterizedTestCaseRegistry&
    UnitTest::parameterized_test_registry() {
  return impl_->parameterized_test_registry();
}
#endif  // GTEST_HAS_PARAM_TEST

// Creates an empty UnitTest.
UnitTest::UnitTest() {
  impl_ = new internal::UnitTestImpl(this);
}

// Destructor of UnitTest.
UnitTest::~UnitTest() {
  delete impl_;
}

// Pushes a trace defined by SCOPED_TRACE() on to the per-thread
// Google Test trace stack.
// L < mutex_
void UnitTest::PushGTestTrace(const internal::TraceInfo& trace) {
  internal::MutexLock lock(&mutex_);
  impl_->gtest_trace_stack()->PushFront(trace);
}

// Pops a trace from the per-thread Google Test trace stack.
// L < mutex_
void UnitTest::PopGTestTrace() {
  internal::MutexLock lock(&mutex_);
  impl_->gtest_trace_stack()->PopFront(NULL);
}

namespace internal {

UnitTestImpl::UnitTestImpl(UnitTest* parent)
    : parent_(parent),
#ifdef _MSC_VER
#pragma warning(push)                    // Saves the current warning state.
#pragma warning(disable:4355)            // Temporarily disables warning 4355
                                         // (using this in initializer).
      default_global_test_part_result_reporter_(this),
      default_per_thread_test_part_result_reporter_(this),
#pragma warning(pop)                     // Restores the warning state again.
#else
      default_global_test_part_result_reporter_(this),
      default_per_thread_test_part_result_reporter_(this),
#endif  // _MSC_VER
      global_test_part_result_repoter_(
          &default_global_test_part_result_reporter_),
      per_thread_test_part_result_reporter_(
          &default_per_thread_test_part_result_reporter_),
      test_cases_(),
#if GTEST_HAS_PARAM_TEST
      parameterized_test_registry_(),
      parameterized_tests_registered_(false),
#endif  // GTEST_HAS_PARAM_TEST
      last_death_test_case_(NULL),
      current_test_case_(NULL),
      current_test_info_(NULL),
      ad_hoc_test_result_(),
      result_printer_(NULL),
      os_stack_trace_getter_(NULL),
#if GTEST_HAS_DEATH_TEST
      elapsed_time_(0),
      internal_run_death_test_flag_(NULL),
      death_test_factory_(new DefaultDeathTestFactory) {
#else
      elapsed_time_(0) {
#endif  // GTEST_HAS_DEATH_TEST
}

UnitTestImpl::~UnitTestImpl() {
  // Deletes every TestCase.
  test_cases_.ForEach(internal::Delete<TestCase>);

  // Deletes every Environment.
  environments_.ForEach(internal::Delete<Environment>);

  // Deletes the current test result printer.
  delete result_printer_;

  delete os_stack_trace_getter_;
}

// A predicate that checks the name of a TestCase against a known
// value.
//
// This is used for implementation of the UnitTest class only.  We put
// it in the anonymous namespace to prevent polluting the outer
// namespace.
//
// TestCaseNameIs is copyable.
class TestCaseNameIs {
 public:
  // Constructor.
  explicit TestCaseNameIs(const String& name)
      : name_(name) {}

  // Returns true iff the name of test_case matches name_.
  bool operator()(const TestCase* test_case) const {
    return test_case != NULL && strcmp(test_case->name(), name_.c_str()) == 0;
  }

 private:
  String name_;
};

// Finds and returns a TestCase with the given name.  If one doesn't
// exist, creates one and returns it.
//
// Arguments:
//
//   test_case_name: name of the test case
//   set_up_tc:      pointer to the function that sets up the test case
//   tear_down_tc:   pointer to the function that tears down the test case
TestCase* UnitTestImpl::GetTestCase(const char* test_case_name,
                                    const char* comment,
                                    Test::SetUpTestCaseFunc set_up_tc,
                                    Test::TearDownTestCaseFunc tear_down_tc) {
  // Can we find a TestCase with the given name?
  internal::ListNode<TestCase*>* node = test_cases_.FindIf(
      TestCaseNameIs(test_case_name));

  if (node == NULL) {
    // No.  Let's create one.
    TestCase* const test_case =
      new TestCase(test_case_name, comment, set_up_tc, tear_down_tc);

    // Is this a death test case?
    if (internal::UnitTestOptions::MatchesFilter(String(test_case_name),
                                                 kDeathTestCaseFilter)) {
      // Yes.  Inserts the test case after the last death test case
      // defined so far.
      node = test_cases_.InsertAfter(last_death_test_case_, test_case);
      last_death_test_case_ = node;
    } else {
      // No.  Appends to the end of the list.
      test_cases_.PushBack(test_case);
      node = test_cases_.Last();
    }
  }

  // Returns the TestCase found.
  return node->element();
}

// Helpers for setting up / tearing down the given environment.  They
// are for use in the List::ForEach() method.
static void SetUpEnvironment(Environment* env) { env->SetUp(); }
static void TearDownEnvironment(Environment* env) { env->TearDown(); }

// Runs all tests in this UnitTest object, prints the result, and
// returns 0 if all tests are successful, or 1 otherwise.  If any
// exception is thrown during a test on Windows, this test is
// considered to be failed, but the rest of the tests will still be
// run.  (We disable exceptions on Linux and Mac OS X, so the issue
// doesn't apply there.)
// When parameterized tests are enabled, it explands and registers
// parameterized tests first in RegisterParameterizedTests().
// All other functions called from RunAllTests() may safely assume that
// parameterized tests are ready to be counted and run.
int UnitTestImpl::RunAllTests() {
  // Makes sure InitGoogleTest() was called.
  if (!GTestIsInitialized()) {
    printf("%s",
           "\nThis test program did NOT call ::testing::InitGoogleTest "
           "before calling RUN_ALL_TESTS().  Please fix it.\n");
    return 1;
  }

  // Do not run any test if the --help flag was specified.
  if (g_help_flag)
    return 0;

  RegisterParameterizedTests();

  // Even if sharding is not on, test runners may want to use the
  // GTEST_SHARD_STATUS_FILE to query whether the test supports the sharding
  // protocol.
  internal::WriteToShardStatusFileIfNeeded();

  // Lists all the tests and exits if the --gtest_list_tests
  // flag was specified.
  if (GTEST_FLAG(list_tests)) {
    ListAllTests();
    return 0;
  }

  // True iff we are in a subprocess for running a thread-safe-style
  // death test.
  bool in_subprocess_for_death_test = false;

#if GTEST_HAS_DEATH_TEST
  internal_run_death_test_flag_.reset(ParseInternalRunDeathTestFlag());
  in_subprocess_for_death_test = (internal_run_death_test_flag_.get() != NULL);
#endif  // GTEST_HAS_DEATH_TEST

  UnitTestEventListenerInterface * const printer = result_printer();

  const bool should_shard = ShouldShard(kTestTotalShards, kTestShardIndex,
                                        in_subprocess_for_death_test);

  // Compares the full test names with the filter to decide which
  // tests to run.
  const bool has_tests_to_run = FilterTests(should_shard
                                              ? HONOR_SHARDING_PROTOCOL
                                              : IGNORE_SHARDING_PROTOCOL) > 0;

  // True iff at least one test has failed.
  bool failed = false;

  // How many times to repeat the tests?  We don't want to repeat them
  // when we are inside the subprocess of a death test.
  const int repeat = in_subprocess_for_death_test ? 1 : GTEST_FLAG(repeat);
  // Repeats forever if the repeat count is negative.
  const bool forever = repeat < 0;
  for (int i = 0; forever || i != repeat; i++) {
    if (repeat != 1) {
      printf("\nRepeating all tests (iteration %d) . . .\n\n", i + 1);
    }

    // Tells the unit test event listener that the tests are about to
    // start.
    printer->OnUnitTestStart(parent_);

    const TimeInMillis start = GetTimeInMillis();

    // Runs each test case if there is at least one test to run.
    if (has_tests_to_run) {
      // Sets up all environments beforehand.
      printer->OnGlobalSetUpStart(parent_);
      environments_.ForEach(SetUpEnvironment);
      printer->OnGlobalSetUpEnd(parent_);

      // Runs the tests only if there was no fatal failure during global
      // set-up.
      if (!Test::HasFatalFailure()) {
        test_cases_.ForEach(TestCase::RunTestCase);
      }

      // Tears down all environments in reverse order afterwards.
      printer->OnGlobalTearDownStart(parent_);
      environments_in_reverse_order_.ForEach(TearDownEnvironment);
      printer->OnGlobalTearDownEnd(parent_);
    }

    elapsed_time_ = GetTimeInMillis() - start;

    // Tells the unit test event listener that the tests have just
    // finished.
    printer->OnUnitTestEnd(parent_);

    // Gets the result and clears it.
    if (!Passed()) {
      failed = true;
    }
    ClearResult();
  }

  // Returns 0 if all tests passed, or 1 other wise.
  return failed ? 1 : 0;
}

// Reads the GTEST_SHARD_STATUS_FILE environment variable, and creates the file
// if the variable is present. If a file already exists at this location, this
// function will write over it. If the variable is present, but the file cannot
// be created, prints an error and exits.
void WriteToShardStatusFileIfNeeded() {
  const char* const test_shard_file = GetEnv(kTestShardStatusFile);
  if (test_shard_file != NULL) {
#ifdef _MSC_VER  // MSVC 8 deprecates fopen().
#pragma warning(push)          // Saves the current warning state.
#pragma warning(disable:4996)  // Temporarily disables warning on
                               // deprecated functions.
#endif
    FILE* const file = fopen(test_shard_file, "w");
#ifdef _MSC_VER
#pragma warning(pop)           // Restores the warning state.
#endif
    if (file == NULL) {
      ColoredPrintf(COLOR_RED,
                    "Could not write to the test shard status file \"%s\" "
                    "specified by the %s environment variable.\n",
                    test_shard_file, kTestShardStatusFile);
      fflush(stdout);
      exit(EXIT_FAILURE);
    }
    fclose(file);
  }
}

// Checks whether sharding is enabled by examining the relevant
// environment variable values. If the variables are present,
// but inconsistent (i.e., shard_index >= total_shards), prints
// an error and exits. If in_subprocess_for_death_test, sharding is
// disabled because it must only be applied to the original test
// process. Otherwise, we could filter out death tests we intended to execute.
bool ShouldShard(const char* total_shards_env,
                 const char* shard_index_env,
                 bool in_subprocess_for_death_test) {
  if (in_subprocess_for_death_test) {
    return false;
  }

  const Int32 total_shards = Int32FromEnvOrDie(total_shards_env, -1);
  const Int32 shard_index = Int32FromEnvOrDie(shard_index_env, -1);

  if (total_shards == -1 && shard_index == -1) {
    return false;
  } else if (total_shards == -1 && shard_index != -1) {
    const Message msg = Message()
      << "Invalid environment variables: you have "
      << kTestShardIndex << " = " << shard_index
      << ", but have left " << kTestTotalShards << " unset.\n";
    ColoredPrintf(COLOR_RED, msg.GetString().c_str());
    fflush(stdout);
    exit(EXIT_FAILURE);
  } else if (total_shards != -1 && shard_index == -1) {
    const Message msg = Message()
      << "Invalid environment variables: you have "
      << kTestTotalShards << " = " << total_shards
      << ", but have left " << kTestShardIndex << " unset.\n";
    ColoredPrintf(COLOR_RED, msg.GetString().c_str());
    fflush(stdout);
    exit(EXIT_FAILURE);
  } else if (shard_index < 0 || shard_index >= total_shards) {
    const Message msg = Message()
      << "Invalid environment variables: we require 0 <= "
      << kTestShardIndex << " < " << kTestTotalShards
      << ", but you have " << kTestShardIndex << "=" << shard_index
      << ", " << kTestTotalShards << "=" << total_shards << ".\n";
    ColoredPrintf(COLOR_RED, msg.GetString().c_str());
    fflush(stdout);
    exit(EXIT_FAILURE);
  }

  return total_shards > 1;
}

// Parses the environment variable var as an Int32. If it is unset,
// returns default_val. If it is not an Int32, prints an error
// and aborts.
Int32 Int32FromEnvOrDie(const char* const var, Int32 default_val) {
  const char* str_val = GetEnv(var);
  if (str_val == NULL) {
    return default_val;
  }

  Int32 result;
  if (!ParseInt32(Message() << "The value of environment variable " << var,
                  str_val, &result)) {
    exit(EXIT_FAILURE);
  }
  return result;
}

// Given the total number of shards, the shard index, and the test id,
// returns true iff the test should be run on this shard. The test id is
// some arbitrary but unique non-negative integer assigned to each test
// method. Assumes that 0 <= shard_index < total_shards.
bool ShouldRunTestOnShard(int total_shards, int shard_index, int test_id) {
  return (test_id % total_shards) == shard_index;
}

// Compares the name of each test with the user-specified filter to
// decide whether the test should be run, then records the result in
// each TestCase and TestInfo object.
// If shard_tests == true, further filters tests based on sharding
// variables in the environment - see
// http://code.google.com/p/googletest/wiki/GoogleTestAdvancedGuide.
// Returns the number of tests that should run.
int UnitTestImpl::FilterTests(ReactionToSharding shard_tests) {
  const Int32 total_shards = shard_tests == HONOR_SHARDING_PROTOCOL ?
      Int32FromEnvOrDie(kTestTotalShards, -1) : -1;
  const Int32 shard_index = shard_tests == HONOR_SHARDING_PROTOCOL ?
      Int32FromEnvOrDie(kTestShardIndex, -1) : -1;

  // num_runnable_tests are the number of tests that will
  // run across all shards (i.e., match filter and are not disabled).
  // num_selected_tests are the number of tests to be run on
  // this shard.
  int num_runnable_tests = 0;
  int num_selected_tests = 0;
  for (const internal::ListNode<TestCase *> *test_case_node =
           test_cases_.Head();
       test_case_node != NULL;
       test_case_node = test_case_node->next()) {
    TestCase * const test_case = test_case_node->element();
    const String &test_case_name = test_case->name();
    test_case->set_should_run(false);

    for (const internal::ListNode<TestInfo *> *test_info_node =
             test_case->test_info_list().Head();
         test_info_node != NULL;
         test_info_node = test_info_node->next()) {
      TestInfo * const test_info = test_info_node->element();
      const String test_name(test_info->name());
      // A test is disabled if test case name or test name matches
      // kDisableTestFilter.
      const bool is_disabled =
          internal::UnitTestOptions::MatchesFilter(test_case_name,
                                                   kDisableTestFilter) ||
          internal::UnitTestOptions::MatchesFilter(test_name,
                                                   kDisableTestFilter);
      test_info->impl()->set_is_disabled(is_disabled);

      const bool is_runnable =
          (GTEST_FLAG(also_run_disabled_tests) || !is_disabled) &&
          internal::UnitTestOptions::FilterMatchesTest(test_case_name,
                                                       test_name);

      const bool is_selected = is_runnable &&
          (shard_tests == IGNORE_SHARDING_PROTOCOL ||
           ShouldRunTestOnShard(total_shards, shard_index,
                                num_runnable_tests));

      num_runnable_tests += is_runnable;
      num_selected_tests += is_selected;

      test_info->impl()->set_should_run(is_selected);
      test_case->set_should_run(test_case->should_run() || is_selected);
    }
  }
  return num_selected_tests;
}

// Lists all tests by name.
void UnitTestImpl::ListAllTests() {
  for (const internal::ListNode<TestCase*>* test_case_node = test_cases_.Head();
       test_case_node != NULL;
       test_case_node = test_case_node->next()) {
    const TestCase* const test_case = test_case_node->element();

    // Prints the test case name following by an indented list of test nodes.
    printf("%s.\n", test_case->name());

    for (const internal::ListNode<TestInfo*>* test_info_node =
         test_case->test_info_list().Head();
         test_info_node != NULL;
         test_info_node = test_info_node->next()) {
      const TestInfo* const test_info = test_info_node->element();

      printf("  %s\n", test_info->name());
    }
  }
  fflush(stdout);
}

// Sets the unit test result printer.
//
// Does nothing if the input and the current printer object are the
// same; otherwise, deletes the old printer object and makes the
// input the current printer.
void UnitTestImpl::set_result_printer(
    UnitTestEventListenerInterface* result_printer) {
  if (result_printer_ != result_printer) {
    delete result_printer_;
    result_printer_ = result_printer;
  }
}

// Returns the current unit test result printer if it is not NULL;
// otherwise, creates an appropriate result printer, makes it the
// current printer, and returns it.
UnitTestEventListenerInterface* UnitTestImpl::result_printer() {
  if (result_printer_ != NULL) {
    return result_printer_;
  }

#if GTEST_HAS_DEATH_TEST
  if (internal_run_death_test_flag_.get() != NULL) {
    result_printer_ = new NullUnitTestResultPrinter;
    return result_printer_;
  }
#endif  // GTEST_HAS_DEATH_TEST

  UnitTestEventsRepeater *repeater = new UnitTestEventsRepeater;
  const String& output_format = internal::UnitTestOptions::GetOutputFormat();
  if (output_format == "xml") {
    repeater->AddListener(new XmlUnitTestResultPrinter(
        internal::UnitTestOptions::GetAbsolutePathToOutputFile().c_str()));
  } else if (output_format != "") {
      printf("WARNING: unrecognized output format \"%s\" ignored.\n",
             output_format.c_str());
      fflush(stdout);
  }
  repeater->AddListener(new PrettyUnitTestResultPrinter);
  result_printer_ = repeater;
  return result_printer_;
}

// Sets the OS stack trace getter.
//
// Does nothing if the input and the current OS stack trace getter are
// the same; otherwise, deletes the old getter and makes the input the
// current getter.
void UnitTestImpl::set_os_stack_trace_getter(
    OsStackTraceGetterInterface* getter) {
  if (os_stack_trace_getter_ != getter) {
    delete os_stack_trace_getter_;
    os_stack_trace_getter_ = getter;
  }
}

// Returns the current OS stack trace getter if it is not NULL;
// otherwise, creates an OsStackTraceGetter, makes it the current
// getter, and returns it.
OsStackTraceGetterInterface* UnitTestImpl::os_stack_trace_getter() {
  if (os_stack_trace_getter_ == NULL) {
    os_stack_trace_getter_ = new OsStackTraceGetter;
  }

  return os_stack_trace_getter_;
}

// Returns the TestResult for the test that's currently running, or
// the TestResult for the ad hoc test if no test is running.
internal::TestResult* UnitTestImpl::current_test_result() {
  return current_test_info_ ?
    current_test_info_->impl()->result() : &ad_hoc_test_result_;
}

// TestInfoImpl constructor. The new instance assumes ownership of the test
// factory object.
TestInfoImpl::TestInfoImpl(TestInfo* parent,
                           const char* test_case_name,
                           const char* name,
                           const char* test_case_comment,
                           const char* comment,
                           TypeId fixture_class_id,
                           internal::TestFactoryBase* factory) :
    parent_(parent),
    test_case_name_(String(test_case_name)),
    name_(String(name)),
    test_case_comment_(String(test_case_comment)),
    comment_(String(comment)),
    fixture_class_id_(fixture_class_id),
    should_run_(false),
    is_disabled_(false),
    factory_(factory) {
}

// TestInfoImpl destructor.
TestInfoImpl::~TestInfoImpl() {
  delete factory_;
}

// Returns the current OS stack trace as a String.
//
// The maximum number of stack frames to be included is specified by
// the gtest_stack_trace_depth flag.  The skip_count parameter
// specifies the number of top frames to be skipped, which doesn't
// count against the number of frames to be included.
//
// For example, if Foo() calls Bar(), which in turn calls
// GetCurrentOsStackTraceExceptTop(..., 1), Foo() will be included in
// the trace but Bar() and GetCurrentOsStackTraceExceptTop() won't.
String GetCurrentOsStackTraceExceptTop(UnitTest* unit_test, int skip_count) {
  // We pass skip_count + 1 to skip this wrapper function in addition
  // to what the user really wants to skip.
  return unit_test->impl()->CurrentOsStackTraceExceptTop(skip_count + 1);
}

// Returns the number of failed test parts in the given test result object.
int GetFailedPartCount(const TestResult* result) {
  return result->failed_part_count();
}

// Used by the GTEST_HIDE_UNREACHABLE_CODE_ macro to suppress unreachable
// code warnings.
namespace {
class ClassUniqueToAlwaysTrue {};
}

bool AlwaysTrue() {
#if GTEST_HAS_EXCEPTIONS
  // This condition is always false so AlwaysTrue() never actually throws,
  // but it makes the compiler think that it may throw.
  if (atoi("42") == 36)  // NOLINT
    throw ClassUniqueToAlwaysTrue();
#endif  // GTEST_HAS_EXCEPTIONS
  return true;
}

// Parses a string as a command line flag.  The string should have
// the format "--flag=value".  When def_optional is true, the "=value"
// part can be omitted.
//
// Returns the value of the flag, or NULL if the parsing failed.
const char* ParseFlagValue(const char* str,
                           const char* flag,
                           bool def_optional) {
  // str and flag must not be NULL.
  if (str == NULL || flag == NULL) return NULL;

  // The flag must start with "--" followed by GTEST_FLAG_PREFIX_.
  const String flag_str = String::Format("--%s%s", GTEST_FLAG_PREFIX_, flag);
  const size_t flag_len = flag_str.GetLength();
  if (strncmp(str, flag_str.c_str(), flag_len) != 0) return NULL;

  // Skips the flag name.
  const char* flag_end = str + flag_len;

  // When def_optional is true, it's OK to not have a "=value" part.
  if (def_optional && (flag_end[0] == '\0')) {
    return flag_end;
  }

  // If def_optional is true and there are more characters after the
  // flag name, or if def_optional is false, there must be a '=' after
  // the flag name.
  if (flag_end[0] != '=') return NULL;

  // Returns the string after "=".
  return flag_end + 1;
}

// Parses a string for a bool flag, in the form of either
// "--flag=value" or "--flag".
//
// In the former case, the value is taken as true as long as it does
// not start with '0', 'f', or 'F'.
//
// In the latter case, the value is taken as true.
//
// On success, stores the value of the flag in *value, and returns
// true.  On failure, returns false without changing *value.
bool ParseBoolFlag(const char* str, const char* flag, bool* value) {
  // Gets the value of the flag as a string.
  const char* const value_str = ParseFlagValue(str, flag, true);

  // Aborts if the parsing failed.
  if (value_str == NULL) return false;

  // Converts the string value to a bool.
  *value = !(*value_str == '0' || *value_str == 'f' || *value_str == 'F');
  return true;
}

// Parses a string for an Int32 flag, in the form of
// "--flag=value".
//
// On success, stores the value of the flag in *value, and returns
// true.  On failure, returns false without changing *value.
bool ParseInt32Flag(const char* str, const char* flag, Int32* value) {
  // Gets the value of the flag as a string.
  const char* const value_str = ParseFlagValue(str, flag, false);

  // Aborts if the parsing failed.
  if (value_str == NULL) return false;

  // Sets *value to the value of the flag.
  return ParseInt32(Message() << "The value of flag --" << flag,
                    value_str, value);
}

// Parses a string for a string flag, in the form of
// "--flag=value".
//
// On success, stores the value of the flag in *value, and returns
// true.  On failure, returns false without changing *value.
bool ParseStringFlag(const char* str, const char* flag, String* value) {
  // Gets the value of the flag as a string.
  const char* const value_str = ParseFlagValue(str, flag, false);

  // Aborts if the parsing failed.
  if (value_str == NULL) return false;

  // Sets *value to the value of the flag.
  *value = value_str;
  return true;
}

// Prints a string containing code-encoded text.  The following escape
// sequences can be used in the string to control the text color:
//
//   @@    prints a single '@' character.
//   @R    changes the color to red.
//   @G    changes the color to green.
//   @Y    changes the color to yellow.
//   @D    changes to the default terminal text color.
//
// TODO(wan@google.com): Write tests for this once we add stdout
// capturing to Google Test.
static void PrintColorEncoded(const char* str) {
  GTestColor color = COLOR_DEFAULT;  // The current color.

  // Conceptually, we split the string into segments divided by escape
  // sequences.  Then we print one segment at a time.  At the end of
  // each iteration, the str pointer advances to the beginning of the
  // next segment.
  for (;;) {
    const char* p = strchr(str, '@');
    if (p == NULL) {
      ColoredPrintf(color, "%s", str);
      return;
    }

    ColoredPrintf(color, "%s", String(str, p - str).c_str());

    const char ch = p[1];
    str = p + 2;
    if (ch == '@') {
      ColoredPrintf(color, "@");
    } else if (ch == 'D') {
      color = COLOR_DEFAULT;
    } else if (ch == 'R') {
      color = COLOR_RED;
    } else if (ch == 'G') {
      color = COLOR_GREEN;
    } else if (ch == 'Y') {
      color = COLOR_YELLOW;
    } else {
      --str;
    }
  }
}

static const char kColorEncodedHelpMessage[] =
"This program contains tests written using " GTEST_NAME_ ". You can use the\n"
"following command line flags to control its behavior:\n"
"\n"
"Test Selection:\n"
"  @G--" GTEST_FLAG_PREFIX_ "list_tests@D\n"
"      List the names of all tests instead of running them. The name of\n"
"      TEST(Foo, Bar) is \"Foo.Bar\".\n"
"  @G--" GTEST_FLAG_PREFIX_ "filter=@YPOSTIVE_PATTERNS"
    "[@G-@YNEGATIVE_PATTERNS]@D\n"
"      Run only the tests whose name matches one of the positive patterns but\n"
"      none of the negative patterns. '?' matches any single character; '*'\n"
"      matches any substring; ':' separates two patterns.\n"
"  @G--" GTEST_FLAG_PREFIX_ "also_run_disabled_tests@D\n"
"      Run all disabled tests too.\n"
"  @G--" GTEST_FLAG_PREFIX_ "repeat=@Y[COUNT]@D\n"
"      Run the tests repeatedly; use a negative count to repeat forever.\n"
"\n"
"Test Output:\n"
"  @G--" GTEST_FLAG_PREFIX_ "color=@Y(@Gyes@Y|@Gno@Y|@Gauto@Y)@D\n"
"      Enable/disable colored output. The default is @Gauto@D.\n"
"  -@G-" GTEST_FLAG_PREFIX_ "print_time@D\n"
"      Print the elapsed time of each test.\n"
"  @G--" GTEST_FLAG_PREFIX_ "output=xml@Y[@G:@YDIRECTORY_PATH@G"
    GTEST_PATH_SEP_ "@Y|@G:@YFILE_PATH]@D\n"
"      Generate an XML report in the given directory or with the given file\n"
"      name. @YFILE_PATH@D defaults to @Gtest_details.xml@D.\n"
"\n"
"Failure Behavior:\n"
"  @G--" GTEST_FLAG_PREFIX_ "break_on_failure@D\n"
"      Turn assertion failures into debugger break-points.\n"
"  @G--" GTEST_FLAG_PREFIX_ "throw_on_failure@D\n"
"      Turn assertion failures into C++ exceptions.\n"
#if GTEST_OS_WINDOWS
"  @G--" GTEST_FLAG_PREFIX_ "catch_exceptions@D\n"
"      Suppress pop-ups caused by exceptions.\n"
#endif  // GTEST_OS_WINDOWS
"\n"
"Except for @G--" GTEST_FLAG_PREFIX_ "list_tests@D, you can alternatively set "
    "the corresponding\n"
"environment variable of a flag (all letters in upper-case). For example, to\n"
"print the elapsed time, you can either specify @G--" GTEST_FLAG_PREFIX_
    "print_time@D or set the\n"
"@G" GTEST_FLAG_PREFIX_UPPER_ "PRINT_TIME@D environment variable to a "
    "non-zero value.\n"
"\n"
"For more information, please read the " GTEST_NAME_ " documentation at\n"
"@G" GTEST_PROJECT_URL_ "@D. If you find a bug in " GTEST_NAME_ "\n"
"(not one in your own code or tests), please report it to\n"
"@G<" GTEST_DEV_EMAIL_ ">@D.\n";

// Parses the command line for Google Test flags, without initializing
// other parts of Google Test.  The type parameter CharType can be
// instantiated to either char or wchar_t.
template <typename CharType>
void ParseGoogleTestFlagsOnlyImpl(int* argc, CharType** argv) {
  for (int i = 1; i < *argc; i++) {
    const String arg_string = StreamableToString(argv[i]);
    const char* const arg = arg_string.c_str();

    using internal::ParseBoolFlag;
    using internal::ParseInt32Flag;
    using internal::ParseStringFlag;

    // Do we see a Google Test flag?
    if (ParseBoolFlag(arg, kAlsoRunDisabledTestsFlag,
                      &GTEST_FLAG(also_run_disabled_tests)) ||
        ParseBoolFlag(arg, kBreakOnFailureFlag,
                      &GTEST_FLAG(break_on_failure)) ||
        ParseBoolFlag(arg, kCatchExceptionsFlag,
                      &GTEST_FLAG(catch_exceptions)) ||
        ParseStringFlag(arg, kColorFlag, &GTEST_FLAG(color)) ||
        ParseStringFlag(arg, kDeathTestStyleFlag,
                        &GTEST_FLAG(death_test_style)) ||
        ParseBoolFlag(arg, kDeathTestUseFork,
                      &GTEST_FLAG(death_test_use_fork)) ||
        ParseStringFlag(arg, kFilterFlag, &GTEST_FLAG(filter)) ||
        ParseStringFlag(arg, kInternalRunDeathTestFlag,
                        &GTEST_FLAG(internal_run_death_test)) ||
        ParseBoolFlag(arg, kListTestsFlag, &GTEST_FLAG(list_tests)) ||
        ParseStringFlag(arg, kOutputFlag, &GTEST_FLAG(output)) ||
        ParseBoolFlag(arg, kPrintTimeFlag, &GTEST_FLAG(print_time)) ||
        ParseInt32Flag(arg, kRepeatFlag, &GTEST_FLAG(repeat)) ||
        ParseBoolFlag(arg, kThrowOnFailureFlag, &GTEST_FLAG(throw_on_failure))
        ) {
      // Yes.  Shift the remainder of the argv list left by one.  Note
      // that argv has (*argc + 1) elements, the last one always being
      // NULL.  The following loop moves the trailing NULL element as
      // well.
      for (int j = i; j != *argc; j++) {
        argv[j] = argv[j + 1];
      }

      // Decrements the argument count.
      (*argc)--;

      // We also need to decrement the iterator as we just removed
      // an element.
      i--;
    } else if (arg_string == "--help" || arg_string == "-h" ||
               arg_string == "-?" || arg_string == "/?") {
      g_help_flag = true;
    }
  }

  if (g_help_flag) {
    // We print the help here instead of in RUN_ALL_TESTS(), as the
    // latter may not be called at all if the user is using Google
    // Test with another testing framework.
    PrintColorEncoded(kColorEncodedHelpMessage);
  }
}

// Parses the command line for Google Test flags, without initializing
// other parts of Google Test.
void ParseGoogleTestFlagsOnly(int* argc, char** argv) {
  ParseGoogleTestFlagsOnlyImpl(argc, argv);
}
void ParseGoogleTestFlagsOnly(int* argc, wchar_t** argv) {
  ParseGoogleTestFlagsOnlyImpl(argc, argv);
}

// The internal implementation of InitGoogleTest().
//
// The type parameter CharType can be instantiated to either char or
// wchar_t.
template <typename CharType>
void InitGoogleTestImpl(int* argc, CharType** argv) {
  g_init_gtest_count++;

  // We don't want to run the initialization code twice.
  if (g_init_gtest_count != 1) return;

  if (*argc <= 0) return;

  internal::g_executable_path = internal::StreamableToString(argv[0]);

#if GTEST_HAS_DEATH_TEST
  g_argvs.clear();
  for (int i = 0; i != *argc; i++) {
    g_argvs.push_back(StreamableToString(argv[i]));
  }
#endif  // GTEST_HAS_DEATH_TEST

  ParseGoogleTestFlagsOnly(argc, argv);
}

}  // namespace internal

// Initializes Google Test.  This must be called before calling
// RUN_ALL_TESTS().  In particular, it parses a command line for the
// flags that Google Test recognizes.  Whenever a Google Test flag is
// seen, it is removed from argv, and *argc is decremented.
//
// No value is returned.  Instead, the Google Test flag variables are
// updated.
//
// Calling the function for the second time has no user-visible effect.
void InitGoogleTest(int* argc, char** argv) {
  internal::InitGoogleTestImpl(argc, argv);
}

// This overloaded version can be used in Windows programs compiled in
// UNICODE mode.
void InitGoogleTest(int* argc, wchar_t** argv) {
  internal::InitGoogleTestImpl(argc, argv);
}

}  // namespace testing
// Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: wan@google.com (Zhanyong Wan), vladl@google.com (Vlad Losev)
//
// This file implements death tests.


#if GTEST_HAS_DEATH_TEST

#if GTEST_OS_MAC
#include <crt_externs.h>
#endif  // GTEST_OS_MAC

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>

#if GTEST_OS_WINDOWS
#include <windows.h>
#else
#include <sys/mman.h>
#include <sys/wait.h>
#endif  // GTEST_OS_WINDOWS

#endif  // GTEST_HAS_DEATH_TEST


// Indicates that this translation unit is part of Google Test's
// implementation.  It must come before gtest-internal-inl.h is
// included, or there will be a compiler error.  This trick is to
// prevent a user from accidentally including gtest-internal-inl.h in
// his code.
#define GTEST_IMPLEMENTATION_ 1
#undef GTEST_IMPLEMENTATION_

namespace testing {

// Constants.

// The default death test style.
static const char kDefaultDeathTestStyle[] = "fast";

GTEST_DEFINE_string_(
    death_test_style,
    internal::StringFromGTestEnv("death_test_style", kDefaultDeathTestStyle),
    "Indicates how to run a death test in a forked child process: "
    "\"threadsafe\" (child process re-executes the test binary "
    "from the beginning, running only the specific death test) or "
    "\"fast\" (child process runs the death test immediately "
    "after forking).");

GTEST_DEFINE_bool_(
    death_test_use_fork,
    internal::BoolFromGTestEnv("death_test_use_fork", false),
    "Instructs to use fork()/_exit() instead of clone() in death tests. "
    "Ignored and always uses fork() on POSIX systems where clone() is not "
    "implemented. Useful when running under valgrind or similar tools if "
    "those do not support clone(). Valgrind 3.3.1 will just fail if "
    "it sees an unsupported combination of clone() flags. "
    "It is not recommended to use this flag w/o valgrind though it will "
    "work in 99% of the cases. Once valgrind is fixed, this flag will "
    "most likely be removed.");

namespace internal {
GTEST_DEFINE_string_(
    internal_run_death_test, "",
    "Indicates the file, line number, temporal index of "
    "the single death test to run, and a file descriptor to "
    "which a success code may be sent, all separated by "
    "colons.  This flag is specified if and only if the current "
    "process is a sub-process launched for running a thread-safe "
    "death test.  FOR INTERNAL USE ONLY.");
}  // namespace internal

#if GTEST_HAS_DEATH_TEST

// ExitedWithCode constructor.
ExitedWithCode::ExitedWithCode(int exit_code) : exit_code_(exit_code) {
}

// ExitedWithCode function-call operator.
bool ExitedWithCode::operator()(int exit_status) const {
#if GTEST_OS_WINDOWS
  return exit_status == exit_code_;
#else
  return WIFEXITED(exit_status) && WEXITSTATUS(exit_status) == exit_code_;
#endif  // GTEST_OS_WINDOWS
}

#if !GTEST_OS_WINDOWS
// KilledBySignal constructor.
KilledBySignal::KilledBySignal(int signum) : signum_(signum) {
}

// KilledBySignal function-call operator.
bool KilledBySignal::operator()(int exit_status) const {
  return WIFSIGNALED(exit_status) && WTERMSIG(exit_status) == signum_;
}
#endif  // !GTEST_OS_WINDOWS

namespace internal {

// Utilities needed for death tests.

// Generates a textual description of a given exit code, in the format
// specified by wait(2).
static String ExitSummary(int exit_code) {
  Message m;
#if GTEST_OS_WINDOWS
  m << "Exited with exit status " << exit_code;
#else
  if (WIFEXITED(exit_code)) {
    m << "Exited with exit status " << WEXITSTATUS(exit_code);
  } else if (WIFSIGNALED(exit_code)) {
    m << "Terminated by signal " << WTERMSIG(exit_code);
  }
#ifdef WCOREDUMP
  if (WCOREDUMP(exit_code)) {
    m << " (core dumped)";
  }
#endif
#endif  // GTEST_OS_WINDOWS
  return m.GetString();
}

// Returns true if exit_status describes a process that was terminated
// by a signal, or exited normally with a nonzero exit code.
bool ExitedUnsuccessfully(int exit_status) {
  return !ExitedWithCode(0)(exit_status);
}

#if !GTEST_OS_WINDOWS
// Generates a textual failure message when a death test finds more than
// one thread running, or cannot determine the number of threads, prior
// to executing the given statement.  It is the responsibility of the
// caller not to pass a thread_count of 1.
static String DeathTestThreadWarning(size_t thread_count) {
  Message msg;
  msg << "Death tests use fork(), which is unsafe particularly"
      << " in a threaded context. For this test, " << GTEST_NAME_ << " ";
  if (thread_count == 0)
    msg << "couldn't detect the number of threads.";
  else
    msg << "detected " << thread_count << " threads.";
  return msg.GetString();
}
#endif  // !GTEST_OS_WINDOWS

// Flag characters for reporting a death test that did not die.
static const char kDeathTestLived = 'L';
static const char kDeathTestReturned = 'R';
static const char kDeathTestInternalError = 'I';

// An enumeration describing all of the possible ways that a death test
// can conclude.  DIED means that the process died while executing the
// test code; LIVED means that process lived beyond the end of the test
// code; and RETURNED means that the test statement attempted a "return,"
// which is not allowed.  IN_PROGRESS means the test has not yet
// concluded.
enum DeathTestOutcome { IN_PROGRESS, DIED, LIVED, RETURNED };

// Routine for aborting the program which is safe to call from an
// exec-style death test child process, in which case the error
// message is propagated back to the parent process.  Otherwise, the
// message is simply printed to stderr.  In either case, the program
// then exits with status 1.
void DeathTestAbort(const String& message) {
  // On a POSIX system, this function may be called from a threadsafe-style
  // death test child process, which operates on a very small stack.  Use
  // the heap for any additional non-minuscule memory requirements.
  const InternalRunDeathTestFlag* const flag =
      GetUnitTestImpl()->internal_run_death_test_flag();
  if (flag != NULL) {
// Suppress MSVC complaints about POSIX functions.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif  // _MSC_VER
    FILE* parent = fdopen(flag->status_fd(), "w");
#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER
    fputc(kDeathTestInternalError, parent);
    fprintf(parent, "%s", message.c_str());
    fflush(parent);
    _exit(1);
  } else {
    fprintf(stderr, "%s", message.c_str());
    fflush(stderr);
    abort();
  }
}

// A replacement for CHECK that calls DeathTestAbort if the assertion
// fails.
#define GTEST_DEATH_TEST_CHECK_(expression) \
  do { \
    if (!(expression)) { \
      DeathTestAbort(::testing::internal::String::Format(\
          "CHECK failed: File %s, line %d: %s", \
          __FILE__, __LINE__, #expression)); \
    } \
  } while (0)

// This macro is similar to GTEST_DEATH_TEST_CHECK_, but it is meant for
// evaluating any system call that fulfills two conditions: it must return
// -1 on failure, and set errno to EINTR when it is interrupted and
// should be tried again.  The macro expands to a loop that repeatedly
// evaluates the expression as long as it evaluates to -1 and sets
// errno to EINTR.  If the expression evaluates to -1 but errno is
// something other than EINTR, DeathTestAbort is called.
#define GTEST_DEATH_TEST_CHECK_SYSCALL_(expression) \
  do { \
    int gtest_retval; \
    do { \
      gtest_retval = (expression); \
    } while (gtest_retval == -1 && errno == EINTR); \
    if (gtest_retval == -1) { \
      DeathTestAbort(::testing::internal::String::Format(\
          "CHECK failed: File %s, line %d: %s != -1", \
          __FILE__, __LINE__, #expression)); \
    } \
  } while (0)

// Returns the message describing the last system error, regardless of the
// platform.
String GetLastSystemErrorMessage() {
#if GTEST_OS_WINDOWS
    const DWORD error_num = ::GetLastError();

    if (error_num == NULL)
      return String("");

    char* message_ptr;

    ::FormatMessageA(
        // The caller does not provide a buffer. The function will allocate one.
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
            // The function must look up an error message in its system error
            // message table.
            FORMAT_MESSAGE_FROM_SYSTEM |
            // Do not expand insert sequences in the message definition.
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,  // Message source. Ignored in this call.
        error_num,
        0x0,  // Use system-default language.
        reinterpret_cast<LPSTR>(&message_ptr),
        0,  // Buffer size. Ignored in this call.
        NULL);  // Message arguments. Ignored in this call.

    const String message = message_ptr;
    ::LocalFree(message_ptr);
    return message;
#else
    return errno == 0 ? String("") : String(strerror(errno));
#endif  // GTEST_OS_WINDOWS
}

// TODO(vladl@google.com): Move the definition of FailFromInternalError
// here.
#if GTEST_OS_WINDOWS
static void FailFromInternalError(HANDLE handle);
#else
static void FailFromInternalError(int fd);
#endif  // GTEST_OS_WINDOWS

// Death test constructor.  Increments the running death test count
// for the current test.
DeathTest::DeathTest() {
  TestInfo* const info = GetUnitTestImpl()->current_test_info();
  if (info == NULL) {
    DeathTestAbort("Cannot run a death test outside of a TEST or "
                   "TEST_F construct");
  }
}

// Creates and returns a death test by dispatching to the current
// death test factory.
bool DeathTest::Create(const char* statement, const RE* regex,
                       const char* file, int line, DeathTest** test) {
  return GetUnitTestImpl()->death_test_factory()->Create(
      statement, regex, file, line, test);
}

const char* DeathTest::LastMessage() {
  return last_death_test_message_.c_str();
}

void DeathTest::set_last_death_test_message(const String& message) {
  last_death_test_message_ = message;
}

String DeathTest::last_death_test_message_;

// Provides cross platform implementation for some death functionality.
// TODO(vladl@google.com): Merge this class with DeathTest in
// gtest-death-test-internal.h.
class DeathTestImpl : public DeathTest {
 protected:
  DeathTestImpl(const char* statement, const RE* regex)
      : statement_(statement),
        regex_(regex),
        spawned_(false),
        status_(-1),
        outcome_(IN_PROGRESS) {}

  virtual bool Passed(bool status_ok);

  const char* statement() const { return statement_; }
  const RE* regex() const { return regex_; }
  bool spawned() const { return spawned_; }
  void set_spawned(bool spawned) { spawned_ = spawned; }
  int status() const { return status_; }
  void set_status(int status) { status_ = status; }
  DeathTestOutcome outcome() const { return outcome_; }
  void set_outcome(DeathTestOutcome outcome) { outcome_ = outcome; }

 private:
  // The textual content of the code this object is testing.  This class
  // doesn't own this string and should not attempt to delete it.
  const char* const statement_;
  // The regular expression which test output must match.  DeathTestImpl
  // doesn't own this object and should not attempt to delete it.
  const RE* const regex_;
  // True if the death test child process has been successfully spawned.
  bool spawned_;
  // The exit status of the child process.
  int status_;
  // How the death test concluded.
  DeathTestOutcome outcome_;
};

// TODO(vladl@google.com): Move definition of DeathTestImpl::Passed() here.

#if GTEST_OS_WINDOWS
// WindowsDeathTest implements death tests on Windows. Due to the
// specifics of starting new processes on Windows, death tests there are
// always threadsafe, and Google Test considers the
// --gtest_death_test_style=fast setting to be equivalent to
// --gtest_death_test_style=threadsafe there.
//
// A few implementation notes:  Like the Linux version, the Windows
// implementation uses pipes for child-to-parent communication. But due to
// the specifics of pipes on Windows, some extra steps are required:
//
// 1. The parent creates a communication pipe and stores handles to both
//    ends of it.
// 2. The parent starts the child and provides it with the information
//    necessary to acquire the handle to the write end of the pipe.
// 3. The child acquires the write end of the pipe and signals the parent
//    using a Windows event.
// 4. Now the parent can release the write end of the pipe on its side. If
//    this is done before step 3, the object's reference count goes down to
//    0 and it is destroyed, preventing the child from acquiring it. The
//    parent now has to release it, or read operations on the read end of
//    the pipe will not return when the child terminates.
// 5. The parent reads child's output through the pipe (outcome code and
//    any possible error messages) from the pipe, and its stderr and then
//    determines whether to fail the test.
//
// Note: to distinguish Win32 API calls from the local method and function
// calls, the former are explicitly resolved in the global namespace.
//
class WindowsDeathTest : public DeathTestImpl {
 public:
  WindowsDeathTest(const char* statement,
                   const RE* regex,
                   const char* file,
                   int line)
      : DeathTestImpl(statement, regex), file_(file), line_(line) {}

  // All of these virtual functions are inherited from DeathTest.
  virtual int Wait();
  virtual void Abort(AbortReason reason);
  virtual TestRole AssumeRole();

 private:
  // The name of the file in which the death test is located.
  const char* const file_;
  // The line number on which the death test is located.
  const int line_;
  // Handle to the read end of the pipe to the child process.
  // The child keeps its write end of the pipe in the status_handle_
  // field of its InternalRunDeathTestFlag class.
  AutoHandle read_handle_;
  // Handle to the write end of the pipe to the child process.
  AutoHandle write_handle_;
  // Child process handle.
  AutoHandle child_handle_;
  // Event the child process uses to signal the parent that it has
  // acquired the handle to the write end of the pipe. After seeing this
  // event the parent can release its own handles to make sure its
  // ReadFile() calls return when the child terminates.
  AutoHandle event_handle_;
};

// Waits for the child in a death test to exit, returning its exit
// status, or 0 if no child process exists.  As a side effect, sets the
// outcome data member.
// TODO(vladl@google.com): Outcome classification logic is common with
//                         ForkingDeathTes::Wait(). Refactor it into a
//                         common function.
int WindowsDeathTest::Wait() {
  if (!spawned())
    return 0;

  // Wait until the child either signals that it has acquired the write end
  // of the pipe or it dies.
  const HANDLE wait_handles[2] = { child_handle_.Get(), event_handle_.Get() };
  switch (::WaitForMultipleObjects(2,
                                   wait_handles,
                                   FALSE,  // Waits for any of the handles.
                                   INFINITE)) {
    case WAIT_OBJECT_0:
    case WAIT_OBJECT_0 + 1:
      break;
    default:
      GTEST_DEATH_TEST_CHECK_(false);  // Should not get here.
  }

  // The child has acquired the write end of the pipe or exited.
  // We release the handle on our side and continue.
  write_handle_.Reset();
  event_handle_.Reset();

  // ReadFile() blocks until data is available (signifying the
  // failure of the death test) or until the pipe is closed (signifying
  // its success), so it's okay to call this in the parent before or
  // after the child process has exited.
  char flag;
  DWORD bytes_read;
  GTEST_DEATH_TEST_CHECK_(::ReadFile(read_handle_.Get(),
                                     &flag,
                                     1,
                                     &bytes_read,
                                     NULL) ||
                          ::GetLastError() == ERROR_BROKEN_PIPE);

  if (bytes_read == 0) {
    set_outcome(DIED);
  } else if (bytes_read == 1) {
    switch (flag) {
      case kDeathTestReturned:
        set_outcome(RETURNED);
        break;
      case kDeathTestLived:
        set_outcome(LIVED);
        break;
      case kDeathTestInternalError:
        FailFromInternalError(read_handle_.Get());  // Does not return.
        break;
      default:
        GTEST_LOG_(FATAL,
                   Message() << "Death test child process reported "
                   << " unexpected status byte ("
                   << static_cast<unsigned int>(flag) << ")");
    }
  } else {
    GTEST_LOG_(FATAL,
               Message() << "Read from death test child process failed: "
                         << GetLastSystemErrorMessage());
  }
  read_handle_.Reset();  // Done with reading.

  // Waits for the child process to exit if it haven't already. This
  // returns immediately if the child has already exited, regardless of
  // whether previous calls to WaitForMultipleObjects synchronized on this
  // handle or not.
  GTEST_DEATH_TEST_CHECK_(
      WAIT_OBJECT_0 == ::WaitForSingleObject(child_handle_.Get(),
                                             INFINITE));
  DWORD status;
  GTEST_DEATH_TEST_CHECK_(::GetExitCodeProcess(child_handle_.Get(),
                                               &status));
  child_handle_.Reset();
  set_status(static_cast<int>(status));
  return this->status();
}

// TODO(vladl@google.com): define a cross-platform way to write to
// status_fd to be used both here and in ForkingDeathTest::Abort().
//
// Signals that the death test did not die as expected. This is called
// from the child process only.
void WindowsDeathTest::Abort(AbortReason reason) {
  const InternalRunDeathTestFlag* const internal_flag =
      GetUnitTestImpl()->internal_run_death_test_flag();
  // The parent process considers the death test to be a failure if
  // it finds any data in our pipe.  So, here we write a single flag byte
  // to the pipe, then exit.
  const char status_ch =
      reason == TEST_DID_NOT_DIE ? kDeathTestLived : kDeathTestReturned;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif  // _MSC_VER
  GTEST_DEATH_TEST_CHECK_SYSCALL_(write(internal_flag->status_fd(),
                                        &status_ch, 1));
#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

  // The write handle will be closed when the child terminates in _exit().
  _exit(1);  // Exits w/o any normal exit hooks (we were supposed to crash)
}

// The AssumeRole process for a Windows death test.  It creates a child
// process with the same executable as the current process to run the
// death test.  The child process is given the --gtest_filter and
// --gtest_internal_run_death_test flags such that it knows to run the
// current death test only.
DeathTest::TestRole WindowsDeathTest::AssumeRole() {
  const UnitTestImpl* const impl = GetUnitTestImpl();
  const InternalRunDeathTestFlag* const flag =
      impl->internal_run_death_test_flag();
  const TestInfo* const info = impl->current_test_info();
  const int death_test_index = info->result()->death_test_count();

  if (flag != NULL) {
    // ParseInternalRunDeathTestFlag() has performed all the necessary
    // processing.
    return EXECUTE_TEST;
  }

  // WindowsDeathTest uses an anonymous pipe to communicate results of
  // a death test.
  SECURITY_ATTRIBUTES handles_are_inheritable = {
    sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
  HANDLE read_handle, write_handle;
  GTEST_DEATH_TEST_CHECK_(::CreatePipe(&read_handle, &write_handle,
                                       &handles_are_inheritable,
                                       0));  // Default buffer size.
  read_handle_.Reset(read_handle);
  write_handle_.Reset(write_handle);
  event_handle_.Reset(::CreateEvent(
      &handles_are_inheritable,
      TRUE,    // The event will automatically reset to non-signaled state.
      FALSE,   // The initial state is non-signalled.
      NULL));  // The even is unnamed.
  GTEST_DEATH_TEST_CHECK_(event_handle_.Get() != NULL);
  const String filter_flag = String::Format("--%s%s=%s.%s",
                                            GTEST_FLAG_PREFIX_, kFilterFlag,
                                            info->test_case_name(),
                                            info->name());
  const String internal_flag = String::Format(
    "--%s%s=%s|%d|%d|%u|%Iu|%Iu",
      GTEST_FLAG_PREFIX_,
      kInternalRunDeathTestFlag,
      file_, line_,
      death_test_index,
      static_cast<unsigned int>(::GetCurrentProcessId()),
      // size_t has the same with as pointers on both 32-bit and 64-bit
      // Windows platforms.
      // See http://msdn.microsoft.com/en-us/library/tcxf1dw6.aspx.
      reinterpret_cast<size_t>(write_handle),
      reinterpret_cast<size_t>(event_handle_.Get()));

  char executable_path[_MAX_PATH + 1];  // NOLINT
  GTEST_DEATH_TEST_CHECK_(
      _MAX_PATH + 1 != ::GetModuleFileNameA(NULL,
                                            executable_path,
                                            _MAX_PATH));

  String command_line = String::Format("%s %s \"%s\"",
                                       ::GetCommandLineA(),
                                       filter_flag.c_str(),
                                       internal_flag.c_str());

  DeathTest::set_last_death_test_message("");

  CaptureStderr();
  // Flush the log buffers since the log streams are shared with the child.
  FlushInfoLog();

  // The child process will share the standard handles with the parent.
  STARTUPINFOA startup_info;
  memset(&startup_info, 0, sizeof(STARTUPINFO));
  startup_info.dwFlags = STARTF_USESTDHANDLES;
  startup_info.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
  startup_info.hStdOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
  startup_info.hStdError = ::GetStdHandle(STD_ERROR_HANDLE);

  PROCESS_INFORMATION process_info;
  GTEST_DEATH_TEST_CHECK_(::CreateProcessA(
      executable_path,
      const_cast<char*>(command_line.c_str()),
      NULL,   // Retuned process handle is not inheritable.
      NULL,   // Retuned thread handle is not inheritable.
      TRUE,   // Child inherits all inheritable handles (for write_handle_).
      0x0,    // Default creation flags.
      NULL,   // Inherit the parent's environment.
      UnitTest::GetInstance()->original_working_dir(),
      &startup_info,
      &process_info));
  child_handle_.Reset(process_info.hProcess);
  ::CloseHandle(process_info.hThread);
  set_spawned(true);
  return OVERSEE_TEST;
}
#else  // We are not on Windows.

// ForkingDeathTest provides implementations for most of the abstract
// methods of the DeathTest interface.  Only the AssumeRole method is
// left undefined.
class ForkingDeathTest : public DeathTestImpl {
 public:
  ForkingDeathTest(const char* statement, const RE* regex);

  // All of these virtual functions are inherited from DeathTest.
  virtual int Wait();
  virtual void Abort(AbortReason reason);

 protected:
  void set_child_pid(pid_t child_pid) { child_pid_ = child_pid; }
  void set_read_fd(int fd) { read_fd_ = fd; }
  void set_write_fd(int fd) { write_fd_ = fd; }

 private:
  // PID of child process during death test; 0 in the child process itself.
  pid_t child_pid_;
  // File descriptors for communicating the death test's status byte.
  int read_fd_;   // Always -1 in the child process.
  int write_fd_;  // Always -1 in the parent process.
};

// Constructs a ForkingDeathTest.
ForkingDeathTest::ForkingDeathTest(const char* statement, const RE* regex)
    : DeathTestImpl(statement, regex),
      child_pid_(-1),
      read_fd_(-1),
      write_fd_(-1) {
}
#endif  // GTEST_OS_WINDOWS

// This is called from a death test parent process to read a failure
// message from the death test child process and log it with the FATAL
// severity. On Windows, the message is read from a pipe handle. On other
// platforms, it is read from a file descriptor.
// TODO(vladl@google.com): Re-factor the code to merge common parts after
// the reading code is abstracted.
#if GTEST_OS_WINDOWS
static void FailFromInternalError(HANDLE handle) {
  Message error;
  char buffer[256];

  bool read_succeeded = true;
  DWORD bytes_read;
  do {
    // ERROR_BROKEN_PIPE arises when the other end of the pipe has been
    // closed. This is a normal condition for us.
    bytes_read = 0;
    read_succeeded = ::ReadFile(handle,
                                buffer,
                                sizeof(buffer) - 1,
                                &bytes_read,
                                NULL) || ::GetLastError() == ERROR_BROKEN_PIPE;
    buffer[bytes_read] = 0;
    error << buffer;
  } while (read_succeeded && bytes_read > 0);

  if (read_succeeded) {
    GTEST_LOG_(FATAL, error);
  } else {
    const DWORD last_error = ::GetLastError();
    const String message = GetLastSystemErrorMessage();
    GTEST_LOG_(FATAL,
               Message() << "Error while reading death test internal: "
               << message << " [" << last_error << "]");
  }
}
#else
static void FailFromInternalError(int fd) {
  Message error;
  char buffer[256];
  ssize_t num_read;

  do {
    while ((num_read = read(fd, buffer, 255)) > 0) {
      buffer[num_read] = '\0';
      error << buffer;
    }
  } while (num_read == -1 && errno == EINTR);

  if (num_read == 0) {
    GTEST_LOG_(FATAL, error);
  } else {
    const int last_error = errno;
    const String message = GetLastSystemErrorMessage();
    GTEST_LOG_(FATAL,
               Message() << "Error while reading death test internal: "
               << message << " [" << last_error << "]");
  }
}
#endif  // GTEST_OS_WINDOWS

#if !GTEST_OS_WINDOWS
// Waits for the child in a death test to exit, returning its exit
// status, or 0 if no child process exists.  As a side effect, sets the
// outcome data member.
int ForkingDeathTest::Wait() {
  if (!spawned())
    return 0;

  // The read() here blocks until data is available (signifying the
  // failure of the death test) or until the pipe is closed (signifying
  // its success), so it's okay to call this in the parent before
  // the child process has exited.
  char flag;
  ssize_t bytes_read;

  do {
    bytes_read = read(read_fd_, &flag, 1);
  } while (bytes_read == -1 && errno == EINTR);

  if (bytes_read == 0) {
    set_outcome(DIED);
  } else if (bytes_read == 1) {
    switch (flag) {
      case kDeathTestReturned:
        set_outcome(RETURNED);
        break;
      case kDeathTestLived:
        set_outcome(LIVED);
        break;
      case kDeathTestInternalError:
        FailFromInternalError(read_fd_);  // Does not return.
        break;
      default:
        GTEST_LOG_(FATAL,
                   Message() << "Death test child process reported unexpected "
                   << "status byte (" << static_cast<unsigned int>(flag)
                   << ")");
    }
  } else {
    const String error_message = GetLastSystemErrorMessage();
    GTEST_LOG_(FATAL,
               Message() << "Read from death test child process failed: "
                         << error_message);
  }

  GTEST_DEATH_TEST_CHECK_SYSCALL_(close(read_fd_));
  int status;
  GTEST_DEATH_TEST_CHECK_SYSCALL_(waitpid(child_pid_, &status, 0));
  set_status(status);
  return status;
}
#endif  // !GTEST_OS_WINDOWS

// Assesses the success or failure of a death test, using both private
// members which have previously been set, and one argument:
//
// Private data members:
//   outcome:  An enumeration describing how the death test
//             concluded: DIED, LIVED, or RETURNED.  The death test fails
//             in the latter two cases.
//   status:   The exit status of the child process. On *nix, it is in the
//             in the format specified by wait(2). On Windows, this is the
//             value supplied to the ExitProcess() API or a numeric code
//             of the exception that terminated the program.
//   regex:    A regular expression object to be applied to
//             the test's captured standard error output; the death test
//             fails if it does not match.
//
// Argument:
//   status_ok: true if exit_status is acceptable in the context of
//              this particular death test, which fails if it is false
//
// Returns true iff all of the above conditions are met.  Otherwise, the
// first failing condition, in the order given above, is the one that is
// reported. Also sets the last death test message string.
bool DeathTestImpl::Passed(bool status_ok) {
  if (!spawned())
    return false;

#if GTEST_HAS_GLOBAL_STRING
  const ::string error_message = GetCapturedStderr();
#else
  const ::std::string error_message = GetCapturedStderr();
#endif  // GTEST_HAS_GLOBAL_STRING

  bool success = false;
  Message buffer;

  buffer << "Death test: " << statement() << "\n";
  switch (outcome()) {
    case LIVED:
      buffer << "    Result: failed to die.\n"
             << " Error msg: " << error_message;
      break;
    case RETURNED:
      buffer << "    Result: illegal return in test statement.\n"
             << " Error msg: " << error_message;
      break;
    case DIED:
      if (status_ok) {
        if (RE::PartialMatch(error_message, *regex())) {
          success = true;
        } else {
          buffer << "    Result: died but not with expected error.\n"
                 << "  Expected: " << regex()->pattern() << "\n"
                 << "Actual msg: " << error_message;
        }
      } else {
        buffer << "    Result: died but not with expected exit code:\n"
               << "            " << ExitSummary(status()) << "\n";
      }
      break;
    case IN_PROGRESS:
    default:
      GTEST_LOG_(FATAL,
                 "DeathTest::Passed somehow called before conclusion of test");
  }

  DeathTest::set_last_death_test_message(buffer.GetString());
  return success;
}

#if !GTEST_OS_WINDOWS
// Signals that the death test code which should have exited, didn't.
// Should be called only in a death test child process.
// Writes a status byte to the child's status file descriptor, then
// calls _exit(1).
void ForkingDeathTest::Abort(AbortReason reason) {
  // The parent process considers the death test to be a failure if
  // it finds any data in our pipe.  So, here we write a single flag byte
  // to the pipe, then exit.
  const char flag =
      reason == TEST_DID_NOT_DIE ? kDeathTestLived : kDeathTestReturned;
  GTEST_DEATH_TEST_CHECK_SYSCALL_(write(write_fd_, &flag, 1));
  GTEST_DEATH_TEST_CHECK_SYSCALL_(close(write_fd_));
  _exit(1);  // Exits w/o any normal exit hooks (we were supposed to crash)
}

// A concrete death test class that forks, then immediately runs the test
// in the child process.
class NoExecDeathTest : public ForkingDeathTest {
 public:
  NoExecDeathTest(const char* statement, const RE* regex) :
      ForkingDeathTest(statement, regex) { }
  virtual TestRole AssumeRole();
};

// The AssumeRole process for a fork-and-run death test.  It implements a
// straightforward fork, with a simple pipe to transmit the status byte.
DeathTest::TestRole NoExecDeathTest::AssumeRole() {
  const size_t thread_count = GetThreadCount();
  if (thread_count != 1) {
    GTEST_LOG_(WARNING, DeathTestThreadWarning(thread_count));
  }

  int pipe_fd[2];
  GTEST_DEATH_TEST_CHECK_(pipe(pipe_fd) != -1);

  DeathTest::set_last_death_test_message("");
  CaptureStderr();
  // When we fork the process below, the log file buffers are copied, but the
  // file descriptors are shared.  We flush all log files here so that closing
  // the file descriptors in the child process doesn't throw off the
  // synchronization between descriptors and buffers in the parent process.
  // This is as close to the fork as possible to avoid a race condition in case
  // there are multiple threads running before the death test, and another
  // thread writes to the log file.
  FlushInfoLog();

  const pid_t child_pid = fork();
  GTEST_DEATH_TEST_CHECK_(child_pid != -1);
  set_child_pid(child_pid);
  if (child_pid == 0) {
    GTEST_DEATH_TEST_CHECK_SYSCALL_(close(pipe_fd[0]));
    set_write_fd(pipe_fd[1]);
    // Redirects all logging to stderr in the child process to prevent
    // concurrent writes to the log files.  We capture stderr in the parent
    // process and append the child process' output to a log.
    LogToStderr();
    return EXECUTE_TEST;
  } else {
    GTEST_DEATH_TEST_CHECK_SYSCALL_(close(pipe_fd[1]));
    set_read_fd(pipe_fd[0]);
    set_spawned(true);
    return OVERSEE_TEST;
  }
}

// A concrete death test class that forks and re-executes the main
// program from the beginning, with command-line flags set that cause
// only this specific death test to be run.
class ExecDeathTest : public ForkingDeathTest {
 public:
  ExecDeathTest(const char* statement, const RE* regex,
                const char* file, int line) :
      ForkingDeathTest(statement, regex), file_(file), line_(line) { }
  virtual TestRole AssumeRole();
 private:
  // The name of the file in which the death test is located.
  const char* const file_;
  // The line number on which the death test is located.
  const int line_;
};

// Utility class for accumulating command-line arguments.
class Arguments {
 public:
  Arguments() {
    args_.push_back(NULL);
  }

  ~Arguments() {
    for (std::vector<char*>::iterator i = args_.begin(); i != args_.end();
         ++i) {
      free(*i);
    }
  }
  void AddArgument(const char* argument) {
    args_.insert(args_.end() - 1, strdup(argument));
  }

  template <typename Str>
  void AddArguments(const ::std::vector<Str>& arguments) {
    for (typename ::std::vector<Str>::const_iterator i = arguments.begin();
         i != arguments.end();
         ++i) {
      args_.insert(args_.end() - 1, strdup(i->c_str()));
    }
  }
  char* const* Argv() {
    return &args_[0];
  }
 private:
  std::vector<char*> args_;
};

// A struct that encompasses the arguments to the child process of a
// threadsafe-style death test process.
struct ExecDeathTestArgs {
  char* const* argv;  // Command-line arguments for the child's call to exec
  int close_fd;       // File descriptor to close; the read end of a pipe
};

#if GTEST_OS_MAC
inline char** GetEnviron() {
  // When Google Test is built as a framework on MacOS X, the environ variable
  // is unavailable. Apple's documentation (man environ) recommends using
  // _NSGetEnviron() instead.
  return *_NSGetEnviron();
}
#else
extern "C" char** environ;        // Some POSIX platforms expect you
                                  // to declare environ. extern "C" makes
                                  // it reside in the global namespace.
inline char** GetEnviron() {
  return environ;
}
#endif  // GTEST_OS_MAC

// The main function for a threadsafe-style death test child process.
// This function is called in a clone()-ed process and thus must avoid
// any potentially unsafe operations like malloc or libc functions.
static int ExecDeathTestChildMain(void* child_arg) {
  ExecDeathTestArgs* const args = static_cast<ExecDeathTestArgs*>(child_arg);
  GTEST_DEATH_TEST_CHECK_SYSCALL_(close(args->close_fd));

  // We need to execute the test program in the same environment where
  // it was originally invoked.  Therefore we change to the original
  // working directory first.
  const char* const original_dir =
      UnitTest::GetInstance()->original_working_dir();
  // We can safely call chdir() as it's a direct system call.
  if (chdir(original_dir) != 0) {
    DeathTestAbort(String::Format("chdir(\"%s\") failed: %s",
                                  original_dir,
                                  GetLastSystemErrorMessage().c_str()));
    return EXIT_FAILURE;
  }

  // We can safely call execve() as it's a direct system call.  We
  // cannot use execvp() as it's a libc function and thus potentially
  // unsafe.  Since execve() doesn't search the PATH, the user must
  // invoke the test program via a valid path that contains at least
  // one path separator.
  execve(args->argv[0], args->argv, GetEnviron());
  DeathTestAbort(String::Format("execve(%s, ...) in %s failed: %s",
                                args->argv[0],
                                original_dir,
                                GetLastSystemErrorMessage().c_str()));
  return EXIT_FAILURE;
}

// Two utility routines that together determine the direction the stack
// grows.
// This could be accomplished more elegantly by a single recursive
// function, but we want to guard against the unlikely possibility of
// a smart compiler optimizing the recursion away.
bool StackLowerThanAddress(const void* ptr) {
  int dummy;
  return &dummy < ptr;
}

bool StackGrowsDown() {
  int dummy;
  return StackLowerThanAddress(&dummy);
}

// A threadsafe implementation of fork(2) for threadsafe-style death tests
// that uses clone(2).  It dies with an error message if anything goes
// wrong.
static pid_t ExecDeathTestFork(char* const* argv, int close_fd) {
  ExecDeathTestArgs args = { argv, close_fd };
  pid_t child_pid;

#if GTEST_HAS_CLONE
  const bool use_fork = GTEST_FLAG(death_test_use_fork);

  if (!use_fork) {
    static const bool stack_grows_down = StackGrowsDown();
    const size_t stack_size = getpagesize();
    // MMAP_ANONYMOUS is not defined on Mac, so we use MAP_ANON instead.
    void* const stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE,
                             MAP_ANON | MAP_PRIVATE, -1, 0);
    GTEST_DEATH_TEST_CHECK_(stack != MAP_FAILED);
    void* const stack_top =
        static_cast<char*>(stack) + (stack_grows_down ? stack_size : 0);

    child_pid = clone(&ExecDeathTestChildMain, stack_top, SIGCHLD, &args);

    GTEST_DEATH_TEST_CHECK_(munmap(stack, stack_size) != -1);
  }
#else
  const bool use_fork = true;
#endif  // GTEST_HAS_CLONE

  if (use_fork && (child_pid = fork()) == 0) {
      ExecDeathTestChildMain(&args);
      _exit(0);
  }

  GTEST_DEATH_TEST_CHECK_(child_pid != -1);
  return child_pid;
}

// The AssumeRole process for a fork-and-exec death test.  It re-executes the
// main program from the beginning, setting the --gtest_filter
// and --gtest_internal_run_death_test flags to cause only the current
// death test to be re-run.
DeathTest::TestRole ExecDeathTest::AssumeRole() {
  const UnitTestImpl* const impl = GetUnitTestImpl();
  const InternalRunDeathTestFlag* const flag =
      impl->internal_run_death_test_flag();
  const TestInfo* const info = impl->current_test_info();
  const int death_test_index = info->result()->death_test_count();

  if (flag != NULL) {
    set_write_fd(flag->status_fd());
    return EXECUTE_TEST;
  }

  int pipe_fd[2];
  GTEST_DEATH_TEST_CHECK_(pipe(pipe_fd) != -1);
  // Clear the close-on-exec flag on the write end of the pipe, lest
  // it be closed when the child process does an exec:
  GTEST_DEATH_TEST_CHECK_(fcntl(pipe_fd[1], F_SETFD, 0) != -1);

  const String filter_flag =
      String::Format("--%s%s=%s.%s",
                     GTEST_FLAG_PREFIX_, kFilterFlag,
                     info->test_case_name(), info->name());
  const String internal_flag =
      String::Format("--%s%s=%s|%d|%d|%d",
                     GTEST_FLAG_PREFIX_, kInternalRunDeathTestFlag,
                     file_, line_, death_test_index, pipe_fd[1]);
  Arguments args;
  args.AddArguments(GetArgvs());
  args.AddArgument(filter_flag.c_str());
  args.AddArgument(internal_flag.c_str());

  DeathTest::set_last_death_test_message("");

  CaptureStderr();
  // See the comment in NoExecDeathTest::AssumeRole for why the next line
  // is necessary.
  FlushInfoLog();

  const pid_t child_pid = ExecDeathTestFork(args.Argv(), pipe_fd[0]);
  GTEST_DEATH_TEST_CHECK_SYSCALL_(close(pipe_fd[1]));
  set_child_pid(child_pid);
  set_read_fd(pipe_fd[0]);
  set_spawned(true);
  return OVERSEE_TEST;
}

#endif  // !GTEST_OS_WINDOWS

// Creates a concrete DeathTest-derived class that depends on the
// --gtest_death_test_style flag, and sets the pointer pointed to
// by the "test" argument to its address.  If the test should be
// skipped, sets that pointer to NULL.  Returns true, unless the
// flag is set to an invalid value.
bool DefaultDeathTestFactory::Create(const char* statement, const RE* regex,
                                     const char* file, int line,
                                     DeathTest** test) {
  UnitTestImpl* const impl = GetUnitTestImpl();
  const InternalRunDeathTestFlag* const flag =
      impl->internal_run_death_test_flag();
  const int death_test_index = impl->current_test_info()
      ->increment_death_test_count();

  if (flag != NULL) {
    if (death_test_index > flag->index()) {
      DeathTest::set_last_death_test_message(String::Format(
          "Death test count (%d) somehow exceeded expected maximum (%d)",
          death_test_index, flag->index()));
      return false;
    }

    if (!(flag->file() == file && flag->line() == line &&
          flag->index() == death_test_index)) {
      *test = NULL;
      return true;
    }
  }

#if GTEST_OS_WINDOWS
  if (GTEST_FLAG(death_test_style) == "threadsafe" ||
      GTEST_FLAG(death_test_style) == "fast") {
    *test = new WindowsDeathTest(statement, regex, file, line);
  }
#else
  if (GTEST_FLAG(death_test_style) == "threadsafe") {
    *test = new ExecDeathTest(statement, regex, file, line);
  } else if (GTEST_FLAG(death_test_style) == "fast") {
    *test = new NoExecDeathTest(statement, regex);
  }
#endif  // GTEST_OS_WINDOWS
  else {  // NOLINT - this is more readable than unbalanced brackets inside #if.
    DeathTest::set_last_death_test_message(String::Format(
        "Unknown death test style \"%s\" encountered",
        GTEST_FLAG(death_test_style).c_str()));
    return false;
  }

  return true;
}

// Splits a given string on a given delimiter, populating a given
// vector with the fields.  GTEST_HAS_DEATH_TEST implies that we have
// ::std::string, so we can use it here.
// TODO(vladl@google.com): Get rid of std::vector to be able to build on
// Visual C++ 7.1 with exceptions disabled.
static void SplitString(const ::std::string& str, char delimiter,
                        ::std::vector< ::std::string>* dest) {
  ::std::vector< ::std::string> parsed;
  ::std::string::size_type pos = 0;
  while (true) {
    const ::std::string::size_type colon = str.find(delimiter, pos);
    if (colon == ::std::string::npos) {
      parsed.push_back(str.substr(pos));
      break;
    } else {
      parsed.push_back(str.substr(pos, colon - pos));
      pos = colon + 1;
    }
  }
  dest->swap(parsed);
}

#if GTEST_OS_WINDOWS
// Recreates the pipe and event handles from the provided parameters,
// signals the event, and returns a file descriptor wrapped around the pipe
// handle. This function is called in the child process only.
int GetStatusFileDescriptor(unsigned int parent_process_id,
                            size_t status_handle_as_size_t,
                            size_t event_handle_as_size_t) {
  AutoHandle parent_process_handle(::OpenProcess(PROCESS_DUP_HANDLE,
                                                   FALSE,  // Non-inheritable.
                                                   parent_process_id));
  if (parent_process_handle.Get() == INVALID_HANDLE_VALUE) {
    DeathTestAbort(String::Format("Unable to open parent process %u",
                                  parent_process_id));
  }

  // TODO(vladl@google.com): Replace the following check with a
  // compile-time assertion when available.
  GTEST_CHECK_(sizeof(HANDLE) <= sizeof(size_t));

  const HANDLE status_handle =
      reinterpret_cast<HANDLE>(status_handle_as_size_t);
  HANDLE dup_status_handle;

  // The newly initialized handle is accessible only in in the parent
  // process. To obtain one accessible within the child, we need to use
  // DuplicateHandle.
  if (!::DuplicateHandle(parent_process_handle.Get(), status_handle,
                         ::GetCurrentProcess(), &dup_status_handle,
                         0x0,    // Requested privileges ignored since
                                 // DUPLICATE_SAME_ACCESS is used.
                         FALSE,  // Request non-inheritable handler.
                         DUPLICATE_SAME_ACCESS)) {
    DeathTestAbort(String::Format(
        "Unable to duplicate the pipe handle %Iu from the parent process %u",
        status_handle_as_size_t, parent_process_id));
  }

  const HANDLE event_handle = reinterpret_cast<HANDLE>(event_handle_as_size_t);
  HANDLE dup_event_handle;

  if (!::DuplicateHandle(parent_process_handle.Get(), event_handle,
                         ::GetCurrentProcess(), &dup_event_handle,
                         0x0,
                         FALSE,
                         DUPLICATE_SAME_ACCESS)) {
    DeathTestAbort(String::Format(
        "Unable to duplicate the event handle %Iu from the parent process %u",
        event_handle_as_size_t, parent_process_id));
  }

  const int status_fd =
      ::_open_osfhandle(reinterpret_cast<intptr_t>(dup_status_handle),
                      O_APPEND | O_TEXT);
  if (status_fd == -1) {
    DeathTestAbort(String::Format(
        "Unable to convert pipe handle %Iu to a file descriptor",
        status_handle_as_size_t));
  }

  // Signals the parent that the write end of the pipe has been acquired
  // so the parent can release its own write end.
  ::SetEvent(dup_event_handle);

  return status_fd;
}
#endif  // GTEST_OS_WINDOWS

// Returns a newly created InternalRunDeathTestFlag object with fields
// initialized from the GTEST_FLAG(internal_run_death_test) flag if
// the flag is specified; otherwise returns NULL.
InternalRunDeathTestFlag* ParseInternalRunDeathTestFlag() {
  if (GTEST_FLAG(internal_run_death_test) == "") return NULL;

  // GTEST_HAS_DEATH_TEST implies that we have ::std::string, so we
  // can use it here.
  int line = -1;
  int index = -1;
  ::std::vector< ::std::string> fields;
  SplitString(GTEST_FLAG(internal_run_death_test).c_str(), '|', &fields);
  int status_fd = -1;

#if GTEST_OS_WINDOWS
  unsigned int parent_process_id = 0;
  size_t status_handle_as_size_t = 0;
  size_t event_handle_as_size_t = 0;

  if (fields.size() != 6
      || !ParseNaturalNumber(fields[1], &line)
      || !ParseNaturalNumber(fields[2], &index)
      || !ParseNaturalNumber(fields[3], &parent_process_id)
      || !ParseNaturalNumber(fields[4], &status_handle_as_size_t)
      || !ParseNaturalNumber(fields[5], &event_handle_as_size_t)) {
    DeathTestAbort(String::Format(
        "Bad --gtest_internal_run_death_test flag: %s",
        GTEST_FLAG(internal_run_death_test).c_str()));
  }
  status_fd = GetStatusFileDescriptor(parent_process_id,
                                      status_handle_as_size_t,
                                      event_handle_as_size_t);
#else
  if (fields.size() != 4
      || !ParseNaturalNumber(fields[1], &line)
      || !ParseNaturalNumber(fields[2], &index)
      || !ParseNaturalNumber(fields[3], &status_fd)) {
    DeathTestAbort(String::Format(
        "Bad --gtest_internal_run_death_test flag: %s",
        GTEST_FLAG(internal_run_death_test).c_str()));
  }
#endif  // GTEST_OS_WINDOWS
  return new InternalRunDeathTestFlag(fields[0], line, index, status_fd);
}

}  // namespace internal

#endif  // GTEST_HAS_DEATH_TEST

}  // namespace testing
// Copyright 2008, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Authors: keith.ray@gmail.com (Keith Ray)


#include <stdlib.h>

#ifdef _WIN32_WCE
#include <windows.h>
#elif GTEST_OS_WINDOWS
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#elif GTEST_OS_SYMBIAN
// Symbian OpenC has PATH_MAX in sys/syslimits.h
#include <sys/syslimits.h>
#include <unistd.h>
#else
#include <limits.h>
#include <sys/stat.h>  // NOLINT
#include <unistd.h>  // NOLINT
#include <climits>  // Some Linux distributions define PATH_MAX here.
#endif  // _WIN32_WCE or _WIN32

#if GTEST_OS_WINDOWS
#define GTEST_PATH_MAX_ _MAX_PATH
#elif defined(PATH_MAX)
#define GTEST_PATH_MAX_ PATH_MAX
#elif defined(_XOPEN_PATH_MAX)
#define GTEST_PATH_MAX_ _XOPEN_PATH_MAX
#else
#define GTEST_PATH_MAX_ _POSIX_PATH_MAX
#endif  // GTEST_OS_WINDOWS


namespace testing {
namespace internal {

#if GTEST_OS_WINDOWS
const char kPathSeparator = '\\';
const char kPathSeparatorString[] = "\\";
#ifdef _WIN32_WCE
// Windows CE doesn't have a current directory. You should not use
// the current directory in tests on Windows CE, but this at least
// provides a reasonable fallback.
const char kCurrentDirectoryString[] = "\\";
// Windows CE doesn't define INVALID_FILE_ATTRIBUTES
const DWORD kInvalidFileAttributes = 0xffffffff;
#else
const char kCurrentDirectoryString[] = ".\\";
#endif  // _WIN32_WCE
#else
const char kPathSeparator = '/';
const char kPathSeparatorString[] = "/";
const char kCurrentDirectoryString[] = "./";
#endif  // GTEST_OS_WINDOWS

// Returns the current working directory, or "" if unsuccessful.
FilePath FilePath::GetCurrentDir() {
#ifdef _WIN32_WCE
// Windows CE doesn't have a current directory, so we just return
// something reasonable.
  return FilePath(kCurrentDirectoryString);
#elif GTEST_OS_WINDOWS
  char cwd[GTEST_PATH_MAX_ + 1] = {};
  return FilePath(_getcwd(cwd, sizeof(cwd)) == NULL ? "" : cwd);
#else
  char cwd[GTEST_PATH_MAX_ + 1] = {};
  return FilePath(getcwd(cwd, sizeof(cwd)) == NULL ? "" : cwd);
#endif
}

// Returns a copy of the FilePath with the case-insensitive extension removed.
// Example: FilePath("dir/file.exe").RemoveExtension("EXE") returns
// FilePath("dir/file"). If a case-insensitive extension is not
// found, returns a copy of the original FilePath.
FilePath FilePath::RemoveExtension(const char* extension) const {
  String dot_extension(String::Format(".%s", extension));
  if (pathname_.EndsWithCaseInsensitive(dot_extension.c_str())) {
    return FilePath(String(pathname_.c_str(), pathname_.GetLength() - 4));
  }
  return *this;
}

// Returns a copy of the FilePath with the directory part removed.
// Example: FilePath("path/to/file").RemoveDirectoryName() returns
// FilePath("file"). If there is no directory part ("just_a_file"), it returns
// the FilePath unmodified. If there is no file part ("just_a_dir/") it
// returns an empty FilePath ("").
// On Windows platform, '\' is the path separator, otherwise it is '/'.
FilePath FilePath::RemoveDirectoryName() const {
  const char* const last_sep = strrchr(c_str(), kPathSeparator);
  return last_sep ? FilePath(String(last_sep + 1)) : *this;
}

// RemoveFileName returns the directory path with the filename removed.
// Example: FilePath("path/to/file").RemoveFileName() returns "path/to/".
// If the FilePath is "a_file" or "/a_file", RemoveFileName returns
// FilePath("./") or, on Windows, FilePath(".\\"). If the filepath does
// not have a file, like "just/a/dir/", it returns the FilePath unmodified.
// On Windows platform, '\' is the path separator, otherwise it is '/'.
FilePath FilePath::RemoveFileName() const {
  const char* const last_sep = strrchr(c_str(), kPathSeparator);
  return FilePath(last_sep ? String(c_str(), last_sep + 1 - c_str())
                           : String(kCurrentDirectoryString));
}

// Helper functions for naming files in a directory for xml output.

// Given directory = "dir", base_name = "test", number = 0,
// extension = "xml", returns "dir/test.xml". If number is greater
// than zero (e.g., 12), returns "dir/test_12.xml".
// On Windows platform, uses \ as the separator rather than /.
FilePath FilePath::MakeFileName(const FilePath& directory,
                                const FilePath& base_name,
                                int number,
                                const char* extension) {
  const FilePath file_name(
      (number == 0) ?
      String::Format("%s.%s", base_name.c_str(), extension) :
      String::Format("%s_%d.%s", base_name.c_str(), number, extension));
  return ConcatPaths(directory, file_name);
}

// Given directory = "dir", relative_path = "test.xml", returns "dir/test.xml".
// On Windows, uses \ as the separator rather than /.
FilePath FilePath::ConcatPaths(const FilePath& directory,
                               const FilePath& relative_path) {
  if (directory.IsEmpty())
    return relative_path;
  const FilePath dir(directory.RemoveTrailingPathSeparator());
  return FilePath(String::Format("%s%c%s", dir.c_str(), kPathSeparator,
                                 relative_path.c_str()));
}

// Returns true if pathname describes something findable in the file-system,
// either a file, directory, or whatever.
bool FilePath::FileOrDirectoryExists() const {
#if GTEST_OS_WINDOWS
#ifdef _WIN32_WCE
  LPCWSTR unicode = String::AnsiToUtf16(pathname_.c_str());
  const DWORD attributes = GetFileAttributes(unicode);
  delete [] unicode;
  return attributes != kInvalidFileAttributes;
#else
  struct _stat file_stat = {};
  return _stat(pathname_.c_str(), &file_stat) == 0;
#endif  // _WIN32_WCE
#else
  struct stat file_stat = {};
  return stat(pathname_.c_str(), &file_stat) == 0;
#endif  // GTEST_OS_WINDOWS
}

// Returns true if pathname describes a directory in the file-system
// that exists.
bool FilePath::DirectoryExists() const {
  bool result = false;
#if GTEST_OS_WINDOWS
  // Don't strip off trailing separator if path is a root directory on
  // Windows (like "C:\\").
  const FilePath& path(IsRootDirectory() ? *this :
                                           RemoveTrailingPathSeparator());
#ifdef _WIN32_WCE
  LPCWSTR unicode = String::AnsiToUtf16(path.c_str());
  const DWORD attributes = GetFileAttributes(unicode);
  delete [] unicode;
  if ((attributes != kInvalidFileAttributes) &&
      (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
    result = true;
  }
#else
  struct _stat file_stat = {};
  result = _stat(path.c_str(), &file_stat) == 0 &&
      (_S_IFDIR & file_stat.st_mode) != 0;
#endif  // _WIN32_WCE
#else
  struct stat file_stat = {};
  result = stat(pathname_.c_str(), &file_stat) == 0 &&
      S_ISDIR(file_stat.st_mode);
#endif  // GTEST_OS_WINDOWS
  return result;
}

// Returns true if pathname describes a root directory. (Windows has one
// root directory per disk drive.)
bool FilePath::IsRootDirectory() const {
#if GTEST_OS_WINDOWS
  // TODO(wan@google.com): on Windows a network share like
  // \\server\share can be a root directory, although it cannot be the
  // current directory.  Handle this properly.
  return pathname_.GetLength() == 3 && IsAbsolutePath();
#else
  return pathname_ == kPathSeparatorString;
#endif
}

// Returns true if pathname describes an absolute path.
bool FilePath::IsAbsolutePath() const {
  const char* const name = pathname_.c_str();
#if GTEST_OS_WINDOWS
  return pathname_.GetLength() >= 3 &&
     ((name[0] >= 'a' && name[0] <= 'z') ||
      (name[0] >= 'A' && name[0] <= 'Z')) &&
     name[1] == ':' &&
     name[2] == kPathSeparator;
#else
  return name[0] == kPathSeparator;
#endif
}

// Returns a pathname for a file that does not currently exist. The pathname
// will be directory/base_name.extension or
// directory/base_name_<number>.extension if directory/base_name.extension
// already exists. The number will be incremented until a pathname is found
// that does not already exist.
// Examples: 'dir/foo_test.xml' or 'dir/foo_test_1.xml'.
// There could be a race condition if two or more processes are calling this
// function at the same time -- they could both pick the same filename.
FilePath FilePath::GenerateUniqueFileName(const FilePath& directory,
                                          const FilePath& base_name,
                                          const char* extension) {
  FilePath full_pathname;
  int number = 0;
  do {
    full_pathname.Set(MakeFileName(directory, base_name, number++, extension));
  } while (full_pathname.FileOrDirectoryExists());
  return full_pathname;
}

// Returns true if FilePath ends with a path separator, which indicates that
// it is intended to represent a directory. Returns false otherwise.
// This does NOT check that a directory (or file) actually exists.
bool FilePath::IsDirectory() const {
  return pathname_.EndsWith(kPathSeparatorString);
}

// Create directories so that path exists. Returns true if successful or if
// the directories already exist; returns false if unable to create directories
// for any reason.
bool FilePath::CreateDirectoriesRecursively() const {
  if (!this->IsDirectory()) {
    return false;
  }

  if (pathname_.GetLength() == 0 || this->DirectoryExists()) {
    return true;
  }

  const FilePath parent(this->RemoveTrailingPathSeparator().RemoveFileName());
  return parent.CreateDirectoriesRecursively() && this->CreateFolder();
}

// Create the directory so that path exists. Returns true if successful or
// if the directory already exists; returns false if unable to create the
// directory for any reason, including if the parent directory does not
// exist. Not named "CreateDirectory" because that's a macro on Windows.
bool FilePath::CreateFolder() const {
#if GTEST_OS_WINDOWS
#ifdef _WIN32_WCE
  FilePath removed_sep(this->RemoveTrailingPathSeparator());
  LPCWSTR unicode = String::AnsiToUtf16(removed_sep.c_str());
  int result = CreateDirectory(unicode, NULL) ? 0 : -1;
  delete [] unicode;
#else
  int result = _mkdir(pathname_.c_str());
#endif  // !WIN32_WCE
#else
  int result = mkdir(pathname_.c_str(), 0777);
#endif  // _WIN32
  if (result == -1) {
    return this->DirectoryExists();  // An error is OK if the directory exists.
  }
  return true;  // No error.
}

// If input name has a trailing separator character, remove it and return the
// name, otherwise return the name string unmodified.
// On Windows platform, uses \ as the separator, other platforms use /.
FilePath FilePath::RemoveTrailingPathSeparator() const {
  return pathname_.EndsWith(kPathSeparatorString)
      ? FilePath(String(pathname_.c_str(), pathname_.GetLength() - 1))
      : *this;
}

// Normalize removes any redundant separators that might be in the pathname.
// For example, "bar///foo" becomes "bar/foo". Does not eliminate other
// redundancies that might be in a pathname involving "." or "..".
void FilePath::Normalize() {
  if (pathname_.c_str() == NULL) {
    pathname_ = "";
    return;
  }
  const char* src = pathname_.c_str();
  char* const dest = new char[pathname_.GetLength() + 1];
  char* dest_ptr = dest;
  memset(dest_ptr, 0, pathname_.GetLength() + 1);

  while (*src != '\0') {
    *dest_ptr++ = *src;
    if (*src != kPathSeparator)
      src++;
    else
      while (*src == kPathSeparator)
        src++;
  }
  *dest_ptr = '\0';
  pathname_ = dest;
  delete[] dest;
}

}  // namespace internal
}  // namespace testing
// Copyright 2008, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: wan@google.com (Zhanyong Wan)


#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#if GTEST_OS_WINDOWS
#include <io.h>
#include <sys/stat.h>
#else
#include <unistd.h>
#endif  // GTEST_OS_WINDOWS

#if GTEST_USES_SIMPLE_RE
#include <string.h>
#endif

#ifdef _WIN32_WCE
#include <windows.h>  // For TerminateProcess()
#endif  // _WIN32_WCE


// Indicates that this translation unit is part of Google Test's
// implementation.  It must come before gtest-internal-inl.h is
// included, or there will be a compiler error.  This trick is to
// prevent a user from accidentally including gtest-internal-inl.h in
// his code.
#define GTEST_IMPLEMENTATION_ 1
#undef GTEST_IMPLEMENTATION_

namespace testing {
namespace internal {

#if GTEST_OS_WINDOWS
// Microsoft does not provide a definition of STDERR_FILENO.
const int kStdErrFileno = 2;
#else
const int kStdErrFileno = STDERR_FILENO;
#endif  // GTEST_OS_WINDOWS

#if GTEST_USES_POSIX_RE

// Implements RE.  Currently only needed for death tests.

RE::~RE() {
  regfree(&partial_regex_);
  regfree(&full_regex_);
  free(const_cast<char*>(pattern_));
}

// Returns true iff regular expression re matches the entire str.
bool RE::FullMatch(const char* str, const RE& re) {
  if (!re.is_valid_) return false;

  regmatch_t match;
  return regexec(&re.full_regex_, str, 1, &match, 0) == 0;
}

// Returns true iff regular expression re matches a substring of str
// (including str itself).
bool RE::PartialMatch(const char* str, const RE& re) {
  if (!re.is_valid_) return false;

  regmatch_t match;
  return regexec(&re.partial_regex_, str, 1, &match, 0) == 0;
}

// Initializes an RE from its string representation.
void RE::Init(const char* regex) {
  pattern_ = strdup(regex);

  // Reserves enough bytes to hold the regular expression used for a
  // full match.
  const size_t full_regex_len = strlen(regex) + 10;
  char* const full_pattern = new char[full_regex_len];

  snprintf(full_pattern, full_regex_len, "^(%s)$", regex);
  is_valid_ = regcomp(&full_regex_, full_pattern, REG_EXTENDED) == 0;
  // We want to call regcomp(&partial_regex_, ...) even if the
  // previous expression returns false.  Otherwise partial_regex_ may
  // not be properly initialized can may cause trouble when it's
  // freed.
  //
  // Some implementation of POSIX regex (e.g. on at least some
  // versions of Cygwin) doesn't accept the empty string as a valid
  // regex.  We change it to an equivalent form "()" to be safe.
  const char* const partial_regex = (*regex == '\0') ? "()" : regex;
  is_valid_ = (regcomp(&partial_regex_, partial_regex, REG_EXTENDED) == 0)
      && is_valid_;
  EXPECT_TRUE(is_valid_)
      << "Regular expression \"" << regex
      << "\" is not a valid POSIX Extended regular expression.";

  delete[] full_pattern;
}

#elif GTEST_USES_SIMPLE_RE

// Returns true iff ch appears anywhere in str (excluding the
// terminating '\0' character).
bool IsInSet(char ch, const char* str) {
  return ch != '\0' && strchr(str, ch) != NULL;
}

// Returns true iff ch belongs to the given classification.  Unlike
// similar functions in <ctype.h>, these aren't affected by the
// current locale.
bool IsDigit(char ch) { return '0' <= ch && ch <= '9'; }
bool IsPunct(char ch) {
  return IsInSet(ch, "^-!\"#$%&'()*+,./:;<=>?@[\\]_`{|}~");
}
bool IsRepeat(char ch) { return IsInSet(ch, "?*+"); }
bool IsWhiteSpace(char ch) { return IsInSet(ch, " \f\n\r\t\v"); }
bool IsWordChar(char ch) {
  return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') ||
      ('0' <= ch && ch <= '9') || ch == '_';
}

// Returns true iff "\\c" is a supported escape sequence.
bool IsValidEscape(char c) {
  return (IsPunct(c) || IsInSet(c, "dDfnrsStvwW"));
}

// Returns true iff the given atom (specified by escaped and pattern)
// matches ch.  The result is undefined if the atom is invalid.
bool AtomMatchesChar(bool escaped, char pattern_char, char ch) {
  if (escaped) {  // "\\p" where p is pattern_char.
    switch (pattern_char) {
      case 'd': return IsDigit(ch);
      case 'D': return !IsDigit(ch);
      case 'f': return ch == '\f';
      case 'n': return ch == '\n';
      case 'r': return ch == '\r';
      case 's': return IsWhiteSpace(ch);
      case 'S': return !IsWhiteSpace(ch);
      case 't': return ch == '\t';
      case 'v': return ch == '\v';
      case 'w': return IsWordChar(ch);
      case 'W': return !IsWordChar(ch);
    }
    return IsPunct(pattern_char) && pattern_char == ch;
  }

  return (pattern_char == '.' && ch != '\n') || pattern_char == ch;
}

// Helper function used by ValidateRegex() to format error messages.
String FormatRegexSyntaxError(const char* regex, int index) {
  return (Message() << "Syntax error at index " << index
          << " in simple regular expression \"" << regex << "\": ").GetString();
}

// Generates non-fatal failures and returns false if regex is invalid;
// otherwise returns true.
bool ValidateRegex(const char* regex) {
  if (regex == NULL) {
    // TODO(wan@google.com): fix the source file location in the
    // assertion failures to match where the regex is used in user
    // code.
    ADD_FAILURE() << "NULL is not a valid simple regular expression.";
    return false;
  }

  bool is_valid = true;

  // True iff ?, *, or + can follow the previous atom.
  bool prev_repeatable = false;
  for (int i = 0; regex[i]; i++) {
    if (regex[i] == '\\') {  // An escape sequence
      i++;
      if (regex[i] == '\0') {
        ADD_FAILURE() << FormatRegexSyntaxError(regex, i - 1)
                      << "'\\' cannot appear at the end.";
        return false;
      }

      if (!IsValidEscape(regex[i])) {
        ADD_FAILURE() << FormatRegexSyntaxError(regex, i - 1)
                      << "invalid escape sequence \"\\" << regex[i] << "\".";
        is_valid = false;
      }
      prev_repeatable = true;
    } else {  // Not an escape sequence.
      const char ch = regex[i];

      if (ch == '^' && i > 0) {
        ADD_FAILURE() << FormatRegexSyntaxError(regex, i)
                      << "'^' can only appear at the beginning.";
        is_valid = false;
      } else if (ch == '$' && regex[i + 1] != '\0') {
        ADD_FAILURE() << FormatRegexSyntaxError(regex, i)
                      << "'$' can only appear at the end.";
        is_valid = false;
      } else if (IsInSet(ch, "()[]{}|")) {
        ADD_FAILURE() << FormatRegexSyntaxError(regex, i)
                      << "'" << ch << "' is unsupported.";
        is_valid = false;
      } else if (IsRepeat(ch) && !prev_repeatable) {
        ADD_FAILURE() << FormatRegexSyntaxError(regex, i)
                      << "'" << ch << "' can only follow a repeatable token.";
        is_valid = false;
      }

      prev_repeatable = !IsInSet(ch, "^$?*+");
    }
  }

  return is_valid;
}

// Matches a repeated regex atom followed by a valid simple regular
// expression.  The regex atom is defined as c if escaped is false,
// or \c otherwise.  repeat is the repetition meta character (?, *,
// or +).  The behavior is undefined if str contains too many
// characters to be indexable by size_t, in which case the test will
// probably time out anyway.  We are fine with this limitation as
// std::string has it too.
bool MatchRepetitionAndRegexAtHead(
    bool escaped, char c, char repeat, const char* regex,
    const char* str) {
  const size_t min_count = (repeat == '+') ? 1 : 0;
  const size_t max_count = (repeat == '?') ? 1 :
      static_cast<size_t>(-1) - 1;
  // We cannot call numeric_limits::max() as it conflicts with the
  // max() macro on Windows.

  for (size_t i = 0; i <= max_count; ++i) {
    // We know that the atom matches each of the first i characters in str.
    if (i >= min_count && MatchRegexAtHead(regex, str + i)) {
      // We have enough matches at the head, and the tail matches too.
      // Since we only care about *whether* the pattern matches str
      // (as opposed to *how* it matches), there is no need to find a
      // greedy match.
      return true;
    }
    if (str[i] == '\0' || !AtomMatchesChar(escaped, c, str[i]))
      return false;
  }
  return false;
}

// Returns true iff regex matches a prefix of str.  regex must be a
// valid simple regular expression and not start with "^", or the
// result is undefined.
bool MatchRegexAtHead(const char* regex, const char* str) {
  if (*regex == '\0')  // An empty regex matches a prefix of anything.
    return true;

  // "$" only matches the end of a string.  Note that regex being
  // valid guarantees that there's nothing after "$" in it.
  if (*regex == '$')
    return *str == '\0';

  // Is the first thing in regex an escape sequence?
  const bool escaped = *regex == '\\';
  if (escaped)
    ++regex;
  if (IsRepeat(regex[1])) {
    // MatchRepetitionAndRegexAtHead() calls MatchRegexAtHead(), so
    // here's an indirect recursion.  It terminates as the regex gets
    // shorter in each recursion.
    return MatchRepetitionAndRegexAtHead(
        escaped, regex[0], regex[1], regex + 2, str);
  } else {
    // regex isn't empty, isn't "$", and doesn't start with a
    // repetition.  We match the first atom of regex with the first
    // character of str and recurse.
    return (*str != '\0') && AtomMatchesChar(escaped, *regex, *str) &&
        MatchRegexAtHead(regex + 1, str + 1);
  }
}

// Returns true iff regex matches any substring of str.  regex must be
// a valid simple regular expression, or the result is undefined.
//
// The algorithm is recursive, but the recursion depth doesn't exceed
// the regex length, so we won't need to worry about running out of
// stack space normally.  In rare cases the time complexity can be
// exponential with respect to the regex length + the string length,
// but usually it's must faster (often close to linear).
bool MatchRegexAnywhere(const char* regex, const char* str) {
  if (regex == NULL || str == NULL)
    return false;

  if (*regex == '^')
    return MatchRegexAtHead(regex + 1, str);

  // A successful match can be anywhere in str.
  do {
    if (MatchRegexAtHead(regex, str))
      return true;
  } while (*str++ != '\0');
  return false;
}

// Implements the RE class.

RE::~RE() {
  free(const_cast<char*>(pattern_));
  free(const_cast<char*>(full_pattern_));
}

// Returns true iff regular expression re matches the entire str.
bool RE::FullMatch(const char* str, const RE& re) {
  return re.is_valid_ && MatchRegexAnywhere(re.full_pattern_, str);
}

// Returns true iff regular expression re matches a substring of str
// (including str itself).
bool RE::PartialMatch(const char* str, const RE& re) {
  return re.is_valid_ && MatchRegexAnywhere(re.pattern_, str);
}

// Initializes an RE from its string representation.
void RE::Init(const char* regex) {
  pattern_ = full_pattern_ = NULL;
  if (regex != NULL) {
#if GTEST_OS_WINDOWS
    pattern_ = _strdup(regex);
#else
    pattern_ = strdup(regex);
#endif
  }

  is_valid_ = ValidateRegex(regex);
  if (!is_valid_) {
    // No need to calculate the full pattern when the regex is invalid.
    return;
  }

  const size_t len = strlen(regex);
  // Reserves enough bytes to hold the regular expression used for a
  // full match: we need space to prepend a '^', append a '$', and
  // terminate the string with '\0'.
  char* buffer = static_cast<char*>(malloc(len + 3));
  full_pattern_ = buffer;

  if (*regex != '^')
    *buffer++ = '^';  // Makes sure full_pattern_ starts with '^'.

  // We don't use snprintf or strncpy, as they trigger a warning when
  // compiled with VC++ 8.0.
  memcpy(buffer, regex, len);
  buffer += len;

  if (len == 0 || regex[len - 1] != '$')
    *buffer++ = '$';  // Makes sure full_pattern_ ends with '$'.

  *buffer = '\0';
}

#endif  // GTEST_USES_POSIX_RE

// Logs a message at the given severity level.
void GTestLog(GTestLogSeverity severity, const char* file,
              int line, const char* msg) {
  const char* const marker =
      severity == GTEST_INFO ?    "[  INFO ]" :
      severity == GTEST_WARNING ? "[WARNING]" :
      severity == GTEST_ERROR ?   "[ ERROR ]" : "[ FATAL ]";
  fprintf(stderr, "\n%s %s:%d: %s\n", marker, file, line, msg);
  if (severity == GTEST_FATAL) {
    fflush(NULL);  // abort() is not guaranteed to flush open file streams.
    abort();
  }
}

#if GTEST_HAS_STD_STRING

// Disable Microsoft deprecation warnings for POSIX functions called from
// this class (creat, dup, dup2, and close)
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif  // _MSC_VER

// Defines the stderr capturer.

class CapturedStderr {
 public:
  // The ctor redirects stderr to a temporary file.
  CapturedStderr() {
    uncaptured_fd_ = dup(kStdErrFileno);

#if GTEST_OS_WINDOWS
    char temp_dir_path[MAX_PATH + 1] = { '\0' };  // NOLINT
    char temp_file_path[MAX_PATH + 1] = { '\0' };  // NOLINT

    ::GetTempPathA(sizeof(temp_dir_path), temp_dir_path);
    ::GetTempFileNameA(temp_dir_path, "gtest_redir", 0, temp_file_path);
    const int captured_fd = creat(temp_file_path, _S_IREAD | _S_IWRITE);
    filename_ = temp_file_path;
#else
    // There's no guarantee that a test has write access to the
    // current directory, so we create the temporary file in the /tmp
    // directory instead.
    char name_template[] = "/tmp/captured_stderr.XXXXXX";
    const int captured_fd = mkstemp(name_template);
    filename_ = name_template;
#endif  // GTEST_OS_WINDOWS
    fflush(NULL);
    dup2(captured_fd, kStdErrFileno);
    close(captured_fd);
  }

  ~CapturedStderr() {
    remove(filename_.c_str());
  }

  // Stops redirecting stderr.
  void StopCapture() {
    // Restores the original stream.
    fflush(NULL);
    dup2(uncaptured_fd_, kStdErrFileno);
    close(uncaptured_fd_);
    uncaptured_fd_ = -1;
  }

  // Returns the name of the temporary file holding the stderr output.
  // GTEST_HAS_DEATH_TEST implies that we have ::std::string, so we
  // can use it here.
  ::std::string filename() const { return filename_; }

 private:
  int uncaptured_fd_;
  ::std::string filename_;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

static CapturedStderr* g_captured_stderr = NULL;

// Returns the size (in bytes) of a file.
static size_t GetFileSize(FILE * file) {
  fseek(file, 0, SEEK_END);
  return static_cast<size_t>(ftell(file));
}

// Reads the entire content of a file as a string.
static ::std::string ReadEntireFile(FILE * file) {
  const size_t file_size = GetFileSize(file);
  char* const buffer = new char[file_size];

  size_t bytes_last_read = 0;  // # of bytes read in the last fread()
  size_t bytes_read = 0;       // # of bytes read so far

  fseek(file, 0, SEEK_SET);

  // Keeps reading the file until we cannot read further or the
  // pre-determined file size is reached.
  do {
    bytes_last_read = fread(buffer+bytes_read, 1, file_size-bytes_read, file);
    bytes_read += bytes_last_read;
  } while (bytes_last_read > 0 && bytes_read < file_size);

  const ::std::string content(buffer, buffer+bytes_read);
  delete[] buffer;

  return content;
}

// Starts capturing stderr.
void CaptureStderr() {
  if (g_captured_stderr != NULL) {
    GTEST_LOG_(FATAL, "Only one stderr capturer can exist at one time.");
  }
  g_captured_stderr = new CapturedStderr;
}

// Stops capturing stderr and returns the captured string.
// GTEST_HAS_DEATH_TEST implies that we have ::std::string, so we can
// use it here.
::std::string GetCapturedStderr() {
  g_captured_stderr->StopCapture();

// Disables Microsoft deprecation warning for fopen and fclose.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif  // _MSC_VER
  FILE* const file = fopen(g_captured_stderr->filename().c_str(), "r");
  const ::std::string content = ReadEntireFile(file);
  fclose(file);
#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

  delete g_captured_stderr;
  g_captured_stderr = NULL;

  return content;
}

#endif  // GTEST_HAS_STD_STRING

#if GTEST_HAS_DEATH_TEST

// A copy of all command line arguments.  Set by InitGoogleTest().
::std::vector<String> g_argvs;

// Returns the command line as a vector of strings.
const ::std::vector<String>& GetArgvs() { return g_argvs; }

#endif  // GTEST_HAS_DEATH_TEST

#ifdef _WIN32_WCE
void abort() {
  DebugBreak();
  TerminateProcess(GetCurrentProcess(), 1);
}
#endif  // _WIN32_WCE

// Returns the name of the environment variable corresponding to the
// given flag.  For example, FlagToEnvVar("foo") will return
// "GTEST_FOO" in the open-source version.
static String FlagToEnvVar(const char* flag) {
  const String full_flag =
      (Message() << GTEST_FLAG_PREFIX_ << flag).GetString();

  Message env_var;
  for (int i = 0; i != full_flag.GetLength(); i++) {
    env_var << static_cast<char>(toupper(full_flag.c_str()[i]));
  }

  return env_var.GetString();
}

// Parses 'str' for a 32-bit signed integer.  If successful, writes
// the result to *value and returns true; otherwise leaves *value
// unchanged and returns false.
bool ParseInt32(const Message& src_text, const char* str, Int32* value) {
  // Parses the environment variable as a decimal integer.
  char* end = NULL;
  const long long_value = strtol(str, &end, 10);  // NOLINT

  // Has strtol() consumed all characters in the string?
  if (*end != '\0') {
    // No - an invalid character was encountered.
    Message msg;
    msg << "WARNING: " << src_text
        << " is expected to be a 32-bit integer, but actually"
        << " has value \"" << str << "\".\n";
    printf("%s", msg.GetString().c_str());
    fflush(stdout);
    return false;
  }

  // Is the parsed value in the range of an Int32?
  const Int32 result = static_cast<Int32>(long_value);
  if (long_value == LONG_MAX || long_value == LONG_MIN ||
      // The parsed value overflows as a long.  (strtol() returns
      // LONG_MAX or LONG_MIN when the input overflows.)
      result != long_value
      // The parsed value overflows as an Int32.
      ) {
    Message msg;
    msg << "WARNING: " << src_text
        << " is expected to be a 32-bit integer, but actually"
        << " has value " << str << ", which overflows.\n";
    printf("%s", msg.GetString().c_str());
    fflush(stdout);
    return false;
  }

  *value = result;
  return true;
}

// Reads and returns the Boolean environment variable corresponding to
// the given flag; if it's not set, returns default_value.
//
// The value is considered true iff it's not "0".
bool BoolFromGTestEnv(const char* flag, bool default_value) {
  const String env_var = FlagToEnvVar(flag);
  const char* const string_value = GetEnv(env_var.c_str());
  return string_value == NULL ?
      default_value : strcmp(string_value, "0") != 0;
}

// Reads and returns a 32-bit integer stored in the environment
// variable corresponding to the given flag; if it isn't set or
// doesn't represent a valid 32-bit integer, returns default_value.
Int32 Int32FromGTestEnv(const char* flag, Int32 default_value) {
  const String env_var = FlagToEnvVar(flag);
  const char* const string_value = GetEnv(env_var.c_str());
  if (string_value == NULL) {
    // The environment variable is not set.
    return default_value;
  }

  Int32 result = default_value;
  if (!ParseInt32(Message() << "Environment variable " << env_var,
                  string_value, &result)) {
    printf("The default value %s is used.\n",
           (Message() << default_value).GetString().c_str());
    fflush(stdout);
    return default_value;
  }

  return result;
}

// Reads and returns the string environment variable corresponding to
// the given flag; if it's not set, returns default_value.
const char* StringFromGTestEnv(const char* flag, const char* default_value) {
  const String env_var = FlagToEnvVar(flag);
  const char* const value = GetEnv(env_var.c_str());
  return value == NULL ? default_value : value;
}

}  // namespace internal
}  // namespace testing
// Copyright 2008, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: mheule@google.com (Markus Heule)
//
// The Google C++ Testing Framework (Google Test)


// Indicates that this translation unit is part of Google Test's
// implementation.  It must come before gtest-internal-inl.h is
// included, or there will be a compiler error.  This trick is to
// prevent a user from accidentally including gtest-internal-inl.h in
// his code.
#define GTEST_IMPLEMENTATION_ 1
#undef GTEST_IMPLEMENTATION_

namespace testing {

// Gets the summary of the failure message by omitting the stack trace
// in it.
internal::String TestPartResult::ExtractSummary(const char* message) {
  const char* const stack_trace = strstr(message, internal::kStackTraceMarker);
  return stack_trace == NULL ? internal::String(message) :
      internal::String(message, stack_trace - message);
}

// Prints a TestPartResult object.
std::ostream& operator<<(std::ostream& os, const TestPartResult& result) {
  return os << result.file_name() << ":"
            << result.line_number() << ": "
            << (result.type() == TPRT_SUCCESS ? "Success" :
                result.type() == TPRT_FATAL_FAILURE ? "Fatal failure" :
                "Non-fatal failure") << ":\n"
            << result.message() << std::endl;
}

// Constructs an empty TestPartResultArray.
TestPartResultArray::TestPartResultArray()
    : list_(new internal::List<TestPartResult>) {
}

// Destructs a TestPartResultArray.
TestPartResultArray::~TestPartResultArray() {
  delete list_;
}

// Appends a TestPartResult to the array.
void TestPartResultArray::Append(const TestPartResult& result) {
  list_->PushBack(result);
}

// Returns the TestPartResult at the given index (0-based).
const TestPartResult& TestPartResultArray::GetTestPartResult(int index) const {
  if (index < 0 || index >= size()) {
    printf("\nInvalid index (%d) into TestPartResultArray.\n", index);
    internal::abort();
  }

  const internal::ListNode<TestPartResult>* p = list_->Head();
  for (int i = 0; i < index; i++) {
    p = p->next();
  }

  return p->element();
}

// Returns the number of TestPartResult objects in the array.
int TestPartResultArray::size() const {
  return list_->size();
}

namespace internal {

HasNewFatalFailureHelper::HasNewFatalFailureHelper()
    : has_new_fatal_failure_(false),
      original_reporter_(UnitTest::GetInstance()->impl()->
                         GetTestPartResultReporterForCurrentThread()) {
  UnitTest::GetInstance()->impl()->SetTestPartResultReporterForCurrentThread(
      this);
}

HasNewFatalFailureHelper::~HasNewFatalFailureHelper() {
  UnitTest::GetInstance()->impl()->SetTestPartResultReporterForCurrentThread(
      original_reporter_);
}

void HasNewFatalFailureHelper::ReportTestPartResult(
    const TestPartResult& result) {
  if (result.fatally_failed())
    has_new_fatal_failure_ = true;
  original_reporter_->ReportTestPartResult(result);
}

}  // namespace internal

}  // namespace testing
// Copyright 2008 Google Inc.
// All Rights Reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: wan@google.com (Zhanyong Wan)


namespace testing {
namespace internal {

#if GTEST_HAS_TYPED_TEST_P

// Verifies that registered_tests match the test names in
// defined_test_names_; returns registered_tests if successful, or
// aborts the program otherwise.
const char* TypedTestCasePState::VerifyRegisteredTestNames(
    const char* file, int line, const char* registered_tests) {
  typedef ::std::set<const char*>::const_iterator DefinedTestIter;
  registered_ = true;

  Message errors;
  ::std::set<String> tests;
  for (const char* names = registered_tests; names != NULL;
       names = SkipComma(names)) {
    const String name = GetPrefixUntilComma(names);
    if (tests.count(name) != 0) {
      errors << "Test " << name << " is listed more than once.\n";
      continue;
    }

    bool found = false;
    for (DefinedTestIter it = defined_test_names_.begin();
         it != defined_test_names_.end();
         ++it) {
      if (name == *it) {
        found = true;
        break;
      }
    }

    if (found) {
      tests.insert(name);
    } else {
      errors << "No test named " << name
             << " can be found in this test case.\n";
    }
  }

  for (DefinedTestIter it = defined_test_names_.begin();
       it != defined_test_names_.end();
       ++it) {
    if (tests.count(*it) == 0) {
      errors << "You forgot to list test " << *it << ".\n";
    }
  }

  const String& errors_str = errors.GetString();
  if (errors_str != "") {
    fprintf(stderr, "%s %s", FormatFileLocation(file, line).c_str(),
            errors_str.c_str());
    fflush(stderr);
    abort();
  }

  return registered_tests;
}

#endif  // GTEST_HAS_TYPED_TEST_P

}  // namespace internal
}  // namespace testing
