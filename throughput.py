#!/usr/bin/env python3

import argparse
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy
import os
import taskrun

def main():
  ap = argparse.ArgumentParser()
  ap.add_argument('exe', help='desbench program')
  ap.add_argument('odir', help='the output directory')
  ap.add_argument('-r', '--runs', type=int, default=1,
                  help='number of runs')
  ap.add_argument('-e', '--exetime', type=int, default=1,
                  help='execution time per run in seconds')
  ap.add_argument('-s', '--step', type=int, default=1,
                  help='step size for number of cpus (threads)')
  args = ap.parse_args();

  if not os.path.isdir(args.odir):
    os.mkdir(args.odir)

  rm = taskrun.ResourceManager(
    taskrun.CounterResource('cpus', os.cpu_count(), os.cpu_count()))
  cob = taskrun.FileCleanupObserver()
  vob = taskrun.VerboseObserver()
  tm = taskrun.TaskManager(resource_manager=rm,
                           observers=[cob, vob],
                           failure_mode=taskrun.FailureMode.AGGRESSIVE_FAIL)

  CPUS_START = 1
  CPUS_STOP = os.cpu_count()
  CPUS_STEP = args.step
  CPUS = [x for x in range(CPUS_START, CPUS_STOP + 1) if x % CPUS_STEP == 0]
  RUNS = args.runs
  MODELS = {
    'empty':   ['empty',  0],
    'simple':  ['simple', 0],
    'sha1':    ['sha',    1],
    'sha256':  ['sha',    256],
    'sha512':  ['sha',    512],
    'mem1m':   ['mem',    1000000],
    'mem10m':  ['mem',    10000000],
    'mem100m': ['mem',    100000000],
    'mem1g':   ['mem',    1000000000],
    'mem10g':  ['mem',    10000000000],
    'mix0':    ['mix',    0],
    'mix1':    ['mix',    1],
    'mix10':   ['mix',    10],
    'mix100':  ['mix',    100],
    'mix1000': ['mix',    1000]
  }

  for model in sorted(MODELS):
    for cpus in CPUS:
      for run in range(0, RUNS):
        name = '{0}_{1}_{2}'.format(model, cpus, run)
        filename = os.path.join(args.odir, name + '.log')
        cmd = ('{0} -e {1} -m 10000 -t {2} -n {3} -g {4}'.format(
          args.exe, args.exetime, cpus, MODELS[model][0], MODELS[model][1]))
        task = taskrun.ProcessTask(tm, name, cmd)
        task.stdout_file = filename
        task.add_condition(taskrun.FileModificationCondition(
          [], [filename]))

  tm.run_tasks()

  data = {}
  for model in sorted(MODELS):
    data[model] = {'runtime': [], 'rate': []}
    for cpus in CPUS:
      runtime_sum = 0
      rate_sum = 0
      for run in range(0, RUNS):
        name = '{0}_{1}_{2}'.format(model, cpus, run)
        filename = os.path.join(args.odir, name + '.log')
        rate = extractRate(filename)
        rate_sum += rate
      rate_sum /= RUNS
      data[model]['rate'].append(rate_sum)

  cmap = plt.get_cmap('gist_rainbow')
  colors = [cmap(idx) for idx in numpy.linspace(0, 1, len(MODELS))]

  fig = plt.figure(figsize=(16,10))
  ax1 = fig.add_subplot(1, 1, 1)
  ax1.set_xlabel('Number of threads')
  ax1.set_ylabel('Events per second')
  ax1.grid(True)
  ax1.set_xlim(CPUS[0], CPUS[-1])
  lines = []
  maxy = 0
  for idx, model in enumerate(sorted(MODELS)):
    line, = ax1.plot(CPUS, data[model]['rate'], color=colors[idx],
                     lw=1, label=model)
    lines.append(line)
    maxy = max(maxy, max(data[model]['rate']))
  ax1.set_ylim(bottom=0, top=maxy*1.1)
  labels = [line.get_label() for line in lines]
  ax1.legend(lines, labels, loc='upper left', fancybox=True, shadow=True,
             ncol=1)
  fig.tight_layout()
  fig.savefig(os.path.join(args.odir, 'throughput.png'))


def extractRate(filename):
  with open(filename, 'r') as fd:
    for line in fd:
      if line.find('Events per real second') == 0:
        words = line.split()
        return float(words[-1])


if __name__ == '__main__':
  main()
