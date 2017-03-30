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
#include <des/des.h>
#include <prim/prim.h>
#include <tclap/CmdLine.h>

#include <cmath>

#include <chrono>
#include <thread>
#include <tuple>
#include <vector>

#include "example/BenchModel.h"
#include "example/EmptyModel.h"
#include "example/MemoryModel.h"
#include "example/MixModel.h"
#include "example/ShaModel.h"
#include "example/SimpleModel.h"

void executionTimer(std::vector<example::BenchModel*>* _models,
                    u64 _executionTime) {
  // go to sleep
  std::this_thread::sleep_for(std::chrono::seconds(_executionTime));

  // wake up and kill the models
  for (example::BenchModel* model : *_models) {
    model->kill();
  }
}

example::BenchModel* createModel(
    const std::string& _type, des::Simulator* _sim, const std::string& _name,
    u64 _id, bool _shiftyEpsilon, bool _verbose, u32 _numModels,
    u64 _generic) {
  if (_type == "empty") {
    return new example::EmptyModel(
        _sim, _name, nullptr, _id, _shiftyEpsilon, _verbose);
  } else if (_type == "mem") {
    if (_generic < _numModels) {
      fprintf(stderr, "ERROR: mem model requires a larger memory usage\n");
      exit(-1);
    }
    return new example::MemoryModel(
        _sim, _name, nullptr, _id, _shiftyEpsilon,
        _generic / _numModels, _verbose);
  } else if (_type == "mix") {
    return new example::MixModel(
        _sim, _name, nullptr, _id, _shiftyEpsilon, _generic, _verbose);
  } else if (_type == "sha") {
    return new example::ShaModel(
        _sim, _name, nullptr, _id, _shiftyEpsilon, _generic, _verbose);
  } else if (_type == "simple") {
    return new example::SimpleModel(
        _sim, _name, nullptr, _id, _shiftyEpsilon, _verbose);
  } else {
    fprintf(stderr, "invalid model type: %s\n", _type.c_str());
    exit(-1);
  }
}

void test(u32 _numThreads, u64 _numModels, const std::string& _modelType,
          u64 _executionTime, bool _shiftyEpsilon, bool _verbose,
          u64 generic) {
  des::Simulator* sim = new des::Simulator(_numThreads);
  if (_verbose) {
    for (u32 id = 0; id < _numModels; id++) {
      sim->addDebugName("Model_" + std::to_string(id));
    }
  }

  std::vector<example::BenchModel*> models(_numModels);
  for (u32 id = 0; id < _numModels; id++) {
    std::string name = "Model_" + std::to_string(id);
    models.at(id) = createModel(_modelType, sim, name, id, _shiftyEpsilon,
                                _verbose, _numModels, generic);
  }

  for (u32 id = 0; id < _numModels; id++) {
    models.at(id)->allModels(&models);
  }

  des::Logger logger;
  sim->setLogger(&logger);
  sim->debugCheck();

  std::thread killer(executionTimer, &models, _executionTime);

  printf("simulation beginning...\n");
  sim->simulate(true);

  killer.join();

  for (u32 id = 0; id < _numModels; id++) {
    delete models.at(id);
  }
  delete sim;
}

s32 main(s32 _argc, char** _argv) {
  u32 threads = U32_MAX;
  u64 models = U64_MAX;
  std::string modelType = "";
  u64 executionTime = 1;
  u64 generic = 0;
  bool shiftyEpsilon = false;
  bool verbose = false;

  try {
    TCLAP::CmdLine cmd(
        "Command description message", ' ', "0.0.1");
    TCLAP::ValueArg<u32> threadsArg(
        "t", "threads", "Number of threads", false, 0, "u32", cmd);
    TCLAP::ValueArg<u32> modelsArg(
        "m", "models", "Number of models", false, 1, "u32", cmd);
    TCLAP::ValueArg<std::string> nameArg(
        "n", "name", "Model type name", false, "empty", "string", cmd);
    TCLAP::ValueArg<u32> executionTimeArg(
        "e", "execution", "Execution time in seconds", false, 1, "u32", cmd);
    TCLAP::ValueArg<u64> genericArg(
        "g", "generic", "Value passed to model constructor", false, 0, "u64",
        cmd);
    TCLAP::SwitchArg shiftyArg(
        "s", "shifty", "Shifty Epsilon", cmd, false);
    TCLAP::SwitchArg verboseArg(
        "v", "verbose", "Turn on verbosity", cmd, false);

    cmd.parse(_argc, _argv);
    threads = threadsArg.getValue();
    models = modelsArg.getValue();
    modelType = nameArg.getValue();
    executionTime = executionTimeArg.getValue();
    shiftyEpsilon = shiftyArg.getValue();
    verbose = verboseArg.getValue();
    generic = genericArg.getValue();
  } catch (TCLAP::ArgException &e) {
    fprintf(stderr, "error: %s for arg %s\n",
            e.error().c_str(), e.argId().c_str());
    exit(-1);
  }

  printf("threads=%u models=%lu type=%s executionTime=%lu shifty=%d "
         "verbose=%d generic=%lu\n",
         threads, models, modelType.c_str(), executionTime, shiftyEpsilon,
         verbose, generic);
  test(threads, models, modelType, executionTime, shiftyEpsilon, verbose,
       generic);
  return 0;
}
