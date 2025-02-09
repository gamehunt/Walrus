FROM ubuntu:latest

WORKDIR /app

RUN apt-get update && apt-get install -y \
    g++ \
    libopus-dev \
    wget \
    nlohmann-json3-dev \
    cmake
RUN wget -O dpp.deb https://dl.dpp.dev/
RUN dpkg -i dpp.deb

COPY ./ /app/

RUN ./build.sh

CMD ["./build/Walrus"]
