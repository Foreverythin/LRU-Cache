import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

dataset = pd.read_csv('cacheCapacityLab_cluster022.csv')

sns.set(style="ticks")

plt.plot(dataset['cacheSize']/50000,dataset['hitRatio'],color='#000000',linestyle='--',linewidth=1,marker='^'
         ,markeredgecolor='#000000',markersize='7')

plt.xlabel('Cache Size',fontweight='bold')

plt.ylabel('Hit Ratio')

plt.grid(True, linestyle='--', linewidth=0.5)

plt.savefig('cacheCapacityLab_cluster022.png', dpi=300)

plt.show()
