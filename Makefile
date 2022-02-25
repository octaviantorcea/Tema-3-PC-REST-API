build: library_client.c buffer.c helpers.c parson.c requests.c
	gcc -Wall library_client.c buffer.c helpers.c parson.c requests.c -o library_client

clean:
	rm -rf library_client
