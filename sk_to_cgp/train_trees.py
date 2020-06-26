import numpy as np
import os
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier
from sklearn.tree import _tree
from copy import deepcopy
import numpy as np 
from math import sqrt, ceil
from itertools import combinations
from sklearn.model_selection import train_test_split, cross_val_score
import graphviz

from itertools import combinations

import pandas as pd

from sklearn.ensemble import VotingClassifier
from sklearn.feature_selection import VarianceThreshold
from sklearn.feature_selection import SelectKBest, SelectPercentile
from sklearn.feature_selection import chi2, f_classif, mutual_info_classif
from sklearn.pipeline import Pipeline
from copy import deepcopy
from sklearn.tree import *
from sklearn.model_selection import GridSearchCV
from sklearn.feature_selection import SelectFromModel
from sklearn.ensemble import ExtraTreesClassifier
from sklearn.neural_network import MLPClassifier
from itertools import product

function_mappings = {
	'chi2': chi2,
	'f_classif': f_classif,
	'mutual_info_classif': mutual_info_classif,
}

def fillWithZeroes(X, X_new):
	X_new2 = deepcopy(X)
	i = 0
	for column in X.T:
		found = 0
		for compColumn in X_new.T:
			if np.array_equal(column, compColumn):
				found = 1
				break
		if found == 0:
			X_new2[:,i] = 0
		i += 1

	return X_new2
				

def pythonizeSOP(sop, classifier = 'other'):
	or_list = []
	expr = ''
	if sop == []:
		expr = '((x0) * !(x0))'
		return expr

	for ands in sop:
		and_list = []
		and_expr = '('
		for attr,negated in ands:
			if negated == 'true':
				and_list.append('not(x%s)' %  (attr))
			else:
				and_list.append('(x%s)' %  (attr))
		and_expr += ' and '.join(and_list)
		and_expr += ')'
		or_list.append(and_expr)
	expr = '(%s)' % (' or '.join(or_list))
	return expr

def pythonizeRF(sopRF):
	exprs = []
	for sopDT in sopRF:
		exprs.append(pythonizeSOP(sopDT))

	print exprs
	if exprs == []:
		finalExpr = '((x0) * !(x0))'
		return finalExpr

	nrInputsMaj = len(exprs)
	sizeTermMaj = int(ceil(nrInputsMaj/2.0))
	ands = []

	for comb in combinations(exprs, sizeTermMaj):
		ands.append(" and ".join(comb))

	finalExpr = '(%s)' % (") or (".join(ands))

	return finalExpr

def trainMajorityRF(train_data, test_data, test2_data, num_trees = 100, apply_SKB = 0, apply_SP = 0, score_f = "chi2", thr = 0.8, k = 10, percent = 50, depth = 10, useDefaultDepth= 1):
	Xtr, ytr = train_data[:,:-1], train_data[:,-1]
	Xte, yte = test_data[:,:-1], test_data[:,-1]
	Xte2, yte2 = test2_data[:,:-1], test2_data[:,-1]
	Xtr_new = deepcopy(Xtr)

	if apply_SKB == 1:
		selector = SelectKBest(function_mappings[score_f], k = k)
		selector.fit(Xtr, ytr)
		Xtr_new = selector.transform(Xtr)
		Xtr_new = fillWithZeroes(Xtr, Xtr_new)

	if apply_SP == 1:
		selector = SelectPercentile(function_mappings[score_f], percentile = percent)
		selector.fit(Xtr, ytr)
		Xtr_new = selector.transform(Xtr)
		Xtr_new = fillWithZeroes(Xtr, Xtr_new)

	num_feats_sub = int(sqrt(Xtr_new.shape[1]))
	num_feats = Xtr_new.shape[1]
	trees = []
	votes = []
	votes2 = []
	for i in range(num_trees):
		cols_idx = np.random.choice(range(num_feats),num_feats - num_feats_sub)

		Xtr_sub = np.array(Xtr_new)
		Xtr_sub[:,cols_idx] = 1
		tree = None
		if useDefaultDepth == 1:
			tree = DecisionTreeClassifier().fit(Xtr_sub, ytr)
		else:
			tree = DecisionTreeClassifier(max_depth = depth).fit(Xtr_sub, ytr)
		trees.append(tree)
		votes.append(tree.predict(Xte))
		votes2.append(tree.predict(Xte2))
	votes = np.array(votes).T
	votes2 = np.array(votes2).T
	final_vote = np.round(votes.sum(axis=1)/float(num_trees)).astype('int')
	final_vote2 = np.round(votes2.sum(axis=1)/float(num_trees)).astype('int')
	acc = (final_vote == yte).mean()
	acc2 = (final_vote2 == yte2).mean()

	return trees, acc, acc2

