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
#include "example/NewDeleteComponent.h"

#include <cassert>
#include <cstdio>
#include <cstring>

namespace example {

NewDeleteComponent::NewDeleteComponent(
    des::Simulator* _simulator, const std::string& _name,
    u64 _id, bool _shiftyEpsilon, bool _verbose)
    : BenchComponent(_simulator, _name, _id, _shiftyEpsilon, _verbose) {
  function();  // queue first event
}

NewDeleteComponent::~NewDeleteComponent() {}

NewDeleteComponent::Event::Event(des::Component* _component,
                             des::EventHandler _handler)
    : des::Event(_component, _handler) {}

void NewDeleteComponent::function() {
  NewDeleteComponent::Event* evt = new NewDeleteComponent::Event(
      this, static_cast<des::EventHandler>(&NewDeleteComponent::handler));
  evt->time = simulator->time() + 1;
  if (shiftyEpsilon_) {
    evt->time.setEpsilon((id_ + count_) % des::EPSILON_INV);
  } else {
    evt->time.setEpsilon(0);
  }
  simulator->addEvent(evt);
}

void NewDeleteComponent::handler(des::Event* _event) {
  Event* me = reinterpret_cast<Event*>(_event);
  delete me;

  count_++;
  if (verbose_ || count_ < 5) {
    dlogf("hello world, from component #%lu, count %lu", id_, count_);
  }

  if (run_) {
    function();  // queue another event
  }
}

}  // namespace example
