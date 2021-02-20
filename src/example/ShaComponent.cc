/*
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
 * See the NOTICE file distributed with this work for additional information
 * regarding copyright ownership.
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
#include "example/ShaComponent.h"

#include <cassert>
#include <cstdio>
#include <cstring>

#include <algorithm>

#include "openssl/sha.h"

namespace example {

static const u64 MAX_HASH_SIZE = std::max(SHA_DIGEST_LENGTH,
                                          std::max(SHA256_DIGEST_LENGTH,
                                                   SHA512_DIGEST_LENGTH));

ShaComponent::ShaComponent(
    des::Simulator* _simulator, const std::string& _name,
    u64 _id, bool _shiftyEpsilon, u64 _shaBits, bool _verbose)
    : BenchComponent(_simulator, _name, _id, _shiftyEpsilon, _verbose),
      shaBits_(_shaBits),
      evt_(this, std::bind(&ShaComponent::handler, this)) {
  hash_ = new unsigned char[MAX_HASH_SIZE];

  nextEvent();  // queue first event
}

ShaComponent::~ShaComponent() {
  delete[] hash_;
}

void ShaComponent::handler() {
  count_++;
  if (verbose_ || count_ < 5) {
    dlogf("hello world, from component #%lu, count %lu", id_, count_);
  }

  if (run_) {
    switch (shaBits_) {
      case 1:
        SHA1(reinterpret_cast<const u8*>(basename().c_str()),
             basename().size(), hash_);
        break;
      case 256:
        SHA256(reinterpret_cast<const u8*>(basename().c_str()),
               basename().size(), hash_);
        break;
      case 512:
        SHA512(reinterpret_cast<const u8*>(basename().c_str()),
               basename().size(), hash_);
        break;
      default:
        assert(false);
    }

    nextEvent();  // queue another event
  }
}

void ShaComponent::nextEvent() {
  evt_.time = simulator->time() + 1;
  if (shiftyEpsilon_) {
    evt_.time.setEpsilon((id_ + count_) % des::EPSILON_INV);
  } else {
    evt_.time.setEpsilon(0);
  }
  simulator->addEvent(&evt_);
}

}  // namespace example
