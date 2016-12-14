FROM debian:jessie

RUN apt-get update -y && \
    apt-get upgrade -y

RUN useradd -ms /bin/bash builder && \
    apt-get install -y gcc libssl-dev cmake make

USER builder
WORKDIR /home/builder

# Compilation
ADD src/* /home/builder/src/
RUN cmake -Bbuild/ -Hsrc/ && \
    make -C build/

