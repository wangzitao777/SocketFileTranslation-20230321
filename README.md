# SocketFileTranslation-20230321
This project aims at regularly translating files from clients, such as one-chip computer(STM-32, Raspberry Pi, etc) to a data collecting server. It can divide a large file into small pieces and then transfer them to the server in blocks, and merge them on the server side
Usage: 
1. If you choose Windows system as your server, please Compile the source file in a bash capable software, or you just modify the header files in the beginning.
   gcc server.c -o server.exe  # Windows
   gcc server.c -o server      # Linux
2. Compile the client.
   gcc client.c -o client
3. Start up the server.
   ./server.exe <port>  # Windows
   ./server     <port>  # Linux
4. Start the clients.
   ./client <ip> <port> <file_name>
5. The file will be saved the same path as the server locates.
