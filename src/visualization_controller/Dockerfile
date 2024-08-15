# Build stage
FROM ubuntu:20.04 AS builder

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

# Copy the project files
COPY . /app
WORKDIR /app

# Debug: Print contents of the directory
RUN ls -la

# Debug: Print contents of CMakeLists.txt
RUN cat CMakeLists.txt

# Debug: Print contents of conanfile.txt
RUN cat conanfile.txt

# Install dependencies with Conan
RUN conan install . --build=missing -if build -pr:h=default -pr:b=default

# Debug: Print contents of the build directory
RUN ls -la build

# Debug: Print contents of conan_paths.cmake
RUN cat build/conan_paths.cmake

# Configure and build the project
RUN cd build \
    && cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
                -DCMAKE_PREFIX_PATH="/root/.conan/data/fast-dds/2.10.1/_/_/package/e4e52682de95d540f84889835e0eb577e7cc608d" \
                -DCMAKE_BUILD_TYPE=Release \
    && cmake --build .

# Runtime stage
FROM ubuntu:20.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libtinyxml2-dev \
    openjdk-11-jre \
    default-jre \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Copy the built executables and necessary libraries
COPY --from=builder /app/build/visualization_controller /app/
COPY --from=builder /app/build/*.so* /usr/local/lib/
COPY --from=builder /usr/local/bin/fastdds_*.sh /usr/local/bin/
COPY --from=builder /usr/local/bin/fastddsgen /usr/local/bin/

# Set the library path
ENV LD_LIBRARY_PATH=/usr/local/lib

WORKDIR /app
CMD ["./visualization_controller"]