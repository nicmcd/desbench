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
#include <des/Event.h>
#include <des/Model.h>
#include <des/Simulator.h>
#include <prim/prim.h>
#include <tclap/CmdLine.h>

#include <cmath>

#include <thread>
#include <tuple>
#include <vector>

#include "example/Model.h"

void test(u32 _numThreads, u64 _numModels, u64 _eventsPerModel,
          bool _shiftyEpsilon, bool _verbose) {
  des::Simulator* sim = new des::Simulator(_numThreads);
  if (_verbose) {
    for (u32 id = 0; id < _numModels; id++) {
      sim->addDebugName("Model_" + std::to_string(id));
    }
  }

  std::vector<example::Model*> models(_numModels);
  for (u32 id = 0; id < _numModels; id++) {
    models.at(id) = new example::Model(sim, "Model_" + std::to_string(id),
                                       nullptr, _eventsPerModel, id, _verbose,
                                       _shiftyEpsilon);
  }
  for (u32 id = 0; id < _numModels; id++) {
    models.at(id)->function(1000000, 2000000, 3000000);
  }

  sim->debugCheck();
  sim->simulate(true);

  for (u32 id = 0; id < _numModels; id++) {
    delete models.at(id);
  }
  delete sim;
}

s32 main(s32 _argc, char** _argv) {
  u32 threads = U32_MAX;
  u64 models = U64_MAX;
  u64 eventsPerModel = U64_MAX;
  bool shiftyEpsilon = false;
  bool verbose = false;

  try {
    TCLAP::CmdLine cmd("Command description message", ' ', "0.0.1");
    TCLAP::ValueArg<u32> threadsArg("t", "threads", "Number of threads", false,
                                    0, "u32", cmd);
    TCLAP::ValueArg<u32> modelsArg("m", "models", "Number of models", false,
                                   1, "u32", cmd);
    TCLAP::ValueArg<u32> eventsArg("e", "events", "Number of events per model",
                                   false, 1, "u32", cmd);
    TCLAP::SwitchArg shiftyArg("s", "shifty", "Shufty Epsilon", cmd,
                               false);
    TCLAP::SwitchArg verboseArg("v", "verbose", "Turn on verbosity", cmd,
                                false);
    cmd.parse(_argc, _argv);
    threads = threadsArg.getValue();
    models = modelsArg.getValue();
    eventsPerModel = eventsArg.getValue();
    shiftyEpsilon = shiftyArg.getValue();
    verbose = verboseArg.getValue();
  } catch (TCLAP::ArgException &e) {
    fprintf(stderr, "error: %s for arg %s\n",
            e.error().c_str(), e.argId().c_str());
  }

  printf("threads=%u models=%lu eventsPerModel=%lu shifty=%d verbose=%d\n",
         threads, models, eventsPerModel, shiftyEpsilon, verbose);
  test(threads, models, eventsPerModel, shiftyEpsilon, verbose);
  return 0;
}
