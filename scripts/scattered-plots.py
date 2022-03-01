import matplotlib.pyplot as plt
import numpy as np
import os, glob

#nBins = int(input('Give number of bins: '))
nBins = 100
datasetSize = 50000
error = 0.03

datafiles = [a for a in os.listdir() if a.endswith('50000.txt') and not(a.startswith('time'))]
n = len(datafiles)
print(datafiles)

data = {}

for i, file in enumerate(datafiles):
    with open(file) as f:
        data[file] = np.array([int(line) for line in f.readlines()])
        m = np.mean(data[file])
        data[file] = (data[file]-m)/m

colors = ['black', 'red', 'green', 'blue']
for i in range(n):
    dat = data[datafiles[i]]
    print(dat.shape)
    print(datafiles[i])
    plt.scatter(dat, np.full_like(dat, fill_value=i), color='black', alpha=0.3, label=datafiles[i][:-4])

plt.axvline(x=-error)
plt.axvline(x=error)
#plt.xlim([-error-0.01,error+0.01])
plt.yticks([0,1,2,3,4,5,6], ['Tab1Perm', 'Poly2', 'Poly3', 'MultShift', 'TwistTab', 'MixedTab', 'Murmur'])
#plt.legend()
plt.savefig('pic.png')



#
#
# i = 0
# n = 0
# for fl in glob.glob("*.txt"):
#   n += 1
# print(str(n) + " folders/colors.")
# color=iter(plt.cm.rainbow(np.linspace(0,1,n))) #set of n colors
# for fl in glob.glob("*.txt"):
#   print(fl)
#   i += 1
#   data = []
#
#   with open(fl) as f:
#     for line in f:
#       tmp = int(line)
#       data.append((datasetSize-tmp)/datasetSize)
#
#   # evaluate the histogram
#   values, base = np.histogram(data, bins='auto')
#   values = [x/len(data) for x in values]
#   #evaluate the cumulative
#   cumulative = np.cumsum(values)
#   # plot the cumulative function
#   c=next(color)
#   plt.plot(base[:-1], cumulative, c=c, label=fl)
#   # plt.xlim([-0.01, 0])
#   # plt.ylim([0, 0.01])
#   #plot the survival function
#   #plt.plot(base[:-1], len(data)-cumulative, c='green')
#
# plt.legend()
# plt.show()
