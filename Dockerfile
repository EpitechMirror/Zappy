# Multi-stage Dockerfile for Zappy Project
# Professional Docker setup with optimized layers

# Base image with all required tools
FROM ubuntu:22.04 as base

# Set non-interactive frontend for apt
ENV DEBIAN_FRONTEND=noninteractive

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    make \
    cmake \
    python3 \
    python3-pip \
    python3-venv \
    pkg-config \
    libssl-dev \
    git \
    wget \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy project files
COPY . .

# Make scripts executable
RUN chmod +x build.sh

# Build stage
FROM base as builder

# Install development dependencies
RUN apt-get update && apt-get install -y \
    valgrind \
    lcov \
    cppcheck \
    clang-format \
    && rm -rf /var/lib/apt/lists/*

# Install Raylib dependencies for GUI
RUN apt-get update && apt-get install -y \
    libasound2-dev \
    mesa-common-dev \
    libx11-dev \
    libxrandr-dev \
    libxi-dev \
    xorg-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

# Build the project
RUN ./build.sh build

# Runtime stage
FROM ubuntu:22.04 as runtime

# Install minimal runtime dependencies
RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    libc6 \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# Install GUI runtime dependencies
RUN apt-get update && apt-get install -y \
    libasound2 \
    libx11-6 \
    libxrandr2 \
    libxi6 \
    libgl1-mesa-glx \
    libglu1-mesa \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN useradd -m -s /bin/bash zappy

# Set working directory
WORKDIR /app

# Copy built binaries and resources
COPY --from=builder /app/dist/ ./dist/
COPY --from=builder /app/resources/ ./resources/
COPY --from=builder /app/ai/ ./ai/
COPY --from=builder /app/docs/ ./docs/

# Copy configuration files
COPY docker-entrypoint.sh ./
RUN chmod +x docker-entrypoint.sh

# Change ownership to zappy user
RUN chown -R zappy:zappy /app

# Switch to non-root user
USER zappy

# Expose default port
EXPOSE 8080

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1

# Default command
ENTRYPOINT ["./docker-entrypoint.sh"]
CMD ["server"]

# Development stage
FROM builder as development

# Install additional development tools
RUN apt-get update && apt-get install -y \
    gdb \
    strace \
    ltrace \
    vim \
    htop \
    && rm -rf /var/lib/apt/lists/*

# Set development environment
ENV DEVELOPMENT=true
ENV BUILD_TYPE=Debug

# Keep container running for development
CMD ["tail", "-f", "/dev/null"]

# Testing stage
FROM builder as testing

# Run all tests
RUN ./build.sh ci

# Create test results
RUN echo "Tests completed successfully" > /app/test_results.txt

# CI stage
FROM builder as ci

# Install CI-specific tools
RUN apt-get update && apt-get install -y \
    curl \
    jq \
    && rm -rf /var/lib/apt/lists/*

# Run CI pipeline
RUN ./build.sh ci

# Generate reports
RUN mkdir -p /app/reports
RUN if [ -d "/app/coverage" ]; then cp -r /app/coverage /app/reports/; fi

# Production stage
FROM runtime as production

# Production-specific configurations
ENV NODE_ENV=production
ENV BUILD_TYPE=Release

# Copy production assets
COPY --from=builder /app/dist/ ./dist/
COPY --from=builder /app/resources/ ./resources/

# Final cleanup
RUN apt-get autoremove -y && apt-get clean
