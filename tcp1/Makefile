.phony:all
all:service client

flags=-Wall

service:service.c
	gcc -o $@ $^ $(flags)
client:client.c
	gcc -o $@ $^ $(flags)


.phony:clean
clean:
	rm -f service client
