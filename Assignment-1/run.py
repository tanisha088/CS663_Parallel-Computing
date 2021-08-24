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
processes=[16, 36, 49, 64]
# data_points=[16*16]
data_points=[16*16, 32*32,64*64,128*128,256*256,512*512,1024*1024]
no_of_iteration=5
# array = []
# print(node_allocator_path)
# print(cur_dir)

matrixsize = len(data_points)
process_count = len(processes)


def main():
	# print("Hello World!")
	os.system("python3 "+monitor_start)
	os.system("rm -rf *.txt ")
	time.sleep(20)

	os.system("make")

	# print(allocator_path)



	for execution in range(0,no_of_iteration):

	# for execution in range(1,2):
	  # print("-------------------iteration = "+str(execution)+"---------")
		for process in processes:
			for size in data_points:
				os.chdir(allocator_path)
				os.system("g++ allocator_improved.cpp -o allocator.out ")
				os.system("./allocator.out "+str(process)+" 8  >/dev/null 2>&1")
				os.chdir(cur_dir)
				os.system("mpirun -n "+str(process)+" -f "+allocator_path+"hosts ./halo "+str(size)+" >> data_"+str(process)+".txt")
	      # break
	    # break
	  # break
	  # print("----------------------------------")


	os.system("python3 plotscript.py")
	# monitor_stop=node_allocator_path+"eagle/monitor/monitord.py stop"
    # os.system("python3 "+monitor_stop+" >/dev/null 2>&1")



if __name__ == "__main__":
    main()
