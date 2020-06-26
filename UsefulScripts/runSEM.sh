scp -p 22 script*.sh augusto@150.162.57.235:/users/gudeh/SEMCheckAll/
ssh -p 22 augusto@150.162.57.235 'cd /users/gudeh/SEMCheckAll/; ./chmod755; ./script1.sh'
ssh -p 22 augusto@150.162.57.235 'cd /users/gudeh/SEMCheckAll/; ./script2.sh'
ssh -p 22 augusto@150.162.57.235 'cd /users/gudeh/SEMCheckAll/; ./script3.sh'
ssh -p 22 augusto@150.162.57.235 'cd /users/gudeh/SEMCheckAll/; ./script4.sh'
ssh -p 22 augusto@150.162.57.235 'cd /users/gudeh/SEMCheckAll/; ./script5.sh'