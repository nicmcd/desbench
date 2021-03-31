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
#include "bench/SimpleComponent.h"

#include <cassert>
#include <cstdio>
#include <cstring>

#include "factory/ObjectFactory.h"

SimpleComponent::SimpleComponent(des::Simulator* _simulator,
                                 const std::string& _name, u64 _id,
                                 nlohmann::json _settings)
    : BenchComponent(_simulator, _name, _id, _settings) {}

void SimpleComponent::initialize() {
  u64 initial_events = initialEvents();
  for (u64 e = 0; e < initial_events; e++) {
    simulator->addEvent(new des::Event(
        this, std::bind(&SimpleComponent::handler, this, -id_, id_, id_),
        des::Time(0), true));
  }
}

void SimpleComponent::handler(s32 _a, f64 _b, char _c) {
  count_++;
  dlogf("hello world, from component #%lu, count %lu", id_, count_);

  if (run_) {
    nextEvent(_a + 1, _b + 1, _c + 1);
  }
}

void SimpleComponent::nextEvent(s32 _a, f64 _b, char _c) {
  SimpleComponent* component =
      reinterpret_cast<SimpleComponent*>(nextComponent());
  des::Time time = nextTime();
  des::Event* event = new des::Event(
      component, std::bind(&SimpleComponent::handler, this, _a, _b, _c), time,
      true);
  simulator->addEvent(event);
}

registerWithObjectFactory("simple", BenchComponent, SimpleComponent,
                          BENCH_ARGS);
