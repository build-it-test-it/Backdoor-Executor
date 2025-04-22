FROM python:3.10-slim

WORKDIR /app

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    libssl-dev \
    libffi-dev \
    python3-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Copy application code first (including requirements.txt)
COPY . .

# Install dependencies
RUN pip install --no-cache-dir -r requirements.txt

# Set environment variables
ENV PYTHONUNBUFFERED=1
ENV FLASK_APP=app.py
ENV PYTHONDONTWRITEBYTECODE=1

# Use the PORT environment variable provided by Render
ENV PORT=10000

# Expose port
EXPOSE ${PORT}

# Run the application with Gunicorn
CMD gunicorn --bind 0.0.0.0:${PORT} --workers 4 --timeout 120 --access-logfile - --error-logfile - app:app
