/*
 * Copyright (c) 2012-2016, Nic McDonald
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
 * - Neither the name of prim nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior
 * written permission.
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
#ifndef EXAMPLE_EMPTYMODEL_H_
#define EXAMPLE_EMPTYMODEL_H_

#include <des/Event.h>
#include <des/Model.h>
#include <des/Simulator.h>
#include <prim/prim.h>

#include <string>

namespace example {

class EmptyModel : public des::Model {
 public:
  EmptyModel(des::Simulator* _simulator, const std::string& _name,
             const des::Model* _parent, u64 _id, u64 _count,
             bool _shiftyEpsilon, bool _verbose);
  ~EmptyModel();
  void function();

 private:
  class Event : public des::Event {
   public:
    Event(des::Model* _model, des::EventHandler _handler);
  };

  void handler(des::Event* _event);

  u64 id_;
  u64 count_;
  bool shiftyEpsilon_;
  bool verbose_;
  Event evt_;
};

}  // namespace example

#endif  // EXAMPLE_EMPTYMODEL_H_
