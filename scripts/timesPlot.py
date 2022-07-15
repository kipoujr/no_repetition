import matplotlib.pyplot as plt
import numpy as np

A = [357482,128529,199340,185371,115872]
B = [293458,102255,204400,203542,108408]
#A = [1892135,385659,1070399,188793,105579]
#B = [1747741,582530,1112823,253676,122928]
#A = [5279359,1409705,2829316,3250718,1958279]
#B = [5349699,1235807,2731691,3023679,1781945]
#A = [105663,35750,71442,25492,19723]
#B = [87230,33720,64928,23890,17363]

performance  = [x/10**4 for x in A]
print(performance)
performance2 = [x/10**4 for x in B]

plt.rcdefaults()
fig, ax = plt.subplots()
y_pos = np.arange(len(A))
ax.set_xlabel('Average Running Time (ms)')
ax.set_yticks(y_pos)
ax.set_yticklabels(['Murmur', 'Mult\nShift', 'Poly2', 'Mixed\nTab', 'Tab1\nPerm'])
ax.invert_yaxis()  # labels read top-to-bottom
ax.tick_params(axis='both', which='major', labelsize=13)

ax.barh(y_pos-0.2, performance , 0.4, label='PC 1')
ax.barh(y_pos+0.2, performance2, 0.4, label='PC 2')
ax.legend()

plt.savefig('pic.png')