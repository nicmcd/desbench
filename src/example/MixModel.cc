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
#include "example/MixModel.h"

#include <cassert>
#include <cstdio>
#include <cstring>

namespace example {

MixModel::MixModel(des::Simulator* _simulator, const std::string& _name,
                   const des::Model* _parent, u64 _id, bool _shiftyEpsilon,
                   u64 _others, bool _verbose)
    : BenchModel(_simulator, _name, _parent, _id, _shiftyEpsilon, _verbose),
      others_(_others) {
  // initialize my event
  evts_.push_back(new Event(this, static_cast<des::EventHandler>(
      &MixModel::handleMine)));

  // initialize others' events
  for (u32 evt = 0; evt < others_; evt++) {
    for (u32 epoch = 0; epoch < 2; epoch++) {
      Event* e = new Event(nullptr, static_cast<des::EventHandler>(
          &MixModel::handleOthers));
      evts_.push_back(e);
    }
  }

  // start on epoch 0 (false)
  epoch_ = false;

  // start the sum
  sum_ = 0;

  // give the random generator a seed
  rnd_.seed(id_);

  // queue first event
  function();
}

MixModel::~MixModel() {
  for (Event* e : evts_) {
    delete e;
  }
}

MixModel::Event::Event(des::Model* _model,
                       des::EventHandler _handler)
    : des::Event(_model, _handler) {}

void MixModel::function() {
  // *** generate an event for me
  evts_.at(0)->time = simulator->time() + 1;
  if (shiftyEpsilon_) {
    evts_.at(0)->time.setEpsilon((id_ + count_) % des::EPSILON_INV);
  } else {
    evts_.at(0)->time.setEpsilon(0);
  }
  simulator->addEvent(evts_.at(0));

  // *** add 'others_' events to randomly selected models
  if (numModels_ > 0) {
    for (u64 other = 0; other < others_; other++) {
      Event* evt = evts_.at(other + 1 + (epoch_ * others_));

      // set other model
      u64 otherId = rnd_() % numModels_;
      evt->model = allModels_->at(otherId);

      // set time and epsilon
      evt->time = simulator->time() + 1;
      if (shiftyEpsilon_) {
        evt->time.setEpsilon((id_ + count_) % des::EPSILON_INV);
      } else {
        evt->time.setEpsilon(0);
      }

      // add event
      simulator->addEvent(evt);
    }
  }

  // switch the epoch
  epoch_ = !epoch_;
}

void MixModel::handleMine(des::Event* _event) {
  Event* me = reinterpret_cast<Event*>(_event);
  (void)me;  // unused

  count_++;
  if (verbose_ || count_ < 5) {
    dlogf("hello world, from model #%lu, count %lu (mine)", id_, count_);
  }

  sum_++;

  if (run_) {
    function();  // queue another event
  }
}

void MixModel::handleOthers(des::Event* _event) {
  Event* me = reinterpret_cast<Event*>(_event);
  (void)me;  // unused

  if (verbose_ || count_ < 5) {
    dlogf("hello world, from model #%lu, count %lu (others)", id_, count_);
  }

  sum_++;
}

}  // namespace example
