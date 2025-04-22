"""
Dropbox Database Module for JIT Backend

This module provides database functionality using Dropbox as a storage backend.
It handles authentication, token refresh, and CRUD operations for the JIT backend.
"""

import os
import json
import time
import logging
import threading
from typing import Dict, Any, Optional, List
import dropbox
from dropbox import DropboxOAuth2FlowNoRedirect
from dropbox.exceptions import AuthError, ApiError

# Configure logging
logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

# Dropbox API credentials from environment variables with fallback to hardcoded values
APP_KEY = os.environ.get("DROPBOX_APP_KEY", "2bi422xpd3xd962")
APP_SECRET = os.environ.get("DROPBOX_APP_SECRET", "j3yx0b41qdvfu86")
REFRESH_TOKEN = os.environ.get("DROPBOX_REFRESH_TOKEN", "RvyL03RE5qAAAAAAAAAAAVMVebvE7jDx8Okd0ploMzr85c6txvCRXpJAt30mxrKF")

# Database file paths in Dropbox
DEVICES_FILE = "/jit_backend/devices.json"
SESSIONS_FILE = "/jit_backend/sessions.json"

# Lock for thread safety
db_lock = threading.Lock()

class DropboxDB:
    """Dropbox database handler for JIT Backend"""
    
    _instance = None
    _dbx = None
    _access_token = None
    _token_expiry = 0
    _refresh_thread = None
    _refresh_interval = 3600  # 1 hour in seconds
    
    def __new__(cls):
        """Singleton pattern to ensure only one instance exists"""
        if cls._instance is None:
            cls._instance = super(DropboxDB, cls).__new__(cls)
            cls._instance._initialize()
        return cls._instance
    
    def _initialize(self):
        """Initialize the Dropbox client and start token refresh thread"""
        self._authenticate()
        
        # Create database files if they don't exist
        self._ensure_db_files_exist()
        
        # Start token refresh thread
        self._refresh_thread = threading.Thread(target=self._token_refresh_loop, daemon=True)
        self._refresh_thread.start()
    
    def _authenticate(self):
        """Authenticate with Dropbox using refresh token"""
        try:
            logger.info("Authenticating with Dropbox...")
            
            # Use refresh token to get access token
            self._dbx = dropbox.Dropbox(
                app_key=APP_KEY,
                app_secret=APP_SECRET,
                oauth2_refresh_token=REFRESH_TOKEN
            )
            
            # Test the connection
            account = self._dbx.users_get_current_account()
            logger.info(f"Successfully connected to Dropbox as {account.name.display_name}")
            
            # Set token expiry to 4 hours from now (Dropbox tokens typically last longer,
            # but we'll refresh more frequently to be safe)
            self._token_expiry = time.time() + 14400  # 4 hours
            
        except AuthError as e:
            logger.error(f"Dropbox authentication error: {str(e)}")
            raise
        except Exception as e:
            logger.error(f"Error connecting to Dropbox: {str(e)}")
            raise
    
    def _token_refresh_loop(self):
        """Background thread to refresh the access token periodically"""
        while True:
            # Sleep for the refresh interval
            time.sleep(self._refresh_interval)
            
            try:
                # Check if token needs refresh (if less than 1 hour remaining)
                if time.time() > (self._token_expiry - 3600):
                    logger.info("Refreshing Dropbox access token...")
                    self._authenticate()
                    logger.info("Dropbox token refreshed successfully")
            except Exception as e:
                logger.error(f"Error refreshing Dropbox token: {str(e)}")
    
    def _ensure_db_files_exist(self):
        """Create database files if they don't exist"""
        try:
            # Check if devices file exists
            try:
                self._dbx.files_get_metadata(DEVICES_FILE)
                logger.info(f"Database file {DEVICES_FILE} exists")
            except ApiError:
                # Create empty devices file
                logger.info(f"Creating database file {DEVICES_FILE}")
                self._dbx.files_upload(json.dumps({}).encode(), DEVICES_FILE)
            
            # Check if sessions file exists
            try:
                self._dbx.files_get_metadata(SESSIONS_FILE)
                logger.info(f"Database file {SESSIONS_FILE} exists")
            except ApiError:
                # Create empty sessions file
                logger.info(f"Creating database file {SESSIONS_FILE}")
                self._dbx.files_upload(json.dumps({}).encode(), SESSIONS_FILE)
                
        except Exception as e:
            logger.error(f"Error ensuring database files exist: {str(e)}")
            raise
    
    def _download_file(self, path: str) -> Dict[str, Any]:
        """Download and parse a JSON file from Dropbox"""
        try:
            # Download the file
            metadata, response = self._dbx.files_download(path)
            content = response.content.decode('utf-8')
            
            # Parse JSON
            return json.loads(content)
        except Exception as e:
            logger.error(f"Error downloading file {path}: {str(e)}")
            # Return empty dict if file doesn't exist or is invalid
            return {}
    
    def _upload_file(self, path: str, data: Dict[str, Any]) -> bool:
        """Upload a JSON file to Dropbox"""
        try:
            # Convert data to JSON string
            content = json.dumps(data, indent=2).encode('utf-8')
            
            # Upload the file (overwrite existing)
            self._dbx.files_upload(
                content,
                path,
                mode=dropbox.files.WriteMode.overwrite
            )
            return True
        except Exception as e:
            logger.error(f"Error uploading file {path}: {str(e)}")
            return False
    
    # Device operations
    def get_devices(self) -> Dict[str, Any]:
        """Get all registered devices"""
        with db_lock:
            return self._download_file(DEVICES_FILE)
    
    def get_device(self, udid: str) -> Optional[Dict[str, Any]]:
        """Get a specific device by UDID"""
        with db_lock:
            devices = self._download_file(DEVICES_FILE)
            return devices.get(udid)
    
    def save_device(self, udid: str, device_data: Dict[str, Any]) -> bool:
        """Save a device to the database"""
        with db_lock:
            devices = self._download_file(DEVICES_FILE)
            devices[udid] = device_data
            return self._upload_file(DEVICES_FILE, devices)
    
    def update_device_activity(self, udid: str) -> bool:
        """Update the last_active timestamp for a device"""
        with db_lock:
            devices = self._download_file(DEVICES_FILE)
            if udid in devices:
                devices[udid]['last_active'] = time.time()
                return self._upload_file(DEVICES_FILE, devices)
            return False
    
    def delete_device(self, udid: str) -> bool:
        """Delete a device from the database"""
        with db_lock:
            devices = self._download_file(DEVICES_FILE)
            if udid in devices:
                del devices[udid]
                return self._upload_file(DEVICES_FILE, devices)
            return False
    
    # Session operations
    def get_sessions(self) -> Dict[str, Any]:
        """Get all JIT sessions"""
        with db_lock:
            return self._download_file(SESSIONS_FILE)
    
    def get_session(self, session_id: str) -> Optional[Dict[str, Any]]:
        """Get a specific JIT session by ID"""
        with db_lock:
            sessions = self._download_file(SESSIONS_FILE)
            return sessions.get(session_id)
    
    def get_device_sessions(self, udid: str) -> List[Dict[str, Any]]:
        """Get all sessions for a specific device"""
        with db_lock:
            sessions = self._download_file(SESSIONS_FILE)
            device_sessions = []
            for session_id, session in sessions.items():
                if session.get('udid') == udid:
                    session_copy = session.copy()
                    session_copy['id'] = session_id
                    device_sessions.append(session_copy)
            return device_sessions
    
    def save_session(self, session_id: str, session_data: Dict[str, Any]) -> bool:
        """Save a JIT session to the database"""
        with db_lock:
            sessions = self._download_file(SESSIONS_FILE)
            sessions[session_id] = session_data
            return self._upload_file(SESSIONS_FILE, sessions)
    
    def update_session_status(self, session_id: str, status: str) -> bool:
        """Update the status of a JIT session"""
        with db_lock:
            sessions = self._download_file(SESSIONS_FILE)
            if session_id in sessions:
                sessions[session_id]['status'] = status
                return self._upload_file(SESSIONS_FILE, sessions)
            return False
    
    def delete_session(self, session_id: str) -> bool:
        """Delete a JIT session from the database"""
        with db_lock:
            sessions = self._download_file(SESSIONS_FILE)
            if session_id in sessions:
                del sessions[session_id]
                return self._upload_file(SESSIONS_FILE, sessions)
            return False
    
    def cleanup_old_sessions(self, max_age_seconds: int = 86400) -> int:
        """Clean up sessions older than the specified age (default: 24 hours)"""
        with db_lock:
            sessions = self._download_file(SESSIONS_FILE)
            current_time = time.time()
            count = 0
            
            # Find sessions to delete
            sessions_to_delete = []
            for session_id, session in sessions.items():
                if current_time - session.get('started_at', 0) > max_age_seconds:
                    sessions_to_delete.append(session_id)
            
            # Delete sessions
            for session_id in sessions_to_delete:
                del sessions[session_id]
                count += 1
            
            # Save updated sessions
            if count > 0:
                self._upload_file(SESSIONS_FILE, sessions)
                logger.info(f"Cleaned up {count} old sessions")
            
            return count