def eval_single( d, eqn_str):
	eqn_str_orig = eqn_str

	for i, d_ in enumerate(d):
		eqn_str = eqn_str.replace('x%d)' % (i), (str(d_)+')'))

	return int(eval(eqn_str))

def eval_equation(eqn_str, data):
	X, y = data[:,:-1], data[:,-1]
	hits = 0
	ypred =  np.apply_along_axis(eval_single, 1,  X, eqn_str)
	return str((np.equal(ypred, y, dtype=int)).mean())

def treeToSOP(tree, featureNames):
	tree_ = tree.tree_
	featureName = [featureNames[i] if i != _tree.TREE_UNDEFINED else "undefined!" for i in tree_.feature]

	ors = []

	if tree_.feature[0] == _tree.TREE_UNDEFINED:
		if np.argmax(tree_.value[0]) == 1:
			ors.append([['1', 'true']])
			ors.append([['1', 'false']])
		return ors

	def recurse(node, depth, expression):
		indent = "\t" * depth
		if tree_.feature[node] != _tree.TREE_UNDEFINED:
			name = featureName[node]

			recurse(tree_.children_left[node], depth + 1, deepcopy(expression + [[name, 'true']]))

			recurse(tree_.children_right[node], depth + 1, deepcopy(expression + [[name, 'false']]))
		else:
			if np.argmax(tree_.value[node]) == 1:
				ors.append(deepcopy(expression))
			
	recurse(0, 1, [])

	return ors

def forestToSOP(forest, featureNames):
	sops = []
	for tree in forest:
		sop = treeToSOP(tree, featureNames)
		sops.append(sop)
	return sops

def trainTree(train_data, test_data, test2_data, apply_scan = 0, columns = [0, 0], apply_SKB = 0, apply_SP = 0, apply_SFM = 0, score_f = "chi2", k = 10, percent = 40, depth = 10, useDefaultDepth= 1):

	Xtr, ytr = train_data[:,:-1], train_data[:,-1]
	Xte, yte = test_data[:,:-1], test_data[:,-1]
	Xte2, yte2 = test2_data[:,:-1], test2_data[:,-1]

	if apply_scan == 1:
		Xtr = fillWithZeroes(Xtr, train_data[:, columns])
		Xte = fillWithZeroes(Xte, test_data[:, columns])
		Xte2 = fillWithZeroes(Xte2, test2_data[:, columns])
	
	Xtr_new = deepcopy(Xtr)

	if apply_SKB == 1:
		selector = SelectKBest(function_mappings[score_f], k = k)
		selector.fit(Xtr, ytr)
		Xtr_new = selector.transform(Xtr)
		Xtr_new = fillWithZeroes(Xtr, Xtr_new)

	if apply_SP == 1:
		selector = SelectPercentile(function_mappings[score_f], percentile = percent)
		selector.fit(Xtr, ytr)
		Xtr_new = selector.transform(Xtr)
		Xtr_new = fillWithZeroes(Xtr, Xtr_new)

	tree = None

	if useDefaultDepth == 1:
		tree = DecisionTreeClassifier().fit(Xtr_new, ytr)
	else:
		tree = DecisionTreeClassifier(max_depth = depth).fit(Xtr_new, ytr)
	ypred_tree = tree.predict(Xte)
	ypred2_tree = tree.predict(Xte2)
	acc_tree = (ypred_tree == yte).mean()
	acc2_tree = (ypred2_tree == yte2).mean()

	return tree, acc_tree, acc2_tree

