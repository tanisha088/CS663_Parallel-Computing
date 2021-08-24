import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
import math
import run


def fn(iter,myarr,matsize,process):

  farray = np.zeros((matsize,3,iter))
  st=0
  for i in range(0,iter):
    val=0
    for j in range(0,matsize*3):
      strapp=0
      if j//3 == 0 :
        strapp = 0
      elif j//3== (1) :
        strapp = 1
      elif j//3== (2) :
        strapp = 2
      elif j//3==(3):
        strapp = 3
      elif j//3==(4):
        strapp = 4
      elif j//3==(5):
        strapp = 5
      else:
        strapp = 6

      farray[strapp][j%3][i]=(myarr[st])
      st=st+1
      j=j+1

  ticks=[]
  for value in data_points:
    ticks.append(str(int(math.sqrt(value))))

  # print(ticks)


  plt.figure()

  i=-0.4
  xtick_lable=[]
  for value in range(0,len(data_points)):

    bpr = plt.boxplot(farray[value][0], positions=[i], sym='', widths=0.6)
    i=i+0.8
    xtick_lable.append(i)
    bpg = plt.boxplot(farray[value][1], positions=[i], sym='', widths=0.6)
    i=i+0.8
    bpb = plt.boxplot(farray[value][2], positions=[i], sym='', widths=0.6)
    i=i+1.2
    plt.setp(bpr['boxes'], color='#D7191C')
    plt.setp(bpg['boxes'], color='#2C7BB6')
    plt.setp(bpb['boxes'], color='#31a354')

  plt.plot([], c='#D7191C', label='TYPE 1')
  plt.plot([], c='#2C7BB6', label='TYPE 2')
  plt.plot([], c='#31a354', label='TYPE 3')
  yval1 = []
  xval1  = []
  xval2  = []
  xval3  = []
  yval2 = []
  yval3 = []
  dict = {0:xtick_lable[0],1:xtick_lable[1],2:xtick_lable[2],3:xtick_lable[3],4:xtick_lable[4],5:xtick_lable[5],6:xtick_lable[6]}
  for i in range(0,len(ticks)):
    yval1.append(np.median(farray[i][0]))
    xval1.append(dict[i]-0.8)
    xval2.append(dict[i])
    xval3.append(dict[i]+0.8)
    yval2.append(np.median(farray[i][1]))
    yval3.append(np.median(farray[i][2]))


  plt.plot(xval1, yval1 , color ='#D7191C')
  plt.plot(xval2, yval2 , color ='#2C7BB6')
  plt.plot(xval3, yval3 , color ='#31a354')
  plt.xlabel("N = √Data_points_per_proces")
  plt.ylabel("Time in sec(log scaled)")
  if(process==16):
    plt.title("BOXPLOT FOR NO.OF PROCESSES =16")
  if(process==36):
    plt.title("BOXPLOT FOR NO.OF PROCESSES =36")
  if(process==49):
    plt.title("BOXPLOT FOR NO.OF PROCESSES =49")
  if(process==64):
    plt.title("BOXPLOT FOR NO.OF PROCESSES =64")
  plt.legend(loc=4)
  plt.xticks(xtick_lable, ticks)
  plt.xlim(-2, len(ticks)*3)
  plt.tight_layout()
  plt.yscale('log')
  # # bp.set_xticklabels(['A'])
  plt.savefig('plot'+str(process)+'.png')
  #plt.show()

iter = run.no_of_iteration
processes = run.processes
matsize = run.matrixsize
data_points = run.data_points
process = run.process_count

# print(matsize,processes,iter)

for value in processes:
  myarr = []
  with open("data_"+str(value)+".txt", "rU") as f:
      for line in f:
          myarr.append((line.rstrip('\n')))

  fn(iter,myarr,matsize,value)
