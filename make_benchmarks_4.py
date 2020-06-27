import numpy as np
import os
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier
import random

folderOut = "Benchmarks_4"
fileProbs = open("probs.csv","w")
print >> fileProbs, ",".join(['Example', 'P_train', 'P_valid', 'P_mean', 'P_new_train', 'P_new_valid', 'P_new_mean', 'P_valid_2'])

for path in os.listdir('./Benchmarks/'):
	if '.train' not in path: continue

	base_name = path.split('.train')[0]
	train_name = path
	valid_name = ".valid.pla"
	valid_2_name = ".valid_2.pla"

	ftrain = open('./Benchmarks/'+path,'r')
	ftest = open('./Benchmarks/'+path.replace('.train','.valid'),'r')

	linesTrain = ftrain.readlines()
	linesTest = ftest.readlines()
	ftrain.close()
	ftest.close()

	os.system("mkdir -p %s" % (folderOut))

	with open("%s/%s.train.pla" % (folderOut, base_name), 'w') as fOutTrain, open("%s/%s.valid.pla" % (folderOut, base_name), 'w') as fOutValid, open("%s/%s.valid_2.pla" % (folderOut, base_name), 'w') as fOutValid2, open("%s/%s.trainvalid.pla" % (folderOut, base_name), 'w') as fOutTrainValid:

		#print base_name
		nrInstancesNewTrain = int(int(linesTrain[2].split()[1])*0.8)
		nrInstancesValid2 = int(int(linesTrain[2].split()[1])*0.2)
		print >> fOutTrain, "%s" % (linesTrain[0].strip())
		print >> fOutTrain, ".o 1"
		print >> fOutTrain, ".p %d" % (nrInstancesNewTrain)
		print >> fOutTrain, ".type fr"
		print >> fOutValid, "%s" % (linesTrain[0].strip())
		print >> fOutValid, ".o 1"
		print >> fOutValid, ".p %d" % (nrInstancesNewTrain)
		print >> fOutValid, ".type fr"
		print >> fOutValid2, "%s" % (linesTrain[0].strip())
		print >> fOutValid2, ".o 1"
		print >> fOutValid2, ".p %d" % (nrInstancesValid2*2)
		print >> fOutValid2, ".type fr"
		print >> fOutTrainValid, "%s" % (linesTrain[0].strip())
		print >> fOutTrainValid, ".o 1"
		print >> fOutTrainValid, ".p %d" % (nrInstancesNewTrain*2)
		print >> fOutTrainValid, ".type fr"
		linesTrainExec = [x.strip() for x in linesTrain[4:len(linesTrain)-1]]
		linesValidExec = [x.strip() for x in linesTest[4:len(linesTrain)-1]]

		random.Random(4).shuffle(linesTrainExec)
		random.Random(4).shuffle(linesValidExec)

		linesTrainExec2 = linesTrainExec
		linesValidExec2 = linesValidExec

		nrEachState = int(nrInstancesValid2/2.0)
		#print nrEachState

		p0Train = 0
		p1Train = 0
		p0Valid = 0
		p1Valid = 0
		##### Get prob of output 1 in train and test set #####

		for elem in linesTrainExec:
			p1Train += int(elem.split()[1])

		for elem in linesValidExec:
			p1Valid += int(elem.split()[1])

		p1Train = float(p1Train)/len(linesTrainExec)
		p1Valid = float(p1Valid)/len(linesValidExec)
		p0Train = 1 - p1Train
		p0Valid = 1 - p1Valid

		p1Mean = np.mean([p1Train, p1Valid])
		p0Mean = np.mean([p0Train, p0Valid])

		#print p1Train
		#print p1Valid
		#print p1Mean

		######################################################

		nrOnesEachSet = int((1280*p1Mean))
		nrZerosEachSet = 1280 - nrOnesEachSet

		#print nrOnesEachSet + nrZerosEachSet
		#print nrZerosEachSet

		#print nrOnesEachSet
		#print nrZerosEachSet

		nrZeros = 0
		nrOnes = 0
		linesToRemove = []
		#i = 0
		for idx, elem in enumerate(linesTrainExec):
			state = int(elem.split()[1])
			if state == 0 and nrZeros < nrZerosEachSet:
				nrZeros += 1
				print >> fOutValid2, "%s" % (elem)
				linesTrainExec[idx] = 'Remove'
			elif state == 1 and nrOnes < nrOnesEachSet:
				nrOnes += 1
				print >> fOutValid2, "%s" % (elem)
				linesTrainExec[idx] = 'Remove'

		nrZeros = 0
		nrOnes = 0
		linesToRemove = []
		for idx, elem in enumerate(linesValidExec):
			state = int(elem.split()[1])
			if state == 0 and nrZeros < nrZerosEachSet:
				nrZeros += 1
				print >> fOutValid2, "%s" % (elem)
				linesValidExec[idx] = 'Remove'
			elif state == 1 and nrOnes < nrOnesEachSet:
				nrOnes += 1
				print >> fOutValid2, "%s" % (elem)
				linesValidExec[idx] = 'Remove'

		for shuffledTrainLine in linesTrainExec:
			if shuffledTrainLine != 'Remove':
				print >> fOutTrain, "%s" % (shuffledTrainLine)
				print >> fOutTrainValid, "%s" % (shuffledTrainLine)

		for shuffledValidLine in linesValidExec:
			if shuffledValidLine != 'Remove':
				print >> fOutValid, "%s" % (shuffledValidLine)
				print >> fOutTrainValid, "%s" % (shuffledValidLine)

		print >> fOutTrain, ".e"
		print >> fOutValid, ".e"
		print >> fOutValid2, ".e"
		print >> fOutTrainValid, ".e"

	with open("Benchmarks_4/%s.train.pla" % (base_name),"r") as fTrain, open("Benchmarks_4/%s.valid.pla" % (base_name),"r") as fValid, open("Benchmarks_4/%s.valid_2.pla" % (base_name),"r") as fValid2:
		linesTrain = fTrain.readlines()
		linesValid = fValid.readlines()
		linesValid2 = fValid2.readlines()
		linesTrainExec = [x.strip() for x in linesTrain[4:len(linesTrain)-1]]
		linesValidExec = [x.strip() for x in linesValid[4:len(linesValid)-1]]
		linesValid2Exec = [x.strip() for x in linesValid2[4:len(linesValid2)-1]]

		p0Train_ = 0
		p1Train_ = 0
		p0Valid_ = 0
		p1Valid_ = 0
		p0Valid2_ = 0
		p1Valid2_ = 0

		for elem in linesTrainExec:
			p1Train_ += int(elem.split()[1])
		for elem in linesValidExec:
			p1Valid_ += int(elem.split()[1])
		for elem in linesValid2Exec:
			p1Valid2_ += int(elem.split()[1])

		p1Train_ = float(p1Train_)/len(linesTrainExec)
		p1Valid_ = float(p1Valid_)/len(linesValidExec)
		p1Valid2_ = float(p1Valid2_)/len(linesValid2Exec)
		p0Train_ = 1 - p1Train_
		p0Valid_ = 1 - p1Valid_
		p0Valid2_ = 1 - p1Valid2_
		p1Mean_ = np.mean([p1Train_, p1Valid_])
		p0Mean_ = np.mean([p0Train_, p0Valid_])

		print >> fileProbs, ",".join([base_name] + ["%.5f" % (x) for x in [p1Train, p1Valid, p1Mean, p1Train_, p1Valid_, p1Mean_, p1Valid2_]])
