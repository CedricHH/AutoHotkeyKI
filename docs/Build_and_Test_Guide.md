# Build and Test Guide for AI Features

This guide helps you build and test the new AI image recognition features on Windows.

## Prerequisites

- Windows 10/11
- Visual Studio 2022 (Community Edition or higher)
- Desktop development with C++ workload
- Windows SDK

## Building AutoHotkey with AI Features

### Option 1: Visual Studio

1. Open `AutoHotkeyx.sln` in Visual Studio 2022
2. Select build configuration:
   - Configuration: **Release** (recommended) or **Debug**
   - Platform: **x64** (recommended) or **Win32**
3. Build the solution:
   - Menu: Build → Build Solution
   - Or press: `Ctrl+Shift+B`
4. Wait for compilation to complete
5. Output will be in `bin` directory

### Option 2: Command Line (MSBuild)

```cmd
# Set up build environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

# Build Release x64
msbuild AutoHotkeyx.sln /p:Configuration=Release /p:Platform=x64

# Or build Debug x64
msbuild AutoHotkeyx.sln /p:Configuration=Debug /p:Platform=x64
```

### Option 3: VS Code

1. Open the project folder in VS Code
2. Press `Ctrl+Shift+B`
3. Select build configuration when prompted
4. Build output will appear in terminal

## Verifying the Build

### 1. Check Build Output

Successful build should show:
```
Build succeeded.
    0 Warning(s)
    0 Error(s)
```

Look for the executable:
- Release: `bin\x64\Release\AutoHotkey.exe`
- Debug: `bin\x64\Debug\AutoHotkey.exe`

### 2. Verify AI Functions are Available

Create a test script `test_ai.ahk`:

```ahk
; Test script to verify AI functions are available

; Test 1: AIGetModelInfo
try {
    info := AIGetModelInfo()
    MsgBox "✓ AIGetModelInfo works!`n`nResult: " info
} catch as err {
    MsgBox "✗ AIGetModelInfo failed: " err.Message
}

; Test 2: AIImageRecognize (with error handling)
try {
    ; This will fail with "Image file not found" - that's expected
    result := AIImageRecognize("nonexistent.png", "objects")
    if InStr(result, "Error:")
        MsgBox "✓ AIImageRecognize works! (Error handling verified)"
    else
        MsgBox "✓ AIImageRecognize works!`n`nResult: " result
} catch as err {
    MsgBox "✗ AIImageRecognize failed: " err.Message
}

; Test 3: AIImageCompare (with error handling)
try {
    ; This will fail with file not found - that's expected
    similar := AIImageCompare("img1.png", "img2.png", 80)
    MsgBox "✓ AIImageCompare works!`n`nResult: " similar
} catch as err {
    MsgBox "✗ AIImageCompare failed: " err.Message
}

; Test 4: AIDecide (with error handling)
try {
    decision := AIDecide("test.png", "if:face,then:unlock", "")
    if InStr(decision, "Error:")
        MsgBox "✓ AIDecide works! (Error handling verified)"
    else
        MsgBox "✓ AIDecide works!`n`nResult: " decision
} catch as err {
    MsgBox "✗ AIDecide failed: " err.Message
}

MsgBox "All function tests completed!"
```

Run the test:
```cmd
AutoHotkey.exe test_ai.ahk
```

Expected behavior:
- All four functions should be recognized
- Error messages should be returned for non-existent files
- No crashes or exceptions

## Testing with Real Images

### 1. Prepare Test Images

Create a test directory with sample images:
```
C:\AI_Test\
  ├─ test_image.png
  ├─ reference.png
  └─ compare.png
```

### 2. Run Example Scripts

Test the examples from `docs/AI_Examples.ahk`:

```ahk
; Modify paths in examples to your test directory
; Example 8 (Model Info) should work without any files:

info := AIGetModelInfo("all")
MsgBox info
```

### 3. Test Each Function

#### AIImageRecognize
```ahk
result := AIImageRecognize("C:\AI_Test\test_image.png", "objects")
MsgBox "Recognition result: " result
```

#### AIImageCompare
```ahk
similar := AIImageCompare("C:\AI_Test\reference.png", "C:\AI_Test\compare.png", 80)
MsgBox "Images similar: " similar
```

