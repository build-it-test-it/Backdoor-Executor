# Executor UI Design Document

## Overview

This document outlines the comprehensive design of the Executor UI system, ensuring compatibility across iOS 15-18+ devices with beautiful visual effects, memory optimization, and a complete script management experience.

## Core Design Principles

1. **Beautiful, Interactive UI**
   - LED-like glowing effects around interactive elements
   - Smooth animations that respond to user interaction
   - Depth through subtle layering and translucency
   - Visual feedback for all user actions

2. **Memory Efficiency**
   - Dynamic resource loading/unloading based on visibility
   - View recycling for list elements
   - Image caching with automatic purging under memory pressure
   - Incremental rendering for complex UI elements

3. **iOS Version Compatibility**
   - Backwards compatibility with iOS 15+ devices
   - Forward compatibility with iOS 18+ features
   - Graceful feature degradation on older devices
   - Adaptive layouts for all screen sizes

4. **Accessibility**
   - Dynamic text sizing support
   - Reduced motion option
   - Voice over compatibility
   - Haptic feedback for interactions

## UI Components

### 1. Floating Button

**Design:**
- Circular, semi-transparent button with pulsing LED border
- Draggable with edge-snapping behavior
- Expands into quick-action menu on long press
- Automatically hides when not in a Roblox game

**Interactions:**
- Tap: Open/close main interface
- Long press: Show quick actions menu
- Drag: Reposition button
- Double tap: Execute last script

**Appearance:**
- Glowing blue accent color with subtle pulsing effect
- Icon transitions based on execution state
- Haptic feedback on press

### 2. Script Editor

**Design:**
- Full-featured code editor with syntax highlighting
- Dark theme with colored syntax elements
- Line numbers and error indicators
- Collapsible sections for long scripts
- Bottom action bar with glowing LED buttons

**Features:**
- Syntax highlighting for Lua
- Auto-completion with context awareness
- Error highlighting with suggestions
- Debug mode with variable inspection
- AI assistance with one-tap debugging

**Memory Optimization:**
- Incremental rendering for large scripts
- Background syntax processing
- Lazy-loading of syntax highlighting rules

### 3. Script Management

**Design:**
- Grid/list toggle view of scripts
- Category tabs with glowing indicators
- Interactive cards with preview and metadata
- Swipe actions for quick operations
- Contextual hold menu for all operations

**Organization:**
- Hierarchical categories with color coding
- Favorites section for quick access
- Recent scripts with timestamps
- Search with syntax highlighting
- Tagging system for organization

**Actions:**
- Rename, delete, duplicate scripts
- Move between categories
- Export/import scripts
- Share via iOS share sheet
- Debug mode launch

### 4. Console Output

**Design:**
- Terminal-like interface with monospaced font
- Syntax highlighting for output
- Collapsible output sections
- Filtering options for different message types
- Auto-scroll with manual override

**Features:**
- Real-time output streaming
- Copy functionality for output text
- Error linking to relevant script positions
- Command history for interactive scripts
- Timestamp toggle for debugging

### 5. Settings Panel

**Design:**
- Grouped settings with clear headings
- Toggle switches with LED effects
- Sliders with reactive glow
- Section navigation with smooth transitions
- Preview panels for visual settings

**Options:**
- UI theme and color scheme
- Effect intensity controls
- Font size and style settings
- Memory usage management
- Performance monitoring

### 6. AI Assistant Panel

**Design:**
- Chat-like interface with message bubbles
- Code blocks with syntax highlighting
- Suggestion chips for common actions
- Context-aware toolbar
- Visual indication of AI "thinking"

**Features:**
- Script generation from descriptions
- Debugging assistance
- Code explanation
- Performance optimization suggestions
- Game-specific script recommendations

## Visual Language

### Color Scheme

