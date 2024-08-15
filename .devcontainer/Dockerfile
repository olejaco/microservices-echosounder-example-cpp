FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

# Install basic build tools and dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    python3 \
    python3-pip \
    git \
    openjdk-11-jdk \
    default-jdk \
    default-jre \
    gradle \
    maven \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Install latest CMake
RUN wget https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-linux-x86_64.sh \
    -q -O /tmp/cmake-install.sh \
    && chmod u+x /tmp/cmake-install.sh \
    && mkdir /opt/cmake-3.26.4 \
    && /tmp/cmake-install.sh --skip-license --prefix=/opt/cmake-3.26.4 \
    && ln -s /opt/cmake-3.26.4/bin/* /usr/local/bin \
    && rm /tmp/cmake-install.sh

# Install Fast-DDS-Gen
RUN git clone --recursive https://github.com/eProsima/Fast-DDS-Gen.git -b v2.5.2 \
    && cd Fast-DDS-Gen \
    && ./gradlew assemble \
    && mkdir -p /usr/local/share/fastddsgen/java \
    && cp build/libs/fastddsgen.jar /usr/local/share/fastddsgen/java/ \
    && cp scripts/fastddsgen /usr/local/bin \
    && chmod +x /usr/local/bin/fastddsgen

# Add FastDDSGen to PATH
ENV PATH="/usr/local/bin:${PATH}"

# Install Conan
RUN pip3 install conan==1.59.0

# Create a non-root user
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && apt-get update \
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# Set up the development environment
USER $USERNAME

# Set the working directory
WORKDIR /workspace