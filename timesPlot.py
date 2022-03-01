import matplotlib.pyplot as plt
import numpy as np

plt.rcdefaults()
fig, ax = plt.subplots()

# Example data
#people would be our hash functions
#performance would be our times
#we don't need error

#people = ('Tom', 'Dick', 'Harry', 'Slim', 'Jim')
#y_pos = np.arange(len(people))
#performance = 3 + 10 * np.random.rand(len(people))
#error = np.random.rand(len(people))

y_pos = np.arange(7)
#A = [6.838,6.649,3.528,1.772,5.721,3.651,2.629] #bottom-k 20 exp with card 5*10**7
#A = [104.064, 123.382, 59.302, 28.299, 102.230, 67.430, 39.470] #sampling, 20 exp with card 10**9
#A = [2.377, 2.868, 3.908, 0.991, 3.166, 2.039, 2.260] #real-data-processed, 500 exp with card 6.5*10**5
A = [5.651, 7.667, 9.727, 1.775, 8.111, 4.263, 5.179] #real-world, 500 exp with card 1.8*10**6
performance = [round(10**3*x/20) for x in A]
ax.set_xlabel('Average Running Time (ms)')

ax.barh(y_pos, performance, align='center')
ax.set_yticks(y_pos)
ax.set_yticklabels(['Murmur', 'MixedTab', 'TwistTab', 'MultShift', 'Poly3', 'Poly2', 'Tab1Perm']) # ax.set_yticklabels(people)
ax.invert_yaxis()  # labels read top-to-bottom
#ax.set_title('Total time for each hash-function')

plt.savefig('pic.png')
