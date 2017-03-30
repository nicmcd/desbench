/*
 * Copyright (c) 2012-2016, Nic McDonald
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of prim nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior
 * written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "example/MemoryModel.h"

#include <cassert>
#include <cstdio>
#include <cstring>

namespace example {

MemoryModel::MemoryModel(des::Simulator* _simulator, const std::string& _name,
                         const des::Model* _parent, u64 _id,
                         bool _shiftyEpsilon, u64 _bytes, bool _verbose)
    : BenchModel(_simulator, _name, _parent, _id, _shiftyEpsilon, _verbose),
      bytes_(_bytes),
      evt_(this, static_cast<des::EventHandler>(&MemoryModel::handler)) {
  // give the random generator a seed
  rnd_.seed(id_);

  // create and initialize memory
  mem_ = new u8[bytes_];
  for (u64 byte = 0; byte < bytes_; byte += 4096) {
    mem_[byte] = (u8)(rnd_() % 256);
  }
  mem_[bytes_ - 1] = (u8)(rnd_() % 256);

  // queue first event
  function();
}

MemoryModel::~MemoryModel() {
  delete[] mem_;
}

MemoryModel::Event::Event(des::Model* _model,
                          des::EventHandler _handler)
    : des::Event(_model, _handler), index(0) {}

void MemoryModel::function() {
  evt_.time = simulator->time() + 1;
  if (shiftyEpsilon_) {
    evt_.time.setEpsilon((id_ + count_) % des::EPSILON_INV);
  } else {
    evt_.time.setEpsilon(0);
  }
  evt_.index = rnd_() % bytes_;
  simulator->addEvent(&evt_);
}

void MemoryModel::handler(des::Event* _event) {
  Event* me = reinterpret_cast<Event*>(_event);

  count_++;
  if (verbose_ || count_ < 5) {
    dlogf("hello world, from model #%lu, count %lu", id_, count_);
  }

  sum_ += mem_[me->index];

  if (run_) {
    function();  // queue another event
  }
}

}  // namespace example
