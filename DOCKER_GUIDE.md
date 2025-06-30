# 🐳 Docker Guide for Zappy Project

This guide explains how to run the Zappy project using Docker, providing containerized environments for development, testing, and production deployment.

## 📋 Table of Contents

- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Available Services](#available-services)
- [Docker Commands](#docker-commands)
- [Development Workflows](#development-workflows)
- [Production Deployment](#production-deployment)
- [Troubleshooting](#troubleshooting)

## 🔧 Prerequisites

Ensure you have the following installed:

```bash
# Docker Engine (version 20.10+ recommended)
sudo apt-get update
sudo apt-get install docker.io docker-compose-plugin

# Verify installation
docker --version
docker compose version
```

## 🚀 Quick Start

### 1. Build and Run All Services

```bash
# Clone and navigate to the project
cd /path/to/Zappy

# Start all services (server + AI)
docker compose up --build

# Or run in detached mode
docker compose up --build -d
```

### 2. Run Individual Components

```bash
# Run only the server
docker compose up --build zappy-server

# Run only the AI
docker compose up --build zappy-ai

# Run server and AI together
docker compose up --build zappy-server zappy-ai
```

### 3. Access the Application

- **Zappy Server**: `localhost:4242`
- **Container Logs**: `docker compose logs -f [service-name]`

## 🏗️ Available Services

Our `docker-compose.yml` defines the following services:

### 1. **zappy-server**
- **Purpose**: Main Zappy server
- **Port**: `4242:4242`
- **Build**: Custom Dockerfile
- **Command**: Runs the server with default parameters

### 2. **zappy-ai** 
- **Purpose**: AI client that connects to the server
- **Dependencies**: Requires `zappy-server`
- **Build**: Same container with AI runtime
- **Command**: Runs AI clients

### 3. **zappy-dev** (Development)
- **Purpose**: Development environment with all tools
- **Volumes**: Source code mounted for live editing
- **Includes**: Build tools, debuggers, coverage tools

## 🐳 Docker Commands

### Basic Operations

```bash
# Build all services
docker compose build

# Start services
docker compose up

# Start specific service
docker compose up zappy-server

# Run in background
docker compose up -d

# Stop all services
docker compose down

# Stop and remove volumes
docker compose down -v

# View logs
docker compose logs -f
```

### Development Commands

```bash
# Build with no cache
docker compose build --no-cache

# Run interactive shell in container
docker compose run --rm zappy-dev bash

# Execute command in running container
docker compose exec zappy-server bash

# View resource usage
docker stats
```

### Debugging Commands

```bash
# Inspect container
docker compose exec zappy-server ps aux

# Check container networking
docker network ls
docker network inspect zappy_default

# View container details
docker compose ps -a
docker inspect [container-name]
```

## 🔄 Development Workflows

### Workflow 1: Live Development

```bash
# Start development environment with volume mounts
docker compose -f docker-compose.yml -f docker-compose.dev.yml up

# This mounts your source code into the container
# Changes are reflected immediately without rebuilds
```

### Workflow 2: Test-Driven Development

```bash
# Run tests in container
docker compose run --rm zappy-dev ./build.sh --test

# Run coverage analysis
docker compose run --rm zappy-dev ./coverage.sh

# Run specific test suite
docker compose run --rm zappy-dev ctest --test-dir build
```

### Workflow 3: Multi-Architecture Builds

```bash
# Build for different platforms
docker buildx build --platform linux/amd64,linux/arm64 -t zappy:latest .

# Create builder instance
docker buildx create --name multiarch --use
docker buildx inspect --bootstrap
```

## 🎯 Sample Usage Scenarios

### Scenario 1: Demo Game Session

```bash
# Terminal 1: Start the server
docker compose up zappy-server

# Terminal 2: Run AI teams
docker compose run --rm zappy-ai python src/main_ai.py \
  --host zappy-server --port 4242 --name TeamA

# Terminal 3: Run competing AI team
docker compose run --rm zappy-ai python src/main_ai.py \
  --host zappy-server --port 4242 --name TeamB
```

### Scenario 2: Development Testing

```bash
# Run complete test suite
docker compose run --rm zappy-dev ./build.sh --test --coverage

# Run only server tests
docker compose run --rm zappy-dev ctest --test-dir build \
  --tests-regex "server_.*"

# Run only AI tests
docker compose run --rm zappy-dev ctest --test-dir build \
  --tests-regex "ai_.*"
```

### Scenario 3: Performance Testing

```bash
# Start server with performance monitoring
docker compose up zappy-server

# Run multiple AI clients for load testing
for i in {1..10}; do
  docker compose run -d --rm zappy-ai python src/main_ai.py \
    --host zappy-server --port 4242 --name "Team$i"
done

# Monitor resource usage
docker stats
```

## 🚀 Production Deployment

### Using Docker Compose (Recommended)

```bash
# Production configuration
docker compose -f docker-compose.yml -f docker-compose.prod.yml up -d

# Health checks
docker compose ps
docker compose logs -f
```

### Using Docker Swarm

```bash
# Initialize swarm
docker swarm init

# Deploy stack
docker stack deploy -c docker-compose.yml zappy

# Check services
docker service ls
docker service logs zappy_zappy-server
```

### Using Kubernetes

```bash
# Convert docker-compose to Kubernetes manifests (using kompose)
kompose convert

# Apply to cluster
kubectl apply -f .

# Check status
kubectl get pods
kubectl logs -f deployment/zappy-server
```

## 🔧 Environment Configuration

### Environment Variables

Create a `.env` file for customization:

```bash
# .env file
ZAPPY_PORT=4242
ZAPPY_WIDTH=20
ZAPPY_HEIGHT=20
ZAPPY_TEAMS=TeamA,TeamB
ZAPPY_CLIENTS=4
ZAPPY_TIME=100

# AI Configuration
AI_DEBUG=true
AI_LOG_LEVEL=INFO
AI_STRATEGY=aggressive
```

### Custom Configuration

```bash
# Override compose file
cp docker-compose.yml docker-compose.override.yml

# Edit docker-compose.override.yml for custom settings
# This file is automatically used by docker compose
```

## 📊 Monitoring and Logging

### Container Logs

```bash
# View all logs
docker compose logs

# Follow specific service logs
docker compose logs -f zappy-server

# View logs with timestamps
docker compose logs -t

# Tail last 100 lines
docker compose logs --tail=100
```

### Health Monitoring

```bash
# Check container health
docker compose ps

# Resource usage
docker stats $(docker compose ps -q)

# Container inspection
docker compose exec zappy-server top
```

## 🐛 Troubleshooting

### Common Issues

#### Issue 1: Port Already in Use
```bash
# Check what's using the port
lsof -i :4242

# Kill the process or change port in docker-compose.yml
```

#### Issue 2: Build Failures
```bash
# Clean build cache
docker system prune -a

# Rebuild from scratch
docker compose build --no-cache
```

#### Issue 3: Container Won't Start
```bash
# Check logs
docker compose logs [service-name]

# Run container interactively
docker compose run --rm [service-name] bash
```

#### Issue 4: Network Issues
```bash
# Check network connectivity
docker network ls
docker network inspect zappy_default

# Test connectivity between containers
docker compose exec zappy-server ping zappy-ai
```

### Performance Optimization

```bash
# Limit resource usage
docker compose --compatibility up

# Add resource limits to docker-compose.yml:
services:
  zappy-server:
    deploy:
      resources:
        limits:
          cpus: '0.5'
          memory: 512M
```

## 🔐 Security Considerations

### Container Security

```bash
# Run containers as non-root user
# (Already configured in our Dockerfile)

# Scan images for vulnerabilities
docker scout cves [image-name]

# Use specific image tags, not 'latest'
# (Already implemented in our setup)
```

### Network Security

```bash
# Use custom networks
docker network create --driver bridge zappy-network

# Expose only necessary ports
# (Already configured in docker-compose.yml)
```

## 📈 Advanced Usage

### Multi-Stage Builds

Our Dockerfile uses multi-stage builds for optimization:
- **Builder stage**: Compiles the application
- **Runtime stage**: Minimal runtime environment
- **Development stage**: Full development tools

### Docker Buildx

```bash
# Enable advanced build features
docker buildx create --use

# Build with build arguments
docker buildx build --build-arg BUILD_TYPE=Release .
```

### Container Orchestration

```bash
# Scale services
docker compose up --scale zappy-ai=3

# Rolling updates
docker compose up -d --force-recreate zappy-server
```

## 🎯 Best Practices

### Development
- Use volume mounts for active development
- Run tests in containers for consistency
- Use specific image tags in production

### Production
- Use multi-stage builds for smaller images
- Implement proper health checks
- Configure log rotation
- Use secrets for sensitive data

### Security
- Run containers as non-root users
- Scan images regularly
- Keep base images updated
- Use minimal base images

---

## 📚 Additional Resources

- [Docker Documentation](https://docs.docker.com/)
- [Docker Compose Reference](https://docs.docker.com/compose/)
- [Dockerfile Best Practices](https://docs.docker.com/develop/dev-best-practices/)
- [Container Security](https://docs.docker.com/engine/security/)

---

**🎉 Happy Containerizing! 🐳**

For more help, check the project's main documentation or create an issue in the repository.
