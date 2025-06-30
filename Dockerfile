# Multi-stage Dockerfile for Zappy project
# Professional production-ready container

# Stage 1: Build dependencies
FROM ubuntu:22.04 AS dependencies

# Install system dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    libraylib-dev \
    python3 \
    python3-pip \
    python3-venv \
    git \
    && rm -rf /var/lib/apt/lists/*

# Stage 2: Build stage
FROM dependencies AS builder

WORKDIR /app
COPY . .

# Build the project
RUN ./build.sh --no-tests
RUN cmake --build build --config Release -j$(nproc)

# Stage 3: AI Environment
FROM python:3.9-slim AS ai-env

WORKDIR /app/ai
COPY ai/requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt
COPY ai/ .

# Stage 4: Server runtime
FROM ubuntu:22.04 AS server

RUN apt-get update && apt-get install -y \
    libpthread-stubs0-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /app/build/bin/zappy_server /usr/local/bin/
COPY --from=builder /app/resources/ /app/resources/

EXPOSE 4242
CMD ["zappy_server"]

# Stage 5: GUI runtime
FROM ubuntu:22.04 AS gui

RUN apt-get update && apt-get install -y \
    libraylib4 \
    libgl1-mesa-glx \
    libxrandr2 \
    libxinerama1 \
    libxcursor1 \
    libxi6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /app/build/bin/zappy_gui /usr/local/bin/
COPY --from=builder /app/resources/ /app/resources/

CMD ["zappy_gui"]

# Stage 6: AI runtime
FROM ai-env AS ai

WORKDIR /app/ai
CMD ["python3", "src/main_ai.py"]

# Stage 7: Development environment
FROM dependencies AS dev

# Install additional development tools
RUN apt-get update && apt-get install -y \
    gdb \
    valgrind \
    clang-format \
    clang-tidy \
    cppcheck \
    lcov \
    doxygen \
    && rm -rf /var/lib/apt/lists/*

# Install Python development dependencies
RUN pip3 install --no-cache-dir \
    black \
    isort \
    flake8 \
    mypy \
    pre-commit

WORKDIR /app

# Default command for development
CMD ["/bin/bash"]
