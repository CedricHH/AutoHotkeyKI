# AI Image Recognition Implementation Notes

## Overview

This document describes the implementation details of the AI-powered image recognition and decision-making features added to AutoHotkey.

## Files Modified/Added

### New Files

1. **source/lib/ai_image.cpp**
   - Contains the implementation of all AI-related functions
   - Provides foundation for future AI/ML library integration
   - Functions: AIImageRecognize, AIImageCompare, AIDecide, AIGetModelInfo

2. **docs/AI_Functions.md**
   - Complete user documentation for AI functions
   - Includes syntax, parameters, return values, and examples
   - Documents use cases and future enhancements

3. **docs/AI_Examples.ahk**
   - Comprehensive example scripts demonstrating all AI functions
   - 10 practical examples covering various use cases
   - Ready-to-use templates for common scenarios

### Modified Files

1. **source/lib/functions.h**
   - Added function declarations for the four new AI functions
   - Placed after ImageSearch for logical grouping

2. **AutoHotkeyx.vcxproj**
   - Added ai_image.cpp to the build configuration
   - Ensures the new file is compiled with the project

3. **README.md**
   - Added AI-Powered Features section
   - Links to documentation and examples

## Architecture

### Current Implementation

The current implementation provides a framework and API structure:

```
AI Functions Layer
    ↓
Basic Validation & Processing
    ↓
Mock Results (Framework)
```

**Key Features:**
- File existence validation
- Parameter validation and error handling
- Consistent API design following AutoHotkey conventions
- String-based return values for flexibility
- Optional parameters with sensible defaults

### Future Integration

The code is designed for easy integration with real AI libraries:

```
AI Functions Layer
    ↓
AI Library Abstraction
    ↓
    ├─ OpenCV (Image Processing)
    ├─ ONNX Runtime (Deep Learning)
    ├─ Tesseract (OCR)
    └─ Custom Models
```

## Function Details

### AIImageRecognize

**Purpose:** Recognize objects, text, or patterns in images

**Current Implementation:**
- Validates image file exists
- Returns mode-specific placeholder results
- Demonstrates API structure

**Future Enhancement:**
- Integrate OpenCV for image preprocessing
- Load ONNX models for object detection (YOLO, SSD)
- Integrate Tesseract for OCR
- Return structured data (JSON/Object)

### AIImageCompare

**Purpose:** Compare two images for similarity

**Current Implementation:**
- Validates both image files exist
- Accepts threshold parameter (0-100)
- Returns boolean result

**Future Enhancement:**
- Implement perceptual hashing (pHash, dHash)
- Calculate SSIM (Structural Similarity Index)
- Use deep learning features for semantic comparison
- Return similarity score along with boolean

### AIDecide

**Purpose:** Make decisions based on image analysis and rules

**Current Implementation:**
- Parses simple rule format "if:condition,then:action"
- Validates image file
- Returns decision description

**Future Enhancement:**
- Integrate rule engine
- Support complex rule syntax
- Connect to AI recognition results
- Support multiple conditions and actions
- Add confidence scores

### AIGetModelInfo

**Purpose:** Provide information about available AI capabilities

**Current Implementation:**
- Returns descriptive text about model types
- Supports filtering by model category

**Future Enhancement:**
- Return actual model metadata
- Include version information
- Show model performance metrics
- Support model downloading/updating

## Integration Points

### Windows-Specific Considerations

AutoHotkey is Windows-only, which affects AI library choices:

**Recommended Libraries:**
- **ONNX Runtime**: Cross-platform, Windows-optimized builds available
- **OpenCV**: Excellent Windows support, pre-built binaries
- **Tesseract**: Windows binaries available
- **DirectML**: Windows-native GPU acceleration

**Build Considerations:**
- Static linking preferred to avoid DLL dependencies
- Consider size impact on executable
- GPU acceleration via DirectX/DirectML

### Memory Management

Current approach:
- Uses AutoHotkey's string management (SetTemp)
- Minimal heap allocations
- Proper cleanup with free()

Future considerations:
- Smart pointers for AI library objects
- Image buffer management
- Model caching strategy

## Testing Strategy

### Current State

No automated tests included to maintain minimal changes approach.

### Recommended Testing

1. **Unit Tests**
   - File validation logic
   - Parameter parsing
   - Error handling

