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
#include "bench/MemoryComponent.h"

#include <cassert>
#include <cstdio>
#include <cstring>

#include "factory/ObjectFactory.h"

MemoryComponent::MemoryComponent(des::Simulator* _simulator,
                                 const std::string& _name, u64 _id,
                                 nlohmann::json _settings)
    : BenchComponent(_simulator, _name, _id, _settings) {
  // Creates and initializes the memory.
  bytes_ = _settings["bytes"].get<u64>();
  assert(bytes_ > 0);
  size_ = _settings["size"].get<u64>();
  assert(size_ > 0);
  assert(size_ <= bytes_);
  mem_ = new u8[bytes_];
  for (u64 byte = 0; byte < bytes_; byte += 4096) {
    mem_[byte] = (u8)(simulator->random()->nextU64() % U8_MAX);
  }
  mem_[bytes_ - 1] = (u8)(simulator->random()->nextU64() % U8_MAX);
}

MemoryComponent::~MemoryComponent() {
  delete[] mem_;
}

void MemoryComponent::initialize() {
  u64 initial_events = initialEvents();
  for (u64 e = 0; e < initial_events; e++) {
    simulator->addEvent(new des::Event(
        this, std::bind(&MemoryComponent::handler, this), des::Time(0), true));
  }
}

void MemoryComponent::handler() {
  count_++;
  dlogf("hello world, from component #%lu, count %lu", id_, count_);

  // Uses memmove to transfer memory from a random source to a random
  // destination.
  u64 src = simulator->random()->nextU64(0, bytes_ - size_);
  u64 dst = simulator->random()->nextU64(0, bytes_ - size_);
  memmove(&mem_[dst], &mem_[src], size_);

  if (run_) {
    nextEvent();
  }
}

void MemoryComponent::nextEvent() {
  MemoryComponent* component =
      reinterpret_cast<MemoryComponent*>(nextComponent());
  des::Time time = nextTime();
  des::Event* event = new des::Event(
      component, std::bind(&MemoryComponent::handler, this), time, true);
  simulator->addEvent(event);
}

registerWithObjectFactory("memory", BenchComponent, MemoryComponent,
                          BENCH_ARGS);
