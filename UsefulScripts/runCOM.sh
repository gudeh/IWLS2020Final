scp -p 22 script*.sh augusto@150.162.57.235:/users/gudeh/COMCheckAll/
ssh -p 22 augusto@150.162.57.235 'cd /users/gudeh/COMCheckAll/; ./chmod755; ./script1.sh'
ssh -p 22 augusto@150.162.57.235 'cd /users/gudeh/COMCheckAll/; ./script2.sh'
ssh -p 22 augusto@150.162.57.235 'cd /users/gudeh/COMCheckAll/; ./script3.sh'
ssh -p 22 augusto@150.162.57.235 'cd /users/gudeh/COMCheckAll/; ./script4.sh'
ssh -p 22 augusto@150.162.57.235 'cd /users/gudeh/COMCheckAll/; ./script5.sh'