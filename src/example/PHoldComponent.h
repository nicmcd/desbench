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
#ifndef EXAMPLE_PHOLDCOMPONENT_H_
#define EXAMPLE_PHOLDCOMPONENT_H_

#include <des/des.h>
#include <rnd/Random.h>
#include <prim/prim.h>

#include <string>

#include "example/BenchComponent.h"

namespace example {


class PHoldComponent: public BenchComponent {
 public:
  PHoldComponent(des::Simulator *_simulator, const std::string &_name,
                 u64 _id, bool _shiftyEpsilon, u64 _remPercentage,
                 bool _verbose);
  ~PHoldComponent() = default;
  void initialize() override;
  void function();

 private:
  void handler();

  bool stagger_;
  u32 numInitialEvts_;
  u64 sum_;
  u64 others_;
  des::Time lookAhead_;
  f64 remPercentage_;
  rnd::Random rnd_;
};

}  // namespace example

#endif  //  EXAMPLE_PHOLDCOMPONENT_H_