def trainNN(train_data, test_data, test2_data, grid = 0, hidden_layers = (100,)):
	Xtr, ytr = train_data[:,:-1], train_data[:,-1]
	Xte, yte = test_data[:,:-1], test_data[:,-1]
	Xte2, yte2 = test2_data[:,:-1], test2_data[:,-1]

	if not grid:
		nn = MLPClassifier(hidden_layer_sizes=hidden_layers).fit(Xtr, ytr)
	else:
		clf = MLPClassifier(hidden_layer_sizes=hidden_layers)
		nn = runGridSearch(clf, Xtr, ytr, tree_params, csv_path = 'tree_gs.csv')

	ypred_nn = nn.predict(Xte)

	acc_nn = (ypred_nn == yte).mean()*100

	return nn, acc_nn

def gen_eqn(example, expr, nameOut):
	os.system("mkdir -p EQNS")
	with open("EQNS/%s.eqn" % (nameOut), "w") as fOut:
		numLines = sum(1 for line in open("%s.names" % (example), "r"))
		nrInputs = numLines - 4
		print >> fOut, "INORDER = %s;" % (" ".join(["x%d" % (a) for a in range(nrInputs)]))
		print >> fOut, "OUTORDER = z1;"
		print "%s" % (expr.replace("and", "*").replace("or", "+").replace("not", "!"))
		print >> fOut, "z1 = %s;" % (expr.replace("and", "*").replace("or", "+").replace("not", "!"))

def gen_eqn_aig(expr, baseName, exampleCfg):
	os.system("mkdir -p EQNS")
	os.system("mkdir -p AIGS")
	os.system("mkdir -p OPT_AIGS")

	with open("EQNS/%s.eqn" % (exampleCfg), "w") as fOut, open("scriptGenAig", "w") as fOut2, open("scriptOptAig", "w") as fOut3:
		numLines = sum(1 for line in open("%s.names" % (baseName), "r"))
		nrInputs = numLines - 3
		print >> fOut, "INORDER = %s;" % (" ".join(["x%d" % (a) for a in range(nrInputs)]))
		print >> fOut, "OUTORDER = z1;"
		print >> fOut, "z1 = %s;" % (expr.replace("and", "*").replace("or", "+").replace("not", "!"))
		print >> fOut2, "read_eqn EQNS/%s.eqn" % (exampleCfg)
		print >> fOut2, "strash"
		print >> fOut2, "write_aiger AIGS/%s.aig" % (exampleCfg)
		print >> fOut3, "read_aiger AIGS/%s.aig" % (exampleCfg)
		print >> fOut3, "refactor"
		print >> fOut3, "rewrite"
		print >> fOut3, "write_aiger OPT_AIGS/%s.aig" % (exampleCfg)

	os.system("./abc -F scriptGenAig")
	os.system("./abc -F scriptOptAig")

def run_aig(baseName, exampleCfg):
	with open("scriptRunAig", "w") as fOut, open("scriptRunAig2", "w") as fOut2:
		print >> fOut, "&r OPT_AIGS/%s.aig" % (exampleCfg)
		print >> fOut, "&ps"
		print >> fOut, "&mltest Benchmarks_3/%s.valid.pla" % (baseName)
		print >> fOut2, "&r OPT_AIGS/%s.aig" % (exampleCfg)
		print >> fOut2, "&ps"
		print >> fOut2, "&mltest Benchmarks_3/%s.valid_2.pla" % (baseName)

	os.system("./abc -F scriptRunAig > teste.txt")
	os.system('sed -r "s/\x1B\[([0-9]{1,3}(;[0-9]{1,2})?)?[mGK]//g" teste.txt > teste2.txt')
	with open("teste2.txt", "r") as fIn:
		lines = fIn.readlines()
		ands = lines[4].split()[8]
		acc_abc = float(lines[7].split()[8])/float(lines[7].split()[2])
	os.system("./abc -F scriptRunAig2 > teste.txt")
	os.system('sed -r "s/\x1B\[([0-9]{1,3}(;[0-9]{1,2})?)?[mGK]//g" teste.txt > teste2.txt')
	with open("teste2.txt", "r") as fIn:
		lines = fIn.readlines()
		acc2_abc = float(lines[7].split()[8])/float(lines[7].split()[2])

	return ands, acc_abc, acc2_abc

