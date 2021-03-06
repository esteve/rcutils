// Copyright 2017 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gmock/gmock.h>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "rcutils/logging_macros.h"
#include "rcutils/time.h"

using ::testing::EndsWith;

size_t g_log_calls = 0;

struct LogEvent
{
  rcutils_log_location_t * location;
  int level;
  std::string name;
  std::string message;
};
LogEvent g_last_log_event;

class TestLoggingMacros : public ::testing::Test
{
public:
  rcutils_logging_output_handler_t previous_output_handler;
  void SetUp()
  {
    g_log_calls = 0;
    EXPECT_FALSE(g_rcutils_logging_initialized);
    rcutils_logging_initialize();
    EXPECT_TRUE(g_rcutils_logging_initialized);
    g_rcutils_logging_severity_threshold = RCUTILS_LOG_SEVERITY_DEBUG;
    EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, g_rcutils_logging_severity_threshold);

    auto rcutils_logging_console_output_handler = [](
      rcutils_log_location_t * location,
      int level, const char * name, const char * format, va_list * args) -> void
      {
        g_log_calls += 1;
        g_last_log_event.location = location;
        g_last_log_event.level = level;
        g_last_log_event.name = name ? name : "";
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, *args);
        g_last_log_event.message = buffer;
      };

    this->previous_output_handler = rcutils_logging_get_output_handler();
    rcutils_logging_set_output_handler(rcutils_logging_console_output_handler);
  }

  void TearDown()
  {
    rcutils_logging_set_output_handler(this->previous_output_handler);
    g_rcutils_logging_initialized = false;
    EXPECT_FALSE(g_rcutils_logging_initialized);
  }
};

TEST_F(TestLoggingMacros, test_logging_named) {
  for (int i : {1, 2, 3}) {
    RCUTILS_LOG_DEBUG_NAMED("name", "message %d", i);
  }
  EXPECT_EQ(3u, g_log_calls);
  EXPECT_TRUE(g_last_log_event.location != NULL);
  if (g_last_log_event.location) {
    EXPECT_STREQ("TestBody", g_last_log_event.location->function_name);
    EXPECT_THAT(g_last_log_event.location->file_name, EndsWith("test_logging_macros.cpp"));
    EXPECT_EQ(78u, g_last_log_event.location->line_number);
  }
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, g_last_log_event.level);
  EXPECT_EQ("name", g_last_log_event.name);
  EXPECT_EQ("message 3", g_last_log_event.message);
}

TEST_F(TestLoggingMacros, test_logging_once) {
  for (int i : {1, 2, 3}) {
    RCUTILS_LOG_INFO_ONCE("message %d", i);
  }
  EXPECT_EQ(1u, g_log_calls);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_INFO, g_last_log_event.level);
  EXPECT_EQ("", g_last_log_event.name);
  EXPECT_EQ("message 1", g_last_log_event.message);
}

TEST_F(TestLoggingMacros, test_logging_expression) {
  for (int i : {1, 2, 3, 4, 5, 6}) {
    RCUTILS_LOG_INFO_EXPRESSION(i % 3, "message %d", i);
  }
  EXPECT_EQ(4u, g_log_calls);
  EXPECT_EQ("message 5", g_last_log_event.message);
}

int g_counter = 0;

bool mod3()
{
  return (g_counter % 3) != 0;
}

TEST_F(TestLoggingMacros, test_logging_function) {
  for (int i : {1, 2, 3, 4, 5, 6}) {
    g_counter = i;
    RCUTILS_LOG_INFO_FUNCTION(&mod3, "message %d", i);
  }
  EXPECT_EQ(4u, g_log_calls);
  EXPECT_EQ("message 5", g_last_log_event.message);
}

TEST_F(TestLoggingMacros, test_logging_skipfirst) {
  for (uint32_t i : {1, 2, 3, 4, 5}) {
    RCUTILS_LOG_WARN_SKIPFIRST("message %u", i);
    EXPECT_EQ(i - 1, g_log_calls);
  }
}

TEST_F(TestLoggingMacros, test_logging_throttle) {
  for (int i : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}) {
    RCUTILS_LOG_ERROR_THROTTLE(RCUTILS_STEADY_TIME, 50 /* ms */, "throttled message %d", i)
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(30ms);
  }
  EXPECT_EQ(5u, g_log_calls);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_ERROR, g_last_log_event.level);
  EXPECT_EQ("", g_last_log_event.name);
  EXPECT_EQ("throttled message 8", g_last_log_event.message);
}

TEST_F(TestLoggingMacros, test_logging_skipfirst_throttle) {
  for (int i : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}) {
    RCUTILS_LOG_FATAL_SKIPFIRST_THROTTLE(
      RCUTILS_STEADY_TIME, 50 /* ms */, "throttled message %d", i)
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(30ms);
  }
  EXPECT_EQ(4u, g_log_calls);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_FATAL, g_last_log_event.level);
  EXPECT_EQ("", g_last_log_event.name);
  EXPECT_EQ("throttled message 8", g_last_log_event.message);
}
