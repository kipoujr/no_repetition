import matplotlib.pyplot as plt
import numpy as np
from math import log10 , floor
from scipy.stats import moment
import os

fontsz = 14
datafiles = [a for a in os.listdir() if a.endswith('.txt') and not(a.startswith('time'))]
n = len(datafiles)
print(datafiles)

data = {}
highMoment = {}
pos = [0,2,3,1,4]
pos1 = [0,3,1,2,4]

plt.figure(figsize=(6.3,3))
for i, file in enumerate(datafiles):
    with open(file) as f:
        data[file] = np.array([int(line) for line in f.readlines()])
        m = np.mean(data[file])
        data[file] = (data[file]-m)/m
        #print( '{0.0e}'.format(moment(data[file], moment=6)) )
        x = moment(data[file], moment=6)
        highMoment[i] = round(x, - int(floor(log10(abs(x)))))

for i in range(n):
    dat = data[datafiles[pos1[i]]]
    #print(dat.shape)
    #print(datafiles[pos1[i]])
    plt.scatter(dat, np.full_like(dat, fill_value=i), color='black', alpha=0.3, label=datafiles[pos1[i]][:-4])

plt.tick_params(axis='x', which='major', labelsize=fontsz)
plt.locator_params(axis='x', nbins=6)
plt.yticks(pos, ['Tab1Perm\n' + str(highMoment[0]),
                 'Poly2\n' + str(highMoment[1]),
                 'MultShift\n' + str(highMoment[2]),
                 'MixedTab\n' + str(highMoment[3]),
                 'Murmur\n' + str(highMoment[4])],
           fontsize=fontsz)

plt.xlim(min(min(data[datafiles[pos1[0]]]), min(data[datafiles[pos1[1]]])), max(max(data[datafiles[pos1[0]]]), max(data[datafiles[pos1[1]]])))
plt.savefig('pic.png',bbox_inches='tight')