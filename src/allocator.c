// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#include <stdlib.h>
#include <stdio.h>

#include "rcutils/allocator.h"
#include "rcutils/macros.h"

static void *
__default_allocate(size_t size, void * state)
{
  (void)state;  // unused
  return malloc(size);
}

static void
__default_deallocate(void * pointer, void * state)
{
  (void)state;  // unused
  free(pointer);
}

static void *
__default_reallocate(void * pointer, size_t size, void * state)
{
  (void)state;  // unused
  return realloc(pointer, size);
}

static void *
__default_zero_allocate(size_t number_of_elements, size_t size_of_element, void * state)
{
  (void)state;  // unused
  return calloc(number_of_elements, size_of_element);
}

rcutils_allocator_t
rcutils_get_zero_initialized_allocator(void)
{
  static rcutils_allocator_t zero_allocator = {
    .allocate = NULL,
    .deallocate = NULL,
    .reallocate = NULL,
    .zero_allocate = NULL,
    .state = NULL,
  };
  return zero_allocator;
}

rcutils_allocator_t
rcutils_get_default_allocator()
{
  static rcutils_allocator_t default_allocator = {
    .allocate = __default_allocate,
    .deallocate = __default_deallocate,
    .reallocate = __default_reallocate,
    .zero_allocate = __default_zero_allocate,
    .state = NULL,
  };
  return default_allocator;
}

bool
rcutils_allocator_is_valid(const rcutils_allocator_t * allocator)
{
  if (
    !allocator ||
    !allocator->allocate ||
    !allocator->deallocate ||
    !allocator->zero_allocate ||
    !allocator->reallocate)
  {
    return false;
  }
  return true;
}

void *
rcutils_reallocf(void * pointer, size_t size, rcutils_allocator_t * allocator)
{
  if (!allocator || !allocator->reallocate || !allocator->deallocate) {
    // cannot deallocate pointer, so print message to stderr and return NULL
    static const char * msg =
      "[c_utilties|allocator.c:" RCUTILS_STRINGIFY(__LINE__) "] rcutils_reallocf(): "
      "invalid allocator or allocator function pointers, memory leaked\n";
    fwrite(msg, sizeof(char), sizeof(msg), stderr);
    return NULL;
  }
  void * new_pointer = allocator->reallocate(pointer, size, allocator->state);
  if (!new_pointer) {
    allocator->deallocate(pointer, allocator->state);
  }
  return new_pointer;
}
