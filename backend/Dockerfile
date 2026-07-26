FROM ubuntu:22.04

RUN apt-get update && apt-get install -y g++ make libcurl4-openssl-dev

WORKDIR /app
COPY . .
RUN mkdir -p database/sessions

RUN g++ -std=c++17 main.cpp auth.cpp storage.cpp session_manager.cpp qrcodegen.cpp attendance.cpp location.cpp email_sender.cpp -o attendanceServer -lpthread -lcurl

CMD ["./attendanceServer"]