def is_naive_equal(baseName, exampleCfg):
	with open("scriptTestNaive", "w") as fOut:
		print >> fOut, "&r OPT_AIGS/%s.aig" % (exampleCfg)
		print >> fOut, "&ps"
		print >> fOut, "&mltest Benchmarks_3/%s.valid_2.pla" % (baseName)
	
	os.system("./abc -F scriptTestNaive > teste.txt")
	os.system('sed -r "s/\x1B\[([0-9]{1,3}(;[0-9]{1,2})?)?[mGK]//g" teste.txt > teste2.txt')
	with open("teste2.txt", "r") as fIn:
		lines = fIn.readlines()
		acc = float(lines[7].replace("(","( ").split()[10])
		acc_naive = float(lines[7].replace("(","( ").split()[17])
		return acc == acc_naive

def plot_PDF(clf, feats = None, labels = None, numTrees = 3, classifier = 'forest'):
	if classifier == 'tree':
		dot_data = export_graphviz(clf, out_file=None, feature_names=feats, class_names = labels, filled=True, rounded=True, special_characters=True)
		graph = graphviz.Source(dot_data)
		graph.render("0")
	else:
		for i in range(numTrees):
			dot_data = export_graphviz(clf[i], out_file=None, feature_names=feats, class_names= labels, filled=True, rounded=True, special_characters=True)
			graph = graphviz.Source(dot_data)
			graph.render("%d" % (i))


listToTest = ['ex%s.data' % (x) for x in ["00","01","02","03","04","05","06","07","08","09"] + [str(x) for x in range(10,100)]]

depths = [10,20]
useDefaultDepth = 0
percentiles = [25, 50, 75]
ks = [0.25, 0.5, 0.75]
score_fs = ['chi2', 'f_classif', 'mutual_info_classif']
num_treess = [3]
execParams = 1
execScan2 = 0
execScanAll = 0
execNN = 1
proportions = ["40-20"]

