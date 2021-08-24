#!/usr/bin/env python
# coding: utf-8

# In[47]:


import pandas as pd
import seaborn as sns
import numpy as np
import matplotlib.pyplot as plt

sns.set()


# In[48]:


bcast_input_format = pd.DataFrame.from_dict({
    "D": [],
    "P": [],
    "ppn": [],
    "mode": [],  # 1 --> optimized, 0 --> standard
    "time": [],
})
reduce_input_format = pd.DataFrame.from_dict({
    "D": [],
    "P": [],
    "ppn": [],
    "mode": [],  # 1 --> optimized, 0 --> standard
    "time": [],
})
gather_input_format = pd.DataFrame.from_dict({
    "D": [],
    "P": [],
    "ppn": [],
    "mode": [],  # 1 --> optimized, 0 --> standard
    "time": [],
})
alltoallv_input_format = pd.DataFrame.from_dict({
    "D": [],
    "P": [],
    "ppn": [],
    "mode": [],  # 1 --> optimized, 0 --> standard
    "time": [],
})


# In[49]:



# for execution in range(10):
for P in [4, 16]:
    # for ppn in [1,8]:
    for D in [16, 256, 2048]:
        no_of_elements=int(D*1024/8)
        fileName="data_"+str(P)+"_"+str(no_of_elements)+".txt"
        # print(fileName)
        with open(fileName,'r') as reader:
            for line in reader:
                # for ppn in [1,8]:
                line=line.split("=")[1].rstrip()
                # print(line)
                # break
                bcast_input_format = bcast_input_format.append({"D": D, "P": P, "ppn": 1, "mode": 0, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                # print(P,D,line)
                bcast_input_format = bcast_input_format.append({"D": D, "P": P, "ppn": 1, "mode": 1, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                # print(P,D,line)
                reduce_input_format = reduce_input_format.append({"D": D, "P": P, "ppn": 1, "mode": 0, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                # print(P,D,line)
                reduce_input_format = reduce_input_format.append({"D": D, "P": P, "ppn": 1, "mode": 1, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                # print(P,D,line)
                gather_input_format = gather_input_format.append({"D": D, "P": P, "ppn": 1, "mode": 0, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                # print(P,D,line)
                gather_input_format = gather_input_format.append({"D": D, "P": P, "ppn": 1, "mode": 1, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                # print(P,D,line)
                alltoallv_input_format = alltoallv_input_format.append({"D": D, "P": P, "ppn": 1, "mode": 0, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                # print(P,D,line)
                alltoallv_input_format = alltoallv_input_format.append({"D": D, "P": P, "ppn": 1, "mode": 1, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                # print(P,D,line)
                bcast_input_format = bcast_input_format.append({"D": D, "P": P, "ppn": 8, "mode": 0, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                # print(P,D,line)
                bcast_input_format = bcast_input_format.append({"D": D, "P": P, "ppn": 8, "mode": 1, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                reduce_input_format = reduce_input_format.append({"D": D, "P": P, "ppn": 8, "mode": 0, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                reduce_input_format = reduce_input_format.append({"D": D, "P": P, "ppn": 8, "mode": 1, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                gather_input_format = gather_input_format.append({"D": D, "P": P, "ppn": 8, "mode": 0, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                gather_input_format = gather_input_format.append({"D": D, "P": P, "ppn": 8, "mode": 1, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                alltoallv_input_format = alltoallv_input_format.append({"D": D, "P": P, "ppn": 8, "mode": 0, "time": float(line)}, ignore_index=True)
                # next(reader)
                line=reader.readline().split("=")[1].rstrip()
                alltoallv_input_format = alltoallv_input_format.append({"D": D, "P": P, "ppn": 8, "mode": 1, "time": float(line)}, ignore_index=True)
                # next(reader)

        # Change with the actual data
bcast_input_format["(P, ppn)"] = list(map(lambda x, y: ("(" + x + ", " + y + ")"), map(str, bcast_input_format["P"]), map(str, bcast_input_format["ppn"])))
reduce_input_format["(P, ppn)"] = list(map(lambda x, y: ("(" + x + ", " + y + ")"), map(str, reduce_input_format["P"]), map(str, reduce_input_format["ppn"])))
gather_input_format["(P, ppn)"] = list(map(lambda x, y: ("(" + x + ", " + y + ")"), map(str, gather_input_format["P"]), map(str, gather_input_format["ppn"])))
alltoallv_input_format["(P, ppn)"] = list(map(lambda x, y: ("(" + x + ", " + y + ")"), map(str, alltoallv_input_format["P"]), map(str, alltoallv_input_format["ppn"])))

# print(bcast_input_format)
# print(reduce_input_format)
# print(gather_input_format)

# # In[50]:


g=sns.catplot(x="(P, ppn)", y="time", data=bcast_input_format, kind="box", col="D", hue="mode")
g.fig.get_axes()[0].set_yscale('log')
# plt.title("BCAST BOXPLOT")
plt.savefig('plot_Bcast'+'.png')
# plt.show()
g=sns.catplot(x="(P, ppn)", y="time", data=reduce_input_format, kind="box", col="D", hue="mode")
g.fig.get_axes()[0].set_yscale('log')
# plt.title("REDUCE BOXPLOT")
plt.savefig('plot_Reduce'+'.png')
# plt.show()
g=sns.catplot(x="(P, ppn)", y="time", data=gather_input_format, kind="box", col="D", hue="mode")
g.fig.get_axes()[0].set_yscale('log')
# plt.title("GATHER BOXPLOT")
plt.savefig('plot_Gather'+'.png')
# plt.show()

g=sns.catplot(x="(P, ppn)", y="time", data=alltoallv_input_format, kind="box", col="D", hue="mode")
g.fig.get_axes()[0].set_yscale('log')
# plt.title("ALLTOALLV BOXPLOT")
plt.savefig('plot_Alltoallv'+'.png')
# plt.show()

# In[ ]:
