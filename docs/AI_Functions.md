# AI Image Recognition and Decision-Making Functions

This document describes the new AI-powered image recognition and decision-making functions added to AutoHotkey.

## Overview

The AI functions extend AutoHotkey's existing image recognition capabilities with intelligent pattern recognition and decision-making features.

## Functions

### AIImageRecognize

Analyzes an image using AI-powered recognition to identify objects, text, or patterns.

**Syntax:**
```ahk
Result := AIImageRecognize(ImageFile [, Mode])
```

**Parameters:**
- `ImageFile` (String): Path to the image file to analyze
- `Mode` (String, Optional): Recognition mode
  - `"objects"` - Detect objects in the image
  - `"text"` - Recognize text (OCR)
  - `"patterns"` - Identify patterns
  - `"all"` - Use all recognition methods (default)

**Return Value:**
- Returns a string describing what was recognized in the image

**Example:**
```ahk
; Recognize objects in a screenshot
result := AIImageRecognize("screenshot.png", "objects")
MsgBox result

; Perform text recognition
text := AIImageRecognize("document.png", "text")
MsgBox "Recognized text: " text
```

### AIImageCompare

Compares two images using AI-powered similarity analysis.

**Syntax:**
```ahk
IsSimilar := AIImageCompare(Image1, Image2 [, Threshold])
```

**Parameters:**
- `Image1` (String): Path to the first image file
- `Image2` (String): Path to the second image file
- `Threshold` (Integer, Optional): Minimum similarity percentage (0-100) to return true. Default is 80.

**Return Value:**
- Returns `true` if images are similar (above threshold), `false` otherwise

**Example:**
```ahk
; Compare two screenshots
if AIImageCompare("before.png", "after.png", 90)
    MsgBox "Images are very similar"
else
    MsgBox "Images are different"

; Use default threshold (80%)
similar := AIImageCompare("image1.png", "image2.png")
```

### AIDecide

Makes an automated decision based on image analysis and predefined rules.

**Syntax:**
```ahk
Decision := AIDecide(ImageFile, Rules [, Options])
```

**Parameters:**
- `ImageFile` (String): Path to the image file to analyze
- `Rules` (String): Decision rules in format "if:condition,then:action"
- `Options` (String, Optional): Additional configuration options

**Return Value:**
- Returns a string describing the decision made

**Example:**
```ahk
; Automated decision-making based on image content
rules := "if:face_detected,then:unlock_screen"
decision := AIDecide("webcam.png", rules)
MsgBox "Decision: " decision

; More complex rules
rules := "if:motion_detected,then:alert_user"
decision := AIDecide("security_cam.png", rules, "sensitivity:high")
```

### AIGetModelInfo

Returns information about available AI models and capabilities.

**Syntax:**
```ahk
Info := AIGetModelInfo([ModelType])
```

**Parameters:**
- `ModelType` (String, Optional): Type of model information to retrieve
  - `"recognition"` - Image recognition models
  - `"comparison"` - Image comparison models
  - `"decision"` - Decision-making models
  - `"all"` - All model information (default)

**Return Value:**
- Returns a string with information about available AI models

**Example:**
```ahk
; Get all model information
info := AIGetModelInfo()
MsgBox info

; Get specific model info
recInfo := AIGetModelInfo("recognition")
```

## Use Cases

### Automated UI Testing
```ahk
; Compare expected vs actual UI state
if AIImageCompare("expected_ui.png", "actual_screenshot.png", 95)
    MsgBox "UI test passed"
else
    MsgBox "UI test failed - interface differs"
```

### Smart Automation
```ahk
; Recognize and act on screen content
result := AIImageRecognize("screen.png", "objects")
if InStr(result, "button")
{
    ; Take action based on recognized content
    Click 100, 200
}
```

### Security and Access Control
```ahk
; Face detection for access control
rules := "if:authorized_face,then:grant_access"
decision := AIDecide("camera_capture.png", rules)
if InStr(decision, "grant_access")
    MsgBox "Access granted"
```

### Document Processing
```ahk
; Extract text from images
text := AIImageRecognize("invoice.png", "text")
; Process extracted text
MsgBox "Extracted text: " text
```

## Technical Notes

### Implementation Details

The AI functions are implemented in `source/lib/ai_image.cpp` using advanced image processing techniques:

- **Phase 2 Implementation**: Real image analysis capabilities
  - **Edge Detection**: Gradient-based edge detection for pattern recognition
  - **Histogram Analysis**: Color distribution analysis for object detection
  - **Text Detection**: High-frequency pattern analysis for text identification
  - **Image Comparison**: Pixel-by-pixel and histogram-based similarity matching
  - **Decision Engine**: Real-time condition evaluation with image analysis

- **Future Enhancement**: Integration with additional AI libraries:
  - OpenCV for advanced computer vision
  - ONNX Runtime for deep learning models
  - Tesseract OCR for enhanced text recognition
  - TensorFlow Lite for embedded AI models

### Performance Considerations

- Image analysis can be computationally intensive
- Consider caching results for frequently analyzed images
- Use appropriate thresholds to balance accuracy vs. performance

### Error Handling

All AI functions return error messages as strings when:
- Image files are not found
- Invalid parameters are provided
- Processing fails

Example error handling:
```ahk
result := AIImageRecognize("nonexistent.png")
if InStr(result, "Error:")
    MsgBox "Recognition failed: " result
```

## Future Enhancements

Planned improvements include:

1. **Advanced Object Detection**: Integration with YOLO or SSD models
2. **OCR Enhancement**: Tesseract integration for accurate text recognition
3. **Face Recognition**: Facial detection and matching capabilities
4. **Custom Models**: Support for loading custom trained models
5. **GPU Acceleration**: CUDA/DirectML support for faster processing
6. **Batch Processing**: Analyze multiple images efficiently
7. **Real-time Processing**: Process video streams or screen captures in real-time

## See Also

- [ImageSearch](https://www.autohotkey.com/docs/v2/lib/ImageSearch.htm) - Traditional image search
- [PixelSearch](https://www.autohotkey.com/docs/v2/lib/PixelSearch.htm) - Pixel-based search
- [PixelGetColor](https://www.autohotkey.com/docs/v2/lib/PixelGetColor.htm) - Get pixel color
