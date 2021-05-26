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
#ifndef BENCH_BENCHCOMPONENT_H_
#define BENCH_BENCHCOMPONENT_H_

#include <string>
#include <vector>

#include "des/des.h"
#include "nlohmann/json.hpp"
#include "prim/prim.h"
#include "rnd/Random.h"

#define BENCH_ARGS des::Simulator*, const std::string&, u64, nlohmann::json

class BenchComponent : public des::ActiveComponent {
 public:
  BenchComponent(des::Simulator* _simulator, const std::string& _name, u64 _id,
                 nlohmann::json _settings);
  virtual ~BenchComponent() = default;

  static BenchComponent* create(BENCH_ARGS);

  void stop();
  void setDestinationComponents(
      const std::vector<BenchComponent*>& _dest_components);

 protected:
  u64 initialEvents();
  des::Time nextTime();
  BenchComponent* nextComponent();

  const u64 id_;
  const u64 initial_events_;
  const des::Tick look_ahead_;
  const bool stagger_tick_;
  const bool stagger_epsilon_;
  const f64 remote_probability_;

  u64 count_;
  bool run_;
  u64 num_dests_;
  std::vector<BenchComponent*> dest_components_;
};

#endif  // BENCH_BENCHCOMPONENT_H_
