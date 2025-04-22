# JIT Backend Deployment Guide

This guide provides instructions for deploying the JIT Backend to various environments, with a focus on Render.com deployment.

## Prerequisites

- Python 3.8+
- pip
- A server or cloud platform to host the backend
- Dropbox account with API credentials
- (Optional) Docker for containerized deployment

## Environment Variables

The following environment variables should be set for proper operation:

- `JWT_SECRET_KEY`: Secret key for JWT token generation and validation
- `FLASK_ENV`: Set to `development` for local testing or `production` for production deployment
- `DROPBOX_APP_KEY`: Your Dropbox application key
- `DROPBOX_APP_SECRET`: Your Dropbox application secret
- `DROPBOX_REFRESH_TOKEN`: Your Dropbox refresh token

## Local Deployment for Testing

1. Clone the repository
2. Install dependencies:
   ```bash
   cd "JIT Backend"
   pip install -r requirements.txt
   ```
3. Run the development server:
   ```bash
   export FLASK_ENV=development
   export FLASK_APP=app.py
   export JWT_SECRET_KEY=your_secret_key
   export DROPBOX_APP_KEY=your_dropbox_app_key
   export DROPBOX_APP_SECRET=your_dropbox_app_secret
   export DROPBOX_REFRESH_TOKEN=your_dropbox_refresh_token
   flask run --host=0.0.0.0 --port=5000
   ```

## Docker Deployment

1. Build the Docker image:
   ```bash
   cd "JIT Backend"
   docker build -t jit-backend .
   ```

2. Run the container:
   ```bash
   docker run -d -p 5000:5000 \
     -e JWT_SECRET_KEY=your_secret_key \
     -e FLASK_ENV=production \
     -e DROPBOX_APP_KEY=your_dropbox_app_key \
     -e DROPBOX_APP_SECRET=your_dropbox_app_secret \
     -e DROPBOX_REFRESH_TOKEN=your_dropbox_refresh_token \
     jit-backend
   ```

## Render.com Deployment

Render.com is the recommended deployment platform for this backend.

### Using render.yml (Recommended)

1. Fork this repository to your GitHub account
2. Connect your GitHub repository to Render.com
3. Render will automatically detect the `render.yml` file and create the service
4. Review and adjust the environment variables as needed

### Manual Setup

1. Create a new Web Service on Render.com
2. Connect your GitHub repository
3. Configure the service:
   - Environment: Docker
   - Dockerfile Path: `./JIT Backend/Dockerfile`
   - Environment Variables:
     - `JWT_SECRET_KEY`: (Generate a secure random string)
     - `FLASK_ENV`: production
     - `DROPBOX_APP_KEY`: Your Dropbox app key
     - `DROPBOX_APP_SECRET`: Your Dropbox app secret
     - `DROPBOX_REFRESH_TOKEN`: Your Dropbox refresh token
4. Deploy the service

## Dropbox Setup

To set up Dropbox for database storage:

1. Create a Dropbox account if you don't have one
2. Go to the [Dropbox Developer Console](https://www.dropbox.com/developers/apps)
3. Create a new app with the following settings:
   - API: Dropbox API
   - Access type: Full Dropbox
   - Name: JIT Backend (or any name you prefer)
4. Note your App Key and App Secret
5. Generate an access token and then convert it to a refresh token
6. Use these credentials in your environment variables

## Security Considerations

1. **JWT Secret Key**: Always use a strong, unique JWT secret key in production.
2. **HTTPS**: Ensure your deployment uses HTTPS to encrypt communication between iOS devices and the backend.
3. **Rate Limiting**: Consider implementing rate limiting to prevent abuse.
4. **Monitoring**: Set up monitoring and logging to track usage and detect issues.
5. **Dropbox Security**: Keep your Dropbox credentials secure and consider rotating them periodically.

## Configuring iOS Apps

After deploying the backend, you need to configure your iOS app to use it:

1. Implement the device registration flow in your iOS app
2. Store the JWT token securely in the iOS keychain
3. When JIT is needed, call the `/enable-jit` endpoint with the appropriate parameters
4. Implement the JIT enablement logic in your iOS app based on the instructions returned by the backend

## Scaling Considerations

The backend is designed to be lightweight and scalable. For higher traffic:

1. Increase the number of Gunicorn workers in the Dockerfile
2. Consider using a more robust database solution for very high traffic
3. Implement caching for frequently accessed data
4. Set up a CDN if serving static assets

## Troubleshooting

### Common Issues

1. **Connection Errors**: Ensure your server is accessible from the internet and that any firewalls allow traffic on port 5000 (or your configured port).
2. **Authentication Errors**: Check that the JWT token is being properly generated and validated.
3. **Dropbox Errors**: Verify that your Dropbox credentials are correct and that the refresh token is valid.
4. **JIT Enablement Failures**: Check the logs for detailed error messages about JIT enablement failures.

### Logs

Check the server logs for detailed error information:

```bash
# For Docker
docker logs <container_id>

# For Render.com
Access the logs through the Render.com dashboard
```

## Updating the Backend

When updating the backend:

1. Make your changes to the code
2. Test locally
3. Push changes to your GitHub repository
4. Render.com will automatically deploy the updated version (if auto-deploy is enabled)
5. Monitor logs to ensure the update was successful

## Support and Maintenance

Regular maintenance tasks:

1. Monitor Dropbox token refresh to ensure database connectivity
2. Check for security updates to dependencies
3. Review logs for error patterns
4. Clean up old sessions and unused data periodically
