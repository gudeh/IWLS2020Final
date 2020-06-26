scp -p 22 *.cpp *.h augusto@150.162.57.235:/users/gudeh/SEMCheckAll/scpCompiler/
ssh -p 22 augusto@150.162.57.235 'cd /users/gudeh/SEMCheckAll/scpCompiler; 
g++ -std=c++14 -O0 -fopenmp -o cgp *.cpp *.h;
cp cgp ..;
cd ..;
./createFolders;
rm -r /users/gudeh/AIGs/*.aig'