# How JIT Enablement Works on iOS

This document provides a comprehensive explanation of how the JIT Enabler solution works to enable Just-In-Time (JIT) compilation for iOS apps without requiring code modifications to the apps themselves.

## Understanding iOS JIT Restrictions

iOS enforces a security policy called W^X (Write XOR Execute), which means memory pages can either be writable OR executable, but not both simultaneously. This policy prevents code from being modified and then executed, which is a common attack vector.

JIT compilation requires the ability to:
1. Write compiled code to memory
2. Execute that compiled code

This creates a challenge under iOS's W^X policy, as JIT engines need to:
1. Write to memory pages (making them writable)
2. Then execute from those same pages (making them executable)

## How Our Solution Enables JIT

Our solution uses a combination of techniques to enable JIT compilation:

### 1. Backend Server Approach

Instead of modifying apps, we use a backend server that:
- Registers iOS devices
- Provides device-specific JIT enablement instructions
- Handles authentication and security

### 2. iOS Shortcut as the Client

The iOS Shortcut serves as a user-friendly client that:
- Communicates with the backend
- Applies the JIT enablement instructions
- Launches the target app

### 3. Version-Specific JIT Enablement

Different iOS versions require different approaches:

#### iOS 15 Method
- Sets the CS_DEBUGGED flag on the process
- Uses legacy methods to toggle memory permissions

#### iOS 16 Method
- Sets the CS_DEBUGGED flag
- Uses a more direct approach to memory permission management

#### iOS 17+ Method
- Uses memory permission toggling to comply with the stricter W^X policy
- Leverages specific memory region marking

## Technical Deep Dive

### Memory Permission Toggling

The key to enabling JIT is toggling memory permissions between writable and executable states:

1. When the JIT compiler needs to generate code:
   - Memory pages are marked as writable (but not executable)
   - The JIT compiler writes the compiled code to these pages

2. When the code needs to be executed:
   - Memory pages are marked as executable (but not writable)
   - The app can now execute the JIT-compiled code

3. This toggling happens rapidly and is coordinated by the JIT engine

### CS_DEBUGGED Flag

On iOS, processes have code signing flags that control what they can do:

1. The CS_DEBUGGED flag indicates a process is being debugged
2. When this flag is set, certain restrictions are relaxed
3. Our solution leverages this to enable the memory permission changes needed for JIT

### Communication Flow

Here's the complete flow of how JIT enablement works:

1. **Device Registration**:
   - The iOS Shortcut gets device information
   - It sends this to the backend to register the device
   - The backend generates and returns a JWT token

2. **App Selection**:
   - User selects which app needs JIT
   - The shortcut identifies the app's bundle ID

3. **JIT Enablement Request**:
   - The shortcut sends the bundle ID to the backend
   - The backend determines the iOS version
   - The backend generates appropriate JIT enablement instructions

4. **Applying JIT Instructions**:
   - The shortcut receives the instructions
   - It applies them to the target app
   - This includes setting the CS_DEBUGGED flag and configuring memory permissions

5. **App Launch**:
   - The shortcut launches the target app
   - The app now has JIT compilation enabled

## Why This Works Without App Modifications

This approach works without modifying apps because:

1. **System-Level Changes**: The changes are made at the process/memory level, not in the app's code
2. **Runtime Modifications**: The modifications happen at runtime, after the app is launched
3. **Transparent Integration**: The app's JIT engine works normally once the memory permissions are properly set

## Security Considerations

Our solution maintains security by:

1. **Authentication**: Using JWT tokens to ensure only authorized devices can request JIT enablement
2. **HTTPS**: Encrypting all communication between the shortcut and backend
3. **Minimal Permissions**: Only modifying the specific permissions needed for JIT
4. **Temporary Changes**: The changes don't persist beyond the app's current session

## Limitations

While effective, this approach has some limitations:

1. **Session-Based**: JIT enablement may need to be reapplied when the app is restarted
2. **iOS Updates**: Apple may change how memory permissions work in future iOS versions
3. **App Compatibility**: Some apps may implement additional security measures that interfere with this approach

## Comparison to Other Methods

### vs. Custom App Development
- **Advantage**: No need to modify or rebuild apps
- **Disadvantage**: May be less persistent than built-in solutions

### vs. Jailbreaking
- **Advantage**: Works on non-jailbroken devices
- **Disadvantage**: Less powerful than full jailbreak solutions

### vs. Developer Mode
- **Advantage**: Works for sideloaded apps without developer accounts
- **Disadvantage**: Requires more user interaction

## Technical FAQ

### Q: Does this violate Apple's security model?
A: This solution works within the constraints of iOS's security model by using legitimate debugging flags and memory permission mechanisms. It doesn't exploit vulnerabilities or bypass security measures.

### Q: Will this work with all apps?
A: It works with most apps that use standard JIT compilation techniques, including emulators, JavaScript engines, and virtual machines. Some apps with custom security measures may require additional steps.

### Q: Is this persistent across app restarts?
A: No, the JIT enablement typically needs to be reapplied when the app is restarted. This is a limitation of working within iOS's security model.

### Q: How does this differ from using a provisioning profile?
A: Provisioning profiles with the dynamic-codesigning entitlement can enable JIT, but they require a paid Apple Developer account and rebuilding the app. Our solution works with existing apps without modification.

## Conclusion

Our JIT Enabler solution provides a user-friendly way to enable JIT compilation for iOS apps without requiring code modifications. By leveraging a backend server and an iOS Shortcut, we can toggle memory permissions and set the necessary flags to enable JIT while maintaining security and compatibility with iOS's W^X policy.