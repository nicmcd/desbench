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
#ifndef EXAMPLE_MODEL_H_
#define EXAMPLE_MODEL_H_

#include <des/Event.h>
#include <des/Model.h>
#include <des/Simulator.h>
#include <prim/prim.h>

#include <string>

namespace example {

class Model : public des::Model {
 public:
  Model(des::Simulator* _simulator, const std::string& _name,
        const Model* _parent, u64 _count, u64 _id, bool _verbose,
        bool _shiftyEpsilon);
  ~Model();
  void function(s32 _a, s32 _b, s32 _c);

 private:
  class Event : public des::Event {
   public:
    Event(des::Model* _model, des::EventHandler _handler);
    s32 a;
    s32 b;
    s32 c;
  };

  void handler(des::Event* _event);

  u64 count_;
  u64 id_;
  bool verbose_;
  bool shiftyEpsilon_;
  Event evt_;
};

}  // namespace example

#endif  // EXAMPLE_MODEL_H_
