all: serverA.c serverB.c serverC.c aws.c monitor.c
	gcc serverA.c -o serverAoutput
	gcc serverB.c -o serverBoutput
	gcc serverC.c -o serverCoutput -lm
	gcc aws.c -o awsoutput
	gcc monitor.c -o monitoroutput
	gcc client.c -o client

serverA: serverA.c
	./serverAoutput
serverB: serverB.c
	./serverBoutput	
serverC: serverC.c
	./serverCoutput
aws: aws.c
	./awsoutput
monitor: monitor.c
	./monitoroutput

.PHONY: all serverA serverB serverC aws monitor 
clean:
	$(RM) serverA serverB serverC aws monitor 