fOut = open("results.csv", "w")
print >> fOut, ",".join(["Example", "Proportion", "Classifier", "Nr_Trees", "Depth", "Select_KBest", "K", "Select_Percentile", "Percentile", "Scoring_Function", "Scan", "Columns", "Neural", "Idx_neural", "Seed", "Accuracy_test1", "Accuracy_test2", "Golden_Accuracy", "ABC_Accuracy", "ABC_accuracy_2", "Ands", "Equation"])
for path in listToTest:
	base_name = path.split('.data')[0]
	c50f_data = base_name + '.data'
	c50f_test = base_name + '.test'
	c50f_test2 = base_name + '.test2'

	for proportion in proportions:

		train_data = np.loadtxt(c50f_data, dtype='int', delimiter=',')
		test_data = np.loadtxt(c50f_test, dtype='int', delimiter=',')
		test2_data = np.loadtxt(c50f_test2, dtype='int',delimiter=',')

		featureNames = list(map(str, list(range(train_data.shape[1]))))
		nrFeatures = len(featureNames)

		if proportion == "80-20":
			train_data = np.concatenate((train_data, test_data))

		bestAcc = 0.0
		acc2_tree = 0.0
		if execParams == 1:
			for seed in range(3):
				for depth in depths:
					print "Applying no preprocessing mechanisms, not mixing sets..."
					nameCfg = "_".join([str(x) for x in [base_name, proportion, "tree", "1", "-" if useDefaultDepth == 1 else depth] + ["-"]*9 + [seed]])
					np.random.seed(seed)
		                        tree, acc_tree, acc2_tree = trainTree(train_data, test_data, test2_data, useDefaultDepth = useDefaultDepth, depth = depth)
					if acc2_tree > bestAcc:
						sop_tree = treeToSOP(tree, featureNames)
						expr_tree = pythonizeSOP(sop_tree, classifier = 'tree')
						gen_eqn_aig(expr_tree, base_name, nameCfg)
						ands, acc_abc, acc2_abc = run_aig(base_name, nameCfg)
						if int(ands) <= 5000:
							bestAcc = acc2_tree
							print >> fOut, ",".join([str(x) for x in [path, proportion, "tree", "1", "-" if useDefaultDepth == 1 else depth] + ["-"]*9 + [seed, acc_tree, acc2_tree, "-", acc_abc, acc2_abc, ands, expr_tree]])
					for num_trees in num_treess:
						nameCfg = "_".join([str(x) for x in [base_name, proportion, "forest", num_trees, "-" if useDefaultDepth == 1 else depth] + ["-"]*9 + [seed]])
						np.random.seed(seed)
						rf, acc_rf, acc2_rf = trainMajorityRF(train_data, test_data, test2_data, num_trees = num_trees, useDefaultDepth = useDefaultDepth, depth = depth)
						if acc2_rf > bestAcc:
							sop_rf = forestToSOP(rf, featureNames)
							expr_rf = pythonizeRF(sop_rf)
							gen_eqn_aig(expr_rf, base_name, nameCfg)
							ands, acc_abc, acc2_abc = run_aig(base_name, nameCfg)
							if int(ands) <= 5000:
								bestAcc = acc2_rf
								print >> fOut, ",".join([str(x) for x in [path, proportion, "forest", num_trees, "-" if useDefaultDepth == 1 else depth] + ["-"]*9 +[seed, acc_rf, acc2_rf, "-", acc_abc, acc2_abc, ands, expr_rf]])
					print "Done!"
		
					for k in ks:
						for score_f in score_fs:
							print "Applying Select K-Best to data, with %s function, with k = %.2f..." % (score_f, k)
							nameCfg = "_".join([str(x) for x in [base_name, proportion, "tree", "1", "-" if useDefaultDepth == 1 else depth, "yes", k, "-", "-", score_f, "-", "-", "-", "-", seed]])
							np.random.seed(seed)
							tree_SKB, acc_tree_SKB, acc2_tree_SKB = trainTree(train_data, test_data, test2_data, apply_SKB = 1, k = int(k*nrFeatures), score_f = score_f, useDefaultDepth = useDefaultDepth, depth = depth)
							if acc2_tree_SKB > bestAcc:
								sop_tree_SKB = treeToSOP(tree_SKB, featureNames)
								expr_tree_SKB = pythonizeSOP(sop_tree_SKB, classifier = 'tree')
								gen_eqn_aig(expr_tree_SKB, base_name, nameCfg)
								ands, acc_abc, acc2_abc = run_aig(base_name, nameCfg)
								if int(ands) <= 5000:
									bestAcc = acc2_tree_SKB
									print >> fOut, ",".join([str(x) for x in [path, proportion, "tree", "1", "-" if useDefaultDepth == 1 else depth, "yes", k, "-", "-", score_f, "-", "-", "-", "-", seed, acc_tree_SKB, acc2_tree_SKB, "-", acc_abc, acc2_abc, ands, expr_tree_SKB]])
							for num_trees in num_treess:
								nameCfg = "_".join([str(x) for x in [base_name, proportion, "forest", num_trees, "-" if useDefaultDepth == 1 else depth, "yes", k, "-", "-", score_f, "-", "-", "-", "-", seed]])
								np.random.seed(seed)
								rf_SKB, acc_rf_SKB, acc2_rf_SKB = trainMajorityRF(train_data, test_data, test2_data, num_trees = num_trees, apply_SKB = 1, k = int(k*nrFeatures), score_f = score_f, useDefaultDepth = useDefaultDepth, depth = depth)
								if acc2_rf_SKB > bestAcc:
									sop_rf_SKB = forestToSOP(rf_SKB, featureNames)
									expr_rf_SKB = pythonizeRF(sop_rf_SKB)
									gen_eqn_aig(expr_rf_SKB, base_name, nameCfg)
									ands, acc_abc, acc2_abc = run_aig(base_name, nameCfg)
									if int(ands) <= 5000:
										bestAcc = acc2_rf_SKB
										print >> fOut, ",".join([str(x) for x in [path, proportion, "forest", num_trees, "-" if useDefaultDepth == 1 else depth, "yes", k, "-", "-", score_f, "-", "-", "-", "-", seed, acc_rf_SKB, acc2_rf_SKB, "-", acc_abc, acc2_abc, ands, expr_rf_SKB]])
							print "Done!"
		
					for percentile in percentiles:
						for score_f in score_fs:
							print "Applying Select Percentile to data, with %s function, with percentile = %f..." % (score_f, percentile)
							nameCfg = "_".join([str(x) for x in [base_name, proportion, "tree", "1", "-" if useDefaultDepth == 1 else depth] + ["-"]*2 + ["yes", percentile, score_f, "-", "-", "-", "-", seed]])
							np.random.seed(seed)
							tree_SP, acc_tree_SP, acc2_tree_SP = trainTree(train_data, test_data, test2_data, apply_SP = 1, percent = percentile, score_f = score_f, useDefaultDepth = useDefaultDepth, depth = depth)
							if acc2_tree_SP > bestAcc:
								sop_tree_SP = treeToSOP(tree_SP, featureNames)
								expr_tree_SP = pythonizeSOP(sop_tree_SP, classifier = 'tree')
								gen_eqn_aig(expr_tree_SP, base_name, nameCfg)
								ands, acc_abc, acc2_abc = run_aig(base_name, nameCfg)
								if int(ands) <= 5000:
									bestAcc = acc2_tree_SP
									print >> fOut, ",".join([str(x) for x in [path, proportion, "tree", "1", "-" if useDefaultDepth == 1 else depth] + ["-"]*2 + ["yes", percentile, score_f, "-", "-", "-", "-", seed, acc_tree_SP, acc2_tree_SP, "-", acc_abc, acc2_abc, ands, expr_tree_SP]])
							for num_trees in num_treess:
								nameCfg = "_".join([str(x) for x in [base_name, proportion, "forest", num_trees, "-" if useDefaultDepth == 1 else depth] + ["-"]*2 + ["yes", percentile, score_f, "-", "-", "-", "-", seed]])
								np.random.seed(seed)
								rf_SP, acc_rf_SP, acc2_rf_SP = trainMajorityRF(train_data, test_data, test2_data, num_trees = num_trees, apply_SP = 1, percent = percentile, score_f = score_f, useDefaultDepth = useDefaultDepth, depth = depth)
								if acc2_rf_SP > bestAcc:
									sop_rf_SP = forestToSOP(rf_SP, featureNames)
									expr_rf_SP = pythonizeRF(sop_rf_SP)
									gen_eqn_aig(expr_rf_SP, base_name, nameCfg)
									ands, acc_abc, acc2_abc = run_aig(base_name, nameCfg)
									if int(ands) <= 5000:
										bestAcc = acc2_rf_SP
										print >> fOut, ",".join([str(x) for x in [path, proportion, "forest", num_trees, "-" if useDefaultDepth == 1 else depth] + ["-"]*2 + ["yes", percentile, score_f, "-", "-", "-", "-", seed, acc_rf_SP, acc2_rf_SP, "-", acc_abc, acc2_abc, ands, expr_rf_SP]])
							print "Done!"
		
		seed = 0
		depth = 10
		np.random.seed(0)
		if execScan2 == 1:
			for column1 in range(nrFeatures-1):
				if bestAcc > 0.95:
					break
				for column2 in range(nrFeatures-1):
					if bestAcc > 0.95:
						break
					if column1 == column2:
						continue
					concCol = "_".join([str(x) for x in [column1, column2]])
					nameCfg = "_".join([str(x) for x in [base_name, proportion, "tree", "1", "-" if useDefaultDepth == 1 else depth] + ["-"]*5 + ["yes", concCol, "-", "-", seed]])
					np.random.seed(seed)
					tree, acc_tree, acc2_tree = trainTree(train_data, test_data, test2_data, apply_scan = 1, columns = [column1, column2], useDefaultDepth = useDefaultDepth, depth = depth)
					if acc2_tree > bestAcc:
						sop_tree = treeToSOP(tree, featureNames)
						expr_tree = pythonizeSOP(sop_tree, classifier = 'tree')
						gen_eqn_aig(expr_tree, base_name, nameCfg)
						ands, acc_abc, acc2_abc = run_aig(base_name, nameCfg)
						if int(ands) <= 5000:
							bestAcc = acc2_tree
							print >> fOut, ",".join([str(x) for x in [path, proportion, "tree", "1", depth] + ["-"]*5 + ["yes", concCol, "-", "-", seed, acc_tree, acc2_tree, "-", acc_abc, acc2_abc, ands, expr_tree]])
	
		if execScanAll == 1:
			idxs = range(16)
			for nrColumns in range(2,8):
				if bestAcc > 0.95:
					break
				for columns in combinations(idxs,nrColumns):
					if bestAcc > 0.95:
						break
					concCol = "_".join([str(x) for x in columns])
					nameCfg = "_".join([str(x) for x in [base_name, proportion, "tree", "1", "-" if useDefaultDepth == 1 else depth] + ["-"]*5 + ["yes", concCol, "-", "-", seed]])
					tree, acc_tree, acc2_tree = trainTree(train_data, test_data, test2_data, apply_scan = 1, columns = list(columns), useDefaultDepth = useDefaultDepth, depth = depth)
					if acc2_tree > bestAcc:
						sop_tree = treeToSOP(tree, featureNames)
						expr_tree = pythonizeSOP(sop_tree, classifier = 'tree')
						gen_eqn_aig(expr_tree, base_name, nameCfg)
						ands, acc_abc, acc2_abc = run_aig(base_name, nameCfg)
						if int(ands) <= 5000:
							bestAcc = acc2_tree
							print >> fOut, ",".join([str(x) for x in [path, proportion, "tree", "1", depth] + ["-"]*5 + ["yes", concCol, "-", "-", seed, acc_tree, acc2_tree, "-", acc_abc, acc2_abc, ands, expr_tree]])
	
		if execNN == 1:
			print "Starting Neural Network solution..."
			for seed in range(3):
				np.random.seed(seed)
				nn, acc_nn = trainNN(train_data, test_data, test2_data, grid = 0)
				nn_importances = np.abs(np.array(nn.coefs_[0]).mean(axis=1))
				srtdImportances = sorted(nn_importances)
	
				feats = []
				for importance in srtdImportances[-4:]:
					feats.append("x%d" % (list(nn_importances).index(importance)))
			
				i = 0
				for nrVars in range(2,5):
					nrOps = nrVars - 1
					for ins in combinations(feats, nrVars):
						for nots in product(['not', ''], repeat=nrVars):
							for ops in product(['or','and','xor'],repeat=nrOps):
								nameCfg = "_".join([str(x) for x in [base_name, proportion, "-", "-", "-"] + ["-"]*5 + ["-", "-", "yes", str(i), seed]])
								strExpr = '(%s(%s))%s' % (nots[0], ins[0], " ".join([' %s (%s(%s))' % (op, _not, _in) for op, _not, _in in zip(ops, nots[1:], ins[1:])]))
								if 'xor' in ops:
									splitExpr = strExpr.split(" xor ")
									finalExpr = '((%s) and not(%s)) or (not(%s) and (%s))' % (splitExpr[0], splitExpr[1], splitExpr[0], splitExpr[1])
									iters = 2
									while iters < len(splitExpr):
										finalExpr = '((%s) and not(%s)) or (not(%s) and (%s))' % (finalExpr, splitExpr[iters], finalExpr, splitExpr[iters])
										iters += 1
								else:
									finalExpr = strExpr
	
								gen_eqn_aig(finalExpr, base_name, nameCfg)
								ands, acc_abc, acc2_abc = run_aig(base_name, nameCfg)
								print >> fOut, ",".join([str(x) for x in [path, proportion, "-", "-", "-"] + ["-"]*5 + ["-", "-", "yes", str(i), seed, "-", acc2_abc, "-", acc_abc, acc2_abc, ands, finalExpr]])
								i += 1	
	
