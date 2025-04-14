# AI Models for Roblox Executor

This directory contains the machine learning models used by the executor's AI system. These models are designed to work entirely offline with no external dependencies.

## Model List

1. **script_assistant_lite.mlmodel** - Core assistant model for understanding user requests
2. **script_generator.mlmodel** - Script generation model for creating Lua code
3. **debug_analyzer.mlmodel** - Model for debugging and analyzing scripts
4. **pattern_recognition.mlmodel** - Model for recognizing Byfron patterns

## Notes for Developers

### Model Specifications

- All models are optimized for iOS 15+ devices
- Models are compressed to minimize size while maintaining functionality
- Each model is designed to work with limited memory resources
- Models are Core ML compatible

### Using Custom Models

You can replace these models with your own trained versions. Make sure:

1. The model file names match exactly as listed above
2. The models are saved in Core ML format (.mlmodel)
3. Input and output specifications match the original models

### Fallback Mechanism

The AI system includes a comprehensive fallback mechanism that will automatically use rule-based approaches if models can't be loaded. This ensures functionality even on devices with limited resources.

### Model Loading Order

Models are loaded in a specific order to optimize memory usage:

1. **script_assistant_lite.mlmodel** (loaded first, essential)
2. **script_generator.mlmodel** (loaded second)
3. **debug_analyzer.mlmodel** (loaded as needed)
4. **pattern_recognition.mlmodel** (loaded as needed)

Models 3 and 4 are only loaded if the device has sufficient memory available.
