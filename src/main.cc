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
#include <des/des.h>
#include <des/util/BasicObserver.h>
#include <des/util/RandomMapper.h>
#include <des/util/RoundRobinMapper.h>
#include <prim/prim.h>
#include <rnd/Random.h>
#include <tclap/CmdLine.h>

#include <cmath>

#include <chrono>
#include <thread>
#include <tuple>
#include <vector>

#include "example/BenchComponent.h"
#include "example/BounceComponent.h"
#include "example/EmptyComponent.h"
#include "example/MemoryComponent.h"
#include "example/MixComponent.h"
#include "example/NewDeleteComponent.h"
#include "example/ShaComponent.h"
#include "example/SimpleComponent.h"
#include "example/PHoldComponent.h"

void executionTimer(std::vector<example::BenchComponent*>* _components,
                    u64 _executionTime) {
  // go to sleep
  std::this_thread::sleep_for(std::chrono::seconds(_executionTime));

  // wake up and kill the components
  for (example::BenchComponent* component : *_components) {
    component->kill();
  }
}

example::BenchComponent* createComponent(
    const std::string& _type, des::Simulator* _sim, const std::string& _name,
    u64 _id, bool _shiftyEpsilon, bool _verbose, u32 _numComponents,
    u64 _generic) {
  if (_type == "bounce") {
    return new example::BounceComponent(
        _sim, _name, _id, _shiftyEpsilon, _generic, _verbose);
  } else if (_type == "empty") {
    return new example::EmptyComponent(
        _sim, _name, _id, _shiftyEpsilon, _verbose);
  } else if (_type == "mem") {
    if (_generic < _numComponents) {
      fprintf(stderr, "ERROR: mem component requires a larger memory usage\n");
      exit(-1);
    }
    return new example::MemoryComponent(
        _sim, _name, _id, _shiftyEpsilon,
        _generic / _numComponents, _verbose);
  } else if (_type == "mix") {
    return new example::MixComponent(
        _sim, _name, _id, _shiftyEpsilon, _generic, _verbose);
  } else if (_type == "new") {
    return new example::NewDeleteComponent(
        _sim, _name, _id, _shiftyEpsilon, _verbose);
  } else if (_type == "sha") {
    return new example::ShaComponent(
        _sim, _name, _id, _shiftyEpsilon, _generic, _verbose);
  } else if (_type == "simple") {
    return new example::SimpleComponent(
        _sim, _name, _id, _shiftyEpsilon, _verbose);
  } else if (_type == "phold") {
    return new example::PHoldComponent(
        _sim, _name, _id, _shiftyEpsilon, _generic, _verbose);
  } else {
    fprintf(stderr, "invalid component type: %s\n", _type.c_str());
    exit(-1);
  }
}

void test(
    u32 _numThreads, std::string _mapping, u64 _numComponents,
    const std::string& _componentType, u64 _executionTime, bool _shiftyEpsilon,
    bool _verbose, u64 generic) {
  rnd::Random rnd;
  rnd.seed(12345678);

  des::Mapper* mapper = nullptr;
  if (_mapping == "round_robin") {
    mapper = new des::RoundRobinMapper();
  } else if (_mapping == "random") {
    mapper = new des::RandomMapper(&rnd);
  } else {
    fprintf(stderr, "invalid mapping algorithm: %s\n", _mapping.c_str());
    exit(-1);
  }

  des::Simulator* sim = new des::Simulator(_numThreads);
  sim->setMapper(mapper);
  des::Logger* log = new des::Logger("-");
  sim->setLogger(log);
  des::BasicObserver* ob = new des::BasicObserver(log, true, true);
  sim->addObserver(ob);

  if (_verbose) {
    for (u32 id = 0; id < _numComponents; id++) {
      sim->addDebugName("Component_" + std::to_string(id));
    }
  }

  std::vector<example::BenchComponent*> components(_numComponents);
  for (u32 id = 0; id < _numComponents; id++) {
    std::string name = "Component_" + std::to_string(id);
    components.at(id) = createComponent(
        _componentType, sim, name, id, _shiftyEpsilon, _verbose,
        _numComponents, generic);
  }

  for (u32 id = 0; id < _numComponents; id++) {
    components.at(id)->allComponents(&components);
  }

  for (u32 id = 0; id < _numComponents; id++) {
    components.at(id)->init();
  }

  sim->debugCheck();

  std::thread killer(executionTimer, &components, _executionTime);

  printf("initializing components...\n");
  sim->initialize();
  printf("simulation beginning...\n");
  sim->simulate();

  killer.join();

  for (u32 id = 0; id < _numComponents; id++) {
    delete components.at(id);
  }
  delete log;
  delete ob;
  delete sim;
  delete mapper;
}

s32 main(s32 _argc, char** _argv) {
  u32 threads = U32_MAX;
  std::string mapping = "";
  u64 components = U64_MAX;
  std::string componentType = "";
  u64 executionTime = 1;
  u64 generic = 0;
  bool shiftyEpsilon = false;
  bool verbose = false;

  try {
    TCLAP::CmdLine cmd(
        "Command description message", ' ', "0.0.1");
    TCLAP::ValueArg<u32> threadsArg(
        "t", "threads", "Number of threads", false, 1, "u32", cmd);
    TCLAP::ValueArg<std::string> mappingArg(
        "m", "mapper", "Mapping algorithm", false, "round_robin", "string",
        cmd);
    TCLAP::ValueArg<u32> componentsArg(
        "c", "components", "Number of components", false, 1, "u32", cmd);
    TCLAP::ValueArg<std::string> nameArg(
        "n", "name", "Component type name", false, "empty", "string", cmd);
    TCLAP::ValueArg<u32> executionTimeArg(
        "e", "execution", "Execution time in seconds", false, 1, "u32", cmd);
    TCLAP::ValueArg<u64> genericArg(
        "g", "generic", "Value passed to component constructor", false, 0,
        "u64", cmd);
    TCLAP::SwitchArg shiftyArg(
        "s", "shifty", "Shifty Epsilon", cmd, false);
    TCLAP::SwitchArg verboseArg(
        "v", "verbose", "Turn on verbosity", cmd, false);

    cmd.parse(_argc, _argv);
    threads = threadsArg.getValue();
    mapping = mappingArg.getValue();
    components = componentsArg.getValue();
    componentType = nameArg.getValue();
    executionTime = executionTimeArg.getValue();
    shiftyEpsilon = shiftyArg.getValue();
    verbose = verboseArg.getValue();
    generic = genericArg.getValue();
  } catch (TCLAP::ArgException &e) {
    fprintf(stderr, "error: %s for arg %s\n",
            e.error().c_str(), e.argId().c_str());
    exit(-1);
  }

  printf("threads=%u mapping=%s components=%lu type=%s executionTime=%lu "
         "shifty=%d verbose=%d generic=%lu\n",
         threads, mapping.c_str(), components, componentType.c_str(),
         executionTime, shiftyEpsilon, verbose, generic);

  test(threads, mapping, components, componentType, executionTime,
       shiftyEpsilon, verbose, generic);

  return 0;
}