#### Primary Scheme (Dark)
- Background: Deep blue-black (#0D1117)
- Surface: Slightly lighter blue-black (#161B22)
- Primary: Electric blue (#58A6FF)
- Secondary: Neon purple (#BD93F9)
- Accent: Cyan (#8BE9FD)
- Error: Bright red (#FF5555)
- Success: Neon green (#50FA7B)
- Warning: Amber (#FFB86C)

#### Alternative Schemes
- Cyberpunk (Yellow/Pink/Blue)
- Matrix (Green/Black)
- Retro (Purple/Blue/Pink)
- Stealth (Gray/Red)
- Light Mode (For accessibility)

### Typography

- Code: SF Mono (or Monaco as fallback)
- UI: SF Pro Text
- Headings: SF Pro Display
- Console: Menlo

### LED Effects System

LED effects are created using a combination of techniques:
- Core Animation layers with bloom effects
- Dynamic shadows with color matching
- Subtle inner and outer glow
- Animated parameters for "breathing" effects

#### Effect Types

1. **Pulse Effect**
   - Rhythmic pulsing glow around buttons and active elements
   - Intensity varies based on importance
   - Color matches the action type

2. **Border Glow**
   - Thin, bright border with outer glow
   - Used for selection indication
   - Color indicates state (selected, running, error)

3. **Breathing Effect**
   - Slow intensity changes over time
   - Applied to background elements
   - Creates a sense of "alive" interface

4. **Reactive Glow**
   - Flares on interaction
   - Provides visual feedback
   - Accompanied by subtle haptic feedback

### Animation System

Animations are carefully designed to be:
- Smooth but not distracting
- Meaningful rather than decorative
- Memory-efficient
- Disabled when low-power mode is active

#### Animation Types

1. **Transitions**
   - Smooth crossfades between UI states
   - Page curl effect for script switching
   - Expand/collapse for hierarchical elements

2. **Interactive Feedback**
   - Button press/release animations
   - Drag state visualization
   - Success/error animations

3. **Background Effects**
   - Subtle particle effects in backgrounds
   - Gradient shifts based on content
   - Dynamic blur that responds to UI state

4. **Progress Indicators**
   - Custom glowing progress bars
   - Circular execution indicators
   - Pulsing loading states

## Memory Management

### Optimization Strategies

1. **View Recycling**
   - Reuse cells in collection views
   - Purge off-screen content
   - Lazy load images and heavy content

2. **Render Optimization**
   - Reduce blur effect quality under memory pressure
   - Decrease animation complexity when needed
   - Use bitmap caching for static content

3. **Resource Management**
   - Unload unused syntax highlighting rules
   - Clear undo history when switching scripts
   - Release image caches on memory warning

4. **Execution Sandboxing**
   - Isolate script execution memory
   - Limit console output buffer size
   - Split large scripts into manageable chunks

### Automatic Memory Reduction

The system automatically responds to memory pressure by:
1. Reducing visual effects
2. Simplifying animations
3. Shrinking caches
4. Releasing non-critical resources
5. Truncating history logs

## Interaction Patterns

### Script Management

1. **Creation Flow**
   - Tap "+" to create a new script
   - Choose category or use default
   - Enter name or use auto-generated name
   - Begin editing immediately

2. **Organization Flow**
   - Long press to enter selection mode
   - Drag to reorder or move to categories
   - Swipe left for quick actions
   - Pinch to expand/collapse categories

3. **Execution Flow**
   - Tap script to view/edit
   - Tap execute button to run
   - Output appears in console tab
   - Success/error notification appears

### Script Editing

1. **Editor Controls**
   - Syntax highlighting updates as you type
   - Auto-complete appears after brief pause
   - Error checking runs on pause in typing
   - Gesture navigation:
     - Pinch to zoom text size
     - Two-finger scroll for quick navigation
     - Triple tap for line selection

2. **Debugging Flow**
   - Enable debug mode from toolbar
   - Set breakpoints by tapping line numbers
   - Step through execution with controls
   - Inspect variables in debug panel
   - Request AI analysis of problems

### AI Assistant Integration

1. **Help Request Flow**
   - Tap assistant button in toolbar
   - Describe what you need in natural language
   - Receive code suggestions or explanations
   - Apply suggestions with one tap
   - Rate responses to improve quality

2. **Automatic Assistance**
   - Error detection with fix suggestions
   - Performance optimization hints
   - Game-specific code recommendations
   - Security warnings for risky scripts

## iOS Version Compatibility

### iOS 15 Support
- Basic LED effects using CALayer properties
- Simplified animations
- Standard blur effects
- Full functionality with visual differences

### iOS 16-17 Enhancements
- Enhanced blur effects
- Material backgrounds
- Improved animations
- Better performance and memory usage

### iOS 18+ Features
- Latest visual effects
- Advanced animations
- Full haptic integration
- Maximum performance optimization

## Accessibility Considerations

1. **Visual Accessibility**
   - Dynamic Type support for all text
   - High contrast mode option
   - Reduced motion setting
   - Reduced transparency option

2. **Motor Accessibility**
   - Large touch targets
   - Adjustable button sizes
   - Alternative navigation methods
   - Customizable gesture sensitivity

3. **Cognitive Accessibility**
   - Clear, consistent layout
   - Progressive disclosure of complex features
   - Helpful error messages
   - Simplified mode option

## Implementation Guidelines

### Memory Efficiency
- Use `UICollectionView` with cell reuse for all lists
- Implement `UITraitCollection` monitoring for appearance changes
- Use `NSCache` with cost functions for resource management
- Implement `didReceiveMemoryWarning` handlers in all ViewControllers

### Visual Effects
- Create LED effects using multiple layers:
  - Base `CAGradientLayer` for core color
  - `CALayer` with shadow properties for glow
  - Animation with `CABasicAnimation` for pulsing
- Scale effect complexity based on device capability
- Cache commonly used effects

### Cross-Version Compatibility
- Check API availability with `@available`
- Provide fallbacks for newer APIs
- Test on oldest supported iOS version
- Use capability checking rather than version checking

## Design Mockups

The UI follows a tab-based design with the following main sections:

1. **Editor Tab**
   - Full-screen code editor
   - Bottom toolbar with execution controls
   - Syntax highlighting with theme-appropriate colors
   - Line numbers and error indicators
   - Debugging panel (expandable)

2. **Scripts Tab**
   - Grid view of script cards with glowing borders
   - Category filters at top with LED indicators
   - Search bar with real-time filtering
   - Card previews with syntax highlighting
   - Context menu with all actions

3. **Console Tab**
   - Terminal-style output view
   - Command input field
   - Output filtering options
   - Monospaced text with syntax highlighting
   - Copy and clear controls

4. **Settings Tab**
   - Grouped settings with toggles and sliders
   - Visual effect previews
   - Performance monitoring graphs
   - Memory usage indicators
   - Backup and restore options

5. **AI Assistant Tab**
   - Conversational interface
   - Code block support
   - Suggestion chips
   - Context-aware toolbar
   - Real-time response indication

## Conclusion

This design creates a beautiful, responsive, and memory-efficient UI that works across iOS 15-18+ devices. The LED effects, animations, and interaction patterns create a sense of a "living" interface while maintaining peak performance and compatibility.
