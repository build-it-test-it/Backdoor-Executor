# JIT Backend for iOS Apps

A Flask-based backend for enabling Just-In-Time (JIT) compilation in sideloaded iOS applications. This backend is designed to be deployed on Render.com and provides a secure, scalable solution for enabling JIT without requiring device pairing or provisioning profiles.

## Features

- **Secure JIT Enablement**: Enables JIT compilation for iOS apps by toggling memory page permissions to comply with iOS's W^X security policy
- **Device Registration and Authentication**: Secure JWT-based authentication for iOS devices
- **Dropbox Database Integration**: Persistent storage using Dropbox API with automatic token refresh for reliable database access without requiring a traditional database server
- **iOS Version-Specific Strategies**: Different JIT enablement strategies optimized for iOS 15, 16, and 17+
- **Secure Communication**: HTTPS and token-based authentication to secure all communications
- **Render.com Deployment Ready**: Configured for easy deployment on Render.com
- **Monitoring and Statistics**: Anonymous usage statistics and detailed logging

## Setup

1. Install the required dependencies:
   ```
   pip install -r requirements.txt
   ```

2. Run the server:
   ```
   python app.py
   ```

## API Endpoints

### Authentication

- `POST /register`: Register a device and get an authentication token
  - Request: `{ "udid": "device-udid", "device_name": "iPhone", "ios_version": "17.0", "device_model": "iPhone15,2" }`
  - Response: `{ "token": "jwt-token", "message": "Device registered successfully" }`

### JIT Enablement

- `POST /enable-jit`: Enable JIT for an application (requires authentication)
  - Request: `{ "bundle_id": "com.example.app", "ios_version": "17.0" }`
  - Response: `{ "status": "JIT enabled", "session_id": "uuid", "message": "Enabled JIT for 'com.example.app'!", "token": "jit-token", "method": "memory_permission_toggle", "instructions": {...} }`

### Session Management

- `GET /session/<session_id>`: Get the status of a JIT session (requires authentication)
  - Response: `{ "status": "completed", "started_at": 1650000000, "completed_at": 1650000010, "bundle_id": "com.example.app", "method": "memory_permission_toggle" }`

- `GET /device/sessions`: Get all JIT sessions for the authenticated device (requires authentication)
  - Response: `{ "sessions": [...] }`

### Monitoring

- `GET /health`: Health check endpoint
  - Response: `{ "status": "healthy", "timestamp": "2023-04-22T12:00:00", "version": "1.0.0" }`

- `GET /stats`: Get anonymous statistics about the JIT backend usage
  - Response: `{ "total_devices": 10, "active_devices": 5, "total_sessions": 100, "completed_sessions": 90, "failed_sessions": 5, "processing_sessions": 5 }`

## Deployment

### Local Development

1. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```

2. Run the development server:
   ```bash
   export FLASK_ENV=development
   export FLASK_APP=app.py
   flask run --host=0.0.0.0 --port=5000
   ```

### Docker Deployment

1. Build the Docker image:
   ```bash
   docker build -t jit-backend .
   ```

2. Run the container:
   ```bash
   docker run -d -p 5000:5000 \
     -e JWT_SECRET_KEY=your_secret_key \
     -e DROPBOX_APP_KEY=your_dropbox_app_key \
     -e DROPBOX_APP_SECRET=your_dropbox_app_secret \
     -e DROPBOX_REFRESH_TOKEN=your_dropbox_refresh_token \
     jit-backend
   ```

### Render.com Deployment

1. Fork this repository to your GitHub account

2. Connect your GitHub repository to Render.com

3. Create a new Web Service with the following settings:
   - Environment: Docker
   - Dockerfile Path: `./JIT Backend/Dockerfile`
   - Environment Variables:
     - `JWT_SECRET_KEY`: (Generate a secure random string)
     - `FLASK_ENV`: production
     - `DROPBOX_APP_KEY`: Your Dropbox app key
     - `DROPBOX_APP_SECRET`: Your Dropbox app secret
     - `DROPBOX_REFRESH_TOKEN`: Your Dropbox refresh token

4. Deploy the service

## Security Considerations

- Always use HTTPS in production
- Generate a strong JWT secret key
- Keep your Dropbox credentials secure
- Implement rate limiting in production environments
- Monitor logs for suspicious activity

## iOS Client Integration

To integrate with this backend from your iOS app:

1. Register your device with the backend
2. Store the JWT token securely
3. When JIT is needed, call the `/enable-jit` endpoint
4. Follow the instructions returned by the backend to enable JIT in your app
5. Implement the memory permission toggling based on the instructions

## Dropbox Database Implementation

The backend uses Dropbox as a database to store device and session information:

1. **Automatic Token Refresh**: The system automatically refreshes the Dropbox access token to maintain continuous database connectivity
2. **Thread-Safe Operations**: All database operations are thread-safe using a global lock
3. **Persistent Storage**: Device registrations, JIT sessions, and usage statistics are stored in JSON files on Dropbox
4. **Automatic Cleanup**: A background thread periodically cleans up old sessions to prevent database bloat
5. **Error Handling**: Comprehensive error handling ensures database operations are reliable

### Database Files

- `/jit_backend/devices.json`: Stores device registration information
- `/jit_backend/sessions.json`: Stores JIT session data

## Troubleshooting

If you encounter issues with JIT enablement:

1. Check the logs for detailed error messages
2. Verify that your device is properly registered
3. Ensure your JWT token is valid and not expired
4. Confirm that the bundle ID is correct and the app is running on the device
5. Check the Dropbox connection status using the `/health` endpoint
