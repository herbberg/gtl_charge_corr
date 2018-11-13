#!/usr/bin/env python

from glob import glob
from xml.dom import minidom

import argparse
import logging
import shutil
import subprocess
import sys, os

class Default:
    modules = 6
    current_dist = 'current_dist'
    vivado_hls = 'vivado_hls'
    project = 'hls_impl'
    solution = 'solution1'

class Status:
    def __init__(self, name, value=False, style=None, hint=None):
        self.name = name
        self.value = value
        self.style = style or ''
        self.hint = hint or ''
    def __repr__(self):
        return colorize(self.name, self.style)

def colorize(text, style):
    """Colorize a text using TTY escape sequences.

    colorize('FOO', '1;34') # bold, blue text on TTY
    """
    if sys.stdout.isatty():
        return "\033[{}m{}\033[0m".format(style, text)
    return text

def integrity_check(path):
    """Check integrity of distribution (module) directory."""
    root_dir = os.path.join(path, '..', '..')
    if not os.path.isdir(root_dir):
        return False
    for name in ['hls', 'testvectors', 'vhdl', 'xml']:
        if not os.path.isdir(os.path.join(root_dir, name)):
            return False
    return True

def locate_testvectors(path):
    """Returns list of found testvectors and number of events, path must point
    to a distribution directory.
    >>> locate_testvectors('L1Menu_sample/')
    [('TestVector_L1Menu_sample_a.txt', 3564), ('TestVector_L1Menu_sample_b.txt', 170)]
    """
    testvectors_dir = os.path.abspath(os.path.join(path, '..', '..', 'testvectors'))
    testvectors = []
    for filename in glob(os.path.join(testvectors_dir, 'TestVector*.txt')):
        with open(filename) as fp:
            events = sum(1 for line in fp)
        testvectors.append((filename, events))
    return testvectors

def auto_testvectors(args):
    """Retruns list of test vectors either supplied by argument or located."""
    testvectors = args.testvector
    # Look for test vectors...
    if not testvectors:
        dist_dir = os.readlink(args.current_dist)
        for testvector, _ in locate_testvectors(dist_dir):
            testvectors.append(testvector)
    if not testvectors:
        logging.warning("running without testvectors (none available)")
    return testvectors

def csynth_report(filename):
    """Prints csynth report (latency)."""
    doc = minidom.parse(filename)
    node = doc.getElementsByTagName('PerformanceEstimates')[0]
    items = [
        # XML tag, expected value
        ['Best-caseLatency', 0],
        ['Average-caseLatency', 0],
        ['Worst-caseLatency', 0],
        ['Interval-min', 1],
        ['Interval-max', 1],
    ]
    logging.info("Latency report:")
    for item in items:
        value = node.getElementsByTagName(item[0])[0].firstChild.nodeValue
        if int(value) != item[1]:
            logging.warning("{}: {} [expected: {}]".format(item[0], value, item[1]))
        else:
            logging.info("{}: {}".format(item[0], value))

def check_call(args):
    """Call system command, handles OSErrors (file not found)."""
    try:
        return subprocess.check_call(args)
    except OSError as e:
        # Catching error 2 (file not found).
        # Usually occures if vivado is not sourced.
        if e.errno == os.errno.ENOENT:
            logging.error(format(e))
            logging.error("executable '%s' not found", args[0])
            sys.exit(1)

def cmd_reset(args):
    """Reset command, removes distribution symlink."""
    if os.path.islink(args.current_dist):
        os.unlink(args.current_dist)
        logging.info("removed symlink %s", args.current_dist)
    cmd_clean(args)

def cmd_clean(args):
    """Clear command, removes project implementation."""
    if os.path.isdir(Default.project):
        shutil.rmtree(Default.project)
        logging.info("removed implementation %s", Default.project)

def cmd_init(args):
    """Init command, creates distribution symlink."""
    if os.path.islink(args.current_dist):
        raise RuntimeError("project already initalized")
    src_dir = os.path.join(args.dist, 'hls', 'module_{}'.format(args.module))
    if not os.path.isdir(src_dir):
        raise IOError("no such directory: {}".format(src_dir))
    if not integrity_check(src_dir):
        raise IOError("not a valid distribution directory: {}".format(src_dir))
    cmd_clean(args)
    logging.info("create symlink: current_dist -> %s", src_dir)
    os.symlink(src_dir, args.current_dist)
    cmd_auto_create_project(args)