fOut.close()

folderBestAigs = "AIGS_BEST"
os.system("mkdir -p %s" % (folderBestAigs))

dfResults = pd.read_csv("results.csv")
dfBests = pd.DataFrame(columns = dfResults.columns)

for example in listToTest:
	dfDescending = dfResults[dfResults['Example'] == example].sort_values(by='Accuracy_test2', ascending=False)
	for index, row in dfDescending.iterrows():
		nameBestCfg = "_".join([str(x) for x in [row['Example'].split(".")[0], row["Proportion"], row['Classifier'], row['Nr_Trees'], row['Depth'], row['Select_KBest'], row['K'], row['Select_Percentile'], row['Percentile'], row['Scoring_Function'], row['Scan'], row['Columns'], row["Neural"], row["Idx_neural"], row['Seed']]])
		if is_naive_equal(row['Example'].split(".")[0], nameBestCfg) == True:
			print "is equal"
		if row['Ands'] <= 5000 and is_naive_equal(row['Example'].split(".")[0], nameBestCfg) == False:
			dfBests = dfBests.append(row)
			os.system("cp OPT_AIGS/%s.aig %s/%s.aig" % (nameBestCfg, folderBestAigs, example.split(".")[0]))
			break

dfBests.to_csv("max_results.csv", index=False)
