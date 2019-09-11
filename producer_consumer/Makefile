all: parent producer consumer

parent: parent.c
	gcc -w parent.c -o parent -lpthread

producer: producer.c
	gcc producer.c -o producer -lpthread

consumer: consumer.c
	gcc consumer.c -o consumer -lpthread

clean:
	rm -f parent consumer producer

thread:
	gcc producer_consumer_thread.c -o producer_consumer_thread -lpthread

