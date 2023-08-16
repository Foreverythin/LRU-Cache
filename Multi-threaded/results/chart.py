import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import numpy as np

sns.set(style="ticks")

##### cache capacity lab #####
# dataset_10 = pd.read_csv('cacheCapacityLab_20threads_cluster010.csv')
# dataset_22 = pd.read_csv('cacheCapacityLab_20threads_cluster022.csv')

# plt.plot(dataset_22['cacheSize']/50000,dataset_22['hitRatio'],color='#000000',alpha=1,linestyle='--',linewidth=1,marker='*'
#          ,markeredgecolor='#000000',markersize='7')

# plt.plot(dataset_22['cacheSize']/50000, dataset_22['hitLatency'], color='#000000', alpha=1, linestyle='--', linewidth=1,marker='^'
#          ,markersize='7')

# plt.plot(dataset_22['cacheSize']/50000, dataset_22['throughput'], color='#000000', alpha=1, linestyle='--', linewidth=1,marker='^'
#          ,markersize='7')

# plt.plot(dataset_10['cacheSize']/50000, dataset_10['throughput'], color='#000000', alpha=1, linestyle='--', linewidth=1,marker='*'
#          ,markersize='7')

# plt.ylabel('Throughput(MQPS)')

# plt.xlabel('Cache Size', fontweight='bold')

# plt.legend(['Cluster022','Cluster010'], loc='upper left')


##### Thread Num lab #####

# bar chart

dataset_10 = pd.read_csv('threadNumLab_cluster010.csv')
dataset_22 = pd.read_csv('threadNumLab_cluster022.csv')

labels = dataset_10['numThread']

x = np.arange(len(labels))
width = 0.25

y = np.arange(0, 10, 1)

fig, ax = plt.subplots()

rects1 = ax.bar(x - width/2, dataset_10['hitLatency'], width, label='Cluster010', color='#000000')
rects2 = ax.bar(x + width/2, dataset_22['hitLatency'], width, label='Cluster022', color='#808080')

# Add the numbers to the top of the bars
for rect in rects1:
    height = rect.get_height()
    ax.annotate('{}'.format(height),
                xy=(rect.get_x() + rect.get_width()/2, height),
                xytext=(0, 3),
                textcoords="offset points",
                ha='center', va='bottom')
    
for rect in rects2:
    height = rect.get_height()
    ax.annotate('{}'.format(height),
                xy=(rect.get_x() + rect.get_width()/2, height),
                xytext=(0, 3),
                textcoords="offset points",
                ha='center', va='bottom')

# Add some texts
ax.set_ylabel('Average hit latency(us)')
ax.set_xlabel('Thread Num', fontweight='bold')
ax.set_xticks(x)
ax.set_yticks(y)
ax.set_xticklabels(labels)
ax.legend()


############################################
# show grid
plt.grid(True, linestyle='--', linewidth=0.5)

plt.savefig('threadNumLab_hitLatency.png', dpi=300)

plt.show()