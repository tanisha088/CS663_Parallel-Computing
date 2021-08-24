import sys
import os
import time

# processes=[16]
processes=[4,16]
# data_points=[16*16]
data_points_size=[16,256,2048]
no_of_iteration=10
# array = []
# print(node_allocator_path)
# print(cur_dir)

matrixsize = len(data_points_size)
process_count = len(processes)


def main():
	# print("Hello World!")
	# os.system("python3 "+monitor_start)

	os.system("rm -rf data* code")
	os.system("make")
	os.system("python script.py")
	# time.sleep(20)


	# print(allocator_path)



	for execution in range(0,no_of_iteration):

	# for execution in range(1,2):
	  # print("-------------------iteration = "+str(execution)+"---------")
		for process in processes:
			for ppn in [1,8]:
				for size in data_points_size:
					no_of_elements=size*1024/8
					# print("mpirun -n "+str(process*ppn)+" -ppn "+ str(ppn)+" -f hostfile ./code "+str(int(no_of_elements))+" "+str(ppn)+" "+str(8) +" >> data_"+str(process)+"_"+str(int(no_of_elements))+".txt")
					os.system("mpirun -n "+str(process*ppn)+" -ppn "+ str(ppn)+" -f hostfile ./code "+str(int(no_of_elements)) +" >> data_"+str(process)+"_"+str(int(no_of_elements))+".txt")
					# os.system("mpirun -n "+str(process*ppn)+" -f hostfile ./code "+str(no_of_elements))
					# break
				# break
			# break
		  # print("----------------------------------")


	os.system("python3 plot.py")
	# monitor_stop=node_allocator_path+"eagle/monitor/monitord.py stop"
    # os.system("python3 "+monitor_stop+" >/dev/null 2>&1")



if __name__ == "__main__":
    main()
