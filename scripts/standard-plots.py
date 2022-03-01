import matplotlib.pyplot as plt
import numpy as np
import glob, os

#nBins = int(input('Give number of bins: '))
nBins = 10000

i = 0
n = 0
for fl in glob.glob("*.txt"):
  n += 1
print(str(n) + " folders/colors.")
color=iter(plt.cm.rainbow(np.linspace(0,1,n))) #set of n colors
for fl in glob.glob("*.txt"):
  print(fl)
  datasetSize = 0  
  i = 0
  data = []
  
  with open(fl) as f:
    for line in f:
      tmp = int(line)
      i += 1
      datasetSize += tmp
  datasetSize /= i
  print(datasetSize)
  
  with open(fl) as f:
    for line in f:
      tmp = int(line)
      data.append((datasetSize-tmp)/datasetSize)
          
  # evaluate the histogram
  values, base = np.histogram(data, bins=nBins)
  values = [x/len(data) for x in values]
  #evaluate the cumulative
  cumulative = np.cumsum(values)
  # plot the cumulative function
  c=next(color)
  plt.plot(base[:-1], cumulative, c=c, label=fl)
  #plt.xlim([-0.012, 0.012])
  #plot the survival function
  #plt.plot(base[:-1], len(data)-cumulative, c='green')

plt.legend()
plt.savefig('pic.png')
