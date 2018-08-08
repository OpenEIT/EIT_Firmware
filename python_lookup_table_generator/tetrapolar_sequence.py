# Python script
# Electrode configuration generator
# Adjacent Drive Method
# 
import numpy as np
# from sets import Set

n_electrodes   = 32 # 
n_multiplexers = 4  # translates into number of columns in file. 

# 
# the rule is that two electrodes are next to each other? 
# other two can be anywhere, but not where the first two are? 
# 
# outer loop -> out electrodes
# inner loop -> in electrodes 
f = open('e_config.txt','w')
full = []

for x in range(0, 32):

	for y in range(0, 32):

		if abs(y-x) < 2:
			print ('skip')
		else:
			if x >= 31: 
				if y >= 31:
					f.write('%d,%d,%d,%d\n' % (31,0,31,0))
					full.append([31,0,31,0])
				else:	

					if y>0:
						f.write('%d,%d,%d,%d\n' % (31,0,y,y+1))
						full.append([31,0,y,y+1])
			else:
				if y >= 31:
					if x >0:
						f.write('%d,%d,%d,%d\n' % (x,x+1,31,0))
						full.append([x,x+1,31,0])
				else: 
					
					# print ('%d,%d,%d,%d' % (x,x+1,y,y+1))
					f.write('%d,%d,%d,%d\n' % (x,x+1,y,y+1))
					full.append([x,x+1,y,y+1])

f.close()

print ('total entries: %d' %len(full))
#print (full) 
# 

# seen = set()
# unique = []
# for x in full: 
# 	srtd = tuple(sorted(x))
# 	if srtd not in seen:
# 		unique.append(x)
# 		seen.add(srtd)
# Are we getting unique entries? 
# 
# bipolar arrangement:
# nchoose 2 = n(N-1)/2 = 32(32-1)/2 = 496
# 8(8-1)/2 = 28
# Adjacent drive method:
# N(N-3) = 928

# 
#print (len(unique))
# print (len(seen))
# a = np.array([[1, 0, 0], [1, 0, 0], [2, 3, 4]])
# uns= np.unique(full, axis=1)
# # print (uns)
# print (len(uns))

# [30, 31, 32, 1]
# [32, 1, 30, 31]

