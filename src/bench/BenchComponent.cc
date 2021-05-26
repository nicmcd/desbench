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
#include "bench/BenchComponent.h"

#include <cassert>
#include <cstdio>
#include <cstring>

#include "factory/ObjectFactory.h"

BenchComponent::BenchComponent(des::Simulator* _simulator,
                               const std::string& _name, u64 _id,
                               nlohmann::json _settings)
    : des::ActiveComponent(_simulator, _name),
      id_(_id),
      initial_events_(_settings["initial_events"].get<u64>()),
      look_ahead_(_settings["look_ahead"].get<des::Tick>()),
      stagger_tick_(_settings["stagger_tick"].get<bool>()),
      stagger_epsilon_(_settings["stagger_epsilon"].get<bool>()),
      remote_probability_(_settings["remote_probability"].get<f64>()),
      count_(0),
      run_(true),
      num_dests_(0) {
  assert(look_ahead_ > 0);
  assert(remote_probability_ >= 0.0 && remote_probability_ <= 1.0);
}

BenchComponent* BenchComponent::create(des::Simulator* _simulator,
                                       const std::string& _name, u64 _id,
                                       nlohmann::json _settings) {
  const std::string& type = _settings["type"].get<std::string>();
  BenchComponent* component =
      factory::ObjectFactory<BenchComponent, BENCH_ARGS>::create(
          type, _simulator, _name, _id, _settings);
  if (component == nullptr) {
    fprintf(stderr, "unknown bench component type: %s\n", type.c_str());
    assert(false);
  }
  return component;
}

void BenchComponent::stop() {
  run_ = false;
}

void BenchComponent::setDestinationComponents(
    const std::vector<BenchComponent*>& _dest_components) {
  num_dests_ = _dest_components.size();
  dest_components_ = _dest_components;
}

u64 BenchComponent::initialEvents() {
  return initial_events_;
}

des::Time BenchComponent::nextTime() {
  des::Time time;
  if (stagger_tick_) {
    time.setTick(simulator->time().tick() + look_ahead_ +
                 (id_ % des::TICK_INV));
  } else {
    time.setTick(simulator->time().tick() + look_ahead_);
  }
  if (stagger_epsilon_) {
    time.setEpsilon((id_ + count_) % des::EPSILON_INV);
  } else {
    time.setEpsilon(0);
  }
  return time;
}

BenchComponent* BenchComponent::nextComponent() {
  if (simulator->random()->nextF64() <= remote_probability_) {
    u64 id = simulator->random()->nextU64() % num_dests_;
    return dest_components_.at(id);
  }
  return this;
}
