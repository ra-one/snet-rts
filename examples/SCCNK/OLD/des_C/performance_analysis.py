import sys
import getopt
import re
import glob

def extract(f, c):
  m = []
  s = "[0-9]+%s[0-9]+\.[0-9]+"%(c)
  for l in f:
    x = re.findall(s, l)
    for y in x:
      items = y.split(c)
      m.append(int(items[0]))

  return m
    

def perf_cal(im, om, m, stable):
  if len(im) * m != len(om):
    assert False, "input/output does not match"
  tp = 0
  cr = 0
  lat = 0
  prev_tp = om[m * (stable + 1) - 1]
  prev_cr = im[stable]
  for i in range(stable, len(im) - stable):
    ts1 = im[i]
    ts2 = om[i * m + m - 1]
    l = ts2 - ts1
    if (l < 0):
      assert False, "negative latency"
    lat = lat + l
    cr = cr + im[i] - prev_cr
    prev_cr = im[i]
    tp = tp + om[i * m + m - 1] - prev_tp
    prev_tp = om[i * m + m - 1]

  lat = float(lat)/(len(im) - 2 * stable)/1000.0/1000.0
  cr = float(cr)/(len(im) - 2 *stable - 1)
  cr = 1000.0*1000.0/cr

  tp = float(tp)/(len(im) - 2*stable - 1)
  tp = 1000.0 * 1000.0/tp

  return(cr, tp, lat)


def tp_latency(d, m, stable):
  rf = glob.glob(d + "/mon_n*_snet_source.log")
  assert len(rf) == 1, "None or more than 2 source file"
  rf = rf[0]
  
  kf = glob.glob(d + "/mon_n*_snet_sink.log")
  assert len(kf) == 1, "None or more than 2 sink file"
  kf = kf[0]
  
  source_f = open(rf, 'r')
  sink_f = open(kf, 'r')
  im = extract(source_f, 'O')
  om = extract(sink_f, 'I')
  source_f.close()
  sink_f.close()
  cr, tp, lat = perf_cal(im, om, m, stable)
  return (cr, tp, lat)

def exe_time():
  exe_tm = []
  rf = glob.glob(d + "/rck*.log")
  for f in rf:
    rck_f = open(f, 'r')
    splited = re.split('#',rck_f.readline())
    exe_tm.append(float(splited[2]))
    rck_f.close()
  return (sum(exe_tm)/len(exe_tm))
  
if __name__=="__main__":
  try:
    opts, args = getopt.getopt(sys.argv[1:], "hd:m:s:", ["help", "dir=", "mul=", "stable="])
  except getopt.GetoptError, err:
    assert False, "Error: wrong options"

  d = None
  mul = 1
  stable = 0
  for o, a in opts:
    if o in ("-h", "--help"):
      print "Usage: python performance_analysis.py -d <directory of log files> -m mul"
      exit()
    elif o in ("-d" , "--dir"):
      d = a
    elif o in ("-m", "--mul"):
      mul = int(a)
    elif o in ("-s", "--stable"):
      stable = int(a)
    else:
      assert False, "Error: unhandled option: %s" %(o)

  if d == None:
    assert False, "Specify directory of log files"
    
  (cr, tp, lat) = tp_latency(d, mul, stable)
  et = exe_time()
  print "Messages / Millisecond"
  print "Input Rate = " + str(cr)
  print "Throughput = " + str(tp)
  print "Latency = " + str(lat)
  print "Average Execution Time: " + str(et)