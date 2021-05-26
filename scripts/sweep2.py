#!/usr/bin/env python3

import argparse
import json
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy
import os
import pprint
import re
import ssplot
import taskrun

def main(args):
  if not os.path.isdir(args.odir):
    os.mkdir(args.odir)

  rm = taskrun.ResourceManager(
    taskrun.CounterResource('cpus', os.cpu_count(), os.cpu_count()))
  cob = taskrun.FileCleanupObserver()
  vob = taskrun.VerboseObserver(show_description=args.verbose)
  tm = taskrun.TaskManager(resource_manager=rm,
                           observers=[cob, vob],
                           failure_mode=taskrun.FailureMode.AGGRESSIVE_FAIL)

  cpus_list = [x for x in range(args.start, args.stop+1, args.step)]

  runs = args.runs

  # Makes the default settings
  cfg_file = os.path.join(args.odir, 'settings.json')
  with open(cfg_file, 'w') as fd:
    cfg = {
      'simulator': {
        'execution_time': 'TBD',
        'core': {
          'executers': 'TBD',
          'seed': 1234,
          'observer_interval': 1.0,
          'observer_power': 11
        },
        'mapper': {
          'algorithm': 'round_robin'
        },
        'observer': {
          'log_summary': True
        },
        'logger': {
          'file': '-'
        }
      },
      'benchmark': {
        'num_components': 'TBD',
        'topology': 'TBD',
        'component': {
          'type': 'empty',
          'initial_events': 'TBD',
          'look_ahead': 1,
          'stagger_tick': False,
          'stagger_epsilon': False,
          'remote_probability': 1.0
        }
      },
      'debug': []
    }
    json.dump(cfg, fd, indent=2)

  # Generates the models
  layouts = [(1024, 1)]
  while True:
    if layouts[-1][0] <= os.cpu_count():
      break
    layouts.append((layouts[-1][0] // 2, layouts[-1][1] * 2),)
  print(layouts)

  models = {}
  for layout in layouts:
    components = layout[0]
    initial_events = layout[1]
    name = '{}x{}'.format(components, initial_events)
    models[name] = [
      '/benchmark/num_components=int={}'.format(components),
      '/benchmark/component/initial_events=int={}'.format(initial_events)]

  for model in models:
    for cpus in cpus_list:
      for run in range(0, runs):
        name = '{0}_{1}_{2}'.format(model, cpus, run)
        filename = os.path.join(args.odir, name + '.log')
        cmd = ''
        if args.numactl:
          cmd += 'numactl {} '.format(args.numactl)
        cmd += args.exe + ' '
        cmd += cfg_file + ' '
        cmd += '/simulator/execution_time=float={} '.format(args.exetime)
        cmd += '/simulator/core/executers=uint={} '.format(cpus)
        cmd += '/benchmark/topology=string={} '.format(args.topo)
        for mod in models[model]:
          cmd += mod + ' '
        task = taskrun.ProcessTask(tm, name, cmd)
        task.stdout_file = filename
        task.add_condition(taskrun.FileModificationCondition(
          [], [filename]))

  tm.randomize()
  res = tm.run_tasks()
  if not res:
    return -1

  # gather all data
  data = {}
  for model in models:
    data[model] = {'rate': []}
    for cpus in cpus_list:
      runtime_sum = 0
      rate_sum = 0
      for run in range(0, runs):
        name = '{0}_{1}_{2}'.format(model, cpus, run)
        filename = os.path.join(args.odir, name + '.log')
        rate = extractRate(filename)
        rate_sum += rate
      rate_sum /= runs
      data[model]['rate'].append(rate_sum)

  # print the CSV file
  with open(os.path.join(args.odir, 'throughput.csv'), 'w') as fd:
    # print the header
    print('Benchmark,', file=fd, end='')
    for cpus in cpus_list:
      print('{},'.format(cpus), file=fd, end='')
    print('', file=fd)

    # print each row
    for model in models:
      print('{},'.format(model), file=fd, end='')
      for idx in range(len(cpus_list)):
        print('{},'.format(data[model]['rate'][idx]), file=fd, end='')
      print('', file=fd)

  # plot the performance
  ys = []
  for model in models:
    print(model)
    ys.append(data[model]['rate'])
  mlp = ssplot.MultilinePlot(plt, cpus_list, ys)
  mlp.set_title('libdes performance')
  mlp.set_xlabel('Number of threads')
  mlp.set_xmajor_ticks(len(cpus_list))
  mlp.set_ylabel('Events per second')
  mlp.set_ymin(0)
  mlp.set_data_labels(list(models))
  mlp.plot(os.path.join(args.odir, 'performance.png'))


def extractRate(filename):
  with open(filename, 'r') as fd:
    for line in fd:
      if line.find('Events per second') == 0:
        words = line.split()
        return float(words[-1])
  assert False, 'Never found the keywords in {}'.format(filename)


if __name__ == '__main__':
  ap = argparse.ArgumentParser()
  ap.add_argument('exe', help='desbench program')
  ap.add_argument('topo', help='component topology')
  ap.add_argument('odir', help='the output directory')
  ap.add_argument('start', type=int, help='starting cpus')
  ap.add_argument('stop', type=int, help='stopping cpus')
  ap.add_argument('step', type=int, help='cpus step')
  ap.add_argument('-r', '--runs', type=int, default=1,
                  help='number of runs')
  ap.add_argument('-e', '--exetime', type=int, default=10,
                  help='execution time per run in seconds')
  ap.add_argument('-n', '--numactl', default=None,
                  help="arguments for numactl");
  ap.add_argument('-v', '--verbose', action='store_true',
                  help="show task descriptions");
  args = ap.parse_args();
  main(args)
