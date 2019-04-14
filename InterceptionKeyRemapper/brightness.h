#pragma once
#include <thread>
#include <Windows.h>

// C.h

//Ensure __func__ is defined, someway or another. This is the name of the currently executing function.
#ifndef __func__
#define __func__ __FUNCTION__
#endif

//These macros make it so that you can just type w(SOME_RANDOM_POUND_DEFINE) to turn a #define'd char string into a wchar_t string. 
#define __L(x) L ## x
#define w(x) __L(x)

//Turn stuff into strings!
#define STRINGIFY(s) #s

//Make printing errors easier.
#define PRINT_INT(val) printf("%s = %d\n", STRINGIFY(val), val);
#define PRINT_INT(val) printf("%s = %d\n", STRINGIFY(val), val);
#define PRINT_LONG(val) printf("%s = %i\n", STRINGIFY(val), val);
#define PRINT_INT64(val) printf("%s = %I64d\n", STRINGIFY(val), val);
#define PRINT_DOUBLE(val) printf("%s = %f\n", STRINGIFY(val), val);
#define PRINT_HEX(val) printf("%s = 0x%X\n", STRINGIFY(val), val);
//#define PRINT_HEX(val) printf("%s = 0x%016llX\n" STRINGIFY(val), val);
#define PRINT_BOOL(val) printf("%s = %s\n", STRINGIFY(val), val? "true":"false");
#define PRINT_WSTR(val) printf("%s = '%S'\n", STRINGIFY(val), val);
#define PRINT_POINT(val) printf("%s = (%d, %d)\n", STRINGIFY(val), (val).x, (val).y);
#define PRINT_RECT(val) printf("%s = (%d, %d, %d, %d)\n", STRINGIFY(val), (val).left, (val).top, (val).right, (val).bottom);

//This marco allows you to easily declare a static string constant (wchar_t*) in a class header
//Meaning, instead of this:
//		static const wchar_t *wImmersiveControlPanelUMID() 
//		{
//			static const wchar_t *str = L"windows.immersivecontrolpanel_cw5n1h2txyewy!microsoft.windows.immersivecontrolpanel";
//			return str;
//		}
//
//All you gotta do is this:
//		STATIC_WSTR(wImmersiveControlPanelUMID, L"windows.immersivecontrolpanel_cw5n1h2txyewy!microsoft.windows.immersivecontrolpanel");
//
//But when you do this, you need to reference it like a function. For example (assuming the class name is ModernHelpers): 
//		wcslen(ModernHelpers::wImmersiveControlPanelUMID());
#define STATIC_WSTR(name, text) inline static const wchar_t *(name)() {static const wchar_t *str = (text); return str;}
#define STATIC_DOUBLE(name, val) inline static double (name)(){return (val);}
#define STATIC_POINT(name, x, y) inline static POINT (name)(){return {(x), (y)};}


//Prints the error specified by @error. If @error is set to ERROR_SUCCESS, it will be populated with GetLastError().
#define PRINT_ERROR(error)\
{\
	DWORD err = (error);\
	if (err == ERROR_SUCCESS)\
		err = GetLastError();\
	wchar_t wErrorText[MAX_PATH];\
	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, err, 0, wErrorText, MAX_PATH, 0);\
	printf("%s(): Error %d - %S \n", __func__, err, wErrorText);\
}


#define PRINT_ERROR_SHORT(error)\
{\
	DWORD err = (error);\
	if (err == ERROR_SUCCESS)\
		err = GetLastError();\
	wchar_t wErrorText[MAX_PATH];\
	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, err, 0, wErrorText, MAX_PATH, 0);\
	printf("Error is %d - %S \n", __func__, err, wErrorText);\
}


