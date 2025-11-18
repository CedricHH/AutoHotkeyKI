; AI Image Recognition and Decision-Making Examples
; This file demonstrates the new AI functions in AutoHotkey

; =============================================================================
; Example 1: Basic Object Recognition
; =============================================================================
Example1_ObjectRecognition()
{
    ; Recognize objects in an image
    imagePath := "C:\Screenshots\desktop.png"
    result := AIImageRecognize(imagePath, "objects")
    MsgBox "Objects detected: " result
    
    ; Use all recognition modes
    fullResult := AIImageRecognize(imagePath, "all")
    MsgBox "Full analysis: " fullResult
}

; =============================================================================
; Example 2: Text Recognition (OCR)
; =============================================================================
Example2_TextRecognition()
{
    ; Extract text from an image
    documentPath := "C:\Documents\invoice.png"
    extractedText := AIImageRecognize(documentPath, "text")
    
    ; Check for specific text
    if InStr(extractedText, "Invoice")
    {
        MsgBox "This is an invoice document"
        ; Process invoice...
    }
    else
    {
        MsgBox "Text found: " extractedText
    }
}

; =============================================================================
; Example 3: Image Comparison for UI Testing
; =============================================================================
Example3_UITesting()
{
    ; Compare expected vs actual UI
    expectedUI := "C:\Tests\expected_dialog.png"
    actualUI := "C:\Tests\actual_screenshot.png"
    
    ; Use 95% similarity threshold
    if AIImageCompare(expectedUI, actualUI, 95)
    {
        MsgBox "UI Test PASSED - Interface matches expected design"
        return true
    }
    else
    {
        MsgBox "UI Test FAILED - Interface differs from expected"
        return false
    }
}

; =============================================================================
; Example 4: Automated Decision Making
; =============================================================================
Example4_AutomatedDecision()
{
    ; Capture current screen
    imagePath := "C:\Temp\current_screen.png"
    
    ; Define decision rules
    rules := "if:error_dialog,then:close_and_retry"
    decision := AIDecide(imagePath, rules)
    
    ; Act on the decision
    if InStr(decision, "close_and_retry")
    {
        MsgBox "Error detected, closing and retrying..."
        ; Execute recovery actions
        Send "{Escape}"
        Sleep 1000
        ; Retry operation...
    }
}

; =============================================================================
; Example 5: Security Monitoring
; =============================================================================
Example5_SecurityMonitoring()
{
    ; Monitor for unauthorized access
    Loop
    {
        ; Capture from security camera
        cameraImage := "C:\Security\camera_feed.png"
        
        ; Check for authorized personnel
        rules := "if:authorized_face,then:allow_access"
        decision := AIDecide(cameraImage, rules, "sensitivity:high")
        
        if InStr(decision, "allow_access")
        {
            MsgBox "Access granted"
            break
        }
        else if InStr(decision, "unauthorized")
        {
            MsgBox "ALERT: Unauthorized access attempt!"
            ; Trigger alarm...
        }
        
        Sleep 2000 ; Check every 2 seconds
    }
}

; =============================================================================
; Example 6: Pattern Recognition for Automation
; =============================================================================
Example6_PatternAutomation()
{
    ; Recognize UI patterns to automate tasks
    screenCapture := "C:\Temp\current_window.png"
    result := AIImageRecognize(screenCapture, "patterns")
    
    ; Look for specific patterns
    if InStr(result, "dialog_pattern")
    {
        MsgBox "Dialog detected, automating response..."
        ; Click OK button based on pattern
        Click 400, 300
    }
    else if InStr(result, "loading_pattern")
    {
        MsgBox "Loading screen detected, waiting..."
        ; Wait for loading to complete
        Sleep 5000
    }
}

