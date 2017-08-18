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
#include "example/BounceComponent.h"

#include <cassert>
#include <cstdio>
#include <cstring>

namespace example {

BounceComponent::BounceComponent(
    des::Simulator* _simulator, const std::string& _name,
    u64 _id, bool _shiftyEpsilon, u64 _events, bool _verbose)
    : BenchComponent(_simulator, _name, _id, _shiftyEpsilon, _verbose),
      events_(_events) {
  assert(_events > 0);

  // give the random generator a seed
  rnd_.seed(id_);
}

BounceComponent::~BounceComponent() {
  for (Event* e : evts_) {
    delete e;
  }
}

BounceComponent::Event::Event(des::ActiveComponent* _component,
                              des::EventHandler _handler)
    : des::Event(_component, _handler) {}

void BounceComponent::init() {
  // if id is 0 initialize events
  if (id_ == 0) {
    evts_.resize(events_, nullptr);
    for (u64 e = 0; e < events_; e++) {
      // make a random event
      u64 otherId = rnd_() % numComponents_;

      Event* evt = new Event(allComponents_->at(otherId),
                             makeHandler(BounceComponent, handle));
      evt->time = 0;

      // add to vector
      evts_.push_back(evt);

      // enqueue
      simulator->addEvent(evt);
    }
  }
}

void BounceComponent::handle(des::Event* _event) {
  Event* me = reinterpret_cast<Event*>(_event);

  count_++;
  if (verbose_ || count_ < 5) {
    dlogf("hello world, from component #%lu, count %lu (mine)", id_, count_);
  }

  if (run_) {
    // set new random component
    u64 otherId = rnd_() % numComponents_;
    me->component = allComponents_->at(otherId);

    // set new time
    me->time = simulator->time() + 1;
    if (shiftyEpsilon_) {
      me->time.setEpsilon((id_ + count_) % des::EPSILON_INV);
    } else {
      me->time.setEpsilon(0);
    }

    // enqueue
    simulator->addEvent(me);
  }
}

}  // namespace example
