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

### Phase 2 Implementation (Current)

The implementation now includes real image processing algorithms:

```
AI Functions Layer
    ↓
Image Loading & Validation (GDI+)
    ↓
    ├─ Edge Detection (Gradient-based)
    ├─ Histogram Analysis (Color distribution)
    ├─ Text Pattern Detection (Edge density + contrast)
    ├─ Region Detection (Peak analysis)
    └─ Similarity Matching (Pixel + Histogram)
    ↓
Results & Decision Logic
```

**Key Features:**
- File existence validation
- Real image analysis using Windows GDI+ APIs
- Multiple computer vision algorithms
- Consistent API design following AutoHotkey conventions
- String-based return values with detailed metrics
- Optional parameters with sensible defaults

**New Algorithms (Phase 2):**
- **CalculateHistogram()** - Luminance histogram generation with RGB to grayscale conversion
- **DetectEdges()** - Gradient-based edge detection using Sobel-like operators
- **DetectTextPatterns()** - Combines edge density and contrast analysis
- **DetectColorRegions()** - Peak detection in color histograms
- **CalculateImageSimilarity()** - Multi-metric comparison (70% pixel, 30% histogram)
- **LoadImageFile()** - Robust image loading with dimension extraction

### Future Integration (Phase 3+)

The code is designed for easy integration with additional AI libraries:

```
AI Functions Layer
    ↓
Current: GDI+ Image Processing + Custom Algorithms
    ↓
Future Add-ons:
    ├─ OpenCV (Advanced Computer Vision)
    ├─ ONNX Runtime (Deep Learning Models)
    ├─ Tesseract (Enhanced OCR)
    └─ Custom ML Models
```

## Function Details

### AIImageRecognize

**Purpose:** Recognize objects, text, or patterns in images

**Phase 2 Implementation:**
- Validates and loads image files
- **Text Mode**: Detects text patterns using edge density and contrast analysis
- **Objects Mode**: Identifies color regions through histogram peak detection
- **Patterns Mode**: Analyzes edge density and distribution
- **All Mode**: Comprehensive analysis with all metrics
- Returns detailed results with resolution, region count, edge count, and percentages

**Future Enhancement:**
- Integrate OpenCV for advanced preprocessing
- Load ONNX models for object detection (YOLO, SSD)
- Integrate Tesseract for actual OCR text extraction
- Return structured data (JSON/Object)

### AIImageCompare

**Purpose:** Compare two images for similarity

**Phase 2 Implementation:**
- Validates and loads both image files
- **Pixel Comparison**: Compares each pixel with configurable color tolerance
- **Histogram Comparison**: Analyzes color distribution similarity
- **Dimension Handling**: Works with different sized images using histogram-only mode
- **Weighted Score**: Combines pixel (70%) and histogram (30%) similarity
- Returns boolean based on configurable threshold (0-100%)

**Future Enhancement:**
- Implement perceptual hashing (pHash, dHash)
- Calculate SSIM (Structural Similarity Index)
- Use deep learning features for semantic comparison
- Return similarity score along with boolean

### AIDecide

**Purpose:** Make decisions based on image analysis and rules

**Phase 2 Implementation:**
- Parses rule format "if:condition,then:action"
- Validates and analyzes image in real-time
- **Supported Conditions:**
  - `text` - Detects text patterns
  - `object`/`region` - Checks for multiple color regions
  - `pattern`/`edge` - Evaluates edge density
  - `complex` - Combination of edges and regions
- Evaluates conditions against actual image analysis
- Returns detailed decision with condition evaluation results

**Future Enhancement:**
- Support multiple conditions (AND/OR logic)
- Add confidence scores and thresholds
- Support nested rules
- Integration with external rule engines

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

### Phase 1: Framework (Complete)
- ✓ API structure defined
- ✓ Function registration
- ✓ Documentation complete
- ✓ Examples provided

### Phase 2: Image Analysis Algorithms (Complete)
- ✓ Edge detection implementation
- ✓ Histogram analysis
- ✓ Text pattern detection
- ✓ Color region detection
- ✓ Image similarity comparison
- ✓ Real-time decision engine
- ✓ GDI+ integration

### Phase 3: Advanced AI Integration (Future)
- [ ] Integrate OpenCV for advanced vision
- [ ] Add ONNX Runtime for deep learning
- [ ] Integrate Tesseract for OCR
- [ ] Support custom trained models
- [ ] GPU acceleration via DirectML

### Phase 4: Optimization (Future)
- [ ] Performance tuning
- [ ] Memory optimization
- [ ] Result caching
- [ ] Parallel processing
- [ ] Multi-threaded analysis

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
