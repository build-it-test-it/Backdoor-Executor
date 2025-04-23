#!/usr/bin/env python3
"""
Script to create a QR code for a shortcut download URL.
"""

import argparse
import os
import sys
import qrcode
from qrcode.image.pil import PilImage

def create_qr_code(url, output_path):
    """
    Create a QR code for the given URL and save it to the output path.
    """
    try:
        # Create QR code instance
        qr = qrcode.QRCode(
            version=1,
            error_correction=qrcode.constants.ERROR_CORRECT_H,
            box_size=10,
            border=4,
        )
        
        # Add data to the QR code
        qr.add_data(url)
        qr.make(fit=True)
        
        # Create an image from the QR code
        img = qr.make_image(fill_color="black", back_color="white")
        
        # Save the image
        img.save(output_path)
        
        print(f"QR code saved to {output_path}")
        return True
    
    except Exception as e:
        print(f"Error creating QR code: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(description="Create a QR code for a shortcut download URL")
    parser.add_argument("--shortcut-path", required=True, help="Path to the signed shortcut file")
    parser.add_argument("--output", required=True, help="Output QR code image file")
    parser.add_argument("--url", required=True, help="URL to encode in the QR code")
    
    args = parser.parse_args()
    
    if not os.path.exists(args.shortcut_path):
        print(f"Error: Shortcut file {args.shortcut_path} does not exist")
        sys.exit(1)
    
    # Create the QR code
    if create_qr_code(args.url, args.output):
        print("QR code created successfully")
    else:
        print("Failed to create QR code")
        sys.exit(1)

if __name__ == "__main__":
    main()