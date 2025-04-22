#!/bin/bash

# JIT Backend Setup Script

echo "Setting up JIT Backend..."

# Check if Python is installed
if ! command -v python3 &> /dev/null; then
    echo "Python 3 is not installed. Please install Python 3 and try again."
    exit 1
fi

# Check if pip is installed
if ! command -v pip3 &> /dev/null; then
    echo "pip3 is not installed. Please install pip3 and try again."
    exit 1
fi

# Create virtual environment
echo "Creating virtual environment..."
python3 -m venv venv
source venv/bin/activate

# Install dependencies
echo "Installing dependencies..."
pip install -r requirements.txt

# Create .env file from example if it doesn't exist
if [ ! -f .env ]; then
    echo "Creating .env file from .env.example..."
    cp .env.example .env
    echo "Please edit the .env file with your own values."
fi

# Generate a random JWT secret key
JWT_SECRET=$(python -c 'import secrets; print(secrets.token_hex(32))')
sed -i "s/your_jwt_secret_key_here/$JWT_SECRET/g" .env

echo "Setup complete!"
echo "To run the server, use: flask run --host=0.0.0.0 --port=5000"
echo "Or in production: gunicorn --bind 0.0.0.0:5000 --workers 4 app:app"