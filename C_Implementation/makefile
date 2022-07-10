CC=cc 
CFLAGS=-Wall
CFLAGS += -I/usr/include/postgresql -std=c99 -lpq


include /home/all/Documents/librdkafka/mklove/Makefile.base

LIBRDKAFKA_CFLAGS += -I/home/all/Documents/librdkafka/src
LIBRDKAFKA_CXXFLAGS += -I/home/all/Documents/librdkafka/src-cpp


consumer: consumer.c /home/all/Documents/librdkafka/src/librdkafka.a save_to_json.c
	$(CC) $(CPPFLAGS) $(LIBRDKAFKA_CFLAGS) $@.c  rt_mb_buffer.c save_to_json.c -o $@ $(LDFLAGS) \
	/home/all/Documents/librdkafka/src/librdkafka.a $(LIBS)
	
clean:
	rm -f *.o consumer

