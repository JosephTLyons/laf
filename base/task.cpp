// LAF Base Library
// Copyright (C) 2019  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/task.h"

#include "base/debug.h"
#include "base/log.h"

namespace base {

task::task()
  : m_running(false)
  , m_completed(false)
{
}

task::~task()
{
  ASSERT(!m_running);
  ASSERT(m_completed);
}

task_token& task::start(thread_pool& pool)
{
  m_running = true;
  pool.execute([this]{ in_worker_thread(); });
  return m_token;
}

void task::in_worker_thread()
{
  try {
    if (!m_token.canceled())
      m_execute(m_token);
  }
  catch (const std::exception& ex) {
    LOG(FATAL, "Exception running task: %s\n", ex.what());
  }

  m_running = false;

  // This must be the latest statement in the worker thread (see
  // task::complete() comment)
  m_completed = true;
}

} // namespace base
