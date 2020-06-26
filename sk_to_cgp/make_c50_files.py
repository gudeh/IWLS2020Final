import numpy as np
import os
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier

def saveNamesFile(nfeat, filename):
	f = open(filename,'w')
	print >> f, 'Fout.\n'
	for i in range(nfeat):
		print >> f, 'X%d:\t0,1.' % (i)

	print >> f, 'Fout:\t0,1.'
	f.close()


output_csv = open('c50_results.csv', 'w')
print >> output_csv, ','.join(['base_name', 'sk_acc_tree', 'sk_acc_rf', 'tr_acc', 'te_acc', 'eq_one', 'eq_zero'])

for path in os.listdir('./Benchmarks_3/'):
	if '.train' not in path: continue
	
	base_name = path.split('.train')[0]
	c50f_data = base_name + '.data'
	c50f_names = base_name + '.names'
	c50f_test = base_name + '.test'
	c50f_test_2 = base_name + '.test2'
	c50f_output = base_name + '.out'

	ftrain = open('./Benchmarks_3/'+path,'r')
	ftest = open('./Benchmarks_3/'+path.replace('.train','.valid'),'r')
	ftest2 = open('./Benchmarks_3/'+path.replace('.train','.valid_2'),'r')

	lines = ftrain.readlines()
	lines_test = ftest.readlines()
	lines_test2 = ftest2.readlines()
	ftrain.close()
	ftest.close()
	ftest2.close()

	train_data = []
	test_data = []
	test2_data = []
	for line in lines:
		if '.' in line: continue
		x, y = line.split()
		x = [_ for _ in x]
		train_data.append(x+[y])

	for line in lines_test:
		if '.' in line: continue
		x, y = line.split()
		x = [_ for _ in x]
		test_data.append(x+[y])

	for line in lines_test2:
		if '.' in line: continue
		x, y = line.split()
		x = [_ for _ in x]
		test2_data.append(x+[y])

	train_data = np.array(train_data)
	test_data = np.array(test_data)
	test2_data = np.array(test2_data)
	np.savetxt(c50f_data, train_data, fmt = '%c', delimiter=',')
	np.savetxt(c50f_test, test_data, fmt = '%c', delimiter=',')
	np.savetxt(c50f_test_2, test2_data, fmt = '%c', delimiter=',')
	saveNamesFile(nfeat = train_data.shape[1]-1, filename =c50f_names)


output_csv.close()
