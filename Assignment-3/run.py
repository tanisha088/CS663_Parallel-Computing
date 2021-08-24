import sys
import os
import time

from pathlib import Path
import numpy as np
home = str(Path.home())
node_allocator_path= home+"/UGP/"
cur_dir=os.getcwd()
monitor_start=node_allocator_path+"eagle/monitor/monitord.py start"
allocator_path=node_allocator_path+"allocator/src/"
# processes=[16]
nodes=[1,2]
# data_points=[16*16]
ppn=[1,2,4]

fileName="tdata.csv"




def main():
	os.system("python3 "+monitor_start)
	os.system("rm -rf *.txt ")
	time.sleep(20)

	os.system("make")

	# print(allocator_path)




	for node in nodes:
		for cores in ppn:
			os.chdir(allocator_path)
			os.system("g++ allocator_improved.cpp -o allocator.out ")
			os.system("./allocator.out "+str(node*cores)+" "+str(cores)+" >/dev/null 2>&1")
			os.chdir(cur_dir)
			# print("mpirun -n "+str(node*cores)+" -ppn "+str(cores)+" -f "+allocator_path+"hosts ./src "+str(fileName)+" >> output_"+str(node)+"_"+str(cores)+".txt")
			os.system("mpirun -n "+str(node*cores)+" -ppn "+str(cores)+" -f "+allocator_path+"hosts ./src "+str(fileName))

		# 	break
		# break

	# monitor_stop=node_allocator_path+"eagle/monitor/monitord.py stop"
    # os.system("python3 "+monitor_stop+" >/dev/null 2>&1")



if __name__ == "__main__":
    main()
