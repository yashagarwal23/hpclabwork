all: parent producer consumer

parent: parent.c
	gcc -w parent.c -o parent

producer: producer.c
	gcc producer.c -o producer

consumer: consumer.c
	gcc consumer.c -o consumer

clean:
	rm -f parent consumer producer producer_consumer_thread

thread:
	gcc producer_consumer_thread.c -o producer_consumer_thread -lpthread

