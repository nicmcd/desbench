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
#include "example/PHoldComponent.h"

#include <cassert>
#include <cstdio>
#include <cstring>

#include <iostream>

namespace example {

PHoldComponent::PHoldComponent(
    des::Simulator *_simulator, const std::string &_name,
    u64 _id, bool _shiftyEpsilon, u64 _remPercentage, bool _verbose)
    : BenchComponent(_simulator, _name, _id, _shiftyEpsilon, _verbose) {
  assert(_remPercentage <= 100);
  remPercentage_ = static_cast<double>(_remPercentage) / 100.0;
  numInitialEvts_ = 10;

  stagger_ = false;
  lookAhead_ = 1;

  // start the sum
  sum_ = 0;

  // give the random generator a seed
  rnd_.seed(id_);
}

void PHoldComponent::initialize() {
  for (u32 e = 0; e < numInitialEvts_; e++) {
    des::Time time;
    if (stagger_) {
      time = simulator->time() + lookAhead_ + (id_ % des::TICK_INV);
    } else {
      time = simulator->time() + lookAhead_;
    }
    simulator->addEvent(new des::Event(
        this, std::bind(&PHoldComponent::handler, this), time, true));
  }
}

void PHoldComponent::handler() {
  count_++;
  if (verbose_ || count_ < 5) {
    dlogf("hello world, from component #%lu, count %lu", id_, count_);
  }

  sum_++;
  if (run_) {
    PHoldComponent* component = this;
    if (rnd_.nextF64() <= remPercentage_) {
      u64 componentId = rnd_.nextU64() % numComponents_;
      component = static_cast<PHoldComponent*>(allComponents_->at(componentId));
    }
    des::Time time = simulator->time() + lookAhead_;
    if (shiftyEpsilon_) {
      time.setEpsilon((id_ + count_) % des::EPSILON_INV);
    } else {
      time.setEpsilon(0);
    }
    simulator->addEvent(new des::Event(
        component,
        std::bind(&PHoldComponent::handler, component),
        time, true));
  }
}

}  // namespace example
