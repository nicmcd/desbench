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
#include <cassert>
#include <chrono>  // NOLINT
#include <cmath>
#include <functional>
#include <thread>  // NOLINT
#include <tuple>
#include <vector>

#include "bench/BenchComponent.h"
#include "des/des.h"
#include "des/util/BasicObserver.h"
#include "des/util/RandomMapper.h"
#include "des/util/RoundRobinMapper.h"
#include "nlohmann/json.hpp"
#include "prim/prim.h"
#include "settings/settings.h"

void executionTimer(std::vector<BenchComponent*>* _components,
                    f64 _execution_time) {
  // Sleeps this thread for '_execution_time' seconds.
  std::this_thread::sleep_for(std::chrono::duration<f64>(_execution_time));
  // Informs the components to stop.
  for (BenchComponent* component : *_components) {
    component->stop();
  }
}

s32 main(s32 _argc, char** _argv) {
  // Turn off buffered output on stdout and stderr.
  setbuf(stdout, nullptr);
  setbuf(stderr, nullptr);

  // Gets JSON settings from the command line.
  printf("Reading settings\n");
  nlohmann::json settings;
  settings::commandLine(_argc, _argv, &settings);
  printf("%s\n", settings::toString(settings).c_str());

  // Creates the simulator core.
  u32 num_executers = settings["simulator"]["core"]["executers"].get<u32>();
  des::Simulator* sim = new des::Simulator(num_executers);
  u64 sim_seed = settings["simulator"]["core"]["seed"].get<u64>();
  sim->seed(sim_seed);
  f64 observer_interval =
      settings["simulator"]["core"]["observer_interval"].get<f64>();
  sim->setObservingInterval(observer_interval);
  u32 observer_power =
      settings["simulator"]["core"]["observer_power"].get<u32>();
  sim->setObservingPower(observer_power);

  // Creates the component mapper.
  des::Mapper* mapper = nullptr;
  std::string mapper_alg =
      settings["simulator"]["mapper"]["algorithm"].get<std::string>();
  if (mapper_alg == "round_robin") {
    mapper = new des::RoundRobinMapper();
  } else if (mapper_alg == "random") {
    mapper = new des::RandomMapper();
  } else {
    fprintf(stderr, "invalid mapping algorithm: %s\n", mapper_alg.c_str());
    exit(-1);
  }
  sim->setMapper(mapper);

  // Creates the logger.
  std::string log_file =
      settings["simulator"]["logger"]["file"].get<std::string>();
  des::Logger* log = new des::Logger(log_file);
  sim->setLogger(log);

  // Creates the observer.
  bool log_summary =
      settings["simulator"]["observer"]["log_summary"].get<bool>();
  des::BasicObserver* ob = new des::BasicObserver(log, log_summary);
  sim->addObserver(ob);

  // Enables debugging on select components.
  for (u32 i = 0; i < settings["debug"].size(); i++) {
    std::string component_name = settings["debug"][i].get<std::string>();
    sim->addDebugName(component_name);
  }

  // Creates all components.
  u32 num_components = settings["benchmark"]["num_components"].get<u32>();
  std::vector<BenchComponent*> components(num_components);
  for (u32 id = 0; id < num_components; id++) {
    std::string name = "Component_" + std::to_string(id);
    components.at(id) = BenchComponent::create(
        sim, name, id, settings["benchmark"]["component"]);
  }
  for (u32 id = 0; id < num_components; id++) {
    components.at(id)->setAllComponents(&components);
  }

  // Checks that all components to be debugged were found.
  sim->debugNameCheck();

  // Create a killer thread that stops the components from running forever.
  f64 execution_time = settings["simulator"]["execution_time"].get<f64>();
  assert(execution_time >= 0.0);
  std::thread killer(executionTimer, &components, execution_time);

  // Runs the simulation.
  sim->simulate();

  // Joins the killer thread which has already completed.
  killer.join();

  // Cleans up all memory.
  for (u32 id = 0; id < num_components; id++) {
    delete components.at(id);
  }
  delete log;
  delete ob;
  delete sim;
  delete mapper;

  return 0;
}
