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

#if __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/find.h"
#include "rcutils/types.h"

size_t
rcutils_find(const char * str, char delimiter)
{
  if (!str || strlen(str) == 0) {
    return 0;
  }
  return rcutils_findn(str, delimiter, strlen(str));
}

size_t
rcutils_findn(const char * str, char delimiter, size_t string_length)
{
  if (!str) {
    return 0;
  }

  for (size_t i = 0; i < string_length; ++i) {
    if (str[i] == delimiter) {
      return i;
    }
  }
  return string_length;
}

size_t
rcutils_find_last(const char * str, char delimiter)
{
  if (!str || strlen(str) == 0) {
    return 0;
  }
  return rcutils_find_lastn(str, delimiter, strlen(str));
}

size_t
rcutils_find_lastn(const char * str, char delimiter, size_t string_length)
{
  if (!str) {
    return 0;
  }
  size_t last_found = string_length;
  for (size_t i = 0; i < string_length; ++i) {
    if (str[i] == delimiter) {
      last_found = i;
    }
  }
  return last_found;
}

#if __cplusplus
}
#endif