2. **Integration Tests**
   - Test with actual images
   - Verify function registration
   - Test from AHK scripts

3. **Performance Tests**
   - Image processing time
   - Memory usage
   - Multiple concurrent calls

## Build Instructions

### Requirements
- Visual Studio 2022 (or compatible)
- Windows SDK
- AutoHotkey build dependencies

### Building
```cmd
# Using Visual Studio
msbuild AutoHotkeyx.sln /p:Configuration=Release /p:Platform=x64

# Using VS Code
Use build task: Ctrl+Shift+B, select "build-debug"
```

### Verification
```ahk
; Test script
result := AIGetModelInfo()
MsgBox result

if (result != "")
    MsgBox "AI functions loaded successfully!"
```

## Security Considerations

### Input Validation

All functions validate:
- File paths for existence
- Parameter ranges (thresholds)
- String format for rules

### Future Security

When integrating AI libraries:
- Validate model files before loading
- Sanitize image file paths
- Implement timeouts for long operations
- Consider sandboxing for untrusted images
- Validate model outputs

## Performance Considerations

### Current Impact

Minimal:
- Simple string operations
- File existence checks only
- No heavy computation

### Future Optimization

When adding AI:
- Cache loaded models in memory
- Use worker threads for processing
- Implement result caching
- Consider downsampling large images
- GPU acceleration for inference

## API Design Decisions

### String Returns vs. Objects

**Decision:** Use string returns for results

**Rationale:**
- Consistent with AutoHotkey v2 patterns
- Easy to parse in scripts
- Flexible for various output formats
- Can evolve to objects later

### Optional Parameters

**Decision:** Use optl<> for optional parameters with defaults

**Rationale:**
- Follows AutoHotkey conventions
- Makes common use cases simple
- Allows advanced customization
- Type-safe with C++ templates

### Error Handling

**Decision:** Return error strings instead of throwing

**Rationale:**
- Scripts can check and handle errors
- Consistent with AutoHotkey patterns
- Prevents script crashes
- Allows graceful degradation

## Migration Path

### Phase 1: Framework (Current)
- ✓ API structure defined
- ✓ Function registration
- ✓ Documentation complete
- ✓ Examples provided

### Phase 2: Basic AI Integration
- [ ] Integrate OpenCV
- [ ] Add basic object detection
- [ ] Implement image similarity
- [ ] Add OCR support

### Phase 3: Advanced Features
- [ ] Deep learning models
- [ ] GPU acceleration
- [ ] Real-time processing
- [ ] Custom model support

### Phase 4: Optimization
- [ ] Performance tuning
- [ ] Memory optimization
- [ ] Caching strategy
- [ ] Parallel processing

## Contributing

### Adding New AI Functions

1. Add function declaration in `source/lib/functions.h`
2. Implement function in `source/lib/ai_image.cpp`
3. Follow existing patterns (bif_impl, parameter types)
4. Document in `docs/AI_Functions.md`
5. Add examples to `docs/AI_Examples.ahk`

### Integrating AI Libraries

1. Add library dependencies to vcxproj
2. Create abstraction layer for library
3. Update function implementations
4. Add error handling for library failures
5. Update documentation with new capabilities
6. Add performance benchmarks

## References

### AutoHotkey Internals
- Function declaration system (md_func macros)
- Built-in function implementation (bif_impl)
- String handling (StrRet, StrArg)
- Parameter handling (optl<>)

### AI/ML Libraries
- [ONNX Runtime](https://onnxruntime.ai/) - Cross-platform inference
- [OpenCV](https://opencv.org/) - Computer vision
- [Tesseract](https://github.com/tesseract-ocr/tesseract) - OCR
- [DirectML](https://docs.microsoft.com/en-us/windows/ai/directml/) - Windows GPU acceleration

### Related Functions
- ImageSearch - Existing image search functionality
- PixelSearch - Pixel-level search
- PixelGetColor - Color retrieval

## Conclusion

The implementation provides a solid foundation for AI-powered image recognition in AutoHotkey. The API is designed to be:

- **Easy to use** - Familiar AutoHotkey patterns
- **Extensible** - Ready for AI library integration
- **Safe** - Proper validation and error handling
- **Well-documented** - Complete docs and examples

The framework is ready for phase 2: actual AI library integration.
