""" Waf tool for generating image data using grit """
import os, re
import Params, Action, Object, Utils

def grit_shared_wrapper(task):
  import shutil
  # wrapper for grit shared
  result = Action.g_actions['grit_shared_cmd'].run(task)
  if result: return result
  if getattr(task, 'outdir', None):
    try:
      for i in task.m_outputs:
        src = os.path.basename(i.bldpath(task.env()))
        dst = task.outdir
        if src.endswith('.h'):
          fp = open(src, 'r')
          data = fp.readlines()
          op = open(os.path.join(dst, os.path.basename(src)), 'w')
          for i in data:
            if i.startswith('#define __') or i.startswith('#ifndef __'):
              # rewrite grit 0.8's broken header guard
              i = i.replace('__','').strip()
              i += '_h_seen\n'
            op.write(i)
          op.close
          os.unlink(src)
        else:
          shutil.move(src, dst)
        #print "moved %s to %s"%(src, dst)
    except OSError, IOError:
      return 1
  return 0

class grit_shared_taskgen(Object.task_gen):
  def __init__(self, *k, **kw):
    Object.task_gen.__init__(self, *k, **kw)
    self.flags = ''
    self.palette = ''

  def clone(self, variant):
    obj = Params.g_build.create_obj('grit_shared')
    obj.palette = self.palette
    obj.source = [a for a in Utils.to_list(self.source)]
    obj.flags = self.flags
    obj.export_incdirs = self.export_incdirs
    if variant != 'default':
      obj.env = Params.g_build.env(variant).copy()
    return obj

  def apply(self):
    find_source_lst = self.path.find_source_lst
    input_nodes = []
    c_nodes = []
    out_nodes = []
    for filename in self.to_list(self.source):
      node = find_source_lst(Utils.split_path(filename))
      input_nodes.append(node)
      out_nodes.append(node.change_ext('.h'))
      c_nodes.append(node.change_ext('.c'))

    if self.flags:
      self.env['GRITFLAGS'] = self.flags
    task = self.create_task('grit_shared', self.env)
    task.set_inputs(input_nodes)
    # palette hacks
    fb = self.path.find_build
    fname = os.path.join(self.palette,self.palette)
    cfile = fb(fname+'.c')
    hfile = fb(fname+'.h')
    c_nodes.append(cfile)
    out_nodes.append(hfile)
    out_nodes.extend(c_nodes)
    task.set_outputs(out_nodes)
    setattr(task, 'outdir', os.path.dirname(input_nodes[0].bldbase(self.env)))
    self.allnodes.append(c_nodes)


def output_for_source(source, env):
  """ Given a file name generate a list of outputs """
  # add img.bin, pal.bin and map.bin to the output as side effects
  if ('-fts' in env['GRITFLAGS']):
    fileName = source.split(os.path.extsep)[0]
    out = []
    out.append('%s.s' % (fileName))
    out.append('%s.h' % (fileName))
  elif ('-ftb' in env['GRITFLAGS']):
    fileName = source.split(os.path.extsep)[0]
    out = []
    out.append('%s.img.bin' % (fileName))
    out.append('%s.pal.bin' % (fileName))
  return out

from Object import extension, taskgen, after

def add_include(task):
  """ Add an #include line to the output file for the given input file  """
  outfile = task.m_outputs[0].abspath(task.env())
  infile = task.m_inputs[0].abspath(task.env())
  fp = open(outfile, 'w')
  fp.write('#include "%s"\n' % (os.path.basename(infile).replace('.c','.h')))
  i = open(infile)
  for k in i.readlines():
    fp.write(k)
  fp.close()

@taskgen
@extension('.pcx')
@after('apply_core')
def img_file(self, node):
  out_source_c = node.change_ext('.c')
  self.allnodes.append(out_source_c)

@taskgen
@extension('.png')
def img_file(self, node):
  """ Convert a png file to a cpp and header file """
  out_source_s = node.change_ext('.c')
  out_source_C = node.change_ext('.cpp')
  out_source_h = node.change_ext('.h')

  if getattr(self, 'gritflags', None):
    self.env['GRITFLAGS'] = getattr(self, 'gritflags')

  tsk = self.create_task('grit')
  tsk.set_inputs(node)
  tsk.set_outputs([out_source_s, out_source_h])

  tsk = self.create_task('c_C')
  tsk.set_inputs(out_source_s)
  tsk.set_outputs(out_source_C)

  # the out file is to be processed as a cpp file
  self.allnodes.append(out_source_C)

def detect(conf):
  dka_bin = '%s/bin' % (Params.g_options.devkitarm)
  grit = 'grit'
  grit = conf.find_program(grit, path_list=[dka_bin], var='GRIT')
  if not grit:
    conf.fatal('grit was not found. It is usually supplied with devkitArm.')

  # lets see if it is the right version of grit... need -pS and -O flags
  v = os.popen(grit).readlines()
  # Find -pS or -O followed by the end of word
  option_re = re.compile('-(:?pS|O)\\b')
  ps_option = [l for l in v if option_re.match(l)]
  ok = (len(ps_option) == 2)
  conf.env['HAVE_GRIT_SHARED'] = 0
  import Runner
  conf.check_message('grit', 'accepts -pS and -O', ok)
  Runner.print_log(''.join(ps_option))
  if ok:
    conf.env['HAVE_GRIT_SHARED'] = 1

  conf.env['GRIT'] = grit
  if not conf.env['GRITFLAGS']:
    conf.env['GRITFLAGS'] = '-pw 16 -gB 8 -m! -ft b -fh! -q'.split()

def setup(bld):
  grit_str = '${GRIT} ${SRC[0].abspath(env)} -o ${SRC[0].bldbase(env)} ${GRITFLAGS} > /dev/null 2>&1 '
  Action.simple_action('grit', grit_str, color='CYAN', prio=10)
  Action.Action('c_C', vars=[], func=add_include , color='CYAN', prio=11)
  grit_str = '${GRIT} ${SRC} ${GRITFLAGS}'
  Action.simple_action('grit_shared_cmd', grit_str, color='CYAN', prio=10)
  Action.Action('grit_shared', vars=[], func=grit_shared_wrapper , color='CYAN', prio=11)