#### AIDecide
```ahk
rules := "if:object_detected,then:process"
decision := AIDecide("C:\AI_Test\test_image.png", rules)
MsgBox "Decision: " decision
```

## Common Build Issues

### Issue 1: Missing Windows SDK

**Error:** Cannot find Windows.h or similar headers

**Solution:**
1. Open Visual Studio Installer
2. Modify your installation
3. Ensure "Windows SDK" is selected
4. Install and rebuild

### Issue 2: Platform Toolset Not Found

**Error:** Platform Toolset 'v143' cannot be found

**Solution:**
1. Right-click project in Solution Explorer
2. Properties → Configuration Properties → General
3. Change "Platform Toolset" to match your VS version
4. Apply and rebuild

### Issue 3: Cannot Open stdafx.h

**Error:** Cannot open source file "stdafx.h"

**Solution:**
- This is a precompiled header issue
- Clean solution: Build → Clean Solution
- Rebuild: Build → Rebuild Solution

### Issue 4: LNK Error - Cannot Find DLL

**Error:** Cannot find required DLL

**Solution:**
- For Debug builds, ensure Visual C++ Runtime is installed
- For Release builds, check if static linking is configured
- Copy required DLLs to output directory

## Troubleshooting

### Functions Not Found in Script

If AutoHotkey doesn't recognize the AI functions:

1. **Verify Build Success**
   - Check build output for errors
   - Ensure ai_image.cpp was compiled
   - Check that functions.h includes AI declarations

2. **Check AutoHotkey Version**
   - Functions are for AutoHotkey v2
   - Won't work with v1

3. **Verify Binary**
   - Make sure you're running the newly built AutoHotkey.exe
   - Check file modification date
   - Check file size (should be larger with new functions)

### Runtime Errors

**"Function not found"**
- Build wasn't successful
- Running wrong executable
- Functions.h not properly updated

**Crash on Function Call**
- Debug build recommended for testing
- Check Visual Studio debugger output
- Verify parameter types match declarations

## Performance Testing

### Basic Performance Test

```ahk
; Measure function call overhead
Loop 1000 {
    AIGetModelInfo("recognition")
}
MsgBox "1000 calls completed"
```

Should complete instantly since no actual AI processing yet.

### Memory Usage

Use Windows Task Manager to monitor:
- Baseline AutoHotkey memory usage
- Memory after loading AI functions
- Memory during function calls

Expected: Minimal increase (functions are lightweight framework)

## Integration Testing

Test integration with existing AutoHotkey features:

```ahk
; Combine with hotkeys
^!a::  ; Ctrl+Alt+A
{
    info := AIGetModelInfo()
    MsgBox info
}

; Combine with loops
Loop Files, "C:\Images\*.png"
{
    result := AIImageRecognize(A_LoopFileFullPath, "objects")
    FileAppend result "`n", "results.txt"
}

; Combine with conditionals
if AIImageCompare("img1.png", "img2.png", 90)
    Run "process_similar.ahk"
else
    Run "process_different.ahk"
```

## Next Steps

Once basic testing is complete:

1. **Phase 2 Development**: Integrate actual AI libraries
   - OpenCV for image processing
   - ONNX Runtime for deep learning
   - Tesseract for OCR

2. **Advanced Testing**: Test with real AI models

3. **Performance Optimization**: Profile and optimize hot paths

4. **Documentation Updates**: Update docs with real capabilities

## Reporting Issues

If you encounter problems:

1. **Check Build Log**
   - Capture full build output
   - Note any warnings

2. **Test Minimal Example**
   - Start with simplest function (AIGetModelInfo)
   - Gradually test more complex ones

3. **Gather Information**
   - Windows version
   - Visual Studio version
   - Build configuration used
   - Error messages (exact text)

4. **Create Issue**
   - Include all gathered information
   - Provide steps to reproduce
   - Attach build log if relevant

## Success Checklist

- [ ] Project builds without errors
- [ ] AutoHotkey.exe is created
- [ ] Test script recognizes AI functions
- [ ] AIGetModelInfo returns expected text
- [ ] AIImageRecognize handles errors correctly
- [ ] AIImageCompare handles errors correctly
- [ ] AIDecide handles errors correctly
- [ ] No crashes during function calls
- [ ] Functions integrate with other AHK features
- [ ] Documentation examples work (with appropriate modifications)

Once all items are checked, the AI framework is successfully integrated and ready for phase 2 development!
