all: ts_lock cas_lock tts_lock ticket_lock array_lock aligned_lock

ts_lock: ts_lock.c
	gcc -std=c99 -o ts_lock ts_lock.c -pthread

cas_lock: cas_lock.c
	gcc -std=c99 -o cas_lock cas_lock.c -pthread

tts_lock: tts_lock.c
	gcc -std=c99 -o tts_lock tts_lock.c -pthread

ticket_lock: ticket_lock.c
	gcc -std=c99 -o ticket_lock ticket_lock.c -pthread

array_lock: array_lock.c
	gcc -std=c99 -o array_lock array_lock.c -pthread

aligned_lock: aligned_lock.c
	gcc -std=c99 -o aligned_lock aligned_lock.c -pthread