; =============================================================================
; Example 7: Comparing Multiple Images
; =============================================================================
Example7_BatchComparison()
{
    ; Compare multiple screenshots
    baseImage := "C:\Images\reference.png"
    testImages := ["test1.png", "test2.png", "test3.png"]
    
    matches := []
    for index, testImage in testImages
    {
        fullPath := "C:\Images\" testImage
        if AIImageCompare(baseImage, fullPath, 85)
        {
            matches.Push(testImage)
        }
    }
    
    if matches.Length > 0
    {
        result := "Matching images: "
        for index, img in matches
            result .= img " "
        MsgBox result
    }
    else
    {
        MsgBox "No matching images found"
    }
}

; =============================================================================
; Example 8: Get AI Model Information
; =============================================================================
Example8_ModelInfo()
{
    ; Get information about available AI models
    allInfo := AIGetModelInfo("all")
    MsgBox "Available AI Capabilities:`n`n" allInfo
    
    ; Get specific model info
    recognitionInfo := AIGetModelInfo("recognition")
    MsgBox "Recognition Model:`n" recognitionInfo
    
    comparisonInfo := AIGetModelInfo("comparison")
    MsgBox "Comparison Model:`n" comparisonInfo
    
    decisionInfo := AIGetModelInfo("decision")
    MsgBox "Decision Model:`n" decisionInfo
}

; =============================================================================
; Example 9: Error Handling
; =============================================================================
Example9_ErrorHandling()
{
    ; Proper error handling for AI functions
    imagePath := "C:\NonExistent\image.png"
    
    result := AIImageRecognize(imagePath, "objects")
    if InStr(result, "Error:")
    {
        MsgBox "Recognition failed: " result
        ; Handle error appropriately
        return false
    }
    
    ; Process successful result
    MsgBox "Recognition succeeded: " result
    return true
}

; =============================================================================
; Example 10: Real-world Automation Scenario
; =============================================================================
Example10_RealWorldAutomation()
{
    ; Automated form filling based on image recognition
    
    ; 1. Capture the current form
    formImage := "C:\Temp\form_capture.png"
    
    ; 2. Recognize form fields
    fields := AIImageRecognize(formImage, "text")
    
    ; 3. Decide which fields to fill
    rules := "if:name_field,then:fill_name|if:email_field,then:fill_email"
    decision := AIDecide(formImage, rules)
    
    ; 4. Execute filling actions based on decision
    if InStr(decision, "fill_name")
    {
        Send "John Doe"
        Send "{Tab}"
    }
    
    if InStr(decision, "fill_email")
    {
        Send "john.doe@example.com"
        Send "{Tab}"
    }
    
    ; 5. Verify form was filled correctly
    Sleep 500
    verifyImage := "C:\Temp\form_filled.png"
    expectedImage := "C:\Templates\expected_form.png"
    
    if AIImageCompare(verifyImage, expectedImage, 90)
    {
        MsgBox "Form filled successfully!"
        Send "{Enter}" ; Submit form
    }
    else
    {
        MsgBox "Form filling verification failed"
    }
}

; =============================================================================
; Main Menu
; =============================================================================
ShowExamplesMenu()
{
    menu := Menu()
    menu.Add("1. Object Recognition", (*) => Example1_ObjectRecognition())
    menu.Add("2. Text Recognition (OCR)", (*) => Example2_TextRecognition())
    menu.Add("3. UI Testing", (*) => Example3_UITesting())
    menu.Add("4. Automated Decision", (*) => Example4_AutomatedDecision())
    menu.Add("5. Security Monitoring", (*) => Example5_SecurityMonitoring())
    menu.Add("6. Pattern Automation", (*) => Example6_PatternAutomation())
    menu.Add("7. Batch Comparison", (*) => Example7_BatchComparison())
    menu.Add("8. Model Information", (*) => Example8_ModelInfo())
    menu.Add("9. Error Handling", (*) => Example9_ErrorHandling())
    menu.Add("10. Real-world Automation", (*) => Example10_RealWorldAutomation())
    menu.Show()
}

; Uncomment to show the examples menu on startup
; ShowExamplesMenu()
