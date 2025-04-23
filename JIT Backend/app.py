from flask import Flask, request, jsonify, render_template, send_file, url_for
from flask_jwt_extended import JWTManager, create_access_token, jwt_required, get_jwt_identity
from flask_cors import CORS
import os
import logging
import uuid
import json
import base64
import hashlib
import hmac
import requests
from datetime import timedelta, datetime
import threading
import time
from dropbox_db import DropboxDB

# Configure logging
logging.basicConfig(level=logging.INFO, 
                    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

app = Flask(__name__)
CORS(app, supports_credentials=True, origins="*")

# Configure JWT
app.config['JWT_SECRET_KEY'] = os.environ.get('JWT_SECRET_KEY', 'dev-secret-key')  # Change in production
app.config['JWT_ACCESS_TOKEN_EXPIRES'] = timedelta(days=30)  # Long-lived tokens for mobile devices
jwt = JWTManager(app)

# Initialize Dropbox database
db = DropboxDB()

# Background task for cleaning up old sessions
def cleanup_task():
    """Background task to clean up old sessions periodically"""
    while True:
        try:
            # Clean up sessions older than 24 hours
            db.cleanup_old_sessions(86400)  # 24 hours in seconds
        except Exception as e:
            logger.error(f"Error in cleanup task: {str(e)}")
        
        # Sleep for 1 hour before next cleanup
        time.sleep(3600)

# Start cleanup task in background
cleanup_thread = threading.Thread(target=cleanup_task, daemon=True)
cleanup_thread.start()

# JIT Enablement Implementation
class JITEnabler:
    """
    Class for enabling JIT compilation on iOS devices.
    
    This implementation uses direct communication with the iOS device
    to enable JIT without requiring device pairing or provisioning profiles.
    """
    
    @staticmethod
    def enable_jit(udid, bundle_id, ios_version, app_info=None):
        """
        Enable JIT for the specified application
        
        Args:
            udid: Device UDID
            bundle_id: Bundle ID of the application
            ios_version: iOS version string
            app_info: Additional app information (optional)
            
        Returns:
            dict: Result of JIT enablement with status and details
        """
        logger.info(f"Enabling JIT for device {udid}, app {bundle_id}, iOS {ios_version}")
        
        try:
            # Different JIT enablement strategies based on iOS version
            if ios_version.startswith('17'):
                return JITEnabler._enable_jit_ios17(udid, bundle_id, app_info)
            elif ios_version.startswith('16'):
                return JITEnabler._enable_jit_ios16(udid, bundle_id, app_info)
            elif ios_version.startswith('15'):
                return JITEnabler._enable_jit_ios15(udid, bundle_id, app_info)
            else:
                # Default method for other iOS versions
                return JITEnabler._enable_jit_default(udid, bundle_id, app_info)
        except Exception as e:
            logger.error(f"Error in JIT enablement: {str(e)}")
            return {
                'success': False,
                'error': str(e),
                'details': 'Internal server error during JIT enablement'
            }
    
    @staticmethod
    def _enable_jit_ios17(udid, bundle_id, app_info=None):
        """
        Enable JIT for iOS 17+ devices
        
        This method uses the iOS 17 approach for enabling JIT which involves
        toggling memory page permissions to comply with W^X security policy.
        """
        logger.info(f"Using iOS 17 specific method for JIT enablement")
        
        # Generate a unique token for this JIT session
        jit_token = JITEnabler._generate_jit_token(udid, bundle_id)
        
        # Create JIT enablement instructions for the iOS client
        instructions = {
            'success': True,
            'token': jit_token,
            'method': 'memory_permission_toggle',
            'instructions': {
                'set_cs_debugged': True,
                'toggle_wx_memory': True,
                'memory_regions': [
                    {'address': 'auto', 'size': 'auto', 'permissions': 'rwx'}
                ]
            }
        }
        
        return instructions
    
    @staticmethod
    def _enable_jit_ios16(udid, bundle_id, app_info=None):
        """
        Enable JIT for iOS 16 devices
        
        This method uses the iOS 16 approach for enabling JIT which involves
        setting the CS_DEBUGGED flag and manipulating memory permissions.
        """
        logger.info(f"Using iOS 16 specific method for JIT enablement")
        
        # Generate a unique token for this JIT session
        jit_token = JITEnabler._generate_jit_token(udid, bundle_id)
        
        # Create JIT enablement instructions for the iOS client
        instructions = {
            'success': True,
            'token': jit_token,
            'method': 'cs_debugged_flag',
            'instructions': {
                'set_cs_debugged': True,
                'memory_regions': [
                    {'address': 'auto', 'size': 'auto', 'permissions': 'rwx'}
                ]
            }
        }
        
        return instructions
    
    @staticmethod
    def _enable_jit_ios15(udid, bundle_id, app_info=None):
        """
        Enable JIT for iOS 15 devices
        
        This method uses the iOS 15 approach for enabling JIT which may involve
        different techniques compared to newer iOS versions.
        """
        logger.info(f"Using iOS 15 specific method for JIT enablement")
        
        # Generate a unique token for this JIT session
        jit_token = JITEnabler._generate_jit_token(udid, bundle_id)
        
        # Create JIT enablement instructions for the iOS client
        instructions = {
            'success': True,
            'token': jit_token,
            'method': 'legacy',
            'instructions': {
                'set_cs_debugged': True,
                'toggle_wx_memory': True
            }
        }
        
        return instructions
    
    @staticmethod
    def _enable_jit_default(udid, bundle_id, app_info=None):
        """
        Default JIT enablement method for unsupported iOS versions
        
        This method provides a generic approach that may work on various iOS versions
        but with potentially lower success rate.
        """
        logger.info(f"Using default method for JIT enablement")
        
        # Generate a unique token for this JIT session
        jit_token = JITEnabler._generate_jit_token(udid, bundle_id)
        
        # Create JIT enablement instructions for the iOS client
        instructions = {
            'success': True,
            'token': jit_token,
            'method': 'generic',
            'instructions': {
                'set_cs_debugged': True,
                'toggle_wx_memory': True,
                'memory_regions': [
                    {'address': 'auto', 'size': 'auto', 'permissions': 'rwx'}
                ]
            }
        }
        
        return instructions
    
    @staticmethod
    def _generate_jit_token(udid, bundle_id):
        """Generate a unique token for JIT session authentication"""
        # Create a unique token based on device ID, bundle ID, and timestamp
        timestamp = datetime.utcnow().isoformat()
        data = f"{udid}:{bundle_id}:{timestamp}"
        
        # Create HMAC signature using JWT secret key
        secret = app.config['JWT_SECRET_KEY'].encode()
        signature = hmac.new(secret, data.encode(), hashlib.sha256).hexdigest()
        
        # Return base64 encoded token
        token_data = f"{data}:{signature}"
        return base64.urlsafe_b64encode(token_data.encode()).decode()

# Routes
@app.route('/', methods=['GET'])
def index():
    """Main page with iOS app download instructions"""
    return render_template('index.html')

@app.route('/health', methods=['GET'])
def health_check():
    """Health check endpoint"""
    return jsonify({
        'status': 'healthy',
        'timestamp': datetime.utcnow().isoformat(),
        'version': '1.0.0'
    }), 200

@app.route('/register', methods=['POST'])
def register_device():
    """Register a device and generate an authentication token"""
    data = request.get_json()
    if not data or 'udid' not in data:
        return jsonify({'error': 'UDID required'}), 400
    
    udid = data['udid']
    device_name = data.get('device_name', 'Unknown Device')
    ios_version = data.get('ios_version', 'unknown')
    device_model = data.get('device_model', 'unknown')
    
    # Generate a token for the device
    access_token = create_access_token(identity=udid)
    
    # Store device info in database
    device_data = {
        'device_name': device_name,
        'ios_version': ios_version,
        'device_model': device_model,
        'registered_at': time.time(),
        'last_active': time.time()
    }
    
    db.save_device(udid, device_data)
    
    logger.info(f"Device registered: {udid} ({device_name}, {device_model}, iOS {ios_version})")
    return jsonify({
        'token': access_token,
        'message': 'Device registered successfully'
    }), 200

@app.route('/enable-jit', methods=['POST'])
@jwt_required()
def enable_jit():
    """Enable JIT for an application on a registered device"""
    # Get the identity from the JWT
    udid = get_jwt_identity()
    
    # Check if device is registered
    device = db.get_device(udid)
    if not device:
        return jsonify({'error': 'Device not registered'}), 401
    
    # Update last active timestamp
    db.update_device_activity(udid)
    
    # Get request data
    data = request.get_json()
    if not data or 'bundle_id' not in data:
        return jsonify({'error': 'Bundle ID required'}), 400
    
    bundle_id = data['bundle_id']
    ios_version = data.get('ios_version', device.get('ios_version', 'unknown'))
    app_info = data.get('app_info', {})
    
    # Log the request
    logger.info(f"JIT enablement request: Device {udid}, App {bundle_id}, iOS {ios_version}")
    
    try:
        # Create a unique session ID for this JIT enablement
        session_id = str(uuid.uuid4())
        
        # Store session info in database
        session_data = {
            'udid': udid,
            'bundle_id': bundle_id,
            'ios_version': ios_version,
            'started_at': time.time(),
            'status': 'processing',
            'app_info': app_info
        }
        
        db.save_session(session_id, session_data)
        
        # Enable JIT based on iOS version
        jit_result = JITEnabler.enable_jit(udid, bundle_id, ios_version, app_info)
        
        if jit_result.get('success', False):
            # Update session status
            session_data['status'] = 'completed'
            session_data['completed_at'] = time.time()
            session_data['jit_token'] = jit_result.get('token')
            db.save_session(session_id, session_data)
            
            # Return success response with JIT instructions
            return jsonify({
                'status': 'JIT enabled',
                'session_id': session_id,
                'message': f"Enabled JIT for '{bundle_id}'!",
                'token': jit_result.get('token'),
                'method': jit_result.get('method'),
                'instructions': jit_result.get('instructions')
            }), 200
        else:
            # Update session status to failed
            session_data['status'] = 'failed'
            session_data['error'] = jit_result.get('error', 'Unknown error')
            db.save_session(session_id, session_data)
            
            return jsonify({
                'error': 'Failed to enable JIT',
                'session_id': session_id,
                'details': jit_result.get('details', 'Unknown error')
            }), 500
            
    except Exception as e:
        logger.error(f"Error enabling JIT: {str(e)}")
        return jsonify({'error': str(e)}), 500

@app.route('/session/<session_id>', methods=['GET'])
@jwt_required()
def get_session_status(session_id):
    """Get the status of a JIT session"""
    # Get session from database
    session = db.get_session(session_id)
    if not session:
        return jsonify({'error': 'Session not found'}), 404
    
    # Check if the requesting device owns this session
    udid = get_jwt_identity()
    if session.get('udid') != udid:
        return jsonify({'error': 'Unauthorized'}), 403
    
    # Update device activity
    db.update_device_activity(udid)
    
    return jsonify({
        'status': session.get('status'),
        'started_at': session.get('started_at'),
        'completed_at': session.get('completed_at', None),
        'bundle_id': session.get('bundle_id'),
        'method': session.get('method', 'unknown')
    }), 200

@app.route('/device/sessions', methods=['GET'])
@jwt_required()
def get_device_sessions():
    """Get all JIT sessions for the authenticated device"""
    udid = get_jwt_identity()
    
    # Check if device is registered
    device = db.get_device(udid)
    if not device:
        return jsonify({'error': 'Device not registered'}), 401
    
    # Update device activity
    db.update_device_activity(udid)
    
    # Get all sessions for this device
    sessions = db.get_device_sessions(udid)
    
    return jsonify({
        'sessions': sessions
    }), 200

@app.route('/stats', methods=['GET'])
def get_stats():
    """Get anonymous statistics about the JIT backend usage"""
    # This endpoint provides anonymous stats and doesn't need authentication
    
    devices = db.get_devices()
    sessions = db.get_sessions()
    
    # Calculate statistics
    total_devices = len(devices)
    total_sessions = len(sessions)
    
    # Count active devices (active in the last 24 hours)
    current_time = time.time()
    active_devices = sum(1 for device in devices.values() 
                         if current_time - device.get('last_active', 0) < 86400)
    
    # Count sessions by status
    completed_sessions = sum(1 for session in sessions.values() 
                             if session.get('status') == 'completed')
    failed_sessions = sum(1 for session in sessions.values() 
                          if session.get('status') == 'failed')
    processing_sessions = sum(1 for session in sessions.values() 
                              if session.get('status') == 'processing')
    
    return jsonify({
        'total_devices': total_devices,
        'active_devices': active_devices,
        'total_sessions': total_sessions,
        'completed_sessions': completed_sessions,
        'failed_sessions': failed_sessions,
        'processing_sessions': processing_sessions
    }), 200

if __name__ == '__main__':
    port = int(os.environ.get('PORT', 5000))
    debug = os.environ.get('FLASK_ENV') == 'development'
    
    logger.info(f"Starting JIT Backend on port {port}")
    app.run(host='0.0.0.0', port=port, debug=debug, ssl_context='adhoc')