//Some macros to help make error checking easier
#define FAIL_IF(statement) if((statement)) {printf("%s: failed because the following was true '%s'\n",__func__, STRINGIFY((statement))); PRINT_ERROR_SHORT(ERROR_SUCCESS); return false;}
#define CONTINUE_IF(statement) if((statement) == false) {printf("%s: failed because the following was false '%s'\n",__func__, STRINGIFY((statement))); PRINT_ERROR_SHORT(ERROR_SUCCESS); return false;}


//Simple support for Tristate (alternative to boost::Tri)
enum class Tri
{
	Yes,
	No,
	Unknown
};

// BRIGHTNESS START

//Used in the GetBrightness function
#pragma comment (lib, "PowrProf.lib")
#include "Powersetting.h"
#include "PowrProf.h"


namespace BrightnessHandler
{

	//Shows the slider. Returns true on success. Note: this has no effect on screen brightness. This is done statically at the cost of perf. 
	bool static ShowSlider()
	{
		HWND hwnd = FindWindow(L"NativeHWNDHost", L"");
		CONTINUE_IF(hwnd != NULL);
		CONTINUE_IF(PostMessage(hwnd, RegisterWindowMessage(L"SHELLHOOK"), 0x0037, 0));
		//CONTINUE_IF(ShowWindow(hwnd, SW_SHOW));
		return true;
	}

	//Gets the AC and DC brightness indexes for the current power configuration. Returns @DCIndex or @ACIndex, depending on power status.
	DWORD static GetBrightness()
	{
		DWORD ACIndex = 69;
		DWORD DCIndex = 96;

		GUID *pPwrGUID;
		GUID subGUID = GUID_VIDEO_SUBGROUP;
		GUID BriGUID = GUID_DEVICE_POWER_POLICY_VIDEO_BRIGHTNESS;

		PowerGetActiveScheme(NULL, &pPwrGUID);
		PowerReadACValueIndex(NULL, pPwrGUID, &subGUID, &BriGUID, &ACIndex);
		PowerReadDCValueIndex(NULL, pPwrGUID, &subGUID, &BriGUID, &DCIndex);

		//Get the power status
		SYSTEM_POWER_STATUS powerStatus;
		FAIL_IF(!GetSystemPowerStatus(&powerStatus));

		if (powerStatus.ACLineStatus)
			return ACIndex;

		return DCIndex;
	}

	//Sets brightness from [0, 100] using the PowerWriteACValueIndex() method. 
	bool static SetBrightness(int val)
	{
		GUID *pPwrGUID;
		GUID subGUID = GUID_VIDEO_SUBGROUP;
		GUID briGUID = GUID_DEVICE_POWER_POLICY_VIDEO_BRIGHTNESS;

		FAIL_IF((PowerGetActiveScheme(NULL, &pPwrGUID) != ERROR_SUCCESS));
		FAIL_IF(PowerWriteACValueIndex(NULL, pPwrGUID, &subGUID, &briGUID, val) != ERROR_SUCCESS);
		FAIL_IF(PowerWriteDCValueIndex(NULL, pPwrGUID, &subGUID, &briGUID, val) != ERROR_SUCCESS);
		//if (PowerApplySettingChanges != NULL) PowerApplySettingChanges(&subGUID, &briGUID);
		FAIL_IF(PowerSetActiveScheme(NULL, pPwrGUID) != ERROR_SUCCESS);

		return true;
	}

	//Increments the brightness by @increment. Negative values are supported. 
	bool static Increment(int increment)
	{
		//Show the slider
		BrightnessHandler::ShowSlider();

		//Gets the brightness. 
		DWORD Index;
		Index = BrightnessHandler::GetBrightness();

		//Exit quickly if this function will have no change in brightness. 
		if (increment == 0 ||
			Index == 100 && increment >= 0 ||
			Index == 0 && increment <= 0)
			return true;

		//Enforce the range on the Index
		if ((int)Index + increment >= 100)
			Index = 100;
		else if ((int)Index + increment <= 0)
			Index = 0;
		else
			Index += increment;

		//Set the brightness, and peace out. 
		BrightnessHandler::SetBrightness(Index);

		return true;
	}

};