def cmd_status(args):
    """Status command, shows integrity status of distribution symlink."""
    dirname = os.path.dirname
    basename = os.path.basename
    status = Status(
        name='NOT_CONFIGURED',
        value=1,
        style='1;33',
        hint="run {} init </path/to/dist> <module_id> to create a '{}' symlink.".format(__file__, args.current_dist)
    )
    if os.path.islink(args.current_dist):
        target_dir = os.readlink(args.current_dist)
        status = Status(
            name='DEAD_SYMLINK',
            value=2,
            style='1;31',
            hint="check the {} symlink.".format(args.current_dist)
        )
        if os.path.exists(target_dir):
            status = Status(
                name='INVALID_LOCATION',
                value=3,
                style='1;31'
            )
            if integrity_check(target_dir):
                status = Status(
                    name='OK',
                    value=0,
                    style='1;32'
                )
                logging.info("%s: %s", args.current_dist, target_dir)
                module_id = basename(target_dir).split('_')[-1]
                logging.info("module_id: %s", module_id)
                menu = basename(dirname(dirname((target_dir))))
                logging.info("menu: %s", menu)
                testvectors = locate_testvectors(target_dir)
                if testvectors:
                    logging.info("testvectors: %s", len(testvectors))
                    for filename, events in testvectors:
                        logging.info("* %s (%s events)", os.path.basename(filename), events)
                else:
                    logging.info("testvectors: %s", 'n/a')
    # add some fancy colors
    logging.info("status: %s", status)
    if status.hint:
        logging.info("hint: %s", status.hint)
    return status.value

def cmd_auto_create_project(args):
    if not os.path.isdir(Default.project):
        context='config/create_project.tcl'
        command = [args.vivado_hls, context]
        check_call(command)

def cmd_csim(args):
    cmd_auto_create_project(args)
    context='config/csim.tcl'
    testvectors = auto_testvectors(args)
    command = [args.vivado_hls, context]
    command.extend(testvectors)
    check_call(command)

def cmd_csynth(args):
    cmd_auto_create_project(args)
    context='config/csynth.tcl'
    command = [args.vivado_hls, context]
    check_call(command)
    csynth_report(os.path.join(Default.project, Default.solution, 'syn/report/csynth.xml'))

def cmd_cosim(args):
    cmd_auto_create_project(args)
    cmd_csim(args)
    cmd_csynth(args)
    context='config/cosim.tcl'
    testvectors = auto_testvectors(args)
    command = [args.vivado_hls, context]
    command.extend(testvectors)
    check_call(command)

def cmd_export(args):
    cmd_auto_create_project(args)
    #cmd_cosim(args)
    context='config/export.tcl'
    command = [args.vivado_hls, context]
    check_call(command)

def parse_args():
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest='command')

    init_parser = subparsers.add_parser('init', help="initialize with module distribution")
    init_parser.add_argument('dist', help="menu distribution directory")
    init_parser.add_argument('module', type=int, help="module ID (0...{})".format(Default.modules-1))
    init_parser.set_defaults(func=cmd_init)

    reset_parser = subparsers.add_parser('reset', help="remove current module distribution")
    reset_parser.set_defaults(func=cmd_reset)

    clear_parser = subparsers.add_parser('clean', help="remove project/solution")
    clear_parser.set_defaults(func=cmd_clean)

    status_parser = subparsers.add_parser('status', help="show module distribution")
    status_parser.set_defaults(func=cmd_status)

    csim_parser = subparsers.add_parser('csim', help="run C simulation")
    csim_parser.add_argument('--testvector', metavar='<file>', default=[], nargs='*', help="run with custom testvectors")
    csim_parser.set_defaults(func=cmd_csim)

    csynth_parser = subparsers.add_parser('csynth', help="run C synthesis")
    csynth_parser.set_defaults(func=cmd_csynth)

    cosim_parser = subparsers.add_parser('cosim', help="run co-simulation")
    cosim_parser.add_argument('--testvector', metavar='<file>', default=[], nargs='*', help="run with custom testvectors")
    cosim_parser.set_defaults(func=cmd_cosim)

    export_parser = subparsers.add_parser('export', help="run export IP core")
    export_parser.set_defaults(func=cmd_export)

    parser.add_argument('--current-dist', metavar='<dir>', default=Default.current_dist, help="current_dist symlink path, default is '{}'".format(Default.current_dist))
    parser.add_argument('--vivado-hls', metavar='<exec>', default=Default.vivado_hls, help="Xilinx Vivado HLS executable, default is '{}'".format(Default.vivado_hls))

    return parser.parse_args()

def main():
    args = parse_args()
    logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
    args.func(args)
    sys.exit()

if __name__ == '__main__':
    main()
