.PHONY:all
all:service client

FLAGS=-Wall

service:service.c
	gcc -o $@ $^ $(FLAGS)
client:client.c
	gcc -o $@ $^ $(FLAGS)


.PHONY:clean
clean:
	rm -f service client
