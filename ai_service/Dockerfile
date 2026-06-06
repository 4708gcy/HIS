# Dockerfile for HIS AI Service v2.0 (LangChain + FastAPI)
# 标准 FastAPI 部署方式：uvicorn

FROM python:3.11-slim

WORKDIR /app

# Install system dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc \
    libfreetype6-dev \
    libpng-dev \
    libjpeg-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy dependency manifest and install Python packages
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy application code
COPY main.py .
COPY schemas.py .
COPY config.yaml .
COPY core/ ./core/
COPY services/ ./services/
COPY routers/ ./routers/
COPY charts.py .

# Create directories for outputs
RUN mkdir -p charts knowledge_base/files

# Expose FastAPI default port
EXPOSE 5001

# 标准 FastAPI 部署：uvicorn
CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "5001"]
