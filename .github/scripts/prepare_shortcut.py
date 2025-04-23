#!/usr/bin/env python3
"""
Script to prepare a shortcut for signing by updating the backend URL.
"""

import argparse
import json
import os
import sys

def update_backend_url(shortcut_data, backend_url):
    """
    Update the backend URL in the shortcut data.
    """
    # Update the default value in the import questions
    for question in shortcut_data.get("WFWorkflowImportQuestions", []):
        if question.get("WFWorkflowImportQuestionParameterKey") == "backendURL":
            question["WFWorkflowImportQuestionDefaultValue"] = backend_url
    
    # Update any URL actions in the shortcut that reference the backend
    for action in shortcut_data.get("WFWorkflowActions", []):
        # Check for URL actions
        if action.get("WFWorkflowActionIdentifier") == "is.workflow.actions.url":
            parameters = action.get("WFWorkflowActionParameters", {})
            url = parameters.get("WFURLActionURL", "")
            
            # If the URL contains the placeholder or old backend URL, update it
            if "{{backendURL}}" in url or "https://your-jit-backend.onrender.com" in url:
                parameters["WFURLActionURL"] = url.replace(
                    "{{backendURL}}", backend_url
                ).replace(
                    "https://your-jit-backend.onrender.com", backend_url
                )
        
        # Check for Get Contents of URL actions
        elif action.get("WFWorkflowActionIdentifier") == "is.workflow.actions.downloadurl":
            parameters = action.get("WFWorkflowActionParameters", {})
            url = parameters.get("WFURL", "")
            
            # If the URL contains the placeholder or old backend URL, update it
            if "{{backendURL}}" in url or "https://your-jit-backend.onrender.com" in url:
                parameters["WFURL"] = url.replace(
                    "{{backendURL}}", backend_url
                ).replace(
                    "https://your-jit-backend.onrender.com", backend_url
                )
    
    return shortcut_data

def main():
    parser = argparse.ArgumentParser(description="Prepare a shortcut for signing")
    parser.add_argument("--input", required=True, help="Input shortcut JSON file")
    parser.add_argument("--output", required=True, help="Output shortcut JSON file")
    parser.add_argument("--backend-url", required=True, help="Backend URL to use in the shortcut")
    
    args = parser.parse_args()
    
    if not os.path.exists(args.input):
        print(f"Error: Input file {args.input} does not exist")
        sys.exit(1)
    
    try:
        with open(args.input, "r") as f:
            shortcut_data = json.load(f)
        
        updated_shortcut = update_backend_url(shortcut_data, args.backend_url)
        
        with open(args.output, "w") as f:
            json.dump(updated_shortcut, f, indent=2)
        
        print(f"Shortcut prepared and saved to {args.output}")
    
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()