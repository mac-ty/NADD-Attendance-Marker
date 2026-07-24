FROM ubuntu:22.04

RUN apt-get update && apt-get install -y g++ make

WORKDIR /app
COPY . .

RUN g++ -std=c++17 main.cpp auth.cpp storage.cpp -o attendanceServer -lpthread

CMD ["./attendanceServer"]