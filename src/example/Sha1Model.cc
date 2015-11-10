/*
 * Copyright (c) 2012-2015, Nic McDonald
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
 * - Neither the name of prim nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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
#include "example/Sha1Model.h"

#include <openssl/sha.h>

#include <cassert>
#include <cstdio>
#include <cstring>

namespace example {

Sha1Model::Sha1Model(des::Simulator* _simulator, const std::string& _name,
                     const des::Model* _parent, u64 _id, u64 _count,
                     bool _shiftyEpsilon, bool _verbose)
    : des::Model(_simulator, _name, _parent), id_(_id), count_(_count),
      shiftyEpsilon_(_shiftyEpsilon), verbose_(_verbose),
      evt_(this, static_cast<des::EventHandler>(&Sha1Model::handler)) {
  if (count_ > 0) {
    function();  // queue first event
  }
}

Sha1Model::~Sha1Model() {
  assert(count_ == 0);
}

Sha1Model::Event::Event(des::Model* _model,
                        des::EventHandler _handler)
    : des::Event(_model, _handler) {}

void Sha1Model::function() {
  evt_.time = simulator->time();
  evt_.time.tick++;
  if (shiftyEpsilon_) {
    evt_.time.epsilon = (id_ + count_) % 254;
  } else {
    evt_.time.epsilon = 0;
  }
  simulator->addEvent(&evt_);
}

void Sha1Model::handler(des::Event* _event) {
  Event* me = reinterpret_cast<Event*>(_event);
  (void)me;

  count_--;
  if (verbose_ || count_ < 5) {
    dlogf("hello world, from model #%lu, count %lu", id_, count_);
  }

  if (count_ > 0) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const u8*>(baseName().c_str()),
         baseName().size(), hash);
    function();  // queue another event
  }
}

}  // namespace example
