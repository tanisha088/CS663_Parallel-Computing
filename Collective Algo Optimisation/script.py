import subprocess
import sys
import os
import math

try:
    os.remove("hostfile")
except OSError:
    pass
f1 = open("nodefile.txt", "r")
# no_of_groups = int(sys.argv[1])
# nodes_per_group = int(sys.argv[2])
# cores = int(sys.argv[3])

core_8=8


lines = f1.readlines()

total_groups = len(lines)
available_groups = 0
groups_checked = 0
final_result = []
nodes_available_per_group=[]

reachable = []
for line in lines:
    nodes = line.strip().split(",")
    nodes_in_this_group = 0
    temp_up_nodes=[]
    for node in nodes:
        status = subprocess.call(["ssh", node, "uptime"])
        if status == 0:
            nodes_in_this_group += 1
            temp_up_nodes.append(node)
        # if nodes_in_this_group == nodes_per_group:
            # break

    nodes_available_per_group.append(nodes_in_this_group)
    reachable.append(temp_up_nodes)
    groups_checked += 1
    # if nodes_in_this_group == nodes_per_group:
    #     final_result.append(reachable)
    #     available_groups += 1
    #     if available_groups == no_of_groups:
    #         break


# print nodes_available_per_group
# print reachable

# nodes_available_per_group=[10,12,5,0,11,3]

temp_list=nodes_available_per_group

minimum_nodes_available_per_group=min(nodes_available_per_group)



# print nodes_available_per_group
# print reachable
# print minimum_nodes_available_per_group

# exit()

def highestPowerof2(n): 
  
    p = int(math.log(n, 2)); 
    return int(pow(2, p));



subprocess.call(["clear"])
print highestPowerof2(minimum_nodes_available_per_group)
count = 0
if len(reachable) < 1 or minimum_nodes_available_per_group<2:
    print "Not enough nodes available"
else:
    f2 = open("hostfile", "w+")
    # if minimum_nodes_available_per_group%2:
        # minimum_nodes_available_per_group=minimum_nodes_available_per_group-1

    for group in reachable:
        nodes_per_group=group[:highestPowerof2(minimum_nodes_available_per_group)]
        # count += 1
        # print(nodes_per_group)
        for node in nodes_per_group:
            f2.write(node + "\n")
        
