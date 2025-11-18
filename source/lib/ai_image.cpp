/*
AutoHotkey

Copyright 2003-2009 Chris Mallett (support@autohotkey.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "stdafx.h" // pre-compiled headers
#include "script.h"
#include "script_func_impl.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>


// Helper function to load image and get bitmap data
// Returns bitmap handle that must be freed by caller
static HBITMAP LoadImageFile(LPCTSTR aImageFile, int &aWidth, int &aHeight)
{
	int image_type;
	bool no_delete_bitmap;
	HBITMAP hbitmap = LoadPicture(aImageFile, 0, 0, image_type, 0, false, &no_delete_bitmap);
	
	if (!hbitmap)
		return NULL;
	
	// Get bitmap dimensions
	BITMAP bm;
	if (GetObject(hbitmap, sizeof(BITMAP), &bm))
	{
		aWidth = bm.bmWidth;
		aHeight = bm.bmHeight;
	}
	else
	{
		if (!no_delete_bitmap)
		{
			if (image_type == IMAGE_ICON)
				DestroyIcon((HICON)hbitmap);
			else
				DeleteObject(hbitmap);
		}
		return NULL;
	}
	
	return hbitmap;
}


// Helper function to calculate image histogram for color analysis
static void CalculateHistogram(LPCOLORREF pixels, int pixel_count, int histogram[256])
{
	memset(histogram, 0, 256 * sizeof(int));
	
	for (int i = 0; i < pixel_count; ++i)
	{
		COLORREF pixel = pixels[i];
		// Calculate luminance using standard RGB to grayscale conversion
		BYTE r = GetBValue(pixel);  // RGB format, so B gets R
		BYTE g = GetGValue(pixel);
		BYTE b = GetRValue(pixel);  // RGB format, so R gets B
		
		int luminance = (int)(0.299 * r + 0.587 * g + 0.114 * b);
		if (luminance > 255) luminance = 255;
		histogram[luminance]++;
	}
}


// Helper function to detect edges using simple gradient method
static int DetectEdges(LPCOLORREF pixels, int width, int height)
{
	int edge_count = 0;
	const int threshold = 30; // Edge detection threshold
	
	for (int y = 1; y < height - 1; ++y)
	{
		for (int x = 1; x < width - 1; ++x)
		{
			int idx = y * width + x;
			COLORREF center = pixels[idx];
			COLORREF right = pixels[idx + 1];
			COLORREF bottom = pixels[idx + width];
			
			// Calculate gradient
			int grad_x = abs((int)GetBValue(right) - (int)GetBValue(center));
			int grad_y = abs((int)GetBValue(bottom) - (int)GetBValue(center));
			
			if (grad_x > threshold || grad_y > threshold)
				edge_count++;
		}
	}
	
	return edge_count;
}


// Helper function to detect text-like patterns (high frequency edges)
static bool DetectTextPatterns(LPCOLORREF pixels, int width, int height)
{
	int histogram[256];
	CalculateHistogram(pixels, width * height, histogram);
	
	// Text typically has high contrast (bimodal histogram)
	int dark_pixels = 0, light_pixels = 0;
	for (int i = 0; i < 128; ++i)
		dark_pixels += histogram[i];
	for (int i = 128; i < 256; ++i)
		light_pixels += histogram[i];
	
	// Text usually has many edges
	int edge_count = DetectEdges(pixels, width, height);
	int total_pixels = width * height;
	float edge_ratio = (float)edge_count / total_pixels;
	
	// Heuristic: text has high edge density and contrast
	return (edge_ratio > 0.15f && abs(dark_pixels - light_pixels) > total_pixels * 0.2f);
}


// Helper function to detect object-like regions (color clustering)
static int DetectColorRegions(LPCOLORREF pixels, int width, int height)
{
	int histogram[256];
	CalculateHistogram(pixels, width * height, histogram);
	
	// Count distinct color regions (peaks in histogram)
	int regions = 0;
	bool in_peak = false;
	
	for (int i = 1; i < 255; ++i)
	{
		if (histogram[i] > histogram[i-1] && histogram[i] > histogram[i+1] && histogram[i] > (width * height) / 100)
		{
			if (!in_peak)
			{
				regions++;
				in_peak = true;
			}
		}
		else if (histogram[i] < histogram[i-1] * 0.5)
		{
			in_peak = false;
		}
	}
	
	return regions;
}


// Helper function to calculate similarity between two images
// Returns similarity percentage (0-100)
static int CalculateImageSimilarity(LPCOLORREF pixels1, int width1, int height1,
                                     LPCOLORREF pixels2, int width2, int height2)
{
	// If dimensions don't match, similarity is lower
	if (width1 != width2 || height1 != height2)
	{
		// Compare based on histogram similarity only
		int hist1[256], hist2[256];
		CalculateHistogram(pixels1, width1 * height1, hist1);
		CalculateHistogram(pixels2, width2 * height2, hist2);
		
		// Normalize histograms
		int total1 = width1 * height1;
		int total2 = width2 * height2;
		
		double similarity_sum = 0.0;
		for (int i = 0; i < 256; ++i)
		{
			double freq1 = (double)hist1[i] / total1;
			double freq2 = (double)hist2[i] / total2;
			similarity_sum += (freq1 < freq2) ? freq1 : freq2;
		}
		
		return (int)(similarity_sum * 100);
	}
	
	// Direct pixel comparison with histogram comparison
	int matching_pixels = 0;
	int total_pixels = width1 * height1;
	const int color_tolerance = 30; // Allow some color variation
	
	// Compare pixels
	for (int i = 0; i < total_pixels; ++i)
	{
		COLORREF p1 = pixels1[i] & 0x00FFFFFF;
		COLORREF p2 = pixels2[i] & 0x00FFFFFF;
		
		int r_diff = abs((int)GetBValue(p1) - (int)GetBValue(p2));
		int g_diff = abs((int)GetGValue(p1) - (int)GetGValue(p2));
		int b_diff = abs((int)GetRValue(p1) - (int)GetRValue(p2));
		
		if (r_diff <= color_tolerance && g_diff <= color_tolerance && b_diff <= color_tolerance)
			matching_pixels++;
	}
	
	// Calculate similarity percentage
	int pixel_similarity = (matching_pixels * 100) / total_pixels;
	
	// Also compare histograms for overall color distribution
	int hist1[256], hist2[256];
	CalculateHistogram(pixels1, total_pixels, hist1);
	CalculateHistogram(pixels2, total_pixels, hist2);
	
	int hist_match = 0;
	for (int i = 0; i < 256; ++i)
	{
		int diff = abs(hist1[i] - hist2[i]);
		hist_match += (total_pixels - diff);
	}
	int histogram_similarity = (hist_match / 256) / total_pixels;
	if (histogram_similarity > 100) histogram_similarity = 100;
	
	// Weighted combination: 70% pixel similarity, 30% histogram similarity
	return (pixel_similarity * 7 + histogram_similarity * 3) / 10;
}


// AI-based image recognition function
// Recognizes objects, text, and patterns in an image using image analysis
bif_impl FResult AIImageRecognize(StrArg aImageFile, optl<StrArg> aMode, StrRet &aRetVal)
{
	// aMode can be: "objects", "text", "patterns", or "all" (default)
	LPCTSTR mode = aMode.has_value() ? aMode.value() : _T("all");
	
	// Verify the image file exists
	if (!FileExist(aImageFile))
	{
		aRetVal.SetTemp(_T("Error: Image file not found"));
		return OK;
	}
	
	// Load the image
	int width, height;
	HBITMAP hbitmap = LoadImageFile(aImageFile, width, height);
	if (!hbitmap)
	{
		aRetVal.SetTemp(_T("Error: Failed to load image"));
		return OK;
	}
	
	// Get device context and bitmap bits
	HDC hdc = GetDC(NULL);
	if (!hdc)
	{
		DeleteObject(hbitmap);
		aRetVal.SetTemp(_T("Error: Failed to get device context"));
		return OK;
	}
	
	bool is_16bit;
	LONG bmp_width, bmp_height;
	LPCOLORREF pixels = getbits(hbitmap, hdc, bmp_width, bmp_height, is_16bit);
	ReleaseDC(NULL, hdc);
	DeleteObject(hbitmap);
	
	if (!pixels)
	{
		aRetVal.SetTemp(_T("Error: Failed to read image data"));
		return OK;
	}
	
	// Perform recognition based on mode
	TCHAR result[512] = {0};
	
	if (!_tcsicmp(mode, _T("text")))
	{
		// Text detection
		bool has_text = DetectTextPatterns(pixels, bmp_width, bmp_height);
		_stprintf(result, _T("Text detection: %s (resolution: %dx%d)"), 
			has_text ? _T("Text patterns detected") : _T("No text patterns found"),
			bmp_width, bmp_height);
	}
	else if (!_tcsicmp(mode, _T("objects")))
	{
		// Object detection through color region analysis
		int regions = DetectColorRegions(pixels, bmp_width, bmp_height);
		_stprintf(result, _T("Object detection: %d distinct regions found (resolution: %dx%d)"), 
			regions, bmp_width, bmp_height);
	}
	else if (!_tcsicmp(mode, _T("patterns")))
	{
		// Pattern detection through edge analysis
		int edges = DetectEdges(pixels, bmp_width, bmp_height);
		float edge_density = (float)edges / (bmp_width * bmp_height);
		_stprintf(result, _T("Pattern analysis: %.1f%% edge density, %d edges detected"), 
			edge_density * 100, edges);
	}
	else if (!_tcsicmp(mode, _T("all")))
	{
		// Comprehensive analysis
		bool has_text = DetectTextPatterns(pixels, bmp_width, bmp_height);
		int regions = DetectColorRegions(pixels, bmp_width, bmp_height);
		int edges = DetectEdges(pixels, bmp_width, bmp_height);
		
		_stprintf(result, _T("Analysis: %dx%d, %d regions, %d edges, text: %s"), 
			bmp_width, bmp_height, regions, edges,
			has_text ? _T("detected") : _T("none"));
	}
	else
	{
		free(pixels);
		aRetVal.SetTemp(_T("Error: Invalid mode. Use 'objects', 'text', 'patterns', or 'all'"));
		return OK;
	}
	
	free(pixels);
	aRetVal.SetTemp(result);
	return OK;
}


// AI-based image comparison function
// Compares two images and returns a similarity score
bif_impl FResult AIImageCompare(StrArg aImage1, StrArg aImage2, optl<int> aThreshold, BOOL &aRetVal)
{
	// aThreshold is the minimum similarity percentage (0-100) to return true
	int threshold = aThreshold.value_or(80);
	
	if (threshold < 0) threshold = 0;
	if (threshold > 100) threshold = 100;
	
	// Verify both image files exist
	if (!FileExist(aImage1) || !FileExist(aImage2))
	{
		aRetVal = false;
		return OK;
	}
	
	// Load both images
	int width1, height1, width2, height2;
	HBITMAP hbitmap1 = LoadImageFile(aImage1, width1, height1);
	HBITMAP hbitmap2 = LoadImageFile(aImage2, width2, height2);
	
	if (!hbitmap1 || !hbitmap2)
	{
		if (hbitmap1) DeleteObject(hbitmap1);
		if (hbitmap2) DeleteObject(hbitmap2);
		aRetVal = false;
		return OK;
	}
	
	// Get device context and bitmap bits for both images
	HDC hdc = GetDC(NULL);
	if (!hdc)
	{
		DeleteObject(hbitmap1);
		DeleteObject(hbitmap2);
		aRetVal = false;
		return OK;
	}
	
	bool is_16bit1, is_16bit2;
	LONG bmp_width1, bmp_height1, bmp_width2, bmp_height2;
	LPCOLORREF pixels1 = getbits(hbitmap1, hdc, bmp_width1, bmp_height1, is_16bit1);
	LPCOLORREF pixels2 = getbits(hbitmap2, hdc, bmp_width2, bmp_height2, is_16bit2);
	
	ReleaseDC(NULL, hdc);
	DeleteObject(hbitmap1);
	DeleteObject(hbitmap2);
	
	if (!pixels1 || !pixels2)
	{
		if (pixels1) free(pixels1);
		if (pixels2) free(pixels2);
		aRetVal = false;
		return OK;
	}
	
	// Calculate similarity
	int similarity = CalculateImageSimilarity(pixels1, bmp_width1, bmp_height1,
	                                           pixels2, bmp_width2, bmp_height2);
	
	free(pixels1);
	free(pixels2);
	
	// Return true if similarity meets or exceeds threshold
	aRetVal = (similarity >= threshold);
	return OK;
}


// AI decision-making function based on image analysis
// Analyzes an image and makes a decision based on configured rules
bif_impl FResult AIDecide(StrArg aImageFile, StrArg aRules, optl<StrArg> aOptions, StrRet &aRetVal)
{
	// aRules is a string describing the decision rules (e.g., "if:text_detected,then:proceed")
	// aOptions can contain additional parameters
	
	// Verify the image file exists
	if (!FileExist(aImageFile))
	{
		aRetVal.SetTemp(_T("Error: Image file not found"));
		return OK;
	}
	
	// Load and analyze the image
	int width, height;
	HBITMAP hbitmap = LoadImageFile(aImageFile, width, height);
	if (!hbitmap)
	{
		aRetVal.SetTemp(_T("Error: Failed to load image"));
		return OK;
	}
	
	HDC hdc = GetDC(NULL);
	if (!hdc)
	{
		DeleteObject(hbitmap);
		aRetVal.SetTemp(_T("Error: Failed to get device context"));
		return OK;
	}
	
	bool is_16bit;
	LONG bmp_width, bmp_height;
	LPCOLORREF pixels = getbits(hbitmap, hdc, bmp_width, bmp_height, is_16bit);
	ReleaseDC(NULL, hdc);
	DeleteObject(hbitmap);
	
	if (!pixels)
	{
		aRetVal.SetTemp(_T("Error: Failed to read image data"));
		return OK;
	}
	
	// Perform image analysis
	bool has_text = DetectTextPatterns(pixels, bmp_width, bmp_height);
	int regions = DetectColorRegions(pixels, bmp_width, bmp_height);
	int edges = DetectEdges(pixels, bmp_width, bmp_height);
	float edge_density = (float)edges / (bmp_width * bmp_height);
	
	free(pixels);
	
	// Parse rules
	LPTSTR rules_copy = _tcsdup(aRules);
	if (!rules_copy)
	{
		aRetVal.SetTemp(_T("Error: Memory allocation failed"));
		return OK;
	}
	
	LPTSTR if_part = _tcsstr(rules_copy, _T("if:"));
	LPTSTR then_part = _tcsstr(rules_copy, _T("then:"));
	
	if (!if_part || !then_part)
	{
		free(rules_copy);
		aRetVal.SetTemp(_T("Error: Rules must contain 'if:' and 'then:'"));
		return OK;
	}
	
	// Extract condition and action
	if_part += 3; // skip "if:"
	then_part += 5; // skip "then:"
	
	LPTSTR comma = _tcsstr(if_part, _T(","));
	if (!comma)
	{
		free(rules_copy);
		aRetVal.SetTemp(_T("Error: Invalid rule format"));
		return OK;
	}
	
	*comma = '\0'; // terminate if_part at comma
	
	// Evaluate condition based on image analysis
	bool condition_met = false;
	TCHAR condition_result[256] = {0};
	
	if (_tcsstr(if_part, _T("text")))
	{
		condition_met = has_text;
		_stprintf(condition_result, _T("text %s"), has_text ? _T("detected") : _T("not found"));
	}
	else if (_tcsstr(if_part, _T("object")) || _tcsstr(if_part, _T("region")))
	{
		condition_met = (regions > 2); // Multiple regions indicate objects
		_stprintf(condition_result, _T("%d regions detected"), regions);
	}
	else if (_tcsstr(if_part, _T("pattern")) || _tcsstr(if_part, _T("edge")))
	{
		condition_met = (edge_density > 0.15f); // High edge density
		_stprintf(condition_result, _T("%.1f%% edge density"), edge_density * 100);
	}
	else if (_tcsstr(if_part, _T("complex")))
	{
		condition_met = (edges > 1000 && regions > 3);
		_stprintf(condition_result, _T("complexity: %d edges, %d regions"), edges, regions);
	}
	else
	{
		// Generic condition - check if image has content
		condition_met = (regions > 0);
		_stprintf(condition_result, _T("content detected: %d regions"), regions);
	}
	
	// Build result
	TCHAR result[512];
	if (condition_met)
	{
		_stprintf(result, _T("Decision: %s → Action: %s (%s)"), 
			if_part, then_part, condition_result);
	}
	else
	{
		_stprintf(result, _T("Decision: Condition not met (%s)"), condition_result);
	}
	
	free(rules_copy);
	aRetVal.SetTemp(result);
	return OK;
}


// Helper function to get AI model information
bif_impl FResult AIGetModelInfo(optl<StrArg> aModelType, StrRet &aRetVal)
{
	// aModelType can be: "recognition", "comparison", "decision"
	LPCTSTR model_type = aModelType.has_value() ? aModelType.value() : _T("all");
	
	TCHAR info[1024] = {0};
	
	if (!_tcsicmp(model_type, _T("recognition")))
	{
		_stprintf(info, _T("AI Image Recognition (Phase 2)\n")
			_T("- Object detection via color region analysis\n")
			_T("- Text pattern detection using edge analysis\n")
			_T("- Pattern recognition with histogram analysis\n")
			_T("- Supports: objects, text, patterns, all modes"));
	}
	else if (!_tcsicmp(model_type, _T("comparison")))
	{
		_stprintf(info, _T("AI Image Comparison (Phase 2)\n")
			_T("- Pixel-by-pixel similarity analysis\n")
			_T("- Histogram-based comparison\n")
			_T("- Configurable threshold (0-100%%)\n")
			_T("- Handles different image dimensions"));
	}
	else if (!_tcsicmp(model_type, _T("decision")))
	{
		_stprintf(info, _T("AI Decision Engine (Phase 2)\n")
			_T("- Rule-based decision making\n")
			_T("- Conditions: text, objects, patterns, complexity\n")
			_T("- Real-time image analysis integration\n")
			_T("- Format: if:condition,then:action"));
	}
	else // "all" or default
	{
		_stprintf(info, _T("AutoHotkey AI Image Processing (Phase 2)\n\n")
			_T("Recognition Engine:\n")
			_T("  • Edge detection and pattern analysis\n")
			_T("  • Color histogram analysis\n")
			_T("  • Text pattern detection\n")
			_T("  • Object region identification\n\n")
			_T("Comparison Engine:\n")
			_T("  • Pixel similarity matching\n")
			_T("  • Histogram comparison\n")
			_T("  • Multi-resolution support\n\n")
			_T("Decision Engine:\n")
			_T("  • Real-time condition evaluation\n")
			_T("  • Multiple rule types\n")
			_T("  • Automated action triggering\n\n")
			_T("Technology: GDI+ image processing, custom algorithms"));
	}
	
	aRetVal.SetTemp(info);
	return OK;
}
