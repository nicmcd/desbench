#!/usr/bin/env python3

import argparse
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy
import os
import pprint
import re
import taskrun

def main():
  ap = argparse.ArgumentParser()
  ap.add_argument('exe', help='desbench program')
  ap.add_argument('odir', help='the output directory')
  ap.add_argument('-r', '--runs', type=int, default=1,
                  help='number of runs')
  ap.add_argument('-e', '--exetime', type=int, default=10,
                  help='execution time per run in seconds')
  ap.add_argument('-s', '--step', type=int, default=4,
                  help='step size for number of cpus (threads)')
  ap.add_argument('-n', '--numactl', default=None,
                  help="arguments for numactl");
  ap.add_argument('-v', '--verbose', action='store_true',
                  help="show task descriptions");
  ap.add_argument('-m', '--models', default=".",
                  help="regex to select models")
  ap.add_argument('-l', '--list', action='store_true',
                  help='list models then exit')
  args = ap.parse_args();

  if not os.path.isdir(args.odir):
    os.mkdir(args.odir)

  rm = taskrun.ResourceManager(
    taskrun.CounterResource('cpus', os.cpu_count(), os.cpu_count()))
  cob = taskrun.FileCleanupObserver()
  vob = taskrun.VerboseObserver(description=args.verbose)
  tm = taskrun.TaskManager(resource_manager=rm,
                           observers=[cob, vob],
                           failure_mode=taskrun.FailureMode.AGGRESSIVE_FAIL)

  CPUS_START = 1
  CPUS_STOP = os.cpu_count()
  CPUS_STEP = args.step
  CPUS = [x for x in range(CPUS_START, CPUS_STOP + 1) if x % CPUS_STEP == 0]
  RUNS = args.runs
  MODELS = {
    'empty':     ['empty',  0],
    'new':       ['new',    0],
    'simple':    ['simple', 0],
    'sha1':      ['sha',    1],
    'sha256':    ['sha',    256],
    'sha512':    ['sha',    512],
    'mem1m':     ['mem',    1000000],
    'mem10m':    ['mem',    10000000],
    'mem100m':   ['mem',    100000000],
    'mem1g':     ['mem',    1000000000],
    'mem10g':    ['mem',    10000000000],
    'mix0':      ['mix',    0],
    'mix1':      ['mix',    1],
    'mix10':     ['mix',    10],
    'mix100':    ['mix',    100],
    'mix1000':   ['mix',    1000],
    'bounce1':   ['bounce', 1],
    'bounce2':   ['bounce', 2],
    'bounce3':   ['bounce', 3],
    'bounce4':   ['bounce', 4],
    'bounce5':   ['bounce', 5],
    'phold0':    ['phold',  0],
    'phold1':    ['phold',  1],
    'phold5':    ['phold',  5],
    'phold10':   ['phold',  10],
    'phold20':   ['phold',  20],
    'phold40':   ['phold',  40],
    'phold80':   ['phold',  80],
    'phold100':  ['phold',  100]
  }

  # filter MODELS based on args.model regex specifier
  regex = re.compile(args.models)
  MODELS = {k:v for k,v in MODELS.items() if re.search(regex, k)}

  # list models then exit if args.list is true
  if args.list:
    pp = pprint.PrettyPrinter(indent=4)
    pp.pprint(MODELS)
    exit(0)

  for model in sorted(MODELS):
    for cpus in CPUS:
      for run in range(0, RUNS):
        name = '{0}_{1}_{2}'.format(model, cpus, run)
        filename = os.path.join(args.odir, name + '.log')
        cmd = ''
        if args.numactl:
          cmd += 'numactl {} '.format(args.numactl)
        cmd += ('{0} -e {1} -c 10000 -t {2} -n {3} -g {4}'.format(
          args.exe, args.exetime, cpus, MODELS[model][0], MODELS[model][1]))
        task = taskrun.ProcessTask(tm, name, cmd)
        task.stdout_file = filename
        task.add_condition(taskrun.FileModificationCondition(
          [], [filename]))

  tm.randomize()
  tm.run_tasks()

  # gather all data
  data = {}
  for model in sorted(MODELS):
    data[model] = {'rate': []}
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

  # print the CSV file
  with open(os.path.join(args.odir, 'throughput.csv'), 'w') as fd:
    # print the header
    print('Benchmark,', file=fd, end='')
    for cpus in CPUS:
      print('{},'.format(cpus), file=fd, end='')
    print('', file=fd)

    # print each row
    for model in sorted(MODELS):
      print('{},'.format(model), file=fd, end='')
      for idx in range(len(CPUS)):
        print('{},'.format(data[model]['rate'][idx]), file=fd, end='')
      print('', file=fd)

  # plot the PNG file
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
      if line.find('Events per second') == 0:
        words = line.split()
        return float(words[-1])
  assert False, 'Never found the keywords in {}'.format(filename)


if __name__ == '__main__':
  main